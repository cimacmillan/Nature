#include "src/example/Dog.hpp"
#include <iostream>

using namespace std;


bool runTests() {
    Dog* dog = new Dog();
    dog->woof();
    if (dog->getCount() == 21) {
        return false;
    }
    return true;
}

int main( int argc, char* argv[] )
{
    bool passed = runTests();
    if (passed) {
        cout << "\033[1;32mTESTS PASSED\033[0m" << endl;
        return 0;
    } else {
        cout << "\033[1;31mTESTS FAILED\033[0m" << endl;
        return 1;
    }
}
