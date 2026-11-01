void dobra(int& x) { x = x * 2; }
void troca(int& a, int& b) { int t = a; a = b; b = t; }
void repassa(int& y) { dobra(y); y++; }
int main() {
    int v[3];
    int n = 1;
    dobra(n);
    troca(n, v[0]);
    repassa(v[1]);
    return n;
}
