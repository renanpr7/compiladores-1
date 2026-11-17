#include <iostream>
using namespace std;

bool g = 5;

bool positivo(int x) {
    return x;
}

bool padrao(bool b = 3) {
    return b;
}

int main() {
    bool b = 5;
    float f = 2.5;
    int x = 7;
    bool c = f;
    bool d = x > 3 ? 2 : 0;
    bool e = 0;
    cout << b << c << d << e << g << endl;
    if (b == true) {
        cout << "verdadeiro" << endl;
    }
    b = x;
    cout << b << positivo(9) << padrao() << endl;
    return 0;
}
