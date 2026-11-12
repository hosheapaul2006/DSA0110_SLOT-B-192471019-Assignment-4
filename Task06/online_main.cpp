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
    auto rejected = [&](auto operation, const char* message) {
        bool threw = false;
        try { operation(); } catch (const std::exception&) { threw = true; }
        tests.check(threw, message);
    };
    tests.check(Vehicle::live == 0, "no initial vehicles");
    const ServiceDate service(2027, 1, 15); // sample date only; not the assignment submission date
    {
        Fleet fleet;
        std::ostringstream empty;
        fleet.list(empty);
        tests.check(fleet.size() == 0 && empty.str().empty() && fleet.find("missing") == nullptr, "empty fleet");
        fleet.add(std::make_unique<Bicycle>("B1", 100));
        fleet.add(std::make_unique<TwoWheeler>("T1", 300, service, 10, 8, 100));
        fleet.add(std::make_unique<Car>("C1", 1500, service, 40, 30, 100, 5));
        fleet.add(std::make_unique<ElectricCar>("E1", 1800, service, 60, 50, 10, 5));
        tests.check(fleet.size() == 4 && Vehicle::live == 4, "four owned types created");
        const char* ids[] = {"B1", "T1", "C1", "E1"};
        const double usage[] = {0, 1, 2, 5};
        const double totals[] = {350, 1150, 5450, 6350};
        std::cout << std::fixed << std::setprecision(2);
        for (int i = 0; i < 4; ++i) {
            tests.check(fleet.find(ids[i])->available(), "new vehicle available");
            fleet.rent(ids[i], 2);
            tests.check(!fleet.find(ids[i])->available(), "rental changes availability");
            Bill bill = fleet.returnRental(ids[i], 3, usage[i]);
            tests.check(bill.total() == totals[i], "type-specific late/energy charge matches independent calculation");
            tests.check(fleet.find(ids[i])->available(), "return restores availability");
            std::cout << ids[i] << ' ' << fleet.find(ids[i])->kind() << " base=" << bill.base
                      << " late=" << bill.late << " energy=" << bill.energy << " total=" << bill.total() << '\n';
        }
        auto* fuel = dynamic_cast<Car*>(fleet.find("C1"));
        auto* electric = dynamic_cast<ElectricCar*>(fleet.find("E1"));
        tests.check(fuel && fuel->fuelLevel() == 28 && electric && electric->chargeLevelKWh() == 45
                    && electric->batteryCapacityKWh() == 60, "return consumes litres or kWh separately");
        fleet.rent("C1", 2);
        tests.check(fleet.returnRental("C1", 1).total() == 3000, "early return keeps booked minimum");
        fleet.rent("C1", 2);
        tests.check(fleet.returnRental("C1", 2).late == 0, "on-time return has no late fee");
        fleet.rent("B1", 1);
        rejected([&] { fleet.rent("B1", 2); }, "double booking rejected");
        rejected([&] { fleet.returnRental("B1", 0); }, "zero actual days rejected");
        rejected([&] { fleet.returnRental("B1", -1); }, "negative actual days rejected");
        rejected([&] { fleet.returnRental("B1", 1, 1); }, "bicycle energy rejected");
        tests.check(!fleet.find("B1")->available() && fleet.find("B1")->bookedDays() == 1, "failed operations preserve booking");
        (void)fleet.returnRental("B1", 1);
        rejected([&] { fleet.returnRental("B1", 1); }, "return while available rejected");
        rejected([&] { fleet.rent("B1", 0); }, "zero booking rejected");
        rejected([&] { fleet.rent("B1", -1); }, "negative booking rejected");
        rejected([&] { fleet.rent("UNKNOWN", 1); }, "unknown rent rejected");
        rejected([&] { fleet.returnRental("UNKNOWN", 1); }, "unknown return rejected");
        fleet.rent("C1", 1);
        for (double bad : {-1.0, 1000.0, std::numeric_limits<double>::infinity(), std::numeric_limits<double>::quiet_NaN()})
            rejected([&] { fleet.returnRental("C1", 1, bad); }, "invalid energy rejected");
        tests.check(fuel->fuelLevel() == 28 && !fuel->available(), "failed energy checks preserve level and active rental");
        (void)fleet.returnRental("C1", 1);
        rejected([&] { fleet.add(nullptr); }, "null owner rejected");
        rejected([&] { fleet.add(std::make_unique<Bicycle>("B1", 100)); }, "duplicate registration rejected");
        tests.check(fleet.size() == 4 && Vehicle::live == 4, "failed additions clean candidate and preserve fleet");
        std::cout << "Fleet after returns:\n";
        fleet.list(std::cout);
        std::ostringstream listed;
        fleet.list(listed);
        tests.check(listed.str().find("fuelLitres=28") != std::string::npos
                 && listed.str().find("batteryKWh=45/60") != std::string::npos
                 && listed.str().find("serviceDue=2027-01-15") != std::string::npos, "list includes specialized information");
        for (int i = 0; i < 4; ++i) fleet.add(std::make_unique<Bicycle>("Extra" + std::to_string(i), 100));
        rejected([&] { fleet.add(std::make_unique<Bicycle>("Overflow", 100)); }, "full fleet rejected");
        tests.check(fleet.size() == Fleet::capacity && Vehicle::live == 8, "full-fleet failure preserves eight owners");
    }
    tests.check(Vehicle::live == 0, "all fleet owners destroy through virtual base");
    tests.check(ServiceDate(2000, 2, 29).text() == "2000-02-29", "Gregorian leap-century accepted");
    rejected([] { ServiceDate d(1900, 2, 29); }, "non-leap-century date rejected");
    rejected([] { ServiceDate d(2026, 2, 30); }, "invalid calendar day rejected");
    rejected([] { ServiceDate d(2026, 13, 1); }, "invalid calendar month rejected");
    rejected([] { Bicycle b("", 100); }, "empty registration rejected");
    rejected([] { Bicycle b("Bad", 0); }, "zero rate rejected");
    rejected([] { Bicycle b("Bad", std::numeric_limits<double>::quiet_NaN()); }, "NaN rate rejected");
    rejected([] { EnergyStore e(0, 0); }, "zero energy capacity rejected");
    rejected([] { EnergyStore e(10, -1); }, "negative level rejected");
    rejected([] { EnergyStore e(10, 11); }, "above-capacity level rejected");
    rejected([] { Seating s(0); }, "zero seating rejected");
    {
        Bicycle huge("Large", 1e308);
        rejected([&] { huge.rent(2); }, "unrepresentable booking rejected");
        tests.check(huge.available(), "booking overflow preserves availability");
        huge.rent(1);
        rejected([&] { huge.returnRental(3); }, "unrepresentable return charge rejected");
        tests.check(!huge.available(), "return overflow preserves booking");
        tests.check(huge.returnRental(1).total() == 1e308, "representable large return succeeds");
    }
    tests.check(Vehicle::live == 0, "no remaining vehicles after boundary cases");
}
