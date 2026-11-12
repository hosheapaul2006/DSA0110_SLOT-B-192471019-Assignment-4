#ifndef PAYROLL_HPP
#define PAYROLL_HPP
#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>

inline double nonnegative(double value) {
    if (!std::isfinite(value) || value < 0)
        throw std::invalid_argument("Amount must be finite and nonnegative");
    return value;
}

class Staff {
    std::string name_;
    int id_;
public:
    Staff(std::string name, int id) : name_(std::move(name)), id_(id) {
        if (name_.empty() || id <= 0) throw std::invalid_argument("Invalid staff identity");
    }
    virtual ~Staff() = default;
    const std::string& name() const { return name_; }
    int id() const { return id_; }
    virtual double pay() const = 0;
};

class SalariedStaff : public Staff {
    double basic_;
    double daRate_;
protected:
    double ordinaryPay() const { return nonnegative(basic_ + basic_ * daRate_); }
public:
    SalariedStaff(std::string name, int id, double basic, double daRate = 0.42)
        : Staff(std::move(name), id), basic_(nonnegative(basic)), daRate_(nonnegative(daRate)) {}
};

class Professor : public SalariedStaff {
    double research_;
public:
    Professor(std::string name, int id, double basic, double research, double daRate = 0.42)
        : SalariedStaff(std::move(name), id, basic, daRate), research_(nonnegative(research)) {}
    double pay() const override { return nonnegative(ordinaryPay() + research_); }
};

class LabAssistant : public SalariedStaff {
public:
    using SalariedStaff::SalariedStaff;
    double pay() const override { return ordinaryPay(); }
};

class GuestFaculty : public Staff {
    double hours_;
    double hourlyRate_;
public:
    GuestFaculty(std::string name, int id, double hours, double rate)
        : Staff(std::move(name), id), hours_(nonnegative(hours)), hourlyRate_(nonnegative(rate)) {}
    double pay() const override { return nonnegative(hours_ * hourlyRate_); }
};
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
