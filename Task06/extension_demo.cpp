#include "electric_bus.hpp"
#include "../tools/checks.hpp"
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
