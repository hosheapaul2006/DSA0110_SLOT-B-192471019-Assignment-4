#include "../tools/checks.hpp"
#include <iostream>
#include <string>
#include <utility>

struct AddressValue { std::string city; int pin; };
class StudentWithPointer {
    AddressValue* home_;
public:
    StudentWithPointer(std::string city, int pin) : home_(new AddressValue{std::move(city), pin}) {}
    StudentWithPointer(const StudentWithPointer& other) : home_(new AddressValue(*other.home_)) {}
    StudentWithPointer& operator=(const StudentWithPointer& other) {
        if (this != &other) {
            AddressValue* replacement = new AddressValue(*other.home_);
            delete home_;
            home_ = replacement;
        }
        return *this;
    }
    ~StudentWithPointer() { delete home_; }
    const AddressValue& home() const { return *home_; }
    void moveCity(std::string city) { home_->city = std::move(city); }
};
struct StudentWithMember { AddressValue home; };
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    StudentWithPointer a("Salem", 636001);
    StudentWithPointer b = a;
    b.moveCity("Chennai");
    tests.check(a.home().city == "Salem" && b.home().city == "Chennai", "deep-copy constructor");
    StudentWithPointer c("Erode", 638001);
    c = a;
    a.moveCity("Madurai");
    tests.check(c.home().city == "Salem" && a.home().city == "Madurai", "deep-copy assignment");
    c = c;
    tests.check(c.home().city == "Salem", "self-assignment");
    tests.check(&a.home() != &b.home() && &b.home() != &c.home(), "independent ownership");
    std::cout << a.home().city << ' ' << b.home().city << ' ' << c.home().city << '\n'
              << "sizeof member=" << sizeof(StudentWithMember) << " pointer owner=" << sizeof(StudentWithPointer)
              << " separately allocated Address=" << sizeof(AddressValue) << '\n';
}
