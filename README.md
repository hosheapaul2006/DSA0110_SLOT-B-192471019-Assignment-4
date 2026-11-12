# DSA0110 - Assignment 4 (CO4)

**Student:** D Hoshea Paul  
**Register number:** 192471019  
**Course:** Object Oriented Programming with C++  
**Slot:** B  
**Topic:** Designing reusable class hierarchies  
**Language:** C++17

[Existing repository](https://github.com/hosheapaul2006/DSA0110_SLOT-B-192471019-Assignment-4) | [Requirement checklist](ASSIGNMENT_CHECKLIST.md) | [Original supplied brief](Reference/Assignment%204.pdf)

The current instruction explicitly excludes an assignment report. No report, cover page, DOCX, consolidated report Markdown or Report folder was created. Required explanations, UML, pseudocode, predictions and evidence are stored in their individual task folders. The PDF in Reference is the unchanged input specification, not a generated report.

The PDF requests `DSA01-192471019-Assignment4`; the existing repository name is preserved as instructed. Confirm faculty accepts that naming discrepancy. Its submission-date field is blank; no deadline or submission date is invented. The official SIMATS Engineering Standard for Algorithmic Pseudocode was not supplied or located; pseudocode uses the prompt's permitted conventional structured fallback and does not claim official-format compliance.

## Verified outcomes and navigation

| Task | Result and evidence |
|---|---|
| [1 - Payroll](Task01/explanation.md) | Shared abstract interface; payments 76000/28400/20000; original design and switch comparison; [UML SVG](Task01/diagrams/hierarchy.svg), [editable Mermaid](Task01/diagrams/hierarchy.md), [tests](Task01/test_cases.md) |
| [2 - University people](Task02/explanation.md) | One Person in virtual diamond; two in nonvirtual/composed designs; actual ambiguity diagnostic; [access table](Task02/access_table.md), [UML SVG](Task02/diagrams/hierarchy.svg), [tests](Task02/test_cases.md) |
| [3 - Construction](Task03/explanation.md) | Exact lifetime traces and prediction; TODO prints `192311001 Kavya (19) Salem-636001`; real reorder warning; deep-owned Address comparison; [tests](Task03/test_cases.md) |
| [4 - Banking](Task04/explanation.md) | Repaired dynamic 3.5/4.0 rates and complete destruction; original ASan bad-delete diagnosis; prediction `3.5 4 3.5`; [tests](Task04/test_cases.md) |
| [5 - Cart](Task05/explanation.md) | Four access methods, pointer search, vector comparison; exact TODO `324.00`; by-value alternative `240.00`; [tests](Task05/test_cases.md) |
| [6 - Fleet](Task06/explanation.md) | Abstract hierarchy; fuel/kWh units; late/availability validation; ElectricBus total 17700 without editing existing classes; [pre-code design](Task06/design_before_implementation.md), [UML SVG](Task06/diagrams/hierarchy.svg), [pseudocode](Task06/pseudocode.md), [tests](Task06/test_cases.md) |

## Environments and genuine evidence

- Local: Windows, GCC 14.2.0 (WinLibs MinGW-w64 x86_64 UCRT POSIX SEH). Exact [compiler version](Reference/compiler_version.txt) and [platform](Reference/platform.txt) are retained.
- Sanitizers: genuine GitHub-hosted Ubuntu 24.04, GCC 13.3.0; [environment](Reference/sanitizer_environment.txt), [14-case results](Reference/sanitizer_summary.json), [successful workflow](https://github.com/hosheapaul2006/DSA0110_SLOT-B-192471019-Assignment-4/actions/runs/43228633828). Local GCC lacks ASan/LSan runtime libraries, so Linux evidence is explicitly separate from Windows execution.
- Online: [Programiz C++ compiler](https://www.programiz.com/cpp-programming/online-compiler/), 24 actual safe-program executions and 39 unchanged JPEG screenshots in Task01..Task06/screenshots/. Each outputs/*_programiz.json identifies the exact submitted source/hash, captured result text and screenshots.

There are 27 independent base source files: 25 compile successfully (24 safe local executions and one unsafe sanitizer-only driver), and two deliberately fail compilation. Fifteen additional mechanically flattened browser builds also compile/run, giving 40 successfully compiled source files and 39 normal local build/run records. Those records contain 325 runtime check evaluations plus two expected compiler-rejection checks (327 total recorded evaluations). Counts include repeated checks in derived browser builds, not 327 distinct input scenarios. Sanitizer observations and the 24 browser comparisons are counted separately.

All 13 safe sanitizer cases exited zero with empty sanitizer stderr. The original bad-delete driver exited 1 with `new-delete-type-mismatch`. Its original history allocation is 100 doubles, but the workflow stops at invalid deletion; no invented leak-byte total or claim that UB is safe is made. PASS for an intentional negative example means its expected diagnostic was observed, not that the example is correct.

Programiz fixed stdout is compared exactly after CRLF decoding, allowing only trailing UI whitespace/success banner. Actual sizeof values and vector address integers are platform-specific observations: their labels/structure and positive numeric values are checked separately, without asserting equal addresses or a universal ABI size. Output assertions in the C++ drivers still run online. Genuine captured text is saved unchanged.

## Compile and run

Each .cpp below is an independent program with its own main. Compile one driver at a time; do not link all .cpp files together. Headers stay beside the source files. All assignment implementations are C++17; Python files only orchestrate builds, auditing and SVG rendering.

From the repository root with g++ on PATH:

```powershell
New-Item -ItemType Directory -Force build | Out-Null
g++ --version
g++ -std=c++17 -Wall -Wextra -Wpedantic -O0 Task01/redesigned.cpp -o build/payroll.exe
.\build\payroll.exe
.\build\payroll.exe --tests
```

Normal execution prints coursework results. --tests adds actual check labels/counts to stderr. Exact commands, source/dependency hashes, UTC execution times, exit codes, compiler logs and stdout/stderr are in every task's outputs/*_verification.json and accompanying files. Tests assert normal, boundary and relevant invalid behavior; failed-state preservation is explicitly checked in Cart and Fleet.

Use the same command pattern, substituting each source below and a distinct executable name:

| Folder | Independent drivers | Purpose/extra option |
|---|---|---|
| Task01 | original_design.cpp; redesigned.cpp; switch_alternative.cpp | Supplied design wrapper; hierarchy; switch/sizeof |
| Task02 | main.cpp; predict_output.cpp; diamond_experiment.cpp; composition_alternative.cpp; access_experiment.cpp | University model; both immutable predictions; actual sizes; access rules |
| Task03 | main.cpp; nonvirtual_experiment.cpp; predict_output.cpp; complete_code.cpp; owning_address.cpp | Both trace models; exact prediction; TODO; pointer ownership |
| Task03 | constructor_experiments/reordered.cpp | Add `-Wreorder`; warning is intentional, executable is safe |
| Task04 | predict_output.cpp; safe_slicing.cpp; repaired.cpp | Exact binding prediction; safe stack slicing; raw/smart cleanup |
| Task05 | main.cpp; complete_code.cpp; pointer_experiments.cpp; return_by_value.cpp | Validated Cart; untouched-TODO context; pointer/vector search; chaining copy experiment |
| Task06 | main.cpp; switch_alternative.cpp; extension_demo.cpp | Four-type fleet, equivalent switch results, ElectricBus extension |

The two expected compile failures are Task02/ambiguity.cpp (`ambiguous`) and Task04/slicing_rejected.cpp (`deleted` copy constructor). Their genuine diagnostics are under outputs/. Task04/original.cpp is intentionally undefined: compile only locally; run it exclusively through the isolated sanitizer workflow below. The original supplied prediction in Task04 is a separate safe program and already has a virtual destructor.

Browser-ready online_*.cpp files contain the mechanically expanded local headers and test helper, with the same implementation/driver. They were compiled locally before their exact bytes were pasted into Programiz. [Manifest](Reference/programiz_manifest.json) maps all 24 suitable base programs to submitted files, expected local stdout and online evidence. No unsafe driver or expected compiler-error source was run online.

## Reproduce verification

```powershell
$env:CXX = 'C:\Projects\SLOT B\.tools\mingw64\bin\g++.exe' # or your installed compiler
python tools/run_all.py
python tools/audit.py
```

run_all rebuilds/runs safe programs, records expected compiler diagnostics and compiles the unsafe driver without executing it. It also regenerates/compiles the self-contained browser variants. Re-running changes timestamps, address observations and output records; it may dirty the tree. Never rewrite the original prediction files. A browser rerun is a separate actual UI action, not performed by the local script.

On an isolated Linux environment, `python3 tools/sanitizers.py` reproduces the 14 cases using:

```text
g++ -std=c++17 -Wall -Wextra -Wpedantic -g -O1 -fsanitize=address -fno-omit-frame-pointer SOURCE -o EXECUTABLE
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1:alloc_dealloc_mismatch=1:new_delete_type_mismatch=1
```

The workflow fetches its exact public GITHUB_SHA with read-only repository permission, runs the script and exports original evidence bytes losslessly in its log. Correct sources require exit 0 and empty sanitizer stderr; the one negative requires nonzero exit and the expected ASan report. Raw evidence is retained under task outputs/ with source/dependency hashes, flags, runner, commit and workflow URL. No fabricated execution date or sanitizer report is used.

## Predictions and extension chronology

| Record committed before execution/implementation | Commit |
|---|---|
| Requirement checklist before code | 08f36a7 |
| Task 2 virtual and nonvirtual exact predictions | a648fec |
| Task 3 Engine/Vehicle/Car prediction | a5865f7 |
| Task 4 rate/vrate/slicing prediction | bb0d9aa |
| Task 6 design and independent expected charges | b2b2680 |
| Four-type fleet baseline before ElectricBus | 368d01c |
| ElectricBus new files and unchanged-baseline hash proof | 1792b3d |

Original predictions are unchanged. Per-task observation notes explain their actual matches. [Extension preservation evidence](Task06/outputs/extension_preservation.json) verifies fleet.hpp and both baseline drivers remained byte-identical when ElectricBus was added. At least one meaningful implementation commit exists for each of the six tasks; no dates were backfilled.

## Structure and remaining student actions

```text
Reference/       Original input PDF/source extracts, approved policies and environment/audit
Task01..Task06/  Independent C++ sources, task-specific explanations and test tables
  diagrams/     UML SVG plus editable Mermaid where required
  outputs/      Immutable predictions, actual results, warnings, ASan and verification JSON
  screenshots/  Genuine Programiz source/output JPEGs
tools/          C++ test helper; build, audit, sanitizer and UML orchestration
.github/        Isolated Ubuntu sanitizer workflow
```

Review and personally execute the tasks as the PDF requires, practise the task-specific viva exercises, and confirm faculty accepts the existing repository name. The named official pseudocode guide can be applied if faculty supplies it; its absence is disclosed rather than fabricated. Submit this repository URL yourself to the correct DSA0110 Slot B Assignment 4 (CO4) Classroom destination. No assignment report was generated or prepared as an attachment, and no Classroom submission has been performed by the assistant.
