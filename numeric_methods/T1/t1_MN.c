#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <fenv.h>

void exceptions (){
    printf(" FE_DIVBYZERO : ");
    if (fetestexcept(FE_DIVBYZERO) != 0) printf("1\n");
    else printf("0\n");
    printf(" FE_INEXACT   : ");
    if (fetestexcept(FE_INEXACT) != 0) printf("1\n");
    else printf("0\n"); 
    printf(" FE_INVALID   : ");
    if (fetestexcept(FE_INVALID) != 0) printf("1\n");
    else printf("0\n");
    printf(" FE_OVERFLOW  : ");
    if (fetestexcept(FE_OVERFLOW) != 0) printf("1\n");
    else printf("0\n");
    printf(" FE_UNDERFLOW : ");
    if (fetestexcept(FE_UNDERFLOW) != 0 ) printf("1\n");
    else printf("0\n");
    feclearexcept(FE_ALL_EXCEPT);
}

uint32_t float_to_ieee754(float f)
{
    uint32_t *ptr = (uint32_t *)&f;
    return *ptr;
}

void Hex_to_bin(uint32_t num, float n)
{

    for (int i = 31; i >= 0; --i)
    {
        if (i == 30 || i == 22) printf(" ");
        printf("%"PRIu32, num >> i & 1);
    }
    printf(" = %g\n",n);
}

int main()
{
    char c;
    float a, b, res;
    scanf("%f %c %f", &a, &c, &b);
    uint32_t ieee_a = float_to_ieee754(a);
    uint32_t ieee_b = float_to_ieee754(b);
    switch (c)
    {
    case '+':
        res = a + b;
        break;

    case '-':
        res = a - b;
        break;

    case 'x':
        res = a * b;
        break;
    case 'X':
        res = a * b;
        break;

    case '/':
        res = a / b;
        break;

    default:
        printf("caractere invalido");
    }

    uint32_t ieee_res = float_to_ieee754(res);

    printf("%f %c %f = %f \n\n", a, c, b, res);

    printf("IEEE 754 representation\n");
    printf("a   = ");
    Hex_to_bin(ieee_a,a);

    printf("b   = ");
    Hex_to_bin(ieee_b,b);

    printf("res = ");
    Hex_to_bin(ieee_res,res);

    printf("\nExceptions:\n");
    exceptions();
     
}