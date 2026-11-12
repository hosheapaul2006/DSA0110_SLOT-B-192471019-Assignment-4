#include "payroll.hpp"
#include "../tools/checks.hpp"
#include <iomanip>
#include <limits>
#include <memory>
#include <vector>

int main(int argc, char** argv) {
    Tests tests(argc, argv);
    std::vector<std::unique_ptr<Staff>> staff;
    staff.push_back(std::make_unique<Professor>("Kavya", 1, 50000, 5000));
    staff.push_back(std::make_unique<LabAssistant>("Arun", 2, 20000));
    staff.push_back(std::make_unique<GuestFaculty>("Meena", 3, 20, 1000));
    const double expected[] = {76000, 28400, 20000};
    std::cout << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < staff.size(); ++i) {
        tests.check(std::abs(staff[i]->pay() - expected[i]) < 1e-8, "polymorphic pay");
        std::cout << staff[i]->id() << ' ' << staff[i]->name() << ": " << staff[i]->pay() << '\n';
    }
    tests.check(Professor("Zero", 4, 0, 0).pay() == 0, "zero salary");
    tests.check(GuestFaculty("Zero", 5, 0, 100).pay() == 0, "zero hours");
    tests.check(LabAssistant("DA", 6, 100, 0.5).pay() == 150, "configurable DA");
    auto rejected = [&](auto action, const char* scenario) {
        bool threw = false;
        try { action(); } catch (const std::invalid_argument&) { threw = true; }
        tests.check(threw, scenario);
    };
    rejected([] { Professor p("Bad", 1, -1, 0); }, "negative basic rejected");
    rejected([] { GuestFaculty g("Bad", 1, 1, std::numeric_limits<double>::infinity()); }, "infinite rate rejected");
    rejected([] { LabAssistant a("", 1, 1); }, "empty identity rejected");
    rejected([] { LabAssistant a("Bad", 0, 1); }, "nonpositive ID rejected");
    rejected([] { GuestFaculty g("Overflow", 1, 1e308, 1e308); (void)g.pay(); }, "pay overflow rejected");
}
