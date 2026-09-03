#include <iostream>
#include <math.h>
using namespace std;

int sqrt_nr(int x, int i){
    if (i <= 0) return 1;
    return (sqrt_nr(x, i-1) + (x/sqrt_nr(x,i-1)))/2;
}

int main(){
    int x=0,i=0;
    do{
        cin >> x;
        cin >> i;
        cout << "sqrt(" << x << ", " << i << ") = " << sqrt_nr(x, i) << endl;
    }while (x>=0 && i>=0);
}
