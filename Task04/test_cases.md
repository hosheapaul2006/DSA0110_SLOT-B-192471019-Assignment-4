# Banking verification

| ID | Scenario | Expected | Actual | Status |
|---|---|---|---|---|
| 1 | Supplied rate/vrate/sliced-copy source | 3.5 4 3.5 plus newline | Exact prediction matched | PASS |
| 2 | Original deletion through nonvirtual base | Sanitizer detects undefined deletion, or limitation investigated | Ubuntu ASan exit 1, new-delete-type-mismatch; allocated type 8 bytes/deallocated type 1 byte | PASS |
| 3 | Stack SeniorAccount accessed directly/through Account*/sliced copy | 4, 3.5, 3.5 | Checked, genuine traces retained | PASS |
| 4 | Repaired raw base-pointer array | Ordinary 3.5, senior 4.0 | Both checked and printed | PASS |
| 5 | Repaired manual deletion | No live objects; one senior destructor | Counters checked | PASS |
| 6 | vector<unique_ptr<Account>> | Both dynamic rates correct; automatic complete cleanup | Rates checked; totals four Account destructors/two senior destructors and zero live | PASS |
| 7 | Empty owner collection | No live accounts | Checked | PASS |
| 8 | Attempt to slice repaired base | Compile error from deleted copy constructor | Genuine GCC diagnostic retained | PASS |

Nine local check evaluations (including exact comparison and expected compile rejection); original.cpp is compile-only locally. Actual sanitizer records are saved in outputs/sanitizer_* and Reference/sanitizer_summary.json: repaired/safe cases exit 0 with empty sanitizer stderr, original exits 1 with the expected report. Prediction commit bb0d9aa precedes execution.
