int g(int a, int b = 5) {
    return a + b;
}

int main() {
    cout << g(1) << g(1, 2) << endl;
    return 0;
}

int g(int a) {
    return a;
}
