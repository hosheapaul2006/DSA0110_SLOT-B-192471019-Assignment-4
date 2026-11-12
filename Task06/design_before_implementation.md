# Fleet design recorded before implementation

## Stage 1 - Interpret as testable requirements

- Vehicle identity is a nonempty, fleet-unique registration; every daily rate is finite and positive.
- Bicycle, TwoWheeler, Car and ElectricCar can each be rented and returned through a Vehicle pointer.
- A booking is for positive whole days and changes availability only after validation. Booking an already rented vehicle fails without changing it.
- Return requires an active booking and positive actual days. Returning early retains the booked minimum. Each extra day costs 1.5 times the daily rate. Invalid returns leave the booking and energy unchanged.
- Bicycle has no energy charge. Fuel vehicles meter litres used; electric vehicles meter kWh used. The return operation accepts nonnegative energy units used during that rental, not a percentage or a price. Units cannot exceed the stored energy; valid consumption reduces the stored level.
- Sample daily rates are 100/300/1500/1800 rupees for bicycle/two-wheeler/car/electric car. Fuel costs 100 per litre and electricity 10 per kWh. ElectricBus will use 5000 per day. These are user-approved sample policies, not external market prices.
- Motor vehicles contain an actual calendar-valid service date, stored for information without scheduling logic. Cars have seats; electric vehicles have battery capacity/current kWh. A displayed charge percentage is derived from those quantities.
- List every vehicle, its type, rate, availability and specialized data. Reject unknown registrations, duplicate registrations, null owners, fleet overflow and invalid physical values.
- All storage is released through virtual destruction. ElectricBus must be added by new files, without edits to existing vehicle implementation files; compare source hashes before/after the extension.

## Stage 2 - Proposed relationships

Vehicle is abstract. Bicycle is-a Vehicle. MotorVehicle is-a Vehicle and has-a ServiceDate. FuelVehicle and ElectricVehicle are separate abstract branches of MotorVehicle. TwoWheeler/Car derive from FuelVehicle; ElectricCar and future ElectricBus derive from ElectricVehicle. Car and the electric passenger vehicles have-a Seating value. FuelVehicle has-a tank EnergyStore in litres; ElectricVehicle has-a battery EnergyStore in kWh. ElectricCar never inherits petrol/diesel fuel behavior.

Public single inheritance supports substitutability; the complete tree is hierarchical/multilevel. Composition represents service date, seating and stored energy. See diagrams/hierarchy.md.

## Stage 3 - Critical decisions

| Decision | Reason and consequence |
|---|---|
| Private registration/rate/booking in Vehicle | All derived types obey one rental-state contract |
| Nonvirtual rent/return entry points | Enforce validation and commit state only after charge succeeds |
| Pure virtual charge and kind, virtual destructor/details | Specialize billing and display; permit safe polymorphic ownership |
| Common base billing helper | Booked minimum and late policy are not copied into each subtype |
| Separate fuel/electric helpers | Same stored-energy mechanics, explicitly different physical units/prices |
| Fixed array Vehicle* for processing | Meets the PDF; contains borrowed pointers, not deleting owners |
| Parallel fixed array unique_ptr<Vehicle> owners | Deterministic ownership and cleanup; no dangling pointers from vector growth |
| Fleet capacity 8; no copying/moving | Prevent duplicate registrations/overflow and accidental transfer of borrowed views |
| Double with finite/overflow checks and 2-decimal display | Small teaching application; no claim of a production decimal accounting policy |

## Stage 4 - Interfaces planned

```text
Vehicle(registration, dailyRate)
    rent(bookedDays)
    returnRental(actualDays, energyUnitsUsed) -> Bill
    charge(actualDays, energyUnitsUsed) -> Bill   // pure virtual, active booking required
    kind() -> text                              // pure virtual
    list(outputStream)
    virtual destructor
MotorVehicle(..., serviceDate)
FuelVehicle(..., tankCapacityLitres, initialLitres, pricePerLitre)
ElectricVehicle(..., batteryCapacityKWh, initialKWh, pricePerKWh)
Fleet.add(unique_ptr<Vehicle>)
Fleet.find(registration) -> borrowed Vehicle* or null
Fleet.rent(registration, bookedDays)
Fleet.returnRental(registration, actualDays, energyUnitsUsed)
Fleet.list(outputStream)
```

## Planned independent checks

For a two-day booking returned after three days, base+late is 3.5*dailyRate. With usage 1 litre for TwoWheeler, 2 litres for Car and 5 kWh for ElectricCar, the expected totals are Bicycle 350, TwoWheeler 1150, Car 5450 and ElectricCar 6350. A two-day ElectricBus returned after three days using 20 kWh should total 17700. Early return of a two-day Car with zero consumption should still cost 3000. These are calculations from the selected policy, recorded before implementation/execution.

Validate date leap-year boundaries, invalid physical fields, empty/full fleets, unknown IDs, duplicate/null owners, zero/negative bookings/returns/energy, excessive energy, double booking, return while available, early/on-time/late returns, overflow rejection and re-renting after return. Prove failed operations preserve state and all owners are destroyed.
