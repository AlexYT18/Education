#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#define ALF 26
struct N
{
	unsigned short N_gr;
	unsigned short N_sp; 
	unsigned short F_z; 
};
void Shifr_Cezaria(char* input, char* output, int k)
{
	char symbol;
	int i = 0;
	while (input[i] != '\0')
	{
		symbol = input[i];
		if (symbol >= 'a' && symbol <= 'z')
		{
			symbol = ((symbol - 'a' + k) % ALF) + 'a';
		}
		else if (symbol >= 'A' && symbol <= 'Z')
		{
			symbol = ((symbol - 'A' + k) % ALF) + 'A';
		}
		output[i] = symbol;
		i++;
	}
	i = 0;
	printf("ROT%d: ", k);
	while (output[i] != '\0')
	{
		printf("%c", output[i]);
		i++;
	}
}
int Evklida(int a, int b)
{
	a = abs(a);
	b = abs(b);
	while (b) 
	{
		while (a >= b)
			a -= b;
		if (!a)
			return b;
		do {
			b -= a;
		} while (b >= a);
	}
	return a;
}
bool svo1(int N, int a)
{
	if (N % a != 0) return true;//N Не делится на а
	else return false;//N делится на а
}
bool svo2(int a, int t, int N, int s)
{
	for (int k = 0; k < s; k++)
	{
		if (pow(a, t * pow(2, k - 1)) == ((-1) % N)) return true;
		else return false;
	}
}
int punkt_1(struct N n)
{
	unsigned long long a = pow((n.N_gr + n.N_sp), 11);
	unsigned long long b = pow(2, 11);
	printf("\n Номер учебной группы : %d\n", n.N_gr);
	printf(" Порядковый номер в группе : %d\n", n.N_sp);
	printf(" Фамилия студента : Цебро\n");
	printf(" X = %d\n", (a + n.F_z) % 11);
	return 0;
}
int punkt_2(struct N n)
{
	int k = (n.F_z + n.N_gr + n.N_sp) % ALF, A;
	char input[100], output[100];
	printf("ROT0: ");
	fgets(input, 100, stdin);
	Shifr_Cezaria(input, output, k);
	return 0;
}
int punkt_3(struct N n)
{
	int B = 17122004, A = pow(n.N_gr * (8 + n.N_sp % 7), 2);
	printf("1) %d\n", Evklida(A, (B % 95 + 900)));
	printf("2) %d\n", Evklida(A, ((B + 50) % 97 + 700)));
	int a_1 = Evklida(A, ((B + 20) % 101 + 1500)), b_1 = Evklida(A, ((B - 40) % 103 + 2500));
	printf("3) %d\n",Evklida(a_1,b_1));
	return 0;
}
int punkt_4()
{
	int N = 4, N_1 = 521, t = 53, s = 9;//27137 -1 = 2^9 * 53 ; t = 53; s = 9 (явл состовным)
	for (int a = 2; a < N; a++)
	{
		if (svo2(a, t, N, s))
		{
			continue;
		}
		else
		{
			printf("Значит число %d простое", N);
			break;
		}
		printf("Число %d составное", N);
	}

	return 0;
}
int punkt_5()
{
	int p = 127, q = 227, n = p * q, f = (p - 1) * (q - 1), e = 101;
	//Вручную вычислить d и расписать
	int d;
	for (d = 1; ; d++) {
		if (e * d % f == 1 && Evklida(d, f) == 1)  break;
	}
	printf("N: %d e: %d d: %d\n", n, e, d);
	return 0;
}
int punkt_9()
{
	int ckey[8] = { 1,3,7,12,27,52,132,264 };
	int okey[8];
	int bincode[8] = { 0 };
	char output[101] = { 'a' };
	int n = 23, m = 501, n_1, shifrogram = 0, deshifrogram[100] = { 0 }, result[100] = { 0 };
	for (int i = 1;; i++)
	{
		if (n * i % m == 1)
		{
			n_1 = i; 
			break;
		}
	}
	printf("Закрытый ключ ki: ");
	for (int i = 0; i < 8; i++) printf("\'%d\' ", ckey[i]);
	printf("\nn = %d; m = %d; n^(-1) = %d\n", n, m, n_1);
	printf("Открытый ключ (ki*n) mod m: ");
	for (int i = 0; i < 8; i++)
	{
		okey[i] = ckey[i] * n % m;
		printf("\'%d\' ", okey[i]);
	}
	int rezim = 0;
	int sum = 0;
	printf("\nВыберите режим: \n1)Зашифровка сообщения\n2)Расшифровка сообщения\n");
	scanf("%d", &rezim);
	int length = 0;
	char input[101] = { 'a' };
	if (rezim == 1)
	{
		printf("Введите количество букв\n");
		scanf("%d", &length);
		printf("Введите текст для шифрования: \n");
		scanf("%s", &input);
		for (int i = 0; i < length; i++)
		{
			unsigned int a = input[i];
			printf("Буква в ASCII-кодировке: %d\n", a);
			for (int j = 7; j != -1; j--)
			{
				bincode[j] = a % 2;
				a = a / 2;
			}
			printf("Бинарный код буквы %c:\n", input[i]);
			for (int j = 0; j < 8; j++)
			{
				printf("%d", bincode[j]);
				if (j == 3)printf(" ");
			}
			int j = 0;
			for (int k = 0; k < 8; k++)
			{
				if (bincode[j] == 1)
				{
					shifrogram += okey[k];
				}
				j++;
			}
			printf("\nШифрограмма: %d\n", shifrogram);
			shifrogram = 0;
		}
	}
	else
	{
		printf("Введите количество букв:\n");
		scanf("%d", &length);
		printf("Введите шифрограммы через пробел:\n");
		for (int i = 0; i < length; i++)
		{
			scanf("%d", &deshifrogram[i]);
		}
		for (int i = 0; i < length; i++)
		{
			result[i] = deshifrogram[i] * n_1 % m;
			printf("Суммарный вес %d символа: %d\n", i + 1, result[i]);
		}
		for (int j = 0; j < length; j++)
		{
			for (int i = 7; i != -1; i--)
			{
				if (result[j] >= ckey[i])
				{
					result[j] -= ckey[i];
					bincode[i] = 1;
				}
				else
				{
					bincode[i] = 0;
				}
			}
			printf("Бинарный код для %d буквы: ", j + 1);
			for (int i = 0; i < 8; i++)
			{
				printf("%d", bincode[i]);
				if (i == 3)
				{
					printf(" ");
				}
			}
			printf("\nБуква в ASCII-кодировке: ");
			
			int st = 0;
			for (int i = 7; i != -1 ; i--)
			{
				if (bincode[i] == 1)
				{
					sum = sum + (int)(pow(2, st) + 0.5);
					st++;
				}
			}
			printf("%d\n", sum);
			printf("Полученная буква при дешифровании: %c\n\n", sum);
			output[j] = (char)sum;
		}
		//printf("Полученный текст: %s", output);
	}
	return 0;
}
void nachalo_raboti()
{
	struct N n = { 1,32,2 };
	punkt_1(n);//В отчет занесено 
	punkt_2(n);//В отчет занесено
	punkt_3(n);//В отчет занесено
	punkt_4();//В отчет занесено
	punkt_5();//В отчет занесено
	punkt_9();//В отчет занесено
} 
int main()
{
	setlocale(LC_ALL, "Rus");
	nachalo_raboti();
	return 0;
}
