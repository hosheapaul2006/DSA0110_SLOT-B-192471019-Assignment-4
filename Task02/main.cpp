#include "people.hpp"
#include "../tools/checks.hpp"
#include <iostream>
#include <limits>

int main(int argc, char** argv) {
    Tests tests(argc, argv);
    Student student("Kavya", 1, "9000000001", "CSE", 9.2);
    Staff staff("Arun", 2, "9000000002", "Computing", 50000);
    TeachingAssistant ta("Meena", 3, "9000000003", "MTech", 9.5, "CSE", 20000);
    ResearchScholar scholar("Ravi", 4, "9000000004", "PhD", 10, 35000);
    Person* viaStudent = static_cast<Student*>(&ta);
    Person* viaStaff = static_cast<Staff*>(&ta);
    tests.check(viaStudent == viaStaff, "TA has one shared Person");
    tests.check(ta.name() == "Meena" && ta.id() == 3, "most-derived identity");
    tests.check(ta.programme().cgpa == 9.5 && ta.salary() == 20000, "both roles retained");
    tests.check(scholar.name() == "Ravi" && scholar.fellowship() == 35000, "research scholar identity and fellowship");
    tests.check(student.contact().phone() == "9000000001" && staff.department() == "Computing", "composed contact and staff fields");
    tests.check(Student("Boundary", 5, "1", "CSE", 0).programme().cgpa == 0, "zero CGPA accepted");
    auto rejected = [&](auto action, const char* scenario) {
        bool threw = false;
        try { action(); } catch (const std::invalid_argument&) { threw = true; }
        tests.check(threw, scenario);
    };
    rejected([] { Student s("Bad", 1, "1", "CSE", 10.1); }, "CGPA above ten rejected");
    rejected([] { Student s("Bad", 1, "1", "CSE", -1); }, "negative CGPA rejected");
    rejected([] { Person p("Bad", 1, ""); }, "empty phone rejected");
    rejected([] { Person p("Bad", 0, "1"); }, "invalid ID rejected");
    rejected([] { Staff s("Bad", 1, "1", "CSE", -1); }, "negative salary rejected");
    rejected([] { ResearchScholar s("Bad", 1, "1", "PhD", 9, std::numeric_limits<double>::quiet_NaN()); }, "NaN fellowship rejected");
    std::cout << "Student: " << student.name() << ' ' << student.programme().name << '\n'
              << "Staff: " << staff.name() << ' ' << staff.department() << '\n'
              << "TeachingAssistant: " << ta.name() << ' ' << ta.programme().name << ' ' << ta.department() << '\n'
              << "Shared Person: " << std::boolalpha << (viaStudent == viaStaff) << '\n'
              << "ResearchScholar: " << scholar.name() << " fellowship=" << scholar.fellowship() << '\n';
}
