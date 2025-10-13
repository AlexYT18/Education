#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#define _CRT_SECURE_DEPRECATE_MEMORY
#include <memory.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <intrin.h>
#pragma intrinsic(__rdtsc)
#define N_max  100000
unsigned __int64 mas_time = 0, spis_time = 0;
typedef struct spisok
{
	int count;
	spisok* next;
};
void sortmas(int* chislo_mas, int N);
spisok* sort(spisok* base, int N)
{
	spisok* poshla, * now, * last, * last_last;
	poshla = (base)->next;
	last = (base);
	last_last = (base);
	unsigned __int64 end2;
	end2 = __rdtsc();

	for (int i = 0; i < N; i++)
	{
		now = (base);
		while (now != poshla)
		{
			if (now->count > poshla->count)
			{
				last->next = poshla->next;
				poshla->next = now;
				if (now == base) base = poshla;
				else last_last->next = poshla;
				poshla = last;
				break;
			}
			last_last = now;
			now = now->next;
		}
		last = poshla;
		poshla = poshla->next;
	}
	end2 = __rdtsc() - end2;
	spis_time += end2;
	return base;
}
void lin_spis_fill(int* vse_shisla, spisok* all_data, int N, int col)
{
	spisok* last_data = all_data;
	for (int i = 0; i < N; i++)
	{
		last_data->count = vse_shisla[i];
		last_data = last_data->next;
	}
}
void lin_spisok_create(spisok* all_data)
{
	spisok* first, * second;
	all_data->next = NULL;
	all_data->count = 0;
	first = all_data->next;
	second = all_data;
	for (int i = 0; i < N_max; i++)
	{
		first = (spisok*)malloc(sizeof(spisok));;
		first->next = NULL;
		first->count = 0;
		second->next = first;
		second = second->next;
	}
}
void free_spisok(spisok* all_data)
{
	spisok* first, * second;
	first = all_data;
	while (first != NULL)
	{
		second = first->next;
		free(first);
		first = second;
	}
}
void gen_massiv(int* massiv, int N)
{
	srand(time(NULL));
	for (int i = 0; i < N; i++)
	{
		massiv[i] = rand();
	}
}
int main()
{
	int* vse_shisla = (int*)calloc(N_max, sizeof(int)), * massiv = (int*)calloc(N_max, sizeof(int));
	spisok* all_data = (spisok*)malloc(sizeof(spisok));
	lin_spisok_create(all_data);
	
	for (int N = 2500; N <= N_max; N += 2500)
	{
		gen_massiv(vse_shisla, N);//Заполняем числа в массив
		for (int i = 0; i < 10; i++)
		{
			for (int jn = 0; jn < N; jn++) { massiv[jn] = vse_shisla[jn]; }//Заполняем массив
			lin_spis_fill(massiv, all_data, N, i);//Заполнение линейного списка
			sortmas(massiv, N);//Сортировка массива
			all_data = sort(all_data, N);
		}
		std::cout << "N" << N << std::endl;
		std::cout << "Time sort massiv(middle):" << mas_time/10 << std::endl;
		std::cout << "Time sort spisok(middle):" << spis_time / 10 << std::endl;
		std::cout << "-----" << std::endl;
		mas_time = 0;
		spis_time = 0;
	}
	free(massiv);
	free_spisok(all_data);
	return 0;
}
//Постфиксы и суфиксы в 4 лабе при воде(++. --)
void sortmas(int* chislo_mas, int N)
{
	int now, last;
	unsigned __int64 end1;
	end1 = __rdtsc();
	for (int i = 1; i < N; i++)
	{
		now = chislo_mas[i];
		last = i - 1;
		while (last >= 0 && chislo_mas[last] > now)
		{
			chislo_mas[last + 1] = chislo_mas[last];
			chislo_mas[last] = now;
			last--;
		}
		chislo_mas[last + 1] = now;
	}
	end1 = __rdtsc() - end1;
	mas_time += end1;
}