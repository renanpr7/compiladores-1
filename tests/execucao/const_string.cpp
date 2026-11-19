#include <iostream>
using namespace std;

void imprime(string t) {
    cout << t << endl;
}

int main() {
    const string s = "oi";
    const string a = "x", b = "y";
    imprime(s);
    string t = s;
    cout << t << " " << a << " " << b << endl;
    return 0;
}
