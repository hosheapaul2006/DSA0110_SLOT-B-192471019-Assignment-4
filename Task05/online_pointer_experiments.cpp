#ifndef CART_HPP
#define CART_HPP
#include <array>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>

class Cart {
    std::array<std::string, 10> items_;
    std::array<double, 10> prices_{};
    std::size_t count_ = 0;
    double discount = 0;
public:
    Cart& add(const std::string& item, double price) {
        if (item.empty() || !std::isfinite(price) || price < 0)
            throw std::invalid_argument("Invalid item or price");
        if (count_ == items_.size()) throw std::length_error("Cart capacity is ten");
        double subtotal = price;
        for (std::size_t i = 0; i < count_; ++i) subtotal += prices_[i];
        if (!std::isfinite(subtotal)) throw std::overflow_error("Cart total overflow");
        items_[count_] = item;
        prices_[count_] = price;
        ++count_;
        return *this;
    }
    Cart& setDiscount(double discount) {
        if (!std::isfinite(discount) || discount < 0 || discount > 100)
            throw std::invalid_argument("Discount must be 0..100 percent");
        this->discount = discount;
        return *this;
    }
    double total() const {
        double amount = 0;
        for (std::size_t i = 0; i < count_; ++i) amount += prices_[i];
        return amount * ((100 - discount) / 100); // multiply by a factor <= 1, avoiding overflow
    }
    std::size_t size() const { return count_; }
    const Cart* identity() const { return this; }
};

inline Cart* highestCart(Cart* const* carts, std::size_t count) {
    if (count != 0 && carts == nullptr) throw std::invalid_argument("Missing pointer array");
    Cart* best = nullptr;
    for (std::size_t i = 0; i < count; ++i)
        if (carts[i] && (!best || carts[i]->total() > best->total())) best = carts[i];
    return best; // empty/all-null -> nullptr; equal totals keep the first cart
}
#endif
#ifndef COURSEWORK_CHECKS_HPP
#define COURSEWORK_CHECKS_HPP
#include <iostream>
#include <stdexcept>
#include <string>
class Tests {
    bool logging;
    unsigned count=0;
public:
    Tests(int argc,char** argv):logging(argc>1 && std::string(argv[1])=="--tests") {}
    void check(bool condition,const char* scenario) {
        ++count;
        if(logging) std::cerr << "CHECK|" << scenario << '|' << (condition?"PASS":"FAIL") << '\n';
        if(!condition) throw std::runtime_error(scenario);
    }
    ~Tests() { if(logging) std::cerr << "TEST_COUNT " << count << '\n'; }
};
#endif
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
