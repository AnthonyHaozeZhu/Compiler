#include <iostream>
using namespace std;
int main()
{
    int a, b, i, t, n;
    a = 0; 
    b = 1;
    i = 1;
    cin >> n;
    cout << "a:" << a << endl;
    cout << "b:" << b << endl;
    cout << "we are going to loop now! " << endl;
    while (i < n)
    {
        t = b;
        b = a + b;
        cout << b << endl;
        a = t;
        i = i + 1;
    }
}
