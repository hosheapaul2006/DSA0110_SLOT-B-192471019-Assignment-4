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
