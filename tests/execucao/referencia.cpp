void dobra(int& x) { x = x * 2; }
void troca(int& a, int& b) { int t = a; a = b; b = t; }
void repassa(int& y) { dobra(y); y++; }
int main() {
    int a = 5;
    int b = 7;
    int v[2];
    v[0] = 1;
    v[1] = 2;
    dobra(a);
    troca(a, b);
    repassa(v[1]);
    cout << a << " " << b << " " << v[1] << endl;
    return 0;
}
