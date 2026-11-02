#include <iostream>
using namespace std;

int calcular(int base, int fator = 2 + 3) {
    return base * fator + (10 - 4) / 2;
}

float calcular(float base, float bonus = 1.5 * 2.0) {
    return base + bonus;
}

void acumular(int& total, int valor, int peso = 1 + 1) {
    total += valor * peso;
}

int main() {
    int soma = 2 + 3 * 4;
    int dados[3];
    dados[0] = calcular(2);
    dados[1] = calcular(2, 3);
    dados[2] = (5 > 3 && !false) ? (20 / 4 + 1) : 0;

    for (int i = 0; i < 1 + 2; i++) {
        if (i == 4 - 3) {
            acumular(soma, dados[i], 1);
            continue;
        }
        acumular(soma, dados[i]);
    }

    int passos = 3 * 2 - 4;
    while (passos > 0) {
        soma += (10 % 3);
        passos--;
    }

    do {
        soma += -(-2);
    } while (3 > 5);

    float media = calcular(2.5);
    cout << soma << " " << dados[0] << " " << dados[1] << " " << dados[2] << endl;
    cout << media << endl;
    return 0;
}
