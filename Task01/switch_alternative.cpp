#include "payroll.hpp"
#include "../tools/checks.hpp"
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
