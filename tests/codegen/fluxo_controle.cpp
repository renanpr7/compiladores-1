void percorrer(int limite) {
    int i = 0;
    if (limite > 0)
        i += 1;
    else
        i -= 1;
    while (i < limite) {
        i++;
        if (i == 5) break;
        continue;
    }
    do i--; while (i > 0);
    for (int j = 0; j < limite; j++) i += j;
    for (i = 0; i < limite; i++) i += 1;
    for (; i > 0;) i--;
    for (;;) break;
}
