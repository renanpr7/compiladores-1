#include <iostream>
using namespace std;

int y = 0;

bool atribui() {
    return y = 5;
}

int main() {
    int x = 0;
    bool b = false;
    b = x = 5;
    cout << x << endl;

    int z = 0;
    bool c = z = 5;
    cout << z << " " << c << endl;

    bool d = atribui();
    cout << y << " " << d << " " << b << endl;
    return 0;
}
