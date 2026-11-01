int main() {
    int a = 10;
    int r = 0;
    if (a > 5) {
        r = 1;
    } else {
        r = 2;
    }
    int m = a > r ? a : r;
    return m;
}
