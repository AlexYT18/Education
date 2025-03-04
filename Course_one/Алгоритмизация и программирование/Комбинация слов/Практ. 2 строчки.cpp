#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
char alp[41] = { "bcdfghjklmnpqrstvwxz" };
int proverka_na_sogl(char s)
{
	for (int i = 0; i < 41; i++)
	{
		if (s == alp[i])
		{
			return 1;
		}
	}
	return 0;
}
int prov_na_bolsh(int i, char str[]) 
{
	if ( (i-1) >= 0 && proverka_na_sogl(str[i - 1]) == 1)
	{
		if (str[i] > str[i - 1]) 
		{
			return 1;
		}
	}
	else 
	{
		return 1;
	}
	return 0;
}
void nahoz_maximuma(char str1[], int* maxi, int* max) {
	int koon = 0, ki = 0;
	for (int i = 0; i < strlen(str1); i++) {
		if ((proverka_na_sogl(str1[i]) == 1) && (prov_na_bolsh(i, str1) == 1)) 
		{
			koon++;
			if (koon == 1) 
			{
				ki = i;
			}
			if (koon > *max) 
			{
				*max = koon;
				*maxi = ki;
			}
		}
		else 
		{
			koon = 0;
		}
	}
}
char* func(char str1[], char str2[], char res[]) {
	int koon = 0, ki = 0, max1 = 0, maximum_str1 = -1, maximum_str2 = 1, maxi2 = -1;
	nahoz_maximuma(str1, &maximum_str1, &max1);
	nahoz_maximuma(str2, &maxi2, &maximum_str2);
	for (int i = 0; i < maximum_str1; i++)
	{
		res[i] = str1[i];
	}
	int dachetu;
	for (int j = maximum_str1; (j - maximum_str1) < maximum_str2; j++) 
	{
		res[j] = str2[j - maximum_str1 + maxi2];
		dachetu = j + 1;
	}
	int i = maximum_str1 + max1;
	while (i < strlen(str1)) 
	{
		res[dachetu] = str1[i];
		dachetu++;
		i++;
	}
	res[dachetu] = '\0';
	return res;
}
int main() 
{
	char str1[256], str2[256], c[513];
	while (true)
	{
		scanf("%s", str1);
		scanf("%s", str2);
		printf("%s\n", func(str1, str2, c));
	}
}