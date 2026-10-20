int calcular(int valor) {
    return valor;
}

int calcular(int valor, int incremento = 1) {
    return valor + incremento;
}

int main() {
    int resultado = calcular(5);
    return resultado;
}
