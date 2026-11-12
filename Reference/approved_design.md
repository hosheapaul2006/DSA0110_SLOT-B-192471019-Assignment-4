# Confirmed design choices

The user confirmed all three recommendations before implementation.

- Sample daily rates (rupees): Bicycle 100, TwoWheeler 300, Car 1500, ElectricCar 1800, ElectricBus 5000. Fuel 100/litre; electricity 10/kWh. These are demonstrations, not externally sourced commercial rates.
- Book positive whole days; charge at least booked days; each extra day costs 150% of the daily rate. Add nonnegative fuel/electric energy charges where applicable. Service due dates are informational. Reject invalid operations and double booking.
- Separate fuel and electric branches. Use an array of Vehicle* with separate unique_ptr ownership. Add ElectricBus without editing existing vehicle classes.
- No report of any format or Report folder. Preserve original supplied PDF only as input. Explanations stay inside task folders.
- Classroom submission is performed by the user. Submission-date field in PDF is blank.
- Official SIMATS pseudocode formatting guide was not supplied or located locally or through exact-title/official-domain searches. Use conventional structured pseudocode under the prompt's explicit fallback; do not claim official-format compliance.
- Sanitizers use Ubuntu GitHub Actions: local GCC 14.2.0 lacks ASan/LSan runtime libraries.
