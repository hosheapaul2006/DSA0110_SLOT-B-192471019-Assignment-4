# Executed fleet tests

| ID | Input/scenario | Expected | Actual | Status |
|---|---|---|---|---|
| 1 | Empty fleet/list/unknown find | Zero entries, empty output, null | Checked | PASS |
| 2 | Create four initial types | Four live owned vehicles | Checked | PASS |
| 3 | Book each for 2 days, return after 3; usage 0/1 litre/2 litres/5 kWh | Totals 350,1150,5450,6350; unavailable while rented, available after return | All 16 per-type checks passed; breakdown printed | PASS |
| 4 | Car and ElectricCar consumption | 28 litres,45 kWh left; capacity 60 kWh | Checked | PASS |
| 5 | Early Car return for 2-day booking | 3000 minimum | Checked | PASS |
| 6 | On-time Car return | No late fee | Checked | PASS |
| 7 | Double booking, actual days 0/-1, bicycle energy | Reject, retain original booking | Four exceptions and state checked | PASS |
| 8 | Return while available; booking 0/-1; unknown rent/return | Reject each | Five exceptions observed | PASS |
| 9 | Energy -1,1000,infinity,NaN | Reject each; retain level and active booking | Four exceptions and preserved state checked | PASS |
| 10 | Null/duplicate owner | Reject; retain four owners, destroy duplicate candidate | Checked | PASS |
| 11 | List after returns | Include levels, battery capacity and service date | Output fields checked | PASS |
| 12 | Fill capacity 8, attempt ninth | Reject, retain eight owned objects | Checked | PASS |
| 13 | End fleet scope | All complete objects released | Vehicle::live equals zero | PASS |
| 14 | Leap year 2000-02-29; invalid 1900-02-29,Feb30,month13 | Accept first; reject invalid dates | Checked | PASS |
| 15 | Empty registration, zero/NaN rate, zero energy capacity, negative/excessive level, zero seats | Reject each | Seven exceptions observed | PASS |
| 16 | Daily rate 1e308, booking 2 days | Overflow; remain available | Checked | PASS |
| 17 | Same rate, active one-day booking returned after 3 days | Overflow; preserve booking | Checked | PASS |
| 18 | Same booking returned on time | Finite 1e308; cleanup complete | Checked | PASS |
| 19 | Four type-code/switch calculations | Same four independently calculated totals | Four comparisons passed; sizes printed | PASS |
| 20 | ElectricBus extension and unchanged existing classes | Late/energy total 17700; no edits to baseline fleet classes | 12 checks passed; total 17700 printed; all three baseline hashes matched | PASS |

Baseline main records 61 check evaluations; switch alternative records 4. Driver assertions execute even in normal demonstration mode; --tests additionally logs labels/counts to stderr. Exact commands/source/dependency hashes and actual output are under outputs/. No timing results are invented: the assignment asks for complexity comparison, and actual sizeof values supply storage observations.

Extension driver: 12 additional check evaluations. Existing vehicle implementations were not edited to add ElectricBus.
