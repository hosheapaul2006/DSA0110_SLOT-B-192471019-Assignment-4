"""Audit recorded evidence and preservation; never manufacture execution results."""
import datetime
import hashlib
import json
from pathlib import Path
import re
import subprocess
import xml.etree.ElementTree as ET
from verify import ROOT, dependencies
from run_all import EXPECTED

checks = []
def check(name, condition, evidence):
    checks.append({'requirement': name, 'status': 'PASS' if condition else 'FAIL', 'evidence': evidence})
    if not condition: raise AssertionError(name + ': ' + evidence)
def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()
def git(*args): return subprocess.run(['git', *args], cwd=ROOT, capture_output=True, check=True).stdout

records = []
for path in sorted(ROOT.glob('Task*/outputs/*_verification.json')):
    if path.name.startswith('sanitizer_'): continue
    record = json.loads(path.read_text(encoding='utf-8')); records.append(record)
    source = ROOT/record['source']
    check(record['source']+' source hash', sha(source)==record['sha256'], str(path.relative_to(ROOT)))
    check(record['source']+' dependencies', dependencies(source)==record['headers_sha256'], 'Exact local include graph hashes')
    check(record['source']+' compile observation', record['status'].startswith('PASS'), 'Actual compile exit '+str(record['compile_exit']))
    if record.get('run_exit') == 0:
        stem = record['source'].split('/',1)[1].removesuffix('.cpp').replace('/','_')
        out = ROOT/record['source'].split('/')[0]/'outputs'
        if '--tests' in record['run_command']:
            stderr=(out/(stem+'_stderr.txt')).read_text()
            match=re.search(r'TEST_COUNT (\d+)',stderr)
            check(record['source']+' runtime checks', bool(match) and int(match[1])==record['test_count'] and 'FAIL' not in stderr, 'Actual exit 0 and logged check count')
        if record['source'] in EXPECTED:
            expected=(ROOT/record['source'].split('/')[0]/EXPECTED[record['source']]).read_text()
            check(record['source']+' exact expected output', (out/(stem+'_actual.txt')).read_text()==expected, 'Only text-mode CRLF decoding')

all_sources={p.relative_to(ROOT).as_posix() for p in ROOT.glob('Task*/**/*.cpp')}
check('every independent source has a record', all_sources=={r['source'] for r in records}, 'All base, derived, diagnostic and isolated-only sources')
for t,commit in [(2,'a648fec'),(3,'a5865f7'),(4,'bb0d9aa')]:
    names=['predict_output.cpp','prediction_before_execution.md','outputs/predict_output_predicted.txt']
    if t==2:names+=['diamond_experiment.cpp','outputs/diamond_experiment_predicted.txt']
    for name in names:
        relative=f'Task{t:02}/'+name
        check(relative+' immutable prediction', git('show',commit+':'+relative)==(ROOT/relative).read_bytes(), 'Pre-execution commit '+commit)
    original=(ROOT/f'Reference/Q{t}_prediction_original.txt').read_bytes()
    check(f'Task{t} supplied prediction source preserved', original==(ROOT/f'Task{t:02}/predict_output.cpp').read_bytes(), 'Extracted original source')
    stems=['predict_output']+(['diamond_experiment'] if t==2 else [])
    for stem in stems:
        record=json.loads((ROOT/f'Task{t:02}/outputs/{stem}_verification.json').read_text())
        committed=int(git('show','-s','--format=%ct',commit).decode().strip())
        observed=datetime.datetime.fromisoformat(record['timestamp_utc']).timestamp()
        check(f'Task{t} {stem} prediction chronology', committed<=observed, 'Prediction commit precedes actual execution record')
check('Task2 only virtual keywords removed', (ROOT/'Task02/predict_output.cpp').read_text().replace(': virtual Person',': Person')==(ROOT/'Task02/diamond_experiment.cpp').read_text(), 'Exact modified supplied source')

