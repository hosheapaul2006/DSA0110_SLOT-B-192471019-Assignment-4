#ifndef ELECTRIC_BUS_HPP
#define ELECTRIC_BUS_HPP
#include "fleet.hpp"
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
