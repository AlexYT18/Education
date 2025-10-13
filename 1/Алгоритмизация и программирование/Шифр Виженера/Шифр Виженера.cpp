#define _CRT_SECURE_NO_WARNINGS
#include <locale.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <Windows.h>
#include <iostream>
#include <cmath>
// этот массив вроде бесполезен
const double p[32] = { 0.0896, 0.0197, 0.0400, 0.0153, 0.0280, 0.0856, 0.0064, 0.0193, 0.0739, 0.0094, 0.0322, 0.0358,
0.0417, 0.0662, 0.0940, 0.0421, 0.0561, 0.0554, 0.0611, 0.0179, 0.0034, 0.0093, 0.0087, 0.0118, 0.0032, 0.0048, 0.0002,
0.0225, 0.0125, 0.0033, 0.0063, 0.0243 };
// сделайте ещё все слова, а не только из 7 букв + придётся границы подправитcь
const char dict[20][20] = { "АААММММ","РЕДИСКА","ШУМАХЕР","ПАРОВОЗ","АВГУСТ","ОМЛЕТНК","ОПАХАТЬ","ЧАКАЛОВ",};
//typedef unsigned long long ull;
//
//ull rdtsc() {
//    unsigned int lo, hi;
//    _asm volatile ("rdtsc\n" : "=a" (lo), "=d" (hi));
//    return ((ull)hi << 32) | lo;
//}
// Отладочная функция для проверки символов
void check_characters(FILE* code) {
    int sym;
    printf("Checking characters in the file:\n");
    while ((sym = fgetc(code)) != EOF) {
        if (sym < 192 || sym > 223) {
            printf("Unexpected character: %c (%d)\n", sym, sym);
        }
    }
    rewind(code);
}
// Поиск длинны ключа
int find_key_len(FILE* code) {
    double ind;
    int n, sym, c, flag;
    int count[32] = { 0 };
    for (int i = 3; i <= 20; i++) { // i - предполагаемая длинна ключа
        flag = 0;
        for (int j = 0; j < i; j++) { // j - номер группы, count - количество каждой буквы в слове
            fseek(code, 0, SEEK_END);
            n = (ftell(code) / 2) / i; // количество букв в группе
            if (j < ((ftell(code) / 2) % i)) {
                n++;
            }
            fseek(code, 0, SEEK_SET);
            c = 0; memset(count, 0, 32 * sizeof(int));
            //192 223
            while ((sym = fgetc(code)) != EOF) {
                if (sym >= 192 && sym <= 223) {
                    if (c == j) {
                        count[sym - 192]++;
                    }
                    c++; if (c == i) c = 0;
                }
            }
            // подсчёт индекса совпадений
            ind = 0;
            for (int f = 0; f < 32; f++) {
                ind += (count[f] * (count[f] - 1));
            }
            ind /= (n * (n - 1));
            if (ind >= 0.20 && ind <= 0.25) {
                flag++;
            }
        }
        if (flag!= 0 && i > 3) return i;
    }
    return 1;
}
// все переменные соответсвуют именами с формулами
// реализация 1-ой формулы из статьи
double pk1(int y, int x, int k, int* n, int* f) {
    double res = 1.0;
    for (int i = (n[k] - f[32 * k + y] + 1); i <= n[k]; i++) {
        res *= (i * p[x] / (i - (n[k] - f[32 * k + y])));
    }
    for (int i = 0; i < (n[k] - f[32 * k + y]); i++) {
        res *= (1 - p[x]);
    }
    return res;
}

// реализация 2-ой формулы из статьи
double pk2(int x, int y, int k, double* pk1) {
    double res = 0.0;
    for (int i = 0; i < 32; i++) {
        // res += pk1(y, i, k, n, f);
        res += pk1[32 * 32 * k + 32 * i + y];
    }
    return pk1[32 * 32 * k + 32 * x + y] / res;
}

