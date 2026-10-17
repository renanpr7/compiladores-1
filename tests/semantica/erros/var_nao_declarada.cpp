int main() {
    int x = nao_existe + 1;
    {
        int interna = 5;
    }
    interna = 10;
    return x;
}
