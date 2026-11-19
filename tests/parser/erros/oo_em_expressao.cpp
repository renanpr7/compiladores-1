int f(int x) { return x; }
int main() {
    int x;
    int y = nullptr;
    x = f(this);
    x = new int;
    return 0;
}
