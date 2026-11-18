#include <iostream>
using namespace std;

int main() {
    float x = 123456.7;
    cout << x - 123456.0 << endl;

    float a = 0.1;
    cout << a << endl;

    float t = 1.0 / 3.0;
    float truncado = 0.333333;
    cout << t << endl;
    cout << (t != truncado) << endl;

    return 0;
}
