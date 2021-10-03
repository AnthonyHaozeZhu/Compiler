#include <iostream>
using namespace std;
int main()
{
    int a, b, i, t, n;
    a = 0; 
    b = 1;
    i = 1;
    cin >> n;
    cout << a << endl;
    cout << b << endl;
    while (i < n)
    {
        t = b;
        b = a + b;
        cout << b << endl;
        a = t;
        i = i + 1;
    }
}