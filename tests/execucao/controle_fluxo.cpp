int main() {
    int total = 0;
    for (int i = 1; i <= 5; i++) {
        if (i == 3) continue;
        else total += i;
    }
    int passos = 0;
    while (passos < 2) {
        total += 1;
        passos++;
    }
    do total += 1; while (false);
    cout << total << endl;
    return 0;
}
