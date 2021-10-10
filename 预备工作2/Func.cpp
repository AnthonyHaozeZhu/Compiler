#include <iostream>
using namespace std;
int square(int a){
	int m;
	m = a * a;
	return m;
}
int main()
{
	int a, s_a;
	cin >> a;
	s_a = square(a);
	cout << s_a << endl;
	return 0;
}
