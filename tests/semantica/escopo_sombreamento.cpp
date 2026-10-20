int x = 1;

int f(int a) {
    float x = 2.5;
    {
        int x = 10;
        int a = 20;
    }
    for (int i = 0; i < 3; i++) {
        int y = i;
    }
    for (int i = 0; i < 2; i++) {
        a = a + i;
    }
    return a;
}

int main() {
    return f(x);
}
