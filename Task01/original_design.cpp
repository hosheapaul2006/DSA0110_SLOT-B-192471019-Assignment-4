#include <iostream>
#include <string>
using std::string;

class Professor   { public: string name; int id; double basic; double da; double research; 
                    double pay() { return basic + basic * 0.42 + research; } }; 
class LabAssistant{ public: string name; int id; double basic; double da; 
                    double pay() { return basic + basic * 0.42; } }; 
class GuestFaculty{ public: string nm;   int code; double hours; double rate; 
                    double pay() { return hours * rate; } };

int main() {
    Professor p{"Kavya", 1, 50000, 999, 5000};
    LabAssistant l{"Arun", 2, 20000, 999};
    GuestFaculty g{"Meena", 3, 20, 1000};
    std::cout << p.pay() << " " << l.pay() << " " << g.pay() << "\n";
}
