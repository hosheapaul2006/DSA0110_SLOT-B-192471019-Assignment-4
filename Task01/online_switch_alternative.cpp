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
#include <iostream>

enum class Kind { Professor, LabAssistant, GuestFaculty };
struct StaffRecord {
    Kind type;
    std::string name;
    int id;
    double basic = 0, daRate = 0.42, research = 0, hours = 0, hourlyRate = 0;
    double pay() const {
        switch (type) {
            case Kind::Professor: return basic + basic * daRate + research;
            case Kind::LabAssistant: return basic + basic * daRate;
            case Kind::GuestFaculty: return hours * hourlyRate;
        }
        throw std::invalid_argument("Unknown kind");
    }
};
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    StaffRecord p{Kind::Professor, "Kavya", 1, 50000, 0.42, 5000};
    StaffRecord l{Kind::LabAssistant, "Arun", 2, 20000};
    StaffRecord g{Kind::GuestFaculty, "Meena", 3, 0, 0.42, 0, 20, 1000};
    tests.check(p.pay() == 76000 && l.pay() == 28400 && g.pay() == 20000, "equivalent switch salaries");
    std::cout << "sizeof StaffRecord=" << sizeof(StaffRecord)
              << " Professor=" << sizeof(Professor) << " LabAssistant=" << sizeof(LabAssistant)
              << " GuestFaculty=" << sizeof(GuestFaculty) << '\n';
}
