# Executed university hierarchy checks

| ID | Input/scenario | Expected | Actual | Status |
|---|---|---|---|---|
| 1 | Exact supplied virtual diamond | Person Student Staff TA, final space and newline | Exact character comparison matched | PASS |
| 2 | Remove only both virtual keywords | Person Student Person Staff TA, final space and newline; no warnings | Exact comparison matched; compiler stderr empty | PASS |
| 3 | TA reached through Student and Staff | Same Person address, identity Meena/3 | Both pointer paths equal; identity checked | PASS |
| 4 | TA roles and scholar fellowship | CGPA 9.5, salary 20000; Ravi fellowship 35000 | Values checked | PASS |
| 5 | Student contact and Staff department | 9000000001 and Computing | Checked | PASS |
| 6 | Boundary CGPA 0 and scholar CGPA 10 | Accepted | Both constructed successfully | PASS |
| 7 | CGPA >10 or <0, empty phone, ID 0, negative salary, NaN fellowship | invalid_argument for each | Six exceptions observed | PASS |
| 8 | Composition containing two role objects | Distinct Person addresses; identities can differ | Both checked; sizes printed | PASS |
| 9 | Public/protected/private derivation | Derived code reaches public/protected; private only via base getter | All derived totals 6; three static assertions compiled | PASS |
| 10 | Nonvirtual TA unqualified name member | Compile error: ambiguous | Genuine GCC error retained | PASS |

18 check evaluations across five successfully compiled/running programs and one expected compiler-error source. See outputs/*_verification.json for exact commands and timestamps. Original predictions were committed in a648fec before first execution and remain unchanged.
