#include <iostream>
using namespace std;

int   maior(int a, int b)      { return a > b ? a : b; }
float maior(float a, float b)  { return a > b ? a : b; }

void dobra(int& x) { x = x * 2; }

int main() {
    int a = 3;
    dobra(a);
    for (int i = 0; i < 2; i++) {
        cout << maior(a, i) << endl;
    }
    return 0;
}
