#include <iostream>
using namespace std;

void ajustar(int& alvo, int delta = 4 * 2 - 3) {
    alvo += delta;
}

void ajustar(int& a, int& b, int escala) {
    int temp = a;
    a = b * escala;
    b = temp + (6 / 2);
}

int combinar(int x, int y = 10 + 5) {
    return x + y;
}

int combinar(int x, int y, int z) {
    return (x + y) * z;
}

int main() {
    int x = 1 + 2 * 3;
    int y = (8 + 2) / 2;
    int v[2];
    v[0] = 3;
    v[1] = 4;

    ajustar(x);
    ajustar(y, 1 + 1);
    ajustar(v[0], v[1], 1 + 2);

    int r1 = combinar(x);
    int r2 = combinar(y, v[0], 2 * 2 - 1);

    if ((2 * 3 == 6) && (10 > 2 + 2)) {
        ajustar(r1, -3 + 5);
    }

    cout << x << " " << y << " " << v[0] << " " << v[1] << endl;
    cout << r1 << " " << r2 << endl;
    return 0;
}
