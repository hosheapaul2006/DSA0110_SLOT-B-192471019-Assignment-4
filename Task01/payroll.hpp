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
