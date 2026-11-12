// Expected compile error: two distinct Person::name members.
#include <string>
struct Person { std::string name; };
struct Student : Person {};
struct Staff : Person {};
struct TeachingAssistant : Student, Staff {};
int main() { TeachingAssistant ta; ta.name = "Ambiguous"; }
