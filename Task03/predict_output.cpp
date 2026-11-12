#include <iostream> 
 
struct Engine { 
    Engine() { std::cout << "Engine "; } 
    ~Engine() { std::cout << "~Engine "; } 
}; 
 
struct Vehicle { 
    Vehicle() { std::cout << "Vehicle "; } 
    ~Vehicle() { std::cout << "~Vehicle "; } 
}; 
 
struct Car : Vehicle { 
    Engine e; 
    Car() { std::cout << "Car "; } 
    ~Car() { std::cout << "~Car "; } 
}; 
 
int main() 
{ 
    { 
        Car c; 
        std::cout << "| "; 
    } 
    std::cout << '\n'; 
} 
 
