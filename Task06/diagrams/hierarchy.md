# Vehicle rental UML

```mermaid
classDiagram
class Vehicle {
  <<abstract>>
  -string registration_
  -double dailyRate_
  -bool rented_
  -int bookedDays_
  +rent(days)
  +returnRental(days, units) Bill
  +charge(days, units) const Bill*
  +kind() const text*
  +~Vehicle() virtual
}
class MotorVehicle { <<abstract>> }
class FuelVehicle { <<abstract>> }
class ElectricVehicle { <<abstract>> }
class Bicycle
class TwoWheeler
class Car
class ElectricCar
class ElectricBus
class ServiceDate { -int year_; -int month_; -int day_ }
class EnergyStore { -double capacity_; -double level_ }
class Seating { -int seats_ }
class Fleet {
  -Vehicle* vehicles_[8]
  -unique_ptr~Vehicle~ owners_[8]
  -size_t count_
  +add(owner)
  +find(registration) Vehicle_ptr
  +rent(registration, days)
  +returnRental(registration, days, units) Bill
  +list(stream)
}
Vehicle <|-- Bicycle
Vehicle <|-- MotorVehicle
MotorVehicle <|-- FuelVehicle
MotorVehicle <|-- ElectricVehicle
FuelVehicle <|-- TwoWheeler
FuelVehicle <|-- Car
ElectricVehicle <|-- ElectricCar
ElectricVehicle <|-- ElectricBus : later extension
MotorVehicle *-- ServiceDate
FuelVehicle *-- EnergyStore : tank in litres
ElectricVehicle *-- EnergyStore : battery in kWh
Car *-- Seating
ElectricCar *-- Seating
ElectricBus *-- Seating
Fleet *-- Vehicle : owns through unique_ptr
```

Generalization arrows denote public single inheritance; composition diamonds denote owned members/objects. ElectricBus is planned here, but implemented only after the four-type baseline is committed.
