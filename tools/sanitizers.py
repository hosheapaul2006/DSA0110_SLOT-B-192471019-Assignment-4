"""Run controlled Linux sanitizer cases; export raw evidence into the job log."""
import base64
import datetime
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import subprocess
from verify import dependencies

ROOT=Path(__file__).resolve().parents[1]
CASES=[
 ('Task01/redesigned.cpp',None), ('Task02/main.cpp',None),
 ('Task03/main.cpp',None), ('Task03/nonvirtual_experiment.cpp',None), ('Task03/owning_address.cpp',None),
 ('Task04/original.cpp',r'new-delete-type-mismatch'), ('Task04/safe_slicing.cpp',None), ('Task04/repaired.cpp',None),
 ('Task05/main.cpp',None), ('Task05/pointer_experiments.cpp',None), ('Task05/complete_code.cpp',None), ('Task05/return_by_value.cpp',None),
 ('Task06/main.cpp',None), ('Task06/extension_demo.cpp',None)]

def main():
    files={};records=[];failed=False
    compiler=subprocess.run(['g++','--version'],capture_output=True,text=True,check=True).stdout
    files['Reference/sanitizer_environment.txt']=(compiler+'\n'+platform.platform()+'\nCPU: '+platform.processor()+'\nRunner: '+os.environ.get('RUNNER_OS','')+'\nCommit: '+os.environ.get('GITHUB_SHA','')+'\n').encode()
    build=ROOT/'build/sanitizer';build.mkdir(parents=True,exist_ok=True)
    for source,expected in CASES:
        path=ROOT/source
        if not path.exists():
            records.append({'source':source,'status':'FAIL: missing source'});failed=True;continue
        stem=source.removesuffix('.cpp').replace('/','_')
        prefix=source.split('/')[0]+'/outputs/sanitizer_'+source.split('/',1)[1].removesuffix('.cpp').replace('/','_')
        exe=build/stem
        command=['g++','-std=c++17','-Wall','-Wextra','-Wpedantic','-g','-O1','-fsanitize=address','-fno-omit-frame-pointer',str(path),'-o',str(exe)]
        compile_result=subprocess.run(command,capture_output=True)
        files[prefix+'_compiler.txt']=compile_result.stdout+compile_result.stderr
        record={'headers_sha256':dependencies(path),'source':source,'sha256':hashlib.sha256(path.read_bytes()).hexdigest(),'compile_command':command,
                'compile_exit':compile_result.returncode,'timestamp_utc':datetime.datetime.now(datetime.timezone.utc).isoformat(),
                'expected_error_pattern':expected,'runner':'GitHub Actions Ubuntu 24.04','commit':os.environ.get('GITHUB_SHA'),
                'run_url':'https://github.com/'+os.environ.get('GITHUB_REPOSITORY','')+'/actions/runs/'+os.environ.get('GITHUB_RUN_ID','')}
        if compile_result.returncode!=0:
            record['status']='FAIL: compile';failed=True
        else:
            env=os.environ.copy();env['ASAN_OPTIONS']='detect_leaks=1:halt_on_error=1:alloc_dealloc_mismatch=1:new_delete_type_mismatch=1'
            result=subprocess.run([str(exe)],capture_output=True,env=env,timeout=180)
            files[prefix+'_stdout.txt']=result.stdout;files[prefix+'_stderr.txt']=result.stderr
            stderr=result.stderr.decode(errors='replace')
            passed=(result.returncode!=0 and re.search(expected,stderr) is not None) if expected else (result.returncode==0 and not stderr)
            record.update(run_command=[str(exe)],run_exit=result.returncode,ASAN_OPTIONS=env['ASAN_OPTIONS'],status='PASS' if passed else 'FAIL')
            failed=failed or not passed
        files[prefix+'_verification.json']=(json.dumps(record,indent=2)+'\n').encode();records.append(record)
        print(source,record['status'],flush=True)
    files['Reference/sanitizer_summary.json']=(json.dumps(records,indent=2)+'\n').encode()
    # Each file's original bytes are exported losslessly. This also preserves
    # expected negative-test logs even if the overall job fails.
    print('BEGIN_EVIDENCE_BUNDLE',flush=True)
    bundle={path:base64.b64encode(data).decode('ascii') for path,data in files.items()}
    print(base64.b64encode(json.dumps(bundle,separators=(',',':')).encode()).decode('ascii'),flush=True)
    print('END_EVIDENCE_BUNDLE',flush=True)
    if failed: raise SystemExit(1)

if __name__=='__main__':main()