for t in [3,5]:
    original=(ROOT/f'Reference/Q{t}_TODO_original.txt').read_text()
    complete=(ROOT/f'Task{t:02}/complete_code.cpp').read_text()
    if t==3:
        insert=re.search(r'    Student\(const std::string& n, int a, long long r, const std::string& c, int p\)\n[^\n]*\n',complete)[0]
        reconstructed=re.sub(r'    // TODO 1:[^\n]*\n    //[^\n]*\n[ \t]*\n',lambda m:insert,original)
    else:
        reconstructed=original
        for i,name in [(1,'add'),(2,'setDiscount')]:
            insert=re.search(r'    Cart& '+name+r'\([^\n]*\n    \{.*?\n    \}\n',complete,re.S)[0]
            reconstructed=re.sub(rf'    // TODO {i}:[^\n]*\n    //[^\n]*\n[ \t]*\n',lambda m:insert,reconstructed)
    check(f'Task{t} TODO-only preservation', reconstructed==complete, 'All supplied bytes outside replaced TODO regions preserved')
check('Task5 by-value experiment isolated', (ROOT/'Task05/complete_code.cpp').read_text().replace('Cart& add(','Cart add(')==(ROOT/'Task05/return_by_value.cpp').read_text(), 'Only return type changed')
baseline=json.loads((ROOT/'Task06/baseline_before_extension.json').read_text())
for name,expected in baseline['files'].items():
    check(name+' unchanged for ElectricBus', sha(ROOT/name)==expected and git('show','368d01c:'+name)==(ROOT/name).read_bytes(), 'Actual committed pre-extension baseline')
check('ElectricBus is a new extension file', subprocess.run(['git','cat-file','-e','368d01c:Task06/electric_bus.hpp'],cwd=ROOT,capture_output=True).returncode!=0, 'Baseline contains no bus implementation')

def normalize(value):
    lines=[]
    for line in value.replace('\r\n','\n').split('\n'):
        if line.startswith('sizeof'):
            sizes=re.findall(r'\d+',line)
            assert all(0<int(n)<=1048576 for n in sizes)
            line=re.sub(r'\d+','<size>',line)
        elif line.startswith('Element address integers before/after reserve='):
            match=re.search(r'=(\d+)/(\d+)$',line)
            assert match and int(match[1])>0 and int(match[2])>0 and match[1]!=match[2]
            line='Element address integers before/after reserve=<address>/<address>'
        lines.append(line)
    return '\n'.join(lines)

manifest=json.loads((ROOT/'Reference/programiz_manifest.json').read_text())
shots=set()
for item in manifest:
    prefix=ROOT/item['base_source'].split('/')[0]/'outputs'/(item['output_stem']+'_programiz')
    record=json.loads(prefix.with_suffix('.json').read_text())
    check(item['base_source']+' Programiz source', record['status']=='PASS' and record['sha256']==sha(ROOT/item['submitted_source']), 'Exact source pasted through browser')
    observed=normalize(prefix.with_suffix('.txt').read_text());expected=normalize((ROOT/item['local_stdout']).read_text())
    suffix=observed[len(expected):].strip()
    check(item['base_source']+' Programiz output', observed.startswith(expected) and suffix in ['', '=== Code Execution Successful ==='], 'Fixed characters exact; platform observations explicitly separated')
    check(item['base_source']+' screenshot records', len(record['screenshots'])>=1, 'Saved after actual Run')
    for name in record['screenshots']:
        image=ROOT/name;shots.add(name)
        check(name+' genuine JPEG artifact', image.is_file() and image.read_bytes().startswith(b'\xff\xd8\xff') and image.stat().st_size>5000, 'Browser-captured file SHA256 '+sha(image))
check('all screenshot artifacts indexed', shots=={p.relative_to(ROOT).as_posix() for p in ROOT.glob('Task*/screenshots/*.jpg')}, 'Every capture has source/output provenance')

