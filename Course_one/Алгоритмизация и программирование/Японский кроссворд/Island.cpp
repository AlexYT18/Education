#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define N 10
#define len 100
#define X 5

int total_line_checking(char vert_string[N], int arr_horiz[N]) {
	int arr[X] = { 0 }; // показывает проверяемую строку в виде чисел 
	int ind_field_string = 0;
	int ind_nums_arr = 0;
	int kol_sim = 0;
	while (vert_string[ind_field_string] != '$') {

		if (vert_string[ind_field_string] == '#') {
			kol_sim = 0;
			while (vert_string[ind_field_string] != '_' && vert_string[ind_field_string] != '$') {
				kol_sim++;
				ind_field_string++;
			}
			arr[ind_nums_arr] = kol_sim;
			ind_nums_arr++;
			if (vert_string[ind_field_string] == '$')
				break;
		}
		ind_field_string++;
	}

	int i = 0;

	while (arr_horiz[i] != 0) {
		if (arr_horiz[i] != arr[i])
			return 0;
		i++;
	}
	if (arr_horiz[i] == 0 && arr[i] != 0)
		return 0;
	return 1;
}

int total_check_field(char field[N][N], int arr_horiz[N][X], int size, char arr_perever[N]) {
	int i = 0;
	while (i < N - 1) {
		arr_perever[i] = '_';
		i++;
	}
	arr_perever[i - 1] = '$';
	int x = 0;
	int y = 0;
	while (y < size) {
		x = 0;
		while (x < size) {
			arr_perever[x] = field[x][y];
			x++;
		}
		if (!(total_line_checking(arr_perever, arr_horiz[y]))) {
			return 0;
		}
		y++;
	}
	return 1;
}

void win(char field[N][N], int size, FILE *f2) {
	int x = 0;
	int y = 0;

	while (x < size) {
		y = 0;
		while (y < size) {
			if (field[x][y] == '#') {
				printf("* ");
				fprintf(f2, "* ");
			}
			else if (field[x][y] == '_') {
				printf("  ");
				fprintf(f2, "  ");
			}
			y++;
		}
		x++;
		printf("\n");
		fprintf(f2, "\n");
	}
}

// функция, которая находит номер острова, который можно передвинуть вправо
// причем это максимально правый остров
// вывод 0 означает, что таких островов нет
int island_seeker_to_right_step(char field[N]) {
	int num = 0;
	int ind = 0;
	int num_island = 0;
	while (field[ind] != '$') {
		if (field[ind] == '#') {
			while (field[ind] != '_' && field[ind] != '$') {
				ind++;
			}
			num++;
			if (field[ind] == '$')
				break;
			if (field[ind + 1] == '_' || field[ind + 1] == '$')
				num_island = num;
		}
		ind++;
	}
	return num_island;
}

// проверка на то, можем ли мы делать шаг вправо или нет
// вывод функции: 1 - можем, 0 - нет
int check_to_right_step(char field[N], int num) {
	// определение индекса после определенного номера острова num
	int i = 0; // счетчик островов
	int j = 0; // счетчик символов
	while (i < num) {
		if (field[j] == '#') {
			while (field[j] != '_' && field[j] != '$') {
				j++;
			}
			i++;
			if (field[j] == '$')
				return 0;
		}
		j++;
	}
	j--;
	// j - индекс _ полсле #
	if (field[j] == '$')
		return 0;
	if (field[j + 1] == '#')
		return 0;
	return 1;
}

// проверяем строку на случай, когда острова принимают максимально правое положение
int end_of_perebor(char field_vert_string[N]) {
	int ind = 0; // индекс сивола в строке
	int fl = 1; // флаг поменяется, когда строка пойдет неверно

	// проверка на ________$
	while (field_vert_string[ind] != '$') {
		if (field_vert_string[ind] == '_')
			ind++;
		else
			break;
	}
	if (field_vert_string[ind] == '$')
		return 1;

	// остальная проверка
	ind = 0;
	while (field_vert_string[ind] != '$') {

		if (field_vert_string[ind] == '#' && field_vert_string[ind + 1] == '_' && (field_vert_string[ind + 2] == '_' || field_vert_string[ind + 2] == '$'))
			return 0;


		if (field_vert_string[ind] == '#' && field_vert_string[ind + 1] == '$' && fl == 1)
			return 1;
		ind++;
	}
}