// реализация 3-ой формулы из статьи
double pklr1(int k, int l, int r, double* pk2) {
    double res1 = 1.0, res2 = 0.0;
    for (int y = 0; y < 32; y++) {
        res2 = 0.0;
        for (int x = 0; x < 32; x++) {
            // res2 += (pk2(x, y, k, n, f) * pk2((x + r) % 32, y, l, n, f));
            res2 += (pk2[32 * 32 * k + 32 * x + y] * pk2[32 * 32 * l + 32 * ((x + r) % 32) + y]);
        }
        res1 *= res2;
    }
    return res1;
}
// реализация 4-ой формулы из статьи (используется таблица с результатами 3-ей формулы)
double pklr2(int k, int l, int r, double* pklr1, int key_len) {
    double res = 0.0;
    for (int s = 0; s < 32; s++) {
        res += pklr1[key_len * key_len * s + key_len * k + l]; // тут кароч трёхмерный массив, но сделан как одномерный \
                        по сути это pklr[s][k][l]
    }
    return pklr1[key_len * key_len * r + key_len * k + l] / res;
}
double p1(int x, int y, int key_len, int* key, int* n, int* f)
{
    double log_res = 0.0; // Работаем в логарифмах

    for (int k = 0; k < key_len; k++) {
        int shifted_index = (x + key[k]) % 33;
        double pk_value = pk1(y, shifted_index, k, n, f);

        if (pk_value <= 0.0) {
            return 0.0;
        }

        log_res += log(pk_value);
    }
    return exp(log_res);
}
double p2(int x, int y, int key_len, int* key, int* n, int* f) {
    double log_sum_prob = -INFINITY; // Логарифм суммы вероятностей
    double log_target_prob = log(p1(x, y, key_len, key, n, f));
    double temp_prob = 0.0;

    for (int m = 0; m < 32; m++) {
        temp_prob = p1(m, y, key_len, key, n, f);

        if (temp_prob > 0.0) {
            log_sum_prob = log(exp(log_sum_prob) + temp_prob); // Сложение логарифмов
        }
    }
    if (log_sum_prob == -INFINITY) {
        return 0.0;
    }
    double normalized_prob = exp(log_target_prob - log_sum_prob);
    return normalized_prob;
}
//double p1(int x, int y, int key_len, int* key, int* n, int* f) {
//    double res = 1.0;
//    for (int k = 0; k < key_len; k++) {
//        res *= pk1(y, (x + key[k]) % 32, k, n, f);
//    }
//    return res;
//}
//double p2(int x, int y, int key_len, int* key, int* n, int* f) {
//    double res = 0.0;
//    for (int m = 0; m < 32; m++) {
//        res += p1(m, y, key_len, key, n, f);
//    }
//    return p1(x, y, key_len, key, n, f) / res;
//}

