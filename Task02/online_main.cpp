#ifndef PEOPLE_HPP
#define PEOPLE_HPP
#include <cmath>
#include <stdexcept>
#include <string>
#include <utility>

class Contact {
    std::string phone_;
public:
    explicit Contact(std::string phone) : phone_(std::move(phone)) {
        if (phone_.empty()) throw std::invalid_argument("Phone is required");
    }
    const std::string& phone() const { return phone_; }
};

class Person {
    std::string name_;
    int id_;
    Contact contact_;
public:
    Person(std::string name, int id, std::string phone)
        : name_(std::move(name)), id_(id), contact_(std::move(phone)) {
        if (name_.empty() || id <= 0) throw std::invalid_argument("Invalid person identity");
    }
    virtual ~Person() = default;
    const std::string& name() const { return name_; }
    int id() const { return id_; }
    const Contact& contact() const { return contact_; }
};

struct Programme {
    std::string name;
    double cgpa;
    Programme(std::string programme, double grade) : name(std::move(programme)), cgpa(grade) {
        if (name.empty() || !std::isfinite(cgpa) || cgpa < 0 || cgpa > 10)
            throw std::invalid_argument("Invalid programme or CGPA (0..10)");
    }
};

class Student : virtual public Person {
    Programme programme_;
public:
    Student(std::string name, int id, std::string phone, std::string programme, double cgpa)
        : Person(std::move(name), id, std::move(phone)), programme_(std::move(programme), cgpa) {}
    const Programme& programme() const { return programme_; }
};

class Staff : virtual public Person {
    std::string department_;
    double salary_;
public:
    Staff(std::string name, int id, std::string phone, std::string department, double salary)
        : Person(std::move(name), id, std::move(phone)), department_(std::move(department)), salary_(salary) {
        if (department_.empty() || !std::isfinite(salary_) || salary_ < 0)
            throw std::invalid_argument("Invalid employment details");
    }
    const std::string& department() const { return department_; }
    double salary() const { return salary_; }
};

class TeachingAssistant : public Student, public Staff {
public:
    TeachingAssistant(const std::string& name, int id, const std::string& phone,
                      const std::string& programme, double cgpa,
                      const std::string& department, double salary)
        : Person(name, id, phone), Student(name, id, phone, programme, cgpa),
          Staff(name, id, phone, department, salary) {}
};

class ResearchScholar : public Student {
    double fellowship_;
public:
    ResearchScholar(const std::string& name, int id, const std::string& phone,
                    const std::string& programme, double cgpa, double fellowship)
        : Person(name, id, phone), Student(name, id, phone, programme, cgpa), fellowship_(fellowship) {
        if (!std::isfinite(fellowship_) || fellowship_ < 0)
            throw std::invalid_argument("Invalid fellowship");
    }
    double fellowship() const { return fellowship_; }
};
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
