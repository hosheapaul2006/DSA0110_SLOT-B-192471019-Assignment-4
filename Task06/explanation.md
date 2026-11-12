# Task 6 - Vehicle rental fleet

## Stages 1-4: interpretation, hierarchy and decisions

The requirements, selected business policies, expected calculations and interfaces were recorded in design_before_implementation.md before code (commit b2b2680). The complete C++ declarations are in fleet.hpp. See diagrams/hierarchy.md for UML and pseudocode.md for all required algorithms. This is task-specific supporting evidence, not a consolidated assignment report.

Vehicle centralizes private registration, daily rate and rental state. Its pure virtual charge/kind functions make it abstract, and its virtual destructor supports ownership through Vehicle*. Bicycle specializes a daily-only calculation. MotorVehicle shares a calendar-valid informational service date. FuelVehicle owns a litre-based tank and price per litre; ElectricVehicle owns a kWh-based battery and price per kWh. TwoWheeler/Car and ElectricCar implement their respective charge overrides using those helpers. Car and ElectricCar compose Seating. Both fuel types use the same user-approved daily-plus-litre formula with different daily rates; no arbitrary seating surcharge is invented merely to make formulas different. The electric branch has no fuel-litre API, and ElectricCar does not inherit from the fuel-powered Car class.

Fleet contains the explicitly requested raw array of base-class pointers, Vehicle* vehicles_[8]. A parallel array of unique_ptr owners manages destruction; views stay valid until the fleet is destroyed. add rejects null, duplicate registration and overflow before committing an owner. Failed candidate additions destroy the candidate safely. Fleet copying/moving is disabled because borrowed-view transfer needs a separate policy. Dynamic allocation is per vehicle; fixed owner/view slots themselves allocate no heap storage.

## Stage 5: behavior and validation

Sample rates and units are explicitly approved in Reference/approved_design.md. rent accepts positive whole-day bookings. Returns charge the booked minimum even if early; extra days cost 150% of the daily rate each. The return parameter is energy units used during this rental: litres for fuel vehicles or kWh for electric vehicles, never a currency amount. This simple model assumes consumption cannot exceed stored energy and does not model mid-rental refueling/recharging. Consumption updates the stored level only after billing/validation succeeds. Service dates are displayed, not enforced as a scheduling rule; energy level is recorded, not a roadworthiness certification. Extending those policies would be separate work.

For booked=2 and actual=3, base+late equals 3.5*dailyRate. The independent pre-code totals are 350 for Bicycle; 1150 for TwoWheeler using 1 litre; 5450 for Car using 2 litres; 6350 for ElectricCar using 5 kWh. main.cpp verifies all four via base pointers, early/on-time return, double booking and invalid operations. State updates follow successful validation, so an invalid return preserves both booking and stored energy. Overflow checks use long-double intermediates and the representable double range. Display uses two decimals; no production decimal money/rounding guarantee is claimed.

The owner counter checks complete lifetime cleanup, while isolated sanitizer runs independently check dynamic storage. Exact tests and actual results are recorded in test_cases.md and outputs/. The later ElectricBus extension will be separately committed after the baseline and validated against baseline source hashes.

## Type-code alternative and complexity

switch_alternative.cpp executes a single record/switch model with the same four expected charge totals. Its record reserves fuel, battery, seats, service and battery-capacity fields even for a bicycle. The hierarchy stores specialized fields but normally has a vptr per polymorphic subobject and alignment padding. Actual sizeof observations are printed; they exclude registration string heap storage and allocator overhead. The hierarchy is not guaranteed smaller for every type. A switch can be easier for a small closed type set; the hierarchy supports an independently added subtype without editing a central switch.

Both designs perform O(1) arithmetic per charge for a fixed number of categories. If a switch grows to t linear branches, its generic worst-case comparison count can be O(t); compiler jump tables and optimization depend on the program. No unsupported timing advantage is claimed. rent/return state updates and energy calculations are O(1). Fleet lookup scans n registrations: O(n) for bounded-size IDs, or O(nL) including up to L character comparisons. Listing is O(n + output length). Owners and views occupy O(n) slots plus actual objects and O(total registration text) dynamic string storage. The fixed sample has n<=8.

Adding ElectricBus to the hierarchy requires a new derived implementation and construction in the extension driver; existing vehicle classes and Fleet stay unchanged. For the switch design, enum, charge switch and creation code must change (three sites), with common record changes if genuinely new fields are needed. Shared billing helper changes would be intentional policy changes, not a requirement for every new category.

## Viva practice

- Explain why unique_ptr ownership and raw processing pointers serve different purposes here.
- Explain why ElectricCar is placed beside the fuel branch rather than inheriting petrol behavior.
- Practise adding an electric van and changing the late-rate policy centrally.
