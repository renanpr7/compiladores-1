#include <iostream>
using namespace std;

int main() {
    int y = 0;
    int x = 1 + (y = 2);
    cout << x << " " << y << endl;

    int c = 3;
    while ((c = c - 1) > 0) {
        cout << c << endl;
    }

    int d = 2;
    while ((d = d - 1)) {
        cout << d << endl;
    }

    return 0;
}
