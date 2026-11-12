# Person member access under inheritance

| Inheritance used by Student | Person public member in Student | Person protected member in Student | Person private member in Student | External code through Student |
|---|---|---|---|---|
| public | public; usable inside Student | protected; usable inside Student | No direct access | Only inherited public members |
| protected | protected; usable inside Student | protected; usable inside Student | No direct access | None of these inherited members |
| private | private; usable inside Student | private; usable inside Student | No direct access | None of these inherited members |

Person's private members still exist in its subobject; inheritance does not remove them or make them directly accessible. Person's own public getter may read them. External access additionally depends on the inheritance path and any public forwarding function supplied by Student. Friends have their own explicit access privileges. Further-derived classes can use inherited protected members through public/protected paths; private inheritance blocks their direct use.

`access_experiment.cpp` executes all three derived contexts and checks external base-pointer convertibility at compile time. The production hierarchy uses public inheritance because a student/staff/scholar really is a Person; restricted inheritance would prevent ordinary external upcasting.
