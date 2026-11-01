#include <iostream>
using namespace std;

int maior(int a, int b) {
    return a + b;
}

float maior(float a, float b) {
    return a + b;
}

int incrementar(int x, int passo = 1) {
    return x + passo;
}

int main() {
    int v[2];
    v[0] = incrementar(5);
    v[1] = maior(v[0], 3);
    cout << v[1] << endl;
    return 0;
}
