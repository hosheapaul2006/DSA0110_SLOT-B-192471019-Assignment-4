#include "construction.hpp"
#include "../tools/checks.hpp"

int main(int argc, char** argv) {
    Tests tests(argc, argv);
    std::cout << "Student scope\n";
    events.clear();
    { Student s("Student identity", 1, "Salem", 636001, "CSE"); }
    tests.check(events == std::vector<std::string>{"Address(Salem)", "Person(Student identity)", "Course(CSE)", "Student",
        "~Student", "~Course(CSE)", "~Person(Student identity)", "~Address(Salem)"}, "Student full construction/destruction order");
    std::cout << "Staff scope\n";
    events.clear();
    { Staff s("Staff identity", 2, "Erode", 638001); }
    tests.check(events == std::vector<std::string>{"Address(Erode)", "Person(Staff identity)", "Staff",
        "~Staff", "~Person(Staff identity)", "~Address(Erode)"}, "Staff full construction/destruction order");
    std::cout << "TeachingAssistant scope\n";
    events.clear();
    {
        TeachingAssistant ta("TA identity", 3, "Chennai", 600001);
        const Person* fromStudent = static_cast<const Student*>(&ta);
        const Person* fromStaff = static_cast<const Staff*>(&ta);
#ifdef NONVIRTUAL_PERSON
        tests.check(fromStudent != fromStaff, "nonvirtual TA has two Persons");
        tests.check(fromStudent->name() == "Student proposal" && fromStaff->name() == "Staff proposal", "nonvirtual role identities retained");
#else
        tests.check(fromStudent == fromStaff, "virtual TA has one Person");
        tests.check(ta.name() == "TA identity" && ta.address().city() == "Chennai", "most-derived initializer selected");
#endif
    }
#ifdef NONVIRTUAL_PERSON
    tests.check(events == std::vector<std::string>{"Address(Salem)", "Person(Student proposal)", "Course(MTech)", "Student",
        "Address(Erode)", "Person(Staff proposal)", "Staff", "TA", "~TA", "~Staff", "~Person(Staff proposal)",
        "~Address(Erode)", "~Student", "~Course(MTech)", "~Person(Student proposal)", "~Address(Salem)"}, "nonvirtual TA complete trace");
#else
    tests.check(events == std::vector<std::string>{"Address(Chennai)", "Person(TA identity)", "Course(MTech)", "Student", "Staff", "TA",
        "~TA", "~Staff", "~Student", "~Course(MTech)", "~Person(TA identity)", "~Address(Chennai)"}, "virtual TA complete trace");
#endif
    std::cout << "sizeof(TeachingAssistant)=" << sizeof(TeachingAssistant) << '\n';
    events.clear();
    bool rejected = false;
    try { Address invalid("Salem", 99999); } catch (const std::invalid_argument&) { rejected = true; }
    tests.check(rejected && events.empty(), "invalid address rejected before construction completes");
    events.clear();
    rejected = false;
    try { Student invalid("Invalid", 9, "Salem", 636001, ""); }
    catch (const std::invalid_argument&) { rejected = true; }
    tests.check(rejected && events == std::vector<std::string>{"Address(Salem)", "Person(Invalid)",
        "~Person(Invalid)", "~Address(Salem)"}, "failed Course construction unwinds completed base/member");
}
