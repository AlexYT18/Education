#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
int n, l, k;
int doska[25][25];
int da_net = 0;
FILE* file1;
FILE* file2;
int Proverka_na_postanovku(int x, int y)
{
    //Проверка по горизонтали и вертикали
    for (int i = 0; y - i >= 0; i++)
    {
        if (doska[x][y - i] == 1)
        {
            return 0;
        }
    }
    for (int i = 0; y + i < n; i++)
    {
        if (doska[x][y + i] == 1)
        {
            return 0;
        }
    }
    for (int i = 0; x - i >= 0; i++)
    {
        if (doska[x - i][y] == 1)
        {
            return 0;
        }
    }
    for (int i = 0; x + i < n; i++)
    {
        if (doska[x + i][y] == 1)
        {
            return 0;
        }
    }
    //Проверка по диагоналям
    for (int i = 0; (x - i >= 0) && (y - i >= 0); i++)
    {
        if (doska[x - i][y - i] == 1)
        {
            return 0;
        }
    }
    for (int i = 0; (x - i >= 0) && (y + i < n); i++)
    {
        if (doska[x - i][y + i] == 1)
        {
            return 0;
        }
    }
    for (int i = 0; (x + i < n) && (y - i >= 0); i++)
    {
        if (doska[x + i][y - i] == 1)
        {
            return 0;
        }
    }
    for (int i = 0; (x + i < n) && (y + i < n); i++)
    {
        if (doska[x + i][y + i] == 1)
        {
            return 0;
        }
    }
    return 1;
}
void reshala(int x, int y, int queen)
{
    int znach;
    while (y < n && x < n)
    {
        if (Proverka_na_postanovku(x, y) == 1)
        {
            doska[x][y] = 1;
            if (queen < l)
            {
                reshala(x, y, queen + 1);
                
            }
            znach = 0;
            for (int i = 0; i < n; i++)
            {
                for (int j = 0; j < n; j++)
                {
                    if (doska[i][j] == 1)
                        znach++;
                }
            }
            if (znach == l + k)
            {
                for (int i = 0; i < n; i++)
                {
                    for (int j = 0; j < n; j++)
                    {
                        if (doska[i][j] == 1)
                        {
                            fprintf(file2, "(%d,%d)", i, j);
                        }
                    }
                }
                fprintf(file2, "\n");
                da_net = 1;
            }
            doska[x][y] = 0;
        }
        y++;
        if (y == n)
        {
            y = 0;
            x++;
        }
    }
}
int main()// в (6,14) почему-то не ставится фигура (вообще в 6 не ставится)
{
    int x, y;
    file1 = fopen("input.txt", "r");//Ввол данных
    fscanf(file1, "%d %d %d", &n, &l, &k);
    for (int i = 0; i < k; i++)
    {
        fscanf(file1, "%d %d", &x, &y);
        doska[x][y] = 1;
    }
    fclose(file1);
    file2 = fopen("output.txt", "w");
    reshala(0, 0, 1);
    fclose(file2);
    if (da_net == 0)//Вывод нет результата
        file2 = fopen("output.txt", "w");
    fprintf(file2, "no solutions");
    fclose(file2);
    return 0;
}