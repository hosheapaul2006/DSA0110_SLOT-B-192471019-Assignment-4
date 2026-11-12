// Compile the same trace model with virtual inheritance removed.
// Its indirect Person initializer is also removed as required by C++.
#define NONVIRTUAL_PERSON
#ifndef CONSTRUCTION_HPP
#define CONSTRUCTION_HPP
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

inline std::vector<std::string> events;
inline void trace(const std::string& message) {
    events.push_back(message);
    std::cout << message << '\n';
}
class Address {
    std::string city_;
    int pin_;
public:
    Address(std::string city, int pin) : city_(std::move(city)), pin_(pin) {
        if (city_.empty() || pin < 100000 || pin > 999999) throw std::invalid_argument("Invalid address");
        trace("Address(" + city_ + ")");
    }
    ~Address() { trace("~Address(" + city_ + ")"); }
    const std::string& city() const { return city_; }
};
class Course {
    std::string title_;
public:
    explicit Course(std::string title) : title_(std::move(title)) {
        if (title_.empty()) throw std::invalid_argument("Course is required");
        trace("Course(" + title_ + ")");
    }
    ~Course() { trace("~Course(" + title_ + ")"); }
};
class Person {
    std::string name_;
    int id_;
    Address address_;
public:
    Person(std::string name, int id, std::string city, int pin)
        : name_(std::move(name)), id_(id), address_(std::move(city), pin) {
        if (name_.empty() || id_ <= 0) throw std::invalid_argument("Invalid person");
        trace("Person(" + name_ + ")");
    }
    virtual ~Person() { trace("~Person(" + name_ + ")"); }
    const std::string& name() const { return name_; }
    const Address& address() const { return address_; }
};
#ifdef NONVIRTUAL_PERSON
#define PERSON_INHERITANCE public
#else
#define PERSON_INHERITANCE virtual public
#endif
class Student : PERSON_INHERITANCE Person {
    Course course_;
public:
    Student(std::string name, int id, std::string city, int pin, std::string course)
        : Person(std::move(name), id, std::move(city), pin), course_(std::move(course)) { trace("Student"); }
    ~Student() override { trace("~Student"); }
};
class Staff : PERSON_INHERITANCE Person {
public:
    Staff(std::string name, int id, std::string city, int pin)
        : Person(std::move(name), id, std::move(city), pin) { trace("Staff"); }
    ~Staff() override { trace("~Staff"); }
};
class TeachingAssistant : public Student, public Staff {
public:
    TeachingAssistant(const std::string& name, int id, const std::string& city, int pin)
        :
#ifndef NONVIRTUAL_PERSON
          Person(name, id, city, pin),
#endif
          Student("Student proposal", 11, "Salem", 636001, "MTech"),
          Staff("Staff proposal", 12, "Erode", 638001) {
#ifdef NONVIRTUAL_PERSON
        // An indirect, nonvirtual Person cannot appear in this initializer list.
        (void)name; (void)id; (void)city; (void)pin;
#endif
        trace("TA");
    }
    ~TeachingAssistant() override { trace("~TA"); }
};
#undef PERSON_INHERITANCE
#endif
#ifndef COURSEWORK_CHECKS_HPP
#define COURSEWORK_CHECKS_HPP
#include <iostream>
#include <stdexcept>
#include <string>
class Tests {
    bool logging;
    unsigned count=0;
public:
    Tests(int argc,char** argv):logging(argc>1 && std::string(argv[1])=="--tests") {}
    void check(bool condition,const char* scenario) {
        ++count;
        if(logging) std::cerr << "CHECK|" << scenario << '|' << (condition?"PASS":"FAIL") << '\n';
        if(!condition) throw std::runtime_error(scenario);
    }
    ~Tests() { if(logging) std::cerr << "TEST_COUNT " << count << '\n'; }
};
#endif

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
