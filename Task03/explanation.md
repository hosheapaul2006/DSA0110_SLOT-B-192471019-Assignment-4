# Task 3 - Constructors and member classes

The independent trace model in construction.hpp concentrates on identity, Address and Course. Task 2 retains the complete academic/employment records. Address is a member of Person; Course is a member of Student. Initializer lists forward values directly to their constructors, avoiding default-construct-then-assign. Neither member needs an arbitrary default value.

## Construction/destruction rules

Construction initializes virtual bases first (depth-first, left-to-right traversal), direct nonvirtual bases in base-list order, then members in declaration order, then the constructor body. Destruction executes the derived destructor body, members in reverse declaration order, direct bases in reverse base order, and virtual bases last. Each most-derived object is responsible for its virtual bases. A Student alone uses its own Person initializer; a TA uses TA's Person initializer and ignores the Student/Staff proposals for that virtual base.

The programs record every constructor/destructor message and compare whole event sequences for standalone Student, standalone Staff and TA. Virtual TA contains one Person/Address with the TA identity and Chennai address. In nonvirtual_experiment.cpp the inheritance macro expands to public inheritance, and TA's indirect Person initializer is necessarily removed: an indirect nonvirtual base cannot be initialized by TA. There are then two Persons/Addresses, with separate Student/Staff proposals, and their destructors follow the reverse owning path. Both programs print actual sizeof(TA); this is measured ABI data, not a universal language constant.

## Supplied exercises

predict_output.cpp is unchanged from the supplied Engine/Vehicle/Car program. Its original prediction was committed before execution. The expected sequence is base Vehicle, member Engine, Car body, bar, Car destructor body, Engine destructor, Vehicle destructor. The final space and newline are compared exactly.

complete_code.cpp changes only TODO 1 from the supplied source. Person initializes name/age, followed by regNo and home in their declaration order. constructor_experiments/reordered.cpp intentionally lists home/regNo/Person backwards. Actual -Wreorder diagnostics prove list spelling does not change real initialization order; its output remains identical. Reading an uninitialized dependency would be unsafe, but this experiment has independent initializer expressions and executes safely.

## Embedded Address versus pointer ownership

owning_address.cpp implements a separately allocated AddressValue with the Rule of Three. The constructor allocates, the copy constructor deep-copies, assignment allocates a replacement before releasing old ownership, and the destructor deletes. Self-assignment is safe. The pointer wrapper is smaller under sizeof but also owns a separately allocated AddressValue plus allocator overhead; counting only the pointer is misleading. The embedded model requires no separate Address allocation and compiler-generated copy/destruction manage its string automatically. A unique_ptr alternative makes destruction automatic but needs an explicit deep-copy policy or disabled copying.

Pointer dereference and member access take O(1). Copying city text of length L takes O(L) time/space in either design. Pointer construction adds a separate allocation/deallocation with allocator-dependent overhead; this is not a fixed time guarantee. Embedded Address has no such separate allocation, although string characters can allocate. Neither layout is generally preferable solely because its wrapper sizeof is smaller. No timing experiment was required or fabricated.

Viva practice: reverse the written initializer list and explain unchanged order; introduce a second Course member and predict the reversed destruction sequence before running.
