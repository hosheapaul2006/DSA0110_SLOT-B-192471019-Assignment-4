#include "cart.hpp"
#include "../tools/checks.hpp"
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
