#include <iostream>
using namespace std;

int main() {
    int i;
    for (i = 0; i < 5; i++);
    cout << i << endl;
    if (i == 0);
    cout << "depois do if" << endl;
    while (i < 0);
    cout << "depois do while" << endl;
    if (i == 5); else cout << "nao chega aqui" << endl;
    do ; while (i < 0);
    cout << "fim" << endl;
    return 0;
}
