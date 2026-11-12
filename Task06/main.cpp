#include "fleet.hpp"
#include "../tools/checks.hpp"
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
