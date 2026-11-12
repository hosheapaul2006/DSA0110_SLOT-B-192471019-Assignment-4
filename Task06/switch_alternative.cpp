#include "fleet.hpp"
#include "../tools/checks.hpp"
#include <iostream>

enum class Kind { Bicycle, TwoWheeler, Car, ElectricCar };
struct VehicleRecord {
    Kind type;
    std::string registration;
    double dailyRate;
    double fuelLitres = 0, batteryKWh = 0;
    int seating = 0;
    double batteryCapacity = 0;
    ServiceDate service{2027, 1, 15};
    Bill charge(int bookedDays, int actualDays, double units) const {
        if (bookedDays <= 0 || actualDays <= 0 || !std::isfinite(units) || units < 0)
            throw std::invalid_argument("Invalid switch-example rental");
        double energy = 0;
        switch (type) {
            case Kind::Bicycle:
                if (units != 0) throw std::invalid_argument("Bicycle energy");
                break;
            case Kind::TwoWheeler:
            case Kind::Car:
                if (units > fuelLitres) throw std::invalid_argument("Insufficient fuel");
                energy = money(static_cast<long double>(units) * 100);
                break;
            case Kind::ElectricCar:
                if (units > batteryKWh) throw std::invalid_argument("Insufficient charge");
                energy = money(static_cast<long double>(units) * 10);
                break;
        }
        const int lateDays = actualDays > bookedDays ? actualDays - bookedDays : 0;
        Bill result{money(static_cast<long double>(bookedDays) * positive(dailyRate)),
                    money(static_cast<long double>(lateDays) * dailyRate * 1.5L), energy};
        (void)result.total();
        return result;
    }
};
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    VehicleRecord records[] = {
        {Kind::Bicycle, "B1", 100},
        {Kind::TwoWheeler, "T1", 300, 8},
        {Kind::Car, "C1", 1500, 30, 0, 5},
        {Kind::ElectricCar, "E1", 1800, 0, 50, 5, 60}
    };
    const double usage[] = {0, 1, 2, 5};
    const double expected[] = {350, 1150, 5450, 6350};
    for (int i = 0; i < 4; ++i) {
        const double actual = records[i].charge(2, 3, usage[i]).total();
        tests.check(actual == expected[i], "switch matches independent four-type totals");
        std::cout << records[i].registration << " switch total=" << actual << '\n';
    }
    std::cout << "sizeof record=" << sizeof(VehicleRecord) << " Bicycle=" << sizeof(Bicycle)
              << " TwoWheeler=" << sizeof(TwoWheeler) << " Car=" << sizeof(Car)
              << " ElectricCar=" << sizeof(ElectricCar) << " Vehicle*=" << sizeof(Vehicle*) << '\n';
}
