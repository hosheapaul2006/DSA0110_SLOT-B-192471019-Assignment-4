#include "cart.hpp"
#include "../tools/checks.hpp"
#include <cstdint>
#include <iostream>
#include <vector>
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    Cart small, large, equal;
    small.add("A", 20);
    large.add("B", 50);
    equal.add("C", 50);
    Cart* customers[] = {nullptr, &small, &large, &equal};
    tests.check(highestCart(customers, 4) == &large, "maximum and stable first tie");
    tests.check(highestCart(nullptr, 0) == nullptr, "empty pointer range");
    Cart* nulls[] = {nullptr, nullptr};
    tests.check(highestCart(nulls, 2) == nullptr, "all-null pointer range");
    Cart* one[] = {&small};
    tests.check(highestCart(one, 1) == &small, "single pointer");
    bool rejected = false;
    try { (void)highestCart(nullptr, 1); } catch (const std::invalid_argument&) { rejected = true; }
    tests.check(rejected, "missing nonempty pointer array");
    std::vector<Cart> objects;
    objects.reserve(1);
    objects.push_back(small); // copies the Cart; it is not an alias to small
    small.add("Later", 5);
    tests.check(objects[0].total() == 20 && small.total() == 25, "vector copy independent from source");
    const auto previousCapacity = objects.capacity();
    const auto oldAddress = reinterpret_cast<std::uintptr_t>(&objects[0]);
    objects.reserve(previousCapacity + 10); // reallocation invalidates old element pointers/references
    tests.check(objects.capacity() > previousCapacity && objects[0].total() == 20, "reallocation preserves value");
    const auto newAddress = reinterpret_cast<std::uintptr_t>(&objects[0]);
    objects.push_back(large);
    const Cart* best = nullptr;
    for (const Cart& cart : objects) if (!best || cart.total() > best->total()) best = &cart;
    tests.check(best && best->total() == 50, "vector-object maximum");
    Cart moved = std::move(equal);
    tests.check(moved.total() == 50, "default member-wise move preserves destination");
    std::cout << "Highest array total=" << large.total() << " vector total=" << best->total() << '\n'
              << "Element address integers before/after reserve=" << oldAddress << '/' << newAddress << '\n'
              << "sizeof Cart=" << sizeof(Cart) << " pointer=" << sizeof(Cart*) << '\n';
    // No invalidated pointer is dereferenced or compared: only saved integer representations are printed.
}
