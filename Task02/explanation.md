# Task 2 - University people hierarchy

Person stores one name/ID and has-a Contact; Student has-a Programme. Student is-a Person and Staff is-a Person: each edge is single public inheritance, together they form hierarchical inheritance. TeachingAssistant is-a Student and a Staff (multiple inheritance). Its combined diamond is a hybrid hierarchy. ResearchScholar is-a Student, yielding the multilevel path ResearchScholar -> Student -> Person. ResearchScholar has a fellowship value. Both role bases virtually inherit Person to share one identity inside a TA. `people.hpp` contains the full declarations; public inheritance permits substitutability while private members and const accessors protect data. The Person virtual destructor supports safe polymorphic ownership.

## Diamond and prediction evidence

The untouched predictions are in prediction_before_execution.md. Both supplied programs print a final space before newline. The original virtual case is expected to construct one Person, Student, Staff, TA. Removing only the two virtual keywords constructs Person/Student then Person/Staff then TA. That exact code never accesses an ambiguous member, so it is valid without a compiler diagnostic; `ambiguity.cpp` separately demonstrates an actual ambiguous name error. Qualifying `ta.Student::name` or `ta.Staff::name` in that simple example selects one identity, but does not merge them.

The most-derived TeachingAssistant initializes Person. Student/Staff initializers of a virtual base are used when those classes are complete objects, and ignored for the virtual base when they are TA subobjects. ResearchScholar must also initialize its virtual Person.

## Composition comparison

`composition_alternative.cpp` contains Student and Staff as TA members. Each member is a complete object with its own Person, even though those two classes use virtual inheritance internally. Access is explicit, such as `ta.student.name()` versus `ta.staff.name()`, avoiding a C++ name lookup ambiguity but permitting contradictory identities. It is a has-a design rather than a substitutable Student/Staff object. A composition design can explicitly share one identity through references, but then adds non-owning lifetime responsibilities; that is a different design from containing two independent objects.

The same program measures sizeof virtual, nonvirtual and composed models with equivalent role fields. Measurements include padding, vptr/vbase bookkeeping and string control objects, but exclude string heap allocations. Virtual inheritance removes duplicate Person state, yet its metadata can offset the saving. No ABI-independent byte count is claimed. Fixed-field access is O(1) in all models; virtual-base access can require an implementation-specific offset lookup. Fixed control storage is O(1), plus O(total text length) for strings. Creating/copying string fields costs O(total text length). See access_table.md for all inheritance/access combinations and diagrams/hierarchy.md for UML.

CGPA uses the demonstration range 0..10, salaries/fellowships are finite nonnegative values, and identities/phone/programme/department are required. These constraints are explicit application assumptions. Viva practice: explain one Person address reached along two paths; add a scholarship member without duplicating identity.
