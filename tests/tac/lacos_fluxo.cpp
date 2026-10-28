int main() {
    int s = 0;
    for (int i = 0; i < 5; i++) {
        if (i == 2) {
            continue;
        }
        if (i == 4) {
            break;
        }
        s += i;
    }
    while (s > 0) {
        s--;
    }
    return s;
}
