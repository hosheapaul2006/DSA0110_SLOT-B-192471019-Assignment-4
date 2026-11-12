# Executed Cart cases

| ID | Input/scenario | Expected | Actual | Status |
|---|---|---|---|---|
| 1 | Rice 60, Dal 120, 10% discount | Reference identity retained; total 162 | Checked | PASS |
| 2 | Pointer adds Oil 180; reference adds Milk 40 | Same object, totals 324 then 360 | Checked; final 360.00 printed | PASS |
| 3 | Dynamic Fruit 100, 5% discount | 95; new/delete handled exactly once | Checked; 95.00 printed | PASS |
| 4 | Empty/free/full-discount cart | Zero | Checked | PASS |
| 5 | Ten items and eleventh attempt | Ten retained, eleventh rejected | Capacity/state checked | PASS |
| 6 | Negative/NaN price, empty item, discount -1/101/infinity | Reject each and preserve prior state | Six exceptions and unchanged total checked | PASS |
| 7 | Two prices 1e308 | Overflow rejected, one finite item retained | Checked | PASS |
| 8 | Pointer array totals 20,50,50 with null | First 50 wins | Checked | PASS |
| 9 | Empty, all-null, singleton, null nonempty array | null, null, singleton, exception | Four checks passed | PASS |
| 10 | Copy Cart into vector then mutate source | Independent vector value | Checked | PASS |
| 11 | Actual vector reserve growth | Values retained; old pointers invalidated by standard | Capacity/value checked; address integers recorded, no stale dereference | PASS |
| 12 | Vector maximum and moved Cart destination | Highest 50; destination total 50 | Checked | PASS |
| 13 | Supplied TODO program | 324.00 plus newline | Exact comparison matched | PASS |
| 14 | Same supplied program with add returning Cart by value | 240.00 plus newline | Exact comparison matched | PASS |

31 recorded evaluations: main 20, pointer/vector experiments 9, two exact output comparisons. See outputs/*_verification.json for actual compiler/run commands, stdout and stderr logs.
