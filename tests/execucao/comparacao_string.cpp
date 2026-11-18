#include <iostream>
using namespace std;

string obter(bool flag) {
    return flag ? "ok" : "erro";
}

int main() {
    string s = "sim";
    const string cs = "sim";
    string vazia;

    if (s == "sim") {
        cout << 1 << endl;
    }
    cout << (s != "nao") << " " << (s == "nao") << " " << (s != "sim") << endl;
    cout << (s == cs) << " " << (vazia == "") << endl;
    cout << (obter(true) == "ok") << " " << (obter(false) != "ok") << endl;
    return 0;
}