void process_alphabet(int* alph, int key_len, int* key, int* n, int* f) 
{
    double flag = 0;
    for (int y = 0; y < 32; y++) {
        double best = 0;
        int res = 0;
        for (int x = 0; x < 32; x++) {
            flag = p2(x, y, key_len, key, n, f);
            if (flag > best) 
            {
                best = flag;
                res = x;
            }
        }
        alph[y] = res;
    }
}
int main(int argc, char const* argv[])
{
    //ull start1 = rdtsc();;
    setlocale(LC_ALL, "rus");
    FILE* code, * message;
    code = fopen("code.txt", "rb");
    message = fopen("message.txt", "wb");
    if (!code || !message) //Проверка открытия файлов
    {
        printf("Error opening files!\n");
        return 1;
    }
    check_characters(code);//Проверка символов
    int key_len = find_key_len(code);
    printf("Длинна ключа: %d\n", key_len);
    fseek(code, 0, SEEK_SET);
    int* n = (int*)calloc(key_len, sizeof(int)); // количество букв в каждой группе
    int* f = (int*)calloc(key_len * 32, sizeof(int)); // количество каждой буквы в каждой группе
    int c = 0, sym;
    // подсёт n и f
    while ((sym = fgetc(code)) != EOF) {
        if (sym >= 192 && sym <= 223) {
            n[c]++;
            f[c * 32 + sym - 192]++;
            c++; if (c == key_len) c = 0;
        }
    }

    double* table_pk1 = (double*)calloc(32 * 32 * key_len, sizeof(double));
    for (int k = 0; k < key_len; k++) {
        for (int x = 0; x < 32; x++) {
            for (int y = 0; y < 32; y++) {
                table_pk1[32 * 32 * k + 32 * x + y] = pk1(y, x, k, n, f);
            }
        }
    }


    double* table_pk2 = (double*)calloc(32 * 32 * key_len, sizeof(double));
    for (int k = 0; k < key_len; k++) {
        for (int x = 0; x < 32; x++) {
            for (int y = 0; y < 32; y++) {
                table_pk2[32 * 32 * k + 32 * x + y] = pk2(x, y, k, table_pk1);
            }
        }
    }

    double* table_pklr1 = (double*)calloc(32 * key_len * key_len, sizeof(double)); // массив одномерный, но заполняется как трёхмерный \
            типа table_pklr1[r][k][l] далее для подобных оптимизаций их удобно использовать так, так что разберитесь с этим
            // заполнение таблицы для 3-ей формулы 
    for (int r = 0; r < 32; r++) {
        for (int k = 0; k < key_len; k++) {
            for (int l = 0; l < key_len; l++) {
                table_pklr1[key_len * key_len * r + key_len * k + l] = pklr1(k, l, r, table_pk2);
            }
        }
    }

    double* table_pklr2 = (double*)calloc(32 * key_len * key_len, sizeof(double)); // массив одномерный, но заполняется как трёхмерный \
            типа table_pklr2[r][k][l] далее для подобных оптимизаций их удобно использовать так, так что разберитесь с этим
            // заполнение таблицы для 4-ой формулы 
    for (int r = 0; r < 32; r++) {
        for (int k = 0; k < key_len; k++) {
            for (int l = 0; l < key_len; l++) {
                table_pklr2[key_len * key_len * r + key_len * k + l] = pklr2(k, l, r, table_pklr1, key_len);
            }
        }
    }
    double sum, best = 0.0;
    int res = 0, o = 0;
    int a = 0;
    int t[7][7] = { 0 }, best_t[7][7] = { 0 };
    double b = 0.0;
    // поиск ключа из словаря
    for (int i = 0; i < 20; i++) {
        sum = 0.0;
        memset(t, 0, 49 * sizeof(int));
        for (int k = 0; k < key_len; k++) {
            for (int l = k + 1; l < key_len; l++) {
                a = dict[i][2 * k + 1] - dict[i][2 * l + 1]; // сдвиг между буквами k и l 
                if (a < 0) {
                    a = 32 + a;
                }
                b = table_pklr2[key_len * key_len * a + key_len * k + l];//Что-то здесь
                // необязательная часть которая ищет таблицу сдвигов для ключа (та которая треугольная из статьи)
                 o = 0;
                 for(int r = 0; r < 32; r++){
                     if (b < pklr2(k, l, r, table_pklr1, key_len)){
                         o++;
                     }
                 }
                 t[k][l] = o + 1;
                
                sum += b;
            }
        }
        if (sum > best) {
            memcpy(best_t, t, 49 * sizeof(int));// тоже часть необязательной фигни
            best = sum;
            res = i;
        }
    }
    
    // вывод ключа
    printf("%.14s\n", dict[res]);
    // формирование списка сдвигов
    int* key = (int*)calloc(key_len, sizeof(int));
    for (int i = 0; i < key_len; i++) {
        //key[i] = dict[res][1] - dict[res][2 * i + 1];
        key[i] = (dict[res][1] - dict[res][2 * i + 1] + 32) % 32;
        if (key[i] < 0) {
            key[i] += 32;
        }
        printf("%d ", key[i]);
    }
    printf("\n");
     //вывод необязательной таблицы
     for (int k = 0; k < key_len; k++){
         for (int l = 0; l < key_len; l++){
             if (l < k){
                 printf("   ");
             }else if (l > k){
                 printf("%2d ", best_t[k][l]);
             }
         }
         printf("\n");
     }
    //поиск перестановки алфавита
    int alph[32] = { 0 };
    //double flag = 0.0;
    //for (int y = 0; y < 32; y++) {
    //    best = 0;
    //    res = 0;
    //    for (int x = 0; x < 32; x++) {
    //        flag = p2(x, y, key_len, key, n, f);//Там res очень мал
    //        if (flag > best) {
    //            best = flag;
    //            res = x;
    //        }
    //    }
    //    alph[y] = res;
    //}
    //// это чисто для понтов
    //int perm[32] = { 0 };
    //for (int i = 0; i < 32; i++) {
    //    perm[alph[i]] = i;
    //}
    //// вывод штуки для понтов
    //for (int i = 0; i < 32; i++) {
    //    printf("%c%c", 208, perm[i] + 192);
    //}
    //printf("\n");
    process_alphabet(alph, key_len, key, n, f);
    //НЖРШФТОЫЮЪЛЬЕКЦАХМВЙБСЩПЯЭИЧУЗДГ

    fseek(code, 0, SEEK_SET);
    c = 0;
    while ((sym = fgetc(code)) != EOF) 
    {
        fputc(192 + (alph[sym - 192] + key[c]) % 32, message);
        c++;
        if (c == key_len) c = 0;

    }
    fclose(code);
    fclose(message);
    free(n);
    free(f);
    free(key);
    return 0;

}
