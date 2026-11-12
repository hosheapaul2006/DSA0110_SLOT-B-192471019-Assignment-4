# Task 1 - Payroll design review

## Diagnosis of the supplied classes

1. Professor and LabAssistant repeat name, id and basic fields.
2. Both repeat `basic + basic * 0.42`; a policy change needs two edits.
3. Every field is public, allowing invalid IDs, negative hours and inconsistent state.
4. GuestFaculty uses nm/code for the same identity represented by name/id elsewhere.
5. The `da` fields are never read: callers can change them without affecting pay.
6. `0.42` is an unexplained magic number rather than a named, configurable DA rate.
7. There is no common interface; a heterogeneous payroll loop cannot call pay uniformly.
8. Adding a category requires another unrelated class and changes to category-specific processing.

`original_design.cpp` preserves the given class definitions inside a compilable wrapper. The demonstration sets the ignored da values to 999 but the given formulas still produce 76000, 28400 and 20000.

## Repair and rationale

Staff owns private identity and declares pure virtual pay plus a virtual destructor. SalariedStaff centralizes basic salary and DA policy; Professor adds research allowance, LabAssistant reuses ordinary pay, and GuestFaculty holds only hours/rate. Derived pay methods use override. `vector<unique_ptr<Staff>>` owns the heterogeneous payroll and processes it through base pointers. Validation rejects negative/nonfinite inputs and nonfinite computed pay. The 42% default is explicitly a supplied demonstration policy, not a current statutory rate. Currency uses double for this teaching example; real payroll requires an explicit decimal/rounding policy.

See [UML](diagrams/hierarchy.md) and [pseudocode](pseudocode.md).

## Extensibility defence (under 100 words)

The common Staff interface lets payroll process every category through pay() without category checks. Shared identity and salaried rules are written once. Private data protects valid state, while each derived class owns its special payment rule. Adding a category requires a new derived class and its construction at the application boundary; the existing classes and payroll loop stay unchanged. A virtual destructor permits complete destruction through Staff pointers, and unique_ptr makes ownership explicit.

## Type-code alternative and complexity

`switch_alternative.cpp` implements the same sample using StaffRecord and records actual sizeof values. One pay call is O(1) for this fixed set of categories, in both designs. Processing n staff is O(n). Per-object fixed storage is O(1), plus O(name length) dynamically owned string storage. A common record reserves research, hours and rate even when irrelevant; the hierarchy specializes fields but usually adds a vptr. sizeof includes implementation-specific padding and string control storage, not heap string characters. A hierarchy is not guaranteed smaller for every type.

Adding a type to the switch design changes three sites: enum, pay switch, application construction. New category-specific fields also change the common record. With the hierarchy, add a derived class and update application construction (two sites); existing pay implementations and the processing loop are untouched. The switch can be simpler for a small closed set; inheritance suits independently extensible behavior.

## Viva practice

- Why a virtual destructor? Deleting a derived object through a base pointer otherwise has undefined behavior.
- Does override make pay virtual? The base virtual declaration supplies polymorphism; override makes a mismatched derived signature a compiler error.
- Practise adding VisitingProfessor, and changing the DA rate without duplicating formulas.
