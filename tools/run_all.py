"""Reproduce safe C++17 executions and compile-only intentional diagnostics.
The assignment implementations remain exclusively C++; this file orchestrates evidence.
"""
import json
import re
from pathlib import Path
from verify import ROOT, verify, flatten

ERRORS = {'Task02/ambiguity.cpp': 'ambiguous', 'Task04/slicing_rejected.cpp': 'deleted'}
UNSAFE = {'Task04/original.cpp'}
EXPECTED = {
    'Task01/original_design.cpp': 'outputs/original_expected.txt',
    'Task02/predict_output.cpp': 'outputs/predict_output_predicted.txt',
    'Task02/diamond_experiment.cpp': 'outputs/diamond_experiment_predicted.txt',
    'Task03/predict_output.cpp': 'outputs/predict_output_predicted.txt',
    'Task03/complete_code.cpp': 'outputs/complete_expected.txt',
    'Task03/constructor_experiments/reordered.cpp': 'outputs/complete_expected.txt',
    'Task04/predict_output.cpp': 'outputs/predict_output_predicted.txt',
    'Task05/complete_code.cpp': 'outputs/complete_expected.txt',
    'Task05/return_by_value.cpp': 'outputs/by_value_expected.txt',
}

def main():
    manifest = []
    sources = sorted(p for p in ROOT.glob('Task*/**/*.cpp') if not p.name.startswith('online_'))
    for path in sources:
        relative = path.relative_to(ROOT).as_posix()
        task, source = relative.split('/', 1)
        if relative in ERRORS:
            verify(task, source, diagnostic='error', needle=ERRORS[relative])
            continue
        if relative in UNSAFE:
            verify(task, source, compile_only=True)
            continue
        browser_source = source
        if re.search(r'^\s*#include\s+"', path.read_text(encoding='utf-8'), re.M):
            browser_source = 'online_' + source.replace('/', '_')
            flatten(task, source, browser_source)
        test_driver = 'Tests tests(' in (ROOT/task/browser_source).read_text(encoding='utf-8')
        if 'constructor_experiments/reordered.cpp' == source:
            verify(task, source, expected=EXPECTED[relative], diagnostic='warning', needle='reorder', extra=['-Wreorder'])
        else:
            verify(task, source, expected=EXPECTED.get(relative), tests=test_driver)
        if browser_source != source:
            verify(task, browser_source, tests=test_driver)
        manifest.append({'base_source': relative, 'submitted_source': task+'/'+browser_source,
                         'local_stdout': task+'/outputs/'+browser_source.removesuffix('.cpp').replace('/', '_')+'_actual.txt',
                         'output_stem': source.removesuffix('.cpp').replace('/', '_'),
                         'comparison': 'Exact fixed stdout; positive sizeof measurements and address integers are platform-specific'})
    (ROOT/'Reference/programiz_manifest.json').write_text(json.dumps(manifest, indent=2)+'\n', encoding='utf-8', newline='\n')
    print('Programiz-suitable safe programs:', len(manifest))

if __name__ == '__main__':
    main()
