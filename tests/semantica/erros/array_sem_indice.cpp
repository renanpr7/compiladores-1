int f(int a) { return a; }

int main() {
    int v[3];
    int y = v;
    v = 5;
    cout << v << endl;
    y = f(v);
    cin >> v;
    v++;
    if (v) {
        y = 1;
    }
    return 0;
}
