# Executed constructor checks

| ID | Scenario | Expected | Actual | Status |
|---|---|---|---|---|
| 1 | Supplied Vehicle/Engine/Car program | Vehicle Engine Car, bar, reverse destructor bodies; exact final space/newline | Prediction matched character for character | PASS |
| 2 | Standalone Student | Address, Person, Course, Student; reverse destruction | Whole event sequence checked in both builds | PASS |
| 3 | Standalone Staff | Address, Person, Staff; reverse destruction | Whole event sequence checked in both builds | PASS |
| 4 | Virtual TA | One shared Person; TA identity/Chennai address; reverse trace | Three checks passed; sizeof 136 bytes | PASS |
| 5 | Nonvirtual TA | Two Persons with Student/Staff proposals; reverse owning trace | Three checks passed; sizeof 208 bytes | PASS |
| 6 | Invalid pin 99999 | Constructor rejects before logging complete Address | Exception and empty event sequence checked in both builds | PASS |
| 7 | Empty Course title | Completed Person/Address unwind; no Student destructor | Exception and exact unwinding trace checked in both builds | PASS |
| 8 | Supplied TODO constructor | 192311001 Kavya (19) Salem-636001 plus newline | Exact output matched | PASS |
| 9 | Reordered initializer list | -Wreorder warning, unchanged valid output | Actual GCC warning retained and output matched | PASS |
| 10 | Owning Address copy/assignment/self-assignment | Independent addresses, independent city strings, self preserved | Four checks passed | PASS |

Six successfully compiled/executed source files, 21 recorded check evaluations. Prediction commit: a5865f7. Actual sizes are Windows GCC 14.2.0 observations, not guaranteed on another ABI. Commands, compiler messages, stdout/stderr and source hashes are under outputs/. The whole-program trace tests include all custom constructors/destructors that execute.
