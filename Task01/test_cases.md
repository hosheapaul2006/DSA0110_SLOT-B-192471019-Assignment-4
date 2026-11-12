# Executed payroll checks

Commands and actual outputs are in outputs/*_verification.json, *_actual.txt and *_stderr.txt. Tests log to stderr only with --tests; normal demonstrations show no test labels.

| ID | Input/scenario | Expected | Actual | Status |
|---|---|---|---|---|
| 1 | Original Professor basic 50000, research 5000, ignored da=999 | 76000 | 76000 | PASS |
| 2 | Original LabAssistant basic 20000, ignored da=999 | 28400 | 28400 | PASS |
| 3 | Original GuestFaculty 20 hours at 1000 | 20000 | 20000 | PASS |
| 4 | Three derived objects via Staff pointers | 76000, 28400, 20000 | All three values checked and printed | PASS |
| 5 | Zero basic/research; zero guest hours | Zero pay | Both checked | PASS |
| 6 | Basic 100, DA fraction 0.5 | 150 | Checked | PASS |
| 7 | Negative basic, infinite rate, empty name, nonpositive ID | invalid_argument in each case | Four exceptions observed | PASS |
| 8 | Hours and rate both 1e308 | Nonfinite result rejected | Exception observed | PASS |
| 9 | Same three salaries through type-code switch | Same values | Checked | PASS |

The drivers record 13 check evaluations: original exact-output comparison (1), redesigned hierarchy (11), switch alternative (1). Rows group related checks. Actual sizeof values are in switch_alternative_actual.txt and are platform-specific.
