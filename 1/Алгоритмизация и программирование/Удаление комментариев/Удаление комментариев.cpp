#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void flag_symbols(char simvol1, char simvol2, int flag_in_comment,
	int flag_comment, int flag_transfer_comment, int* flag_continue,
	int flag_false_symbol,FILE* file_c, FILE* file_wc)
{
	if (simvol1 == '\\' && flag_in_comment == 0 &&
		flag_comment == 0 && flag_transfer_comment == 0 && flag_false_symbol == 0 && simvol2 != '/')
	{
		if (!feof(file_c))fputc(simvol1, file_wc);
		if (!feof(file_c))fputc(simvol2, file_wc);
		*flag_continue = 1;
	}
}
void flag_in_one_str_1(int* flag_in_one_str, int flag_comment, int flag_in_comment,
	int flag_transfer_comment, int flag_in_str, int* flag_continue, char simvol1, FILE* file_c, FILE* file_wc) {
	if (simvol1 == '\'' && *flag_in_one_str == 0 && flag_in_str == 0 &&
		flag_comment == 0 && flag_in_comment == 0 && flag_transfer_comment == 0) {
		*flag_in_one_str = 1;
		if (!feof(file_c))fputc(simvol1, file_wc);
		if (!feof(file_c))fseek(file_c, EOF, 1);
		*flag_continue = 1;
	}
}
void flag_in_one_str_0(int* flag_in_one_str, int flag_in_str, char simvol1) {
	if (simvol1 == '\'' && *flag_in_one_str == 1 && flag_in_str == 0) {
		*flag_in_one_str = 0;
	}
}
void flag_in_str_0(int* flag_in_str, int flag_comment, int flag_in_comment,
	int flag_transfer_comment, int flag_in_one_str, int* flag_continue,
	char simvol1, FILE* file_c, FILE* file_wc) {
	if (simvol1 == '\"' && *flag_in_str == 1 && flag_in_one_str == 0)
	{
		*flag_in_str = 0;
		if (flag_comment == 0 && flag_in_comment == 0 && flag_transfer_comment == 0)
			if (!feof(file_c))fputc(simvol1, file_wc);
		if (!feof(file_c))fseek(file_c, EOF, 1);
		*flag_continue = 1;
	}
}
void flag_in_str_1(int* flag_in_str, int flag_comment, int flag_in_comment,
	int flag_transfer_comment, int flag_in_one_str, char simvol1) {
	if (simvol1 == '\"' && *flag_in_str == 0 &&
		flag_comment == 0 && flag_in_comment == 0 &&
		flag_transfer_comment == 0 && flag_in_one_str == 0)
		*flag_in_str = 1;
}
void flag_in_comment_0(char simvol1, char simvol2, int flag_in_str,
	int* flag_in_comment, int* flag_continue, int flag_in_one_str) {
	if (simvol1 == '*' && simvol2 == '/' && flag_in_str == 0 && flag_in_one_str == 0 && *flag_in_comment == 1) {
		*flag_in_comment = 0;
		*flag_continue = 1;
	}
}
void flag_in_comment_1(char simvol1, char simvol2, int flag_in_str,
	int* flag_in_comment, int flag_in_one_str, int flag_comment, FILE* file_c) {
	if (simvol1 == '/' && simvol2 == '*' && *flag_in_comment == 0
		&& flag_in_str == 0 && flag_in_one_str == 0 && flag_comment == 0) {
		*flag_in_comment = 1;
		fseek(file_c, 1, 1);
	}
}
void flag_comment_0(int* flag_comment, char simvol1, int* flag_continue, FILE* file_c) {
	if (*flag_comment == 1 && (simvol1 == '\n' || simvol1 == '\r')) {
		*flag_comment = 0;
		fseek(file_c, -2, 1);
		*flag_continue = 1;
	}
}
void flag_comment_1(char simvol1, char simvol2, int flag_in_str,
	int flag_in_comment, int* flag_comment, int flag_in_one_str) 
{
	if (simvol1 == '/' && simvol2 == '/' && flag_in_str == 0
		&& flag_in_comment == 0 && flag_in_one_str == 0) // flag_in_str не обнулился
		*flag_comment = 1;

}
void flag_transfer_comment_0(int* flag_transfer_comment, char simvol1, char simvol2, int* flag_continue, FILE* file_c) {
	if (*flag_transfer_comment == 1 && simvol1 != '\\' && (simvol2 == '\n' || simvol2 == '\r' || simvol2 == '\\')) //Проблема
	{
		*flag_transfer_comment = 0;
		fseek(file_c, EOF, 1);
		*flag_continue = 1;
	}
	else if (*flag_transfer_comment == 1 && simvol1 == '\\' && (simvol2 == '\n' || simvol2 == '\r' || simvol2 == '\\'))
	{
		fseek(file_c, 1, 1);
	}
}
void flag_transfer_comment_1(int* flag_comment, char simvol1, char simvol2,
	int* flag_transfer_comment, int* flag_continue, FILE* file_c) {
	if (*flag_comment == 1 && simvol1 == '\\' && (simvol2 == '\n' || simvol2 == '\r'))
	{
		if (simvol2 == '\r')
			fseek(file_c, 1, 1);
		else
			fseek(file_c, 0, 1);
		*flag_transfer_comment = 1;
		*flag_comment = 0;
		*flag_continue = 1;
	}
}
void writing(int flag_comment, int flag_in_comment, int flag_transfer_comment,
	char simvol1, FILE* file_c, FILE* file_wc) {
	if (flag_comment == 0 && flag_in_comment == 0 && flag_transfer_comment == 0)
		if (!feof(file_c))fputc(simvol1, file_wc);
}
void seek(FILE* file_c)
{
	if (!feof(file_c))
		fseek(file_c, EOF, 1);
}
void writing_final_symbol(char simvol1, char simvol2, int flag_comment,
	int flag_in_comment, int flag_transfer_comment, FILE* file_wc) {
	if (simvol2 == EOF && simvol1 != EOF && flag_comment == 0 && flag_in_comment == 0 && flag_transfer_comment == 0)
		fputc(simvol1, file_wc);
}
void check_false_symbol(char simvol1, int* flag_false_symbol)
{
	if (simvol1 != '\\')
		*flag_false_symbol = 0;
}
void check_new_line(char simvol1, char simvol2, int* flag_false_symbol,
	int* flag_in_str, int* flag_in_one_str, int* flag_transfer_str,
	int* flag_transfer_false_symbol)
{
	if (simvol1 == '\n') {
		*flag_false_symbol = 1;//?
		*flag_in_str = 0;
		*flag_in_one_str = 0;
		if (*flag_transfer_str == 1)
		{
			*flag_in_str = 1;
			*flag_transfer_str = 0;
		}
		if (*flag_transfer_false_symbol == 1)
		{
			*flag_false_symbol = 0;
			*flag_transfer_false_symbol = 0;
		}
	}
}

