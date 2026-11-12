# Task 5 - Cart pointers and this

## Four ways and chaining

main.cpp uses a named Cart, a borrowed pointer with ->, a reference alias, and a new Cart explicitly released with delete. A temporary unique_ptr guard protects that last owner if a demonstration operation throws, then release transfers it to the explicit delete. A stack object dies at scope exit; a borrowed pointer/reference does not delete it. A raw dynamic owner must be deleted exactly once. After deletion, clearing one observer does not repair any other dangling aliases. A pointer may be null or reseated; a reference must bind to an object and cannot be reseated, but can still dangle if its object's lifetime ends.

Inside a nonstatic method, this is the pointer to the object receiving the call. *this is that object as an lvalue. Returning Cart& from add/setDiscount retains identity through a chain. `this->discount = discount` distinguishes the member from the parameter with the same name. identity() returns this explicitly. The const overload context makes its result a const Cart*.

## Supplied TODO and by-value experiment

complete_code.cpp preserves every supplied line outside the two TODO regions. add stores item/price if n<10 and returns *this. setDiscount assigns this->discount and returns *this. The supplied valid sample must print 324.00 plus newline. That intentionally minimal exercise ignores an eleventh item; the separately designed Cart in cart.hpp reports capacity/invalid inputs with exceptions and checks overflow. This difference is explicit rather than an unrelated rewrite of the supplied program.

return_by_value.cpp changes only add's return type to Cart. c.add(Rice) first changes c, then returns a separate copy. add(Dal) and setDiscount(10) operate on temporary copies, so c retains only Rice with zero discount. The later pointer add(Oil) changes c; the final output is 240.00. Elision can remove copying but cannot change which object subsequent calls modify. Temporary references from the chain are used only within their full expression; no dangling reference is stored.

## Array of pointers and vector of objects

highestCart indexes an array of borrowed Cart pointers and keeps the first maximum. It skips null entries and returns null for empty/all-null input; a missing nonempty array is rejected. It never takes ownership. Under the fixed ten-item capacity, each total is O(1) and a search over n carts is O(n) with O(1) auxiliary space. With variable k items per cart the scan is O(nk), not O(n) independently of k. The storage for an array of n pointers is n*sizeof(Cart*) plus the separately existing objects.

pointer_experiments.cpp executes a vector<Cart> alternative and copies a source object into it, then verifies independent state. The vector owns its objects, normally stores them contiguously and needs O(n*sizeof(Cart)) fixed storage plus item strings and spare capacity. Growth can move or copy elements in O(n) element operations, each with member-wise string costs. Once reserve/reallocation changes storage, pointers, references and iterators into the old elements are invalid. The experiment prints address integers saved before/after actual reserve without dereferencing any stale pointer. An array of pointers to separately allocated/stack Carts is unaffected by an unrelated vector's reallocation, but its pointed-to lifetimes still require care. Cart's Rule-of-Zero members supply copying/moving; default moves need not make a moved-from Cart empty, so no such claim is made.

sizeof outputs are real platform observations, exclude heap string characters and do not prove a universal space winner. Double prices are pedagogical; the program prints two decimals but does not claim a financial decimal-rounding contract. Viva practice: show how copying into a vector differs from a reference alias; change tie handling to return the last highest cart.
