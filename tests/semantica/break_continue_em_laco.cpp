int main() {
    int i = 0;
    while (i < 10) {
        i++;
        if (i == 2) continue;
        if (i == 8) break;
    }
    do {
        i--;
        if (i < 5) break;
    } while (i > 0);
    for (int j = 0; j < 3; j++) {
        { if (j == 1) continue; }
    }
    return i;
}