void check_transfer_str(char simvol1, char simvol2, int flag_comment,
	int flag_in_comment, int flag_transfer_comment, int flag_in_str,
	int* flag_transfer_str, int* flag_transfer_false_symbol)
{
	if (((simvol1 == '\\' && (simvol2 == '\r' || simvol2 == '\n')) &&
		flag_comment == 0 && flag_in_comment == 0 && flag_transfer_comment == 0))
	{
		if (flag_in_str == 1)
			*flag_transfer_str = 1;
		*flag_transfer_false_symbol = 1;
	}
}
void deleting_of_comments(FILE* file_c, FILE* file_wc)
{
	int flag_in_one_str = 0, flag_in_str = 0, flag_transfer_comment = 0;
	int	flag_comment = 0, flag_in_comment = 0, flag_continue = 0;
	int	flag_false_symbol = 1, flag_transfer_str = 0, flag_transfer_one_str = 0;
	int flag_transfer_false_symbol = 0;
	char simvol1 = 0, simvol2 = 0;
	while (!feof(file_c))
	{
		simvol1 = fgetc(file_c);
		simvol2 = fgetc(file_c);
		check_false_symbol(simvol1, &flag_false_symbol);

		check_new_line(simvol1, simvol2, &flag_false_symbol,
			&flag_in_str, &flag_in_one_str, &flag_transfer_str,
			&flag_transfer_false_symbol);//Начало другой линии

		check_transfer_str(simvol1, simvol2, flag_comment,
			flag_in_comment, flag_transfer_comment, flag_in_str,
			&flag_transfer_str, &flag_transfer_false_symbol);

		flag_symbols(simvol1, simvol2, flag_in_comment, flag_comment,
			flag_transfer_comment, &flag_continue, flag_false_symbol, file_c, file_wc);

		flag_in_one_str_1(&flag_in_one_str, flag_comment, flag_in_comment,
			flag_transfer_comment, flag_in_str, &flag_continue, simvol1, file_c, file_wc);//tut
		if (flag_continue == 1)
		{
			flag_continue = 0;
			continue;
		}

		flag_in_one_str_0(&flag_in_one_str, flag_in_str, simvol1);//tut

		flag_in_str_0(&flag_in_str, flag_comment, flag_in_comment,
			flag_transfer_comment, flag_in_one_str, &flag_continue,
			simvol1, file_c, file_wc);
		if (flag_continue == 1) {
			flag_continue = 0;
			continue;
		}

		flag_in_str_1(&flag_in_str, flag_comment, flag_in_comment,
			flag_transfer_comment, flag_in_one_str, simvol1);

		flag_in_comment_0(simvol1, simvol2, flag_in_str, &flag_in_comment, &flag_continue, flag_in_one_str);
		if (flag_continue == 1) {
			flag_continue = 0;
			continue;
		}

		flag_in_comment_1(simvol1, simvol2, flag_in_str, &flag_in_comment, flag_in_one_str, flag_comment, file_c);

		flag_comment_0(&flag_comment, simvol1, &flag_continue, file_c);
		if (flag_continue == 1) {
			flag_continue = 0;
			continue;
		}

		flag_comment_1(simvol1, simvol2, flag_in_str, flag_in_comment, &flag_comment, flag_in_one_str);

		flag_transfer_comment_0(&flag_transfer_comment, simvol1, simvol2, &flag_continue, file_c);//Проблема
		if (flag_continue == 1) {
			flag_continue = 0;
			continue;
		}

		flag_transfer_comment_1(&flag_comment, simvol1, simvol2, &flag_transfer_comment, &flag_continue, file_c);

		writing(flag_comment, flag_in_comment, flag_transfer_comment, simvol1, file_c, file_wc);//Запись в файл

		seek(file_c);
	}
	writing_final_symbol(simvol1, simvol2, flag_comment, flag_in_comment, flag_transfer_comment, file_wc);

	fclose(file_c);
	fclose(file_wc);
}

int main()
{
	FILE* file_c = NULL;
	file_c = fopen("test.c", "rb");
	FILE* file_wc = NULL;
	file_wc = fopen("test.wc", "wb");
	deleting_of_comments(file_c, file_wc);
	return 0;	
}