"""Build/evidence orchestration. Assignment solutions and benchmarks are C++17."""
import argparse
import datetime
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess

ROOT=Path(__file__).resolve().parents[1]
PORTABLE=ROOT.parent/'.tools/mingw64/bin/g++.exe'
CXX=os.environ.get('CXX',str(PORTABLE) if PORTABLE.exists() else shutil.which('g++') or 'g++')

def dependencies(path, seen=None):
    seen = set() if seen is None else seen
    path = path.resolve()
    if path in seen: return {}
    seen.add(path)
    result = {}
    for name in re.findall(r'^\s*#include\s+"([^"]+)"', path.read_text(encoding='utf-8'), re.M):
        included = (path.parent/name).resolve()
        result[included.relative_to(ROOT).as_posix()] = hashlib.sha256(included.read_bytes()).hexdigest()
        result.update(dependencies(included, seen))
    return result

def verify(task,source,opt='-O0',expected=None,diagnostic=None,needle=None,compile_only=False,tests=False,extra=None,tag=None):
    path=ROOT/task/source; out=ROOT/task/'outputs';out.mkdir(parents=True,exist_ok=True)
    stem=source.replace('/','_').replace('.cpp','')+('_'+opt[1:] if 'benchmark' in source else '')
    if tag: stem+='_'+tag
    build=ROOT/'build'/task;build.mkdir(parents=True,exist_ok=True)
    exe=build/(stem+'.exe')
    env=os.environ.copy();env['PATH']=str(Path(CXX).parent)+os.pathsep+env.get('PATH','')
    command=[CXX,'-std=c++17','-Wall','-Wextra','-Wpedantic',opt,str(path),'-o',str(exe)]+(extra or [])
    run=subprocess.run(command,capture_output=True,env=env,timeout=180)
    (out/(stem+'_compiler.txt')).write_bytes(run.stdout+run.stderr)
    record={'source':path.relative_to(ROOT).as_posix(),'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),
            'timestamp_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),'optimization':opt,
            'compile_command':command,'compile_exit':run.returncode,'expected_diagnostic':diagnostic,
            'compile_only':compile_only,'test_count':0,'tag':tag,
            'headers_sha256':dependencies(path)}
    if diagnostic=='error':
        assert run.returncode!=0 and (not needle or needle.lower() in run.stderr.decode(errors='replace').lower()),run.stderr.decode(errors='replace')
        record.update(status='PASS: expected compiler error',test_count=1)
    else:
        assert run.returncode==0,run.stderr.decode(errors='replace')
        if diagnostic=='warning':
            assert b'warning:' in run.stderr and (not needle or needle.lower() in run.stderr.decode(errors='replace').lower())
        elif not compile_only:
            assert not run.stderr,run.stderr.decode(errors='replace')
        if compile_only:
            record['status']='PASS: compiled only; execution reserved for isolated sanitizer test'
            record['test_count']=1 if diagnostic else 0
        else:
            command_run=[str(exe)]+(['--tests'] if tests else [])
            actual=subprocess.run(command_run,capture_output=True,env=env,timeout=240)
            (out/(stem+'_actual.txt')).write_bytes(actual.stdout)
            (out/(stem+'_stderr.txt')).write_bytes(actual.stderr)
            assert actual.returncode==0,(actual.returncode,actual.stderr.decode(errors='replace'))
            text=actual.stderr.decode(errors='replace')
            if tests:
                assert 'FAIL' not in text and re.search(r'TEST_COUNT (\d+)',text),text
                record['test_count']=int(re.search(r'TEST_COUNT (\d+)',text)[1])
            else: assert not actual.stderr,text
            record.update(run_command=command_run,run_exit=actual.returncode,status='PASS')
            if expected:
                expected_text=(ROOT/task/expected).read_text(encoding='utf-8')
                observed=actual.stdout.decode('utf-8').replace('\r\n','\n')
                assert expected_text==observed,(repr(expected_text),repr(observed))
                record['expected_comparison']='PASS: exact characters after platform newline decoding'
                record['test_count']+=1
    (out/(stem+'_verification.json')).write_text(json.dumps(record,indent=2)+'\n',encoding='utf-8')
    print(record['source'],opt,record['status'],'checks='+str(record['test_count']))
    return record

def flatten(task,source='main.cpp',destination='online.cpp'):
    visited=set()
    def expand(path):
        path=path.resolve()
        if path in visited:return ''
        visited.add(path);result=[]
        for line in path.read_text(encoding='utf-8').splitlines():
            match=re.fullmatch(r'\s*#include\s+"([^"]+)"\s*',line)
            result.append(expand(path.parent/match[1]) if match else line+'\n')
        return ''.join(result)
    (ROOT/task/destination).write_text(expand(ROOT/task/source),encoding='utf-8',newline='\n')

if __name__=='__main__':
    p=argparse.ArgumentParser();p.add_argument('task');p.add_argument('source');p.add_argument('--opt',default='-O0')
    p.add_argument('--expected');p.add_argument('--diagnostic',choices=['error','warning']);p.add_argument('--needle')
    p.add_argument('--compile-only',action='store_true');p.add_argument('--tests',action='store_true');p.add_argument('--extra',nargs='*');p.add_argument('--tag')
    a=p.parse_args();verify(a.task,a.source,a.opt,a.expected,a.diagnostic,a.needle,a.compile_only,a.tests,a.extra,a.tag)
