#include <stdio.h>
#include <math.h>
#define M_PI 3.14159265358979323846

long double factorial_new(long double x,int i)
{
	long double x0 = x;
	int stepen = (2 * i + 1);
	int konec = (2 * i - 2);
	int n = 1;
	while (n <= konec)
	{
		x *= x0;
		x = x / (n * n);
		n++;
	}
	while (n < stepen)
	{
		x *= x0;
		n++;
	}
	x = x / (2 * i - 1);
	return x;
}
int main()
{
	long double chislitel_1, itogf, x, promz_2, cosd, sind;
	long double sum_f = 0, e = 0.0001, starsumm = 0;
	scanf_s("%Lf", &x);
	for (int i = 1;; i++) 
	{
		if (i % 6 == 0)
		{
			continue;
		}
		sind = sin(i * M_PI / 6);
		cosd = cos(i * M_PI / 3);
		if (i%2 == 0)
		{
			chislitel_1 = 1;
		}
		else
		{
			chislitel_1 = -1;
		}

		promz_2 = (factorial_new(x, i));
		itogf = cosd * sind*chislitel_1*promz_2;
		sum_f = sum_f + itogf;
		printf("I %5.d | itogf = %15.12f | sumf %15.12f\n", i, itogf,sum_f);//(sum_f - starsumm)
		starsumm = sum_f;
		if (i == 40)
		{
			break;
		}
	}
}