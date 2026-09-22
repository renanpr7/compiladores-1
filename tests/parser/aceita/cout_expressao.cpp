#include <iostream>
using namespace std;

int maior(int a, int b) { return a > b ? a : b; }

int main() {
    int a = 3;
    int v[2];
    v[0] = 1;
    cout << a + 2;
    cout << a * 2 - 1;
    cout << maior(a, 1);
    cout << "soma: " << a + v[0] << endl;
    cout << (a > 1 ? a : 0) << endl;
    return 0;
}
