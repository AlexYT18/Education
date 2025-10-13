#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <malloc.h>
#include <Windows.h>
struct vhod
{
    unsigned long long m = pow(2, 64);//2^32 
    unsigned long long c = 13;
    unsigned long long a = 1152921504606846981;// +5
    int N = 10000;
};
unsigned long long generation(unsigned long long x, struct vhod dan)
{
    x = (dan.a * x + dan.c) % dan.m;
    return x;
}
unsigned long long generation_kr(unsigned long long x, struct vhod dan)
{
    x = (dan.a * x + dan.c);
    return x;
}
void moshnost(unsigned long long a)
{
    unsigned long long f1 = a - 1;
    unsigned long long f2 = a - 1;
    for (int i = 2; i < 9999999; i++)
    {
        f1 = f1 * f2;
        if (f1 == 0)
        {
            printf("Power LKG: %d\n", i);
            break;
        }
    }
}
void period(struct vhod dan)
{
    srand((unsigned int)time(NULL));
    unsigned long long X = rand(), x;
    x = X;
    X = dan.a * x + dan.c;
    printf("Период ЛКГ равен: \n%llu\n", X);//Доделать 
}
void hi_kvardat(unsigned long long x, struct vhod dan)
{
    int rabros_hi[10] = { 0 }, crugi = 0, n = 1000000, np = n / 10;
    unsigned long long step = dan.m / 10;
    srand(time(NULL));
    x = rand();
    double V1, V2, V3, V4, V5, V6, V7, V8, V9, V10;
    int prom1 = 0, prom2 = 0, prom3 = 0, prom4 = 0;
    for (crugi = 1; crugi <= 100; crugi++)
    {
        double v = 0.0;
        for (int i = 0; i != n; i++)
        {
            x = generation(x, dan);
            if (x >= 0 && x <= step)
            {
                rabros_hi[0]++;
            }
            if (x > step && x <= step * 2)
            {
                rabros_hi[1]++;
            }
            if (x > step * 2 && x <= step * 3)
            {
                rabros_hi[2]++;
            }
            if (x > step * 3 && x <= step * 4)
            {
                rabros_hi[3]++;
            }
            if (x > step * 4 && x <= step * 5)
            {
                rabros_hi[4]++;
            }
            if (x > step * 5 && x <= step * 6)
            {
                rabros_hi[5]++;
            }
            if (x > step * 6 && x <= step * 7)
            {
                rabros_hi[6]++;
            }
            if (x > step * 7 && x <= step * 8)
            {
                rabros_hi[7]++;
            }
            if (x > step * 8 && x <= step * 9)
            {
                rabros_hi[8]++;
            }
            if (x > step * 9 && x <= step * 10)
            {
                rabros_hi[9]++;
            }
        }
        V1 = (rabros_hi[0] - np) * (rabros_hi[0] - np);
        V1 = V1 / np;
        V2 = (rabros_hi[1] - np) * (rabros_hi[1] - np);
        V2 = V2 / np;
        V3 = (rabros_hi[2] - np) * (rabros_hi[2] - np);
        V3 = V3 / np;
        V4 = (rabros_hi[3] - np) * (rabros_hi[3] - np);
        V4 = V4 / np;
        V5 = (rabros_hi[4] - np) * (rabros_hi[4] - np);
        V5 = V5 / np;
        V6 = (rabros_hi[5] - np) * (rabros_hi[5] - np);
        V6 = V6 / np;
        V7 = (rabros_hi[6] - np) * (rabros_hi[6] - np);
        V7 = V7 / np;
        V8 = (rabros_hi[7] - np) * (rabros_hi[7] - np);
        V8 = V8 / np;
        V9 = (rabros_hi[8] - np) * (rabros_hi[8] - np);
        V9 = V9 / np;
        V10 = (rabros_hi[9] - np) * (rabros_hi[9] - np);
        V10 = V10 / np;
        v = V1 + V2 + V3 + V4 + V5 + V6 + V7 + V8 + V9 + V10;
        //printf("V = %.5lf\n", v);
        for (int i = 0; i < 10; i++) rabros_hi[i] = 0;
        if ((0 <= v && v <= 2.09) || (21.7 < v)) prom1++;
        if ((2.09 < v && v <= 3.32) || (16.92 < v && v <= 21.7)) prom2++;
        if ((3.32 < v && v <= 5.38) || (12.24 < v && v <= 16.92)) prom3++;
        if ((5.38 < v && v <= 8.34) || (8.34 < v && v <= 12.24)) prom4++;
    }
    printf("Good %d%%\n", prom4);
    printf("Almost good %d%%\n", prom3);
    printf("Suspicious %d%%\n", prom2);
    printf("Bad %d%%\n", prom1);
}
void fill(unsigned long long x, unsigned long long* massiv, struct vhod dan)
{
    for (int i = 0; i < 10000000; i++)
    {
        x = rand();
        massiv[i] = generation(x, dan);
    }
}
void kriteri_intervalov(struct vhod dan)
{
    int j = -1, s = 0, n = 10000, r = 0, t = 10, count[100] = { 0 };
    double b = 0.6;
    double a = 0.4;
    int sobir[4] = { 0 };
    unsigned long long* massiv_chisel = (unsigned long long*)calloc(10000000, sizeof(unsigned long long));
    long double umn = powl(2, -64);
    srand(time(NULL));
    unsigned long long x = rand();
    fill(x, massiv_chisel, dan);
    for (int cruck = 0; cruck != 100; cruck++)
    {
        while (s < n)//G6 с возвратом к шагу G2
        {

            r = 0;//G2
            while (true)//G3
            {
                j++;//G3
                long double prov = massiv_chisel[j] * umn;
                if (prov >= a && prov < b)//G3
                {
                    if (r >= t)//G5
                    {
                        count[t]++;
                        break;
                    }
                    else//G5
                    {
                        count[r]++;
                        break;
                    }
                }
                else
                {
                    r++;//G4
                }
            }
            s++;//G6
        }
        long double v = 0.0;
        int k = 0;
        for (int i = 0; i < t; i++)
        {
            long double pr = (b - a) * powl((1 - (b - a)), i);
            long double chislitel = powl(count[i] - n * pr, 2) / (n * pr);
            if (chislitel != 0)
            {
                k++;
                v += chislitel;
            }
        }
        n = k;
        if ((0 <= v && v <= 2.56) || (23.2 < v)) sobir[0]++;
        if ((2.56 < v && v <= 3.94) || (18.31 < v && v <= 23.2))sobir[1]++;
        if ((3.94 < v && v <= 6.18) || (13.44 < v && v <= 18.31))sobir[2]++;
        if ((6.18 < v && v <= 9.34) || (9.34 < v && v <= 13.44))sobir[3]++;//Взять для 10, тогда все верно будет
        for (int i = 0; i < 10; i++) count[i] = 0;
        s = 0;
    }
    printf("Good %d%%\n", sobir[3]);
    printf("Almost good %d%%\n", sobir[2]);
    printf("Suspicious %d%%\n", sobir[1]);
    printf("Bad %d%%\n", sobir[0]);
}
void rabotaem(struct vhod dan)
{
    srand(time(NULL));
    unsigned long long x = rand();
    moshnost(dan.a);
    //period(dan);// считается 10 месяцев.
    for (int i = 0; i < 4; i++)
    {
        printf("--- Hi-Kvadrat ---\n");
        hi_kvardat(x, dan);
        printf("--- Krit_Interval ---\n");
        kriteri_intervalov(dan);
    }
}
//void opred_a(unsigned long int m = pow(2, 64))
//{
//    for (unsigned long long a = pow(2,60);; a++)
//    {
//      if (((a) % 8 == 5) && (a >= 0.01 * m))
//        {
//            printf("%llu", a);
//            break;
//
//        }
//    }
int main()
{
    setlocale(LC_ALL, "RUS");
    vhod dan;
    printf("LKG \n");
    rabotaem(dan);
    //opred_a();
    return 0;
}