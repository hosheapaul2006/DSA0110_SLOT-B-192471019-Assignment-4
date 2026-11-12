# Payroll pseudocode

Conventional structured pseudocode is used. The named official SIMATS formatting guide was not supplied or located; official-format compliance is not claimed.

```text
BEGIN PROCESS_PAYROLL
    CREATE Professor, LabAssistant, GuestFaculty
    STORE each owner in a collection of pointers to Staff
    FOR EACH staff IN collection
        amount <- staff.pay()       // dynamically selects the derived implementation
        PRINT staff.id, staff.name, amount
    END FOR
    DESTROY owners                  // virtual destructor releases the complete objects
END

FUNCTION SALARIED_PAY(basic, daRate)
    REQUIRE finite, nonnegative arguments
    RETURN basic + basic * daRate
END FUNCTION
```
