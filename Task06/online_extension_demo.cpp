#ifndef ELECTRIC_BUS_HPP
#define ELECTRIC_BUS_HPP
#ifndef FLEET_HPP
#define FLEET_HPP
#include <array>
#include <cmath>
#include <iomanip>
#include <limits>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

inline double positive(double value) {
    if (!std::isfinite(value) || value <= 0) throw std::invalid_argument("Expected finite positive value");
    return value;
}
inline double money(long double amount) {
    if (!std::isfinite(amount) || amount < 0 || amount > std::numeric_limits<double>::max())
        throw std::overflow_error("Charge outside representable range");
    return static_cast<double>(amount);
}
struct Bill {
    double base, late, energy;
    double total() const { return money(static_cast<long double>(base) + late + energy); }
};

class ServiceDate {
    int year_, month_, day_;
public:
    ServiceDate(int year, int month, int day) : year_(year), month_(month), day_(day) {
        const bool leap = year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
        const int days[] = {31, leap ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        if (year < 1000 || year > 9999 || month < 1 || month > 12 || day < 1 || day > days[month - 1])
            throw std::invalid_argument("Invalid service date");
    }
    std::string text() const {
        std::ostringstream out;
        out << year_ << '-' << std::setfill('0') << std::setw(2) << month_ << '-' << std::setw(2) << day_;
        return out.str();
    }
};
class EnergyStore {
    double capacity_, level_;
public:
    EnergyStore(double capacity, double level) : capacity_(positive(capacity)), level_(level) {
        if (!std::isfinite(level) || level < 0 || level > capacity) throw std::invalid_argument("Invalid energy level");
    }
    void validate(double units) const {
        if (!std::isfinite(units) || units < 0 || units > level_) throw std::invalid_argument("Invalid consumed energy");
    }
    void consume(double units) { validate(units); level_ -= units; }
    double level() const { return level_; }
    double capacity() const { return capacity_; }
};
class Seating {
    int seats_;
public:
    explicit Seating(int seats) : seats_(seats) {
        if (seats <= 0) throw std::invalid_argument("Seating capacity must be positive");
    }
    int seats() const { return seats_; }
};

class Vehicle {
    std::string registration_;
    double dailyRate_;
    bool rented_ = false;
    int bookedDays_ = 0;
protected:
    Bill makeBill(int actualDays, double energyCharge) const {
        if (!rented_) throw std::logic_error("Vehicle has no active rental");
        if (actualDays <= 0) throw std::invalid_argument("Actual days must be positive");
        const int lateDays = actualDays > bookedDays_ ? actualDays - bookedDays_ : 0;
        Bill result{money(static_cast<long double>(bookedDays_) * dailyRate_),
                    money(static_cast<long double>(lateDays) * dailyRate_ * 1.5L), energyCharge};
        (void)result.total();
        return result;
    }
    virtual void consume(double units) {
        if (units != 0) throw std::invalid_argument("Bicycle has no energy consumption");
    }
    virtual void details(std::ostream&) const {}
public:
    inline static int live = 0;
    Vehicle(std::string registration, double dailyRate)
        : registration_(std::move(registration)), dailyRate_(positive(dailyRate)) {
        if (registration_.empty()) throw std::invalid_argument("Registration is required");
        ++live;
    }
    Vehicle(const Vehicle&) = delete;
    Vehicle& operator=(const Vehicle&) = delete;
    virtual ~Vehicle() { --live; }
    const std::string& registration() const { return registration_; }
    double dailyRate() const { return dailyRate_; }
    bool available() const { return !rented_; }
    int bookedDays() const { return bookedDays_; }
    void rent(int days) {
        if (rented_) throw std::logic_error("Vehicle is already rented");
        if (days <= 0) throw std::invalid_argument("Booked days must be positive");
        (void)money(static_cast<long double>(days) * dailyRate_);
        bookedDays_ = days;
        rented_ = true;
    }
    Bill returnRental(int actualDays, double energyUnits = 0) {
        Bill bill = charge(actualDays, energyUnits); // validate/calculations before mutation
        consume(energyUnits);
        rented_ = false;
        bookedDays_ = 0;
        return bill;
    }
    virtual Bill charge(int actualDays, double energyUnits) const = 0;
    virtual const char* kind() const = 0;
    void list(std::ostream& out) const {
        out << registration_ << ' ' << kind() << " daily=" << dailyRate_
            << " state=" << (available() ? "available" : "rented");
        details(out);
        out << '\n';
    }
};
class Bicycle : public Vehicle {
public:
    using Vehicle::Vehicle;
    const char* kind() const override { return "Bicycle"; }
    Bill charge(int days, double units) const override {
        if (units != 0) throw std::invalid_argument("Bicycle energy must be zero");
        return makeBill(days, 0);
    }
};
class MotorVehicle : public Vehicle {
    ServiceDate service_;
protected:
    void details(std::ostream& out) const override { out << " serviceDue=" << service_.text(); }
public:
    MotorVehicle(std::string registration, double rate, ServiceDate service)
        : Vehicle(std::move(registration), rate), service_(service) {}
};
class FuelVehicle : public MotorVehicle {
    EnergyStore tank_;
    double pricePerLitre_;
protected:
    double fuelCost(double litres) const {
        tank_.validate(litres);
        return money(static_cast<long double>(litres) * pricePerLitre_);
    }
    void consume(double litres) override { tank_.consume(litres); }
    void details(std::ostream& out) const override {
        MotorVehicle::details(out);
        out << " fuelLitres=" << tank_.level() << '/' << tank_.capacity();
    }
public:
    FuelVehicle(std::string registration, double rate, ServiceDate service,
                double capacityLitres, double initialLitres, double pricePerLitre)
        : MotorVehicle(std::move(registration), rate, service), tank_(capacityLitres, initialLitres),
          pricePerLitre_(positive(pricePerLitre)) {}
    double fuelLevel() const { return tank_.level(); }
};
class TwoWheeler : public FuelVehicle {
public:
    using FuelVehicle::FuelVehicle;
    const char* kind() const override { return "TwoWheeler"; }
    Bill charge(int days, double litres) const override { return makeBill(days, fuelCost(litres)); }
};
class Car : public FuelVehicle {
    Seating seating_;
protected:
    void details(std::ostream& out) const override { FuelVehicle::details(out); out << " seats=" << seating_.seats(); }
public:
    Car(std::string registration, double rate, ServiceDate service,
        double tankCapacity, double initialLitres, double pricePerLitre, int seats)
        : FuelVehicle(std::move(registration), rate, service, tankCapacity, initialLitres, pricePerLitre), seating_(seats) {}
    const char* kind() const override { return "Car"; }
    Bill charge(int days, double litres) const override { return makeBill(days, fuelCost(litres)); }
};
class ElectricVehicle : public MotorVehicle {
    EnergyStore battery_;
    double pricePerKWh_;
protected:
    double electricityCost(double kWh) const {
        battery_.validate(kWh);
        return money(static_cast<long double>(kWh) * pricePerKWh_);
    }
    void consume(double kWh) override { battery_.consume(kWh); }
    void details(std::ostream& out) const override {
        MotorVehicle::details(out);
        out << " batteryKWh=" << battery_.level() << '/' << battery_.capacity()
            << " chargePercent=" << 100 * (battery_.level() / battery_.capacity());
    }
public:
    ElectricVehicle(std::string registration, double rate, ServiceDate service,
                    double capacityKWh, double initialKWh, double pricePerKWh)
        : MotorVehicle(std::move(registration), rate, service), battery_(capacityKWh, initialKWh),
          pricePerKWh_(positive(pricePerKWh)) {}
    double chargeLevelKWh() const { return battery_.level(); }
    double batteryCapacityKWh() const { return battery_.capacity(); }
};
class ElectricCar : public ElectricVehicle {
    Seating seating_;
protected:
    void details(std::ostream& out) const override { ElectricVehicle::details(out); out << " seats=" << seating_.seats(); }
public:
    ElectricCar(std::string registration, double rate, ServiceDate service,
                double batteryCapacity, double initialKWh, double pricePerKWh, int seats)
        : ElectricVehicle(std::move(registration), rate, service, batteryCapacity, initialKWh, pricePerKWh), seating_(seats) {}
    const char* kind() const override { return "ElectricCar"; }
    Bill charge(int days, double kWh) const override { return makeBill(days, electricityCost(kWh)); }
};

class Fleet {
public:
    static constexpr std::size_t capacity = 8;
private:
    std::array<std::unique_ptr<Vehicle>, capacity> owners_;
    Vehicle* vehicles_[capacity]{}; // PDF-required array of base-class pointers; borrowed views
    std::size_t count_ = 0;
    Vehicle& require(const std::string& registration) {
        Vehicle* result = find(registration);
        if (!result) throw std::out_of_range("Unknown registration");
        return *result;
    }
public:
    Fleet() = default;
    Fleet(const Fleet&) = delete;
    Fleet& operator=(const Fleet&) = delete;
    Fleet(Fleet&&) = delete;
    Fleet& operator=(Fleet&&) = delete;
    std::size_t size() const { return count_; }
    Vehicle* find(const std::string& registration) const {
        for (std::size_t i = 0; i < count_; ++i)
            if (vehicles_[i]->registration() == registration) return vehicles_[i];
        return nullptr;
    }
    Vehicle& add(std::unique_ptr<Vehicle> owner) {
        if (!owner) throw std::invalid_argument("Null vehicle owner");
        if (find(owner->registration())) throw std::invalid_argument("Duplicate registration");
        if (count_ == capacity) throw std::length_error("Fleet is full");
        owners_[count_] = std::move(owner);
        vehicles_[count_] = owners_[count_].get();
        return *vehicles_[count_++];
    }
    void rent(const std::string& registration, int days) { require(registration).rent(days); }
    Bill returnRental(const std::string& registration, int days, double units = 0) {
        return require(registration).returnRental(days, units);
    }
    void list(std::ostream& out) const {
        for (std::size_t i = 0; i < count_; ++i) vehicles_[i]->list(out);
    }
};
#endif
// Extension only: fleet.hpp and all existing vehicle classes remain unchanged.
class ElectricBus : public ElectricVehicle {
    Seating seating_;
protected:
    void details(std::ostream& out) const override {
        ElectricVehicle::details(out);
        out << " seats=" << seating_.seats();
    }
public:
    ElectricBus(std::string registration, double dailyRate, ServiceDate service,
                double capacityKWh, double initialKWh, double pricePerKWh, int seats)
        : ElectricVehicle(std::move(registration), dailyRate, service, capacityKWh, initialKWh, pricePerKWh), seating_(seats) {}
    const char* kind() const override { return "ElectricBus"; }
    Bill charge(int days, double kWh) const override { return makeBill(days, electricityCost(kWh)); }
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
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    tests.check(Vehicle::live == 0, "no initial extension objects");
    {
        Fleet fleet;
        Vehicle& base = fleet.add(std::make_unique<ElectricBus>("EB1", 5000, ServiceDate(2027, 1, 15), 300, 250, 10, 40));
        fleet.add(std::make_unique<Bicycle>("B2", 100));
        tests.check(std::string(base.kind()) == "ElectricBus" && fleet.size() == 2, "new subtype accepted without Fleet edits");
        fleet.rent("EB1", 2);
        Bill bill = fleet.returnRental("EB1", 3, 20);
        tests.check(bill.base == 10000 && bill.late == 7500 && bill.energy == 200 && bill.total() == 17700,
                    "ElectricBus independent late and kWh calculation");
        auto* bus = dynamic_cast<ElectricBus*>(fleet.find("EB1"));
        tests.check(bus && bus->chargeLevelKWh() == 230 && bus->batteryCapacityKWh() == 300 && bus->available(),
                    "ElectricBus consumes kWh and restores availability");
        fleet.rent("B2", 1);
        tests.check(fleet.returnRental("B2", 1).total() == 100, "existing type still works with extension");
        fleet.rent("EB1", 2);
        tests.check(fleet.returnRental("EB1", 1).total() == 10000, "ElectricBus booked minimum");
        fleet.rent("EB1", 1);
        for (double units : {-1.0, 231.0, std::numeric_limits<double>::quiet_NaN()}) {
            bool rejected = false;
            try { (void)fleet.returnRental("EB1", 1, units); }
            catch (const std::invalid_argument&) { rejected = true; }
            tests.check(rejected, "invalid ElectricBus energy rejected");
        }
        tests.check(!bus->available() && bus->chargeLevelKWh() == 230, "failed extension return preserves state");
        (void)fleet.returnRental("EB1", 1);
        bool rejected = false;
        try { ElectricBus bad("Bad", 5000, ServiceDate(2027, 1, 15), 300, 250, 10, 0); }
        catch (const std::invalid_argument&) { rejected = true; }
        tests.check(rejected && Vehicle::live == 2, "failed derived construction unwinds complete base");
        std::cout << std::fixed << std::setprecision(2)
                  << "ElectricBus base=" << bill.base << " late=" << bill.late
                  << " electricity=" << bill.energy << " total=" << bill.total() << '\n';
        fleet.list(std::cout);
    }
    tests.check(Vehicle::live == 0, "all extension and existing owners released");
}
