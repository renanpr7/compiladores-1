int g = 5;

int f(int a, int b = a) {
    return a + b;
}

int h(int x = g) {
    return x;
}

int k(int x = h(1)) {
    return x;
}

int ok(int x = 2 * 3, float y = -1.5, string s = "oi") {
    return x;
}

int main() {
    int g = 10;
    cout << f(1) << h() << k() << ok() << endl;
    return 0;
}
