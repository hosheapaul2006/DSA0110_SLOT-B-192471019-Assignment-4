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
#include <iomanip>
#include <limits>
#include <memory>
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    Cart direct;
    tests.check(&direct.add("Rice", 60).add("Dal", 120).setDiscount(10) == &direct, "chaining preserves original identity");
    tests.check(direct.total() == 162, "direct access");
    Cart* pointer = &direct;
    pointer->add("Oil", 180);
    tests.check(pointer->total() == 324 && pointer->identity() == &direct, "pointer access and this");
    Cart& reference = direct;
    reference.add("Milk", 40);
    tests.check(reference.total() == 360 && &reference == &direct, "reference aliases original");
    Cart* dynamic = new Cart;
    std::unique_ptr<Cart> guard(dynamic); // exception safety while showing explicit new/delete
    dynamic->add("Fruit", 100).setDiscount(5);
    tests.check(dynamic->total() == 95, "dynamic object access");
    std::cout << std::fixed << std::setprecision(2)
              << "Direct/pointer/reference cart=" << direct.total() << '\n'
              << "Dynamic cart=" << dynamic->total() << '\n';
    delete guard.release(); // releases exactly the object created with new
    dynamic = nullptr;
    tests.check(dynamic == nullptr, "deleted observer cleared");
    Cart empty;
    tests.check(empty.total() == 0, "empty cart");
    tests.check(empty.add("Free", 0).setDiscount(100).total() == 0, "free item and full discount");
    Cart full;
    for (int i = 0; i < 10; ++i) full.add("Item", 1);
    tests.check(full.size() == 10 && full.total() == 10, "ten-item capacity boundary");
    auto rejected = [&](auto action, const char* scenario) {
        bool threw = false;
        try { action(); } catch (const std::exception&) { threw = true; }
        tests.check(threw, scenario);
    };
    rejected([&] { full.add("Eleventh", 1); }, "full cart rejects eleventh");
    tests.check(full.size() == 10 && full.total() == 10, "failed full add leaves state unchanged");
    rejected([&] { direct.add("Bad", -1); }, "negative price");
    rejected([&] { direct.add("Bad", std::numeric_limits<double>::quiet_NaN()); }, "NaN price");
    rejected([&] { direct.add("", 1); }, "empty item");
    rejected([&] { direct.setDiscount(-1); }, "negative discount");
    rejected([&] { direct.setDiscount(101); }, "discount above one hundred");
    rejected([&] { direct.setDiscount(std::numeric_limits<double>::infinity()); }, "infinite discount");
    tests.check(direct.total() == 360, "failed operations preserve existing cart");
    Cart large;
    large.add("Large", 1e308);
    rejected([&] { large.add("Overflow", 1e308); }, "sum overflow rejected");
    tests.check(large.size() == 1 && std::isfinite(large.total()), "overflow rejection preserves finite total");
}
