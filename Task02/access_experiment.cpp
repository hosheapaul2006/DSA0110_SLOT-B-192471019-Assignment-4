#include "../tools/checks.hpp"
#include <type_traits>
#include <iostream>
struct PersonAccess {
    int publicValue = 1;
protected:
    int protectedValue = 2;
private:
    int privateValue = 3;
public:
    int readPrivate() const { return privateValue; }
};
struct PublicStudent : public PersonAccess {
    int inside() const { return publicValue + protectedValue + readPrivate(); }
};
struct ProtectedStudent : protected PersonAccess {
    int inside() const { return publicValue + protectedValue + readPrivate(); }
};
struct PrivateStudent : private PersonAccess {
    int inside() const { return publicValue + protectedValue + readPrivate(); }
};
static_assert(std::is_convertible<PublicStudent*, PersonAccess*>::value, "public base conversion");
static_assert(!std::is_convertible<ProtectedStudent*, PersonAccess*>::value, "protected external conversion");
static_assert(!std::is_convertible<PrivateStudent*, PersonAccess*>::value, "private external conversion");
int main(int argc, char** argv) {
    Tests tests(argc, argv);
    tests.check(PublicStudent{}.inside() == 6 && ProtectedStudent{}.inside() == 6 && PrivateStudent{}.inside() == 6,
                "derived code reaches public/protected and uses getter for private");
    std::cout << "Derived totals: " << PublicStudent{}.inside() << ' ' << ProtectedStudent{}.inside()
              << ' ' << PrivateStudent{}.inside() << '\n';
}