sanitizers=json.loads((ROOT/'Reference/sanitizer_summary.json').read_text())
for record in sanitizers:
    check(record['source']+' actual sanitizer code', sha(ROOT/record['source'])==record['sha256'] and dependencies(ROOT/record['source'])==record['headers_sha256'], record['run_url'])
    prefix=ROOT/record['source'].split('/')[0]/'outputs'/('sanitizer_'+record['source'].split('/',1)[1].removesuffix('.cpp').replace('/','_'))
    stderr=Path(str(prefix)+'_stderr.txt').read_text()
    expected=record['expected_error_pattern']
    passed=(record['run_exit']!=0 and re.search(expected,stderr)) if expected else (record['run_exit']==0 and stderr=='')
    check(record['source']+' sanitizer observation', bool(passed) and record['compile_exit']==0 and record['status']=='PASS', 'Actual exit '+str(record['run_exit']))

for task in range(1,7):
    for name in ['explanation.md','test_cases.md']:
        check(f'Task{task:02}/'+name+' exists', (ROOT/f'Task{task:02}'/name).is_file(), 'Task-local supporting documentation')
for task in ['Task01','Task02','Task06']:
    svg=ROOT/task/'diagrams/hierarchy.svg'
    check(task+' UML SVG', ET.parse(svg).getroot().tag.endswith('svg'), 'Rendered layout visually reviewed; editable Mermaid also supplied')
defence=(ROOT/'Task01/explanation.md').read_text().split('## Extensibility defence (under 100 words)')[1].split('## Type-code')[0]
check('Task1 defence word limit', len(defence.split())<=100, str(len(defence.split()))+' words')
check('no outstanding C++ TODO markers', all(not re.search(r'\bTODO\s+\d',p.read_text()) for p in ROOT.glob('Task*/**/*.cpp')), 'Original TODO context preserved only as reference text')
check('no report generated', not any(p.is_dir() and p.name.lower()=='report' for p in ROOT.rglob('*') if '.git' not in p.parts) and not any(p for p in ROOT.rglob('*.docx')) and {p.relative_to(ROOT).as_posix() for p in ROOT.rglob('*.pdf')}=={'Reference/Assignment 4.pdf'}, 'Only unchanged input PDF exists')
tracked=git('ls-files').decode().splitlines()
check('no tracked build products', not any(p.endswith(('.exe','.o','.obj','.zip')) or p.startswith('build/') for p in tracked), 'Tracked-file inventory')
check('README and requirement checklist', all((ROOT/p).is_file() for p in ['README.md','ASSIGNMENT_CHECKLIST.md']), 'Repository navigation and traceable requirement status')
base=[r for r in records if not Path(r['source']).name.startswith('online_')]
result={'timestamp_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
        'base_compiled_programs':sum(r['compile_exit']==0 for r in base),
        'base_normal_executions':sum(r.get('run_exit')==0 for r in base),
        'compiled_source_files_including_browser_builds':sum(r['compile_exit']==0 for r in records),
        'normal_build_run_records':sum(r.get('run_exit')==0 for r in records),
        'expected_compiler_error_sources':sum(r['compile_exit']!=0 for r in records),
        'runtime_check_evaluations':sum(r['test_count'] for r in records if r.get('run_exit')==0),
        'base_runtime_check_evaluations':sum(r['test_count'] for r in base if r.get('run_exit')==0),
        'recorded_evaluations_including_diagnostics':sum(r['test_count'] for r in records),
        'programiz_programs':len(manifest),'genuine_screenshots':len(shots),
        'sanitizer_cases':len(sanitizers),'sanitizer_clean_cases':sum(not r['expected_error_pattern'] for r in sanitizers),
        'sanitizer_expected_error_cases':sum(bool(r['expected_error_pattern']) for r in sanitizers),'audit_checks':checks}
(ROOT/'Reference/final_audit.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8',newline='\n')
print(json.dumps({k:v for k,v in result.items() if k!='audit_checks'},indent=2))
print('Evidence audit checks:',len(checks),'PASS')
