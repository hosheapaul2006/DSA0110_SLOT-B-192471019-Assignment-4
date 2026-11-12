# Fleet algorithms

The official named SIMATS formatting guide was not supplied or located. These use conventional structured pseudocode, as explicitly allowed by the preparation prompt; official formatting compliance is not claimed.

```text
PROCEDURE RENT(registration, bookedDays)
    vehicle <- FIND registration IN basePointerArray
    IF vehicle is absent THEN REJECT unknown vehicle
    IF bookedDays <= 0 OR vehicle is already rented THEN REJECT
    CHECK bookedDays * dailyRate is representable
    vehicle.bookedDays <- bookedDays
    vehicle.rented <- TRUE
END PROCEDURE

FUNCTION CHARGE(vehicle, actualDays, energyUsed)
    REQUIRE active booking AND actualDays > 0
    REQUIRE finite nonnegative energyUsed
    REQUIRE energyUsed <= stored energy for motor vehicles
    REQUIRE energyUsed = 0 for bicycles
    base <- bookedDays * dailyRate
    lateDays <- MAX(0, actualDays - bookedDays)
    late <- lateDays * dailyRate * 1.5
    energy <- energyUsed * type-specific unitPrice
    CHECK all amounts and total are representable
    RETURN Bill(base, late, energy)
END FUNCTION

PROCEDURE RETURN_VEHICLE(registration, actualDays, energyUsed)
    vehicle <- FIND registration IN basePointerArray
    IF absent THEN REJECT
    bill <- dynamically dispatched vehicle.CHARGE(actualDays, energyUsed)
    // No state changed if validation or calculation fails
    vehicle.CONSUME(energyUsed)
    vehicle.rented <- FALSE
    vehicle.bookedDays <- 0
    RETURN bill
END PROCEDURE

PROCEDURE LIST_VEHICLES
    FOR index <- 0 TO fleetCount - 1
        PRINT common details THROUGH basePointerArray[index]
        dynamically PRINT specialized details
    END FOR
END PROCEDURE
```

Charge/state updates take O(1) arithmetic operations. Searching/listing n vehicles is O(n) assuming bounded registration text; comparing arbitrary registration strings of length L makes a lookup O(nL). Space is O(n) fixed pointer/owner slots plus the actual objects and registration strings.
