int maior(int x, int y) {
    return x > y ? x : y;
}

float maior(float x, float y) {
    return x > y ? x : y;
}

int main() {
    int a = 2;
    float b = 3.5;
    float c = a + b * 2;
    bool ok = (a < b) && (a != 0);
    float r = ok ? a : b;
    int v[5];
    v[a] = 10;
    int m1 = maior(a, v[a]);
    float m2 = maior(b, r);
    return 0;
}