// _#__#_#_ num = 1  -> _#_#_#__
// num - номер острова, который остается на месте. Относительно него происходит перемещение
// если num = 0, то острова смещаются максимально влево
void all_to_left(char field[N], int num, int string[X], int kol_sim_in_string) {
	// определение индекса после определенного номера острова num
	int i = 0; // счетчик островов
	int j = 0; // счетчик символов
	while (i < num) {
		if (field[j] == '#') {
			while (field[j] != '_') {
				j++;
			}
			i++;
		}
		j++;
	}
	i = j;

	// удаление островов после num
	while (field[j] != '$') {
		field[j] = '_';
		j++;
	}

	// дозаполнение поля так, чтобы было максимально влево от определенного номера
	int z = 0;
	while (num < kol_sim_in_string) {
		z = 0;
		while (z < string[num]) {
			field[i + z] = '#';
			z++;
		}
		num++;
		i = i + z + 1;
	}
}

// num - номер острова, который мы хотим переместить вправо на клетку
// мы уже подразумеваем, что проверка на последующий индекс пройден
// поэтому не стоит делать проверку в этой функции
void sdvig_island_right_to_one_kletka(char field[N], int num) {
	// определение индекса до и после номера острова num
	int i = 0; // счетчик островов 
	int j = 0; // счетчик символов, индекс после острова
	int i_down = 0; // индекс до острова
	while (i < num) {
		if (field[j] == '#') {
			i_down = j;
			while (field[j] != '_') {
				j++;
			}
			i++;
		}
		j++;
	}
	j--;
	field[i_down] = '_';
	field[j] = '#';
}


int line_cheking(char vert_string[N], int arr_horiz[N]) {
	int arr[X] = { 0 }; // показывает проверяемую строку в виде чисел 
	int ind_field_string = 0;
	int ind_nums_arr = 0;
	int kol_sim = 0;
	while (vert_string[ind_field_string] != '$') {

		if (vert_string[ind_field_string] == '#') {
			kol_sim = 0;
			while (vert_string[ind_field_string] != '_' && vert_string[ind_field_string] != '$') {
				kol_sim++;
				ind_field_string++;
			}
			arr[ind_nums_arr] = kol_sim;
			ind_nums_arr++;
			if (vert_string[ind_field_string] == '$')
				break;
		}
		ind_field_string++;
	}

	int i = 0;

	while (arr_horiz[i] != 0) {
		if (arr_horiz[i] < arr[i])
			return 0;
		if ((arr_horiz[i] > arr[i]) && (arr[i + 1] != 0))
			return 0;
		i++;
	}
	if (arr_horiz[i] == 0 && arr[i] != 0)
		return 0;
	return 1;
}



// Проверка проходит только по вертикали, ибо горизонталь он составляет по правилам
// проверяем полученную строчку на совместимость с полем
// 0 - не прошел проверку. 1 - прошел
int field_checking(char field[N][N], int arr_horiz[N][X], int size, char arr_perever[N]) {
	int i = 0;
	while (i < N - 1) {
		arr_perever[i] = '_';
		i++;
	}
	arr_perever[i-1] = '$';
	int x = 0;
	int y = 0;
	while (y < size) {
		x = 0;
		while (x < size) {
			arr_perever[x] = field[x][y];
			x++;
		}
		if (!(line_cheking(arr_perever, arr_horiz[y]))) {
			return 0;
		}
		y++;
	}
	return 1;
}


