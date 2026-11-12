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
