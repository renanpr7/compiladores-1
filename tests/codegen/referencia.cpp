void dobra(int& x) { x = x * 2; }
void le(int& x) { cin >> x; }
int main() {
    int a = 5;
    int v[2];
    dobra(a);
    dobra(v[0]);
    le(a);
    return 0;
}
