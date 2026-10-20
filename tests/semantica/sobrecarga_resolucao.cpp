int maior(int a, int b) {
    return a > b ? a : b;
}

float maior(float a, float b) {
    return a > b ? a : b;
}

string maior(string a, string b) {
    return a;
}

int somar(int a, int b) {
    return a + b;
}

int somar(int a, int b, int c) {
    return a + b + c;
}

int main() {
    int mi = maior(10, 20);
    float mf = maior(1.5, 2.5);
    string ms = maior("a", "b");
    int s2 = somar(1, 2);
    int s3 = somar(1, 2, 3);
    return mi + s2 + s3;
}
