int seleciona(int valor) {
    return valor;
}

float seleciona(float valor) {
    return valor;
}

int soma(int valor, int incremento = 3) {
    return valor + incremento;
}

int main() {
    int inteiro = seleciona(1);
    float real = seleciona(1.5);
    return soma(inteiro) + real;
}
