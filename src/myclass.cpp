#include "myclass.h"
#include <iostream>

MyClass::MyClass()
{
    std::cout << "MyClass constructor" << std::endl;
}
void MyClass::foo()
{
    std::cout << "FOO!" << std::endl;
}

int MyClass::add(int a, int b)
{
    return a + b;
}
