# Banking verification

| ID | Scenario | Expected | Actual | Status |
|---|---|---|---|---|
| 1 | Supplied rate/vrate/sliced-copy source | 3.5 4 3.5 plus newline | Exact prediction matched | PASS |
| 2 | Original deletion through nonvirtual base | Sanitizer detects undefined deletion, or limitation investigated | Locally compiled only; isolated run not completed yet | PENDING |
| 3 | Stack SeniorAccount accessed directly/through Account*/sliced copy | 4, 3.5, 3.5 | Checked, genuine traces retained | PASS |
| 4 | Repaired raw base-pointer array | Ordinary 3.5, senior 4.0 | Both checked and printed | PASS |
| 5 | Repaired manual deletion | No live objects; one senior destructor | Counters checked | PASS |
| 6 | vector<unique_ptr<Account>> | Both dynamic rates correct; automatic complete cleanup | Rates checked; totals four Account destructors/two senior destructors and zero live | PASS |
| 7 | Empty owner collection | No live accounts | Checked | PASS |
| 8 | Attempt to slice repaired base | Compile error from deleted copy constructor | Genuine GCC diagnostic retained | PASS |

Nine local check evaluations (including exact comparison and expected compile rejection); original.cpp is compile-only locally. Sanitizer results are recorded separately when genuinely available. Prediction commit bb0d9aa precedes execution.
