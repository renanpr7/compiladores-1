#include <iostream>
using namespace std;

int g = 10;

int f() {
    cout << "[f]";
    g = 20;
    return 1;
}

int main() {
    cout << "a" << f() << endl;
    g = 10;
    cout << g << " " << f() << " " << g << endl;
    int x = 5;
    cout << x << " " << (x = 9) << " " << x << endl;
    cout << endl << f() << endl;
    return 0;
}
