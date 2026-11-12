#include "people.hpp"
#include "../tools/checks.hpp"
#include <iostream>

namespace Nonvirtual {
struct Student : Person { Programme programme; };
struct Staff : Person { std::string department; double salary; };
struct TeachingAssistant : Student, Staff {};
}
struct ComposedTA { Student student; Staff staff; };

int main(int argc, char** argv) {
    Tests tests(argc, argv);
    ComposedTA ta{Student("Academic identity", 1, "1", "MTech", 9),
                  Staff("Employment identity", 2, "2", "CSE", 20000)};
    const Person* p1 = &ta.student;
    const Person* p2 = &ta.staff;
    tests.check(p1 != p2, "composition owns two independent Persons");
    tests.check(ta.student.name() != ta.staff.name(), "role identities can diverge in composition");
    std::cout << "sizeof Person=" << sizeof(Person) << " Student=" << sizeof(Student)
              << " Staff=" << sizeof(Staff) << '\n'
              << "sizeof virtual TA=" << sizeof(TeachingAssistant)
              << " nonvirtual TA=" << sizeof(Nonvirtual::TeachingAssistant)
              << " composed TA=" << sizeof(ComposedTA) << '\n'
              << ta.student.name() << " | " << ta.staff.name() << '\n';
}
