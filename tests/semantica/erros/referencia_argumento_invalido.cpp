void dobra(int& x) { x = x * 2; }
int main() {
    const int N = 2;
    dobra(5);
    dobra(N);
    return 0;
}