// основная решающая функция, связывающая все остальные
// осуществляет перебор с возвратом
void trytry(char field[N][N], int arr_vert[N][X], int arr_horiz[N][X], int num_of_string, int size, char not_checked_yet_string[N], char arr_perever[N], int * megaflag, FILE * f2) {
	int a, i, j, z, kol_chisel_in_arr_vert;
	int x = 0;
	int num_seeked_island;
	int fl_for_string = 0;
	int q, w;
	int flag = 0;
	q = 0;

	x = 0;
	while (x < size) {
		not_checked_yet_string[x] = '_';
		x++;
	}
	not_checked_yet_string[size] = '$';

	do {

		//количество чисел в строке
		a = 0; // индекс массива arr_vert
		kol_chisel_in_arr_vert = 0;
		do {
			kol_chisel_in_arr_vert++;
			a = arr_vert[num_of_string][kol_chisel_in_arr_vert];
		} while (a != 0);

		if (fl_for_string == 0 && arr_vert[num_of_string][0] != 0) {
			// начальное положение
			i = 0; // номер символа в поле
			j = 0; // счетчик введенных островов
			z = 0; // счетчик введенных символов у одного острова
			while (j < kol_chisel_in_arr_vert) {
				z = 0;
				while (z < arr_vert[num_of_string][j]) {
					not_checked_yet_string[z + i] = '#';
					z++;
				}
				j++;
				i = i + z + 1;
			}
			fl_for_string = 1;
		}
		else if (arr_vert[num_of_string][0] != 0) {
			num_seeked_island = (island_seeker_to_right_step(not_checked_yet_string));
			sdvig_island_right_to_one_kletka(not_checked_yet_string, num_seeked_island);
			all_to_left(not_checked_yet_string, num_seeked_island, arr_vert[num_of_string], kol_chisel_in_arr_vert);
		}

		// запись в field
		q = 0;
		w = 0;
		while (q < size) {
			field[num_of_string][q] = not_checked_yet_string[q];
			q++;
		}

		if (field_checking(field, arr_horiz, size, arr_perever)) {
			if (num_of_string < size - 1) {
				trytry(field, arr_vert, arr_horiz, num_of_string + 1, size, not_checked_yet_string, arr_perever, megaflag, f2);

				x = 0;
				while (x < size) {
					not_checked_yet_string[x] = field[num_of_string][x];
					x++;
				}

			}
			else if (total_check_field(field, arr_horiz, size, arr_perever)) {
				win(field, size, f2);
				printf("\n");
				fprintf(f2, "\n");
				x = 0;
				while (x < size) {
					field[num_of_string][x] = '_';
					x++;
				}
				field[num_of_string][x] = '$';

				*megaflag = 1;
			}
		}

		if (end_of_perebor(not_checked_yet_string)) { //else if !(field_checking(field, arr_horiz, size, arr_perever)) &&
			//стираем 
			x = 0;
			while (x < size) {
				field[num_of_string][x] = '_';
				x++;
			}
			field[num_of_string][x] = '$';
		}

	} while (!(end_of_perebor(not_checked_yet_string)));
}



int main() {
	char field_of_islands[N][N];
	// _ - клетка пока ничем не занята, # - остров, $ - ограничение поля

	int n, i = 0, j = 0, a, x, q, w, s = 0;
	int arr_horizontal[N][X], arr_vertical[N][X];
	char not_checked_yet_string[N] = { "________$" };
	char arr_perever[N] = { "________$"};
	char str[len];
	FILE* f1;
	FILE* f2;
	int* megaflag;
	f1 = fopen("input.txt", "r");
	f2 = fopen("output.txt", "w");
	
	
	while (!(feof(f1))) {
		fscanf(f1, "%d", &n);
		printf("%d\n", n);
		i = 0;
		while (i != n) {
			j = 0;
			do {
				fscanf(f1, "%d", &a);
				arr_vertical[i][j] = a;
				j++;
				printf("%d ", a);
			} while (a != 0);
			printf("\n");
			i++;
		}

		i = 0;
		while (i != n) {
			j = 0;
			do {
				fscanf(f1, "%d", &a);
				arr_horizontal[i][j] = a;
				j++;
				printf("%d ", a);
			} while (a != 0);
			printf("\n");
			i++;
		}

		x = 0;
		while (x < n) {
			not_checked_yet_string[x] = '_';
			x++;
		}
		not_checked_yet_string[n] = '$';
		
		// поле
		q = 0;
		w = 0;
		while (q < n) {
			w = 0;
			while (w < n) {
				field_of_islands[q][w] = '_';
				w++;
			}
			field_of_islands[q][w] = '$';
			q++;
		}
		w = 0;
		while (w < n) {
			field_of_islands[n][w] = '$';
			w++;
		}


		//win_two(field_of_islands);
		s = 0;
		megaflag = &s;
		printf("Next problem\n");
		fprintf(f2, "Next problem\n");
		trytry(field_of_islands, arr_vertical, arr_horizontal, 0, n, not_checked_yet_string, arr_perever, megaflag, f2);
		if (*megaflag == 0) {
			printf("No map\n");
			fprintf(f2, "No map\n");
		}
	}

	fclose(f1);
	fclose(f2);
	return 0;
}