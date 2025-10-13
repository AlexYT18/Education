int i, j;
  for (i=0; i < 4; i++) puts (C[i]);
  for (i=0; i < sizeof C / sizeof C[0]; i++) {
    putchar ('\"');
    for (j=0; C[i][j]; j++)
      if (C[i][j] == '\\' || C[i][j] == '\"')
        printf ("\\%c", C[i][j]);
      else putchar (C[i][j]);
    puts ("\",");
  }
  for (i=4; i < 27; i++) puts (C[i]);
  puts ("/* *"")");
  for (i=27; i < 32; i++) puts (C[i]);
  for (i=0; i < sizeof C / sizeof C[0]; i++) {
    printf ("  C[%2d]:='", i+1);
    for (j=0; C[i][j]; j++)
      if (C[i][j] == '\'') printf ("''");
      else putchar (C[i][j]);
    puts ("';");
  }
  for (i=32; i < sizeof C / sizeof C[0]; i++) puts (C[i]);
  puts ("{ *""/ }");


//﻿#define _CRT_SECURE_NO_WARNINGS
//#include <iostream>
//#include <stdio.h>
//#include <string.h>
//#include <locale.h>
//#include <stdlib.h>
//#include <string>
//#include <string.h>
//FILE* input;
//struct splay_tree
//{
//	long long data;
//	int po = 1;
//	splay_tree* left = NULL;
//	splay_tree* right = NULL;
//};
//splay_tree* new_data(long long data)
//{
//	splay_tree* tree = new splay_tree;
//	tree->data = data;
//	tree->left = tree->right = NULL;
//	return (tree);
//}
//splay_tree* left_povorot(splay_tree* x)
//{
//	splay_tree* vruchit = x->right;
//	x->right = vruchit->left;//nullptr
//	vruchit->left = x;
//	return  vruchit;
//}
//splay_tree* right_povorot(splay_tree* x)
//{
//	splay_tree* vruchit = x->left;
//	x->left = vruchit->right;
//	vruchit->right = x;
//	return  vruchit;
//}
//splay_tree* splay(splay_tree* node, long long data)
//{
//	if (node == NULL || node->data == data)
//	{
//		return node;
//	}
//	if (node->data > data)
//	{
//		if (node->left == NULL)
//		{
//			return node;
//		}
//
//		if (node->left->data > data)//zig-zig
//		{
//
//			node->left->left = splay(node->left->left, data);
//			node = right_povorot(node);
//		}
//		else
//		{
//			if (node->left->data < data)//zig-zag 
//			{
//				node->left->right = splay(node->left->right, data);
//				node->left = left_povorot(node->left);
//			}
//		}
//		if (node->left == NULL)
//		{
//			return (node);
//		}
//		else
//		{
//			return(right_povorot(node));
//		}
//	}
//	else
//	{
//
//		if (node->right == NULL)
//		{
//			return node;
//		}
//
//		if (node->right->data > data)//zag-zig 
//		{
//			node->right->left = splay(node->right->left, data);
//			node->right = right_povorot(node->right);
//		}
//		else
//		{
//			if (node->right->data < data)//zag-zag 
//			{
//				node->right->right = splay(node->right->right, data);
//				node = left_povorot(node);
//			}
//		}
//		if (node->right == NULL)
//		{
//			return(node);
//		}
//		else
//		{
//			return (left_povorot(node));
//		}
//	}
//}
//splay_tree* insert(splay_tree* base, long long scan)//Возможно переписать с рекурсией
//{
//	if (base == NULL)
//	{
//		return new_data(scan);
//	}
//	if (base->data == scan)
//	{
//		base->po++;
//		return base;
//	}
//	if (base->data > scan)
//	{
//		base->left = insert(base->left, scan);
//	}
//	else
//	{
//		base->right = insert(base->right, scan);
//	}
//	return base;
//}
//void flag_symbols(char simvol1, char simvol2, int flag_in_comment,
//	int flag_comment, int flag_transfer_comment, int* flag_continue,
//	int flag_false_symbol, FILE* file_c, FILE* file_wc)
//{
//	if (simvol1 == '\\' && flag_in_comment == 0 &&
//		flag_comment == 0 && flag_transfer_comment == 0 && flag_false_symbol == 0 && simvol2 != '/')
//	{
//		if (!feof(file_c))fputc(simvol1, file_wc);
//		if (!feof(file_c))fputc(simvol2, file_wc);
//		*flag_continue = 1;
//	}
//}
//void flag_in_one_str_1(int* flag_in_one_str, int flag_comment, int flag_in_comment,
//	int flag_transfer_comment, int flag_in_str, int* flag_continue, char simvol1, FILE* file_c, FILE* file_wc) {
//	if (simvol1 == '\'' && *flag_in_one_str == 0 && flag_in_str == 0 &&
//		flag_comment == 0 && flag_in_comment == 0 && flag_transfer_comment == 0) {
//		*flag_in_one_str = 1;
//		if (!feof(file_c))fputc(simvol1, file_wc);
//		if (!feof(file_c))fseek(file_c, EOF, 1);
//		*flag_continue = 1;
//	}
//}
//void flag_in_one_str_0(int* flag_in_one_str, int flag_in_str, char simvol1) {
//	if (simvol1 == '\'' && *flag_in_one_str == 1 && flag_in_str == 0) {
//		*flag_in_one_str = 0;
//	}
//}
//void flag_in_str_0(int* flag_in_str, int flag_comment, int flag_in_comment,
//	int flag_transfer_comment, int flag_in_one_str, int* flag_continue,
//	char simvol1, FILE* file_c, FILE* file_wc) {
//	if (simvol1 == '\"' && *flag_in_str == 1 && flag_in_one_str == 0)
//	{
//		*flag_in_str = 0;
//		if (flag_comment == 0 && flag_in_comment == 0 && flag_transfer_comment == 0)
//			if (!feof(file_c))fputc(simvol1, file_wc);
//		if (!feof(file_c))fseek(file_c, EOF, 1);
//		*flag_continue = 1;
//	}
//}
//void flag_in_str_1(int* flag_in_str, int flag_comment, int flag_in_comment,
//	int flag_transfer_comment, int flag_in_one_str, char simvol1) {
//	if (simvol1 == '\"' && *flag_in_str == 0 &&
//		flag_comment == 0 && flag_in_comment == 0 &&
//		flag_transfer_comment == 0 && flag_in_one_str == 0)
//		*flag_in_str = 1;
//}
//void flag_in_comment_0(char simvol1, char simvol2, int flag_in_str,
//	int* flag_in_comment, int* flag_continue, int flag_in_one_str) {
//	if (simvol1 == '*' && simvol2 == '/' && flag_in_str == 0 && flag_in_one_str == 0 && *flag_in_comment == 1) {
//		*flag_in_comment = 0;
//		*flag_continue = 1;
//	}
//}
//void flag_in_comment_1(char simvol1, char simvol2, int flag_in_str,
//	int* flag_in_comment, int flag_in_one_str, int flag_comment, FILE* file_c) {
//	if (simvol1 == '/' && simvol2 == '*' && *flag_in_comment == 0
//		&& flag_in_str == 0 && flag_in_one_str == 0 && flag_comment == 0) {
//		*flag_in_comment = 1;
//		fseek(file_c, 1, 1);
//	}
//}
//void flag_comment_0(int* flag_comment, char simvol1, int* flag_continue, FILE* file_c) {
//	if (*flag_comment == 1 && (simvol1 == '\n' || simvol1 == '\r')) {
//		*flag_comment = 0;
//		fseek(file_c, -2, 1);
//		*flag_continue = 1;
//	}
//}
//void flag_comment_1(char simvol1, char simvol2, int flag_in_str,
//	int flag_in_comment, int* flag_comment, int flag_in_one_str)
//{
//	if (simvol1 == '/' && simvol2 == '/' && flag_in_str == 0
//		&& flag_in_comment == 0 && flag_in_one_str == 0) // flag_in_str не обнулился
//		*flag_comment = 1;
//
//}
//void flag_transfer_comment_0(int* flag_transfer_comment, char simvol1, char simvol2, int* flag_continue, FILE* file_c)
//{
//	if (*flag_transfer_comment == 1 && simvol1 != '\\' && (simvol2 == '\n' || simvol2 == '\r' || simvol2 == '\\'))
//	{
//		*flag_transfer_comment = 0;
//		fseek(file_c, EOF, 1);
//		*flag_continue = 1;
//	}
//	else if (*flag_transfer_comment == 1 && simvol1 == '\\' && (simvol2 == '\n' || simvol2 == '\r' || simvol2 == '\\'))
//	{
//		fseek(file_c, 1, 1);
//	}
//}
//void flag_transfer_comment_1(int* flag_comment, char simvol1, char simvol2,
//	int* flag_transfer_comment, int* flag_continue, FILE* file_c) {
//	if (*flag_comment == 1 && simvol1 == '\\' && (simvol2 == '\n' || simvol2 == '\r'))
//	{
//		if (simvol2 == '\r')
//			fseek(file_c, 1, 1);
//		else
//			fseek(file_c, 0, 1);
//		*flag_transfer_comment = 1;
//		*flag_comment = 0;
//		*flag_continue = 1;
//	}
//}
//void writing(int flag_comment, int flag_in_comment, int flag_transfer_comment,
//	char simvol1, FILE* file_c, FILE* file_wc) {
//	if (flag_comment == 0 && flag_in_comment == 0 && flag_transfer_comment == 0)
//		if (!feof(file_c))fputc(simvol1, file_wc);
//}
//void seek(FILE* file_c)
//{
//	if (!feof(file_c))
//		fseek(file_c, EOF, 1);
//}
//void writing_final_symbol(char simvol1, char simvol2, int flag_comment,
//	int flag_in_comment, int flag_transfer_comment, FILE* file_wc) {
//	if (simvol2 == EOF && simvol1 != EOF && flag_comment == 0 && flag_in_comment == 0 && flag_transfer_comment == 0)
//		fputc(simvol1, file_wc);
//}
//void check_false_symbol(char simvol1, int* flag_false_symbol)
//{
//	if (simvol1 != '\\')
//		*flag_false_symbol = 0;
//}
//void check_new_line(char simvol1, char simvol2, int* flag_false_symbol,
//	int* flag_in_str, int* flag_in_one_str, int* flag_transfer_str,
//	int* flag_transfer_false_symbol)
//{
//	if (simvol1 == '\n') {
//		*flag_false_symbol = 1;//?
//		*flag_in_str = 0;
//		*flag_in_one_str = 0;
//		if (*flag_transfer_str == 1)
//		{
//			*flag_in_str = 1;
//			*flag_transfer_str = 0;
//		}
//		if (*flag_transfer_false_symbol == 1)
//		{
//			*flag_false_symbol = 0;
//			*flag_transfer_false_symbol = 0;
//		}
//	}
//}
//
//void check_transfer_str(char simvol1, char simvol2, int flag_comment,
//	int flag_in_comment, int flag_transfer_comment, int flag_in_str,
//	int* flag_transfer_str, int* flag_transfer_false_symbol)
//{
//	if (((simvol1 == '\\' && (simvol2 == '\r' || simvol2 == '\n')) &&
//		flag_comment == 0 && flag_in_comment == 0 && flag_transfer_comment == 0))
//	{
//		if (flag_in_str == 1)
//			*flag_transfer_str = 1;
//		*flag_transfer_false_symbol = 1;
//	}
//}
//void deleting_of_comments(FILE* file_c, FILE* file_wc)
//{
//	int flag_in_one_str = 0, flag_in_str = 0, flag_transfer_comment = 0;
//	int	flag_comment = 0, flag_in_comment = 0, flag_continue = 0;
//	int	flag_false_symbol = 1, flag_transfer_str = 0, flag_transfer_one_str = 0;
//	int flag_transfer_false_symbol = 0;
//	char simvol1 = 0, simvol2 = 0;
//	while (!feof(file_c))
//	{
//		simvol1 = fgetc(file_c);
//		simvol2 = fgetc(file_c);
//		check_false_symbol(simvol1, &flag_false_symbol);
//
//		check_new_line(simvol1, simvol2, &flag_false_symbol,
//			&flag_in_str, &flag_in_one_str, &flag_transfer_str,
//			&flag_transfer_false_symbol);//Начало другой линии
//
//		check_transfer_str(simvol1, simvol2, flag_comment,
//			flag_in_comment, flag_transfer_comment, flag_in_str,
//			&flag_transfer_str, &flag_transfer_false_symbol);
//
//		flag_symbols(simvol1, simvol2, flag_in_comment, flag_comment,
//			flag_transfer_comment, &flag_continue, flag_false_symbol, file_c, file_wc);
//
//		flag_in_one_str_1(&flag_in_one_str, flag_comment, flag_in_comment,
//			flag_transfer_comment, flag_in_str, &flag_continue, simvol1, file_c, file_wc);//tut
//		if (flag_continue == 1)
//		{
//			flag_continue = 0;
//			continue;
//		}
//
//		flag_in_one_str_0(&flag_in_one_str, flag_in_str, simvol1);//tut
//
//		flag_in_str_0(&flag_in_str, flag_comment, flag_in_comment,
//			flag_transfer_comment, flag_in_one_str, &flag_continue,
//			simvol1, file_c, file_wc);
//		if (flag_continue == 1) {
//			flag_continue = 0;
//			continue;
//		}
//
//		flag_in_str_1(&flag_in_str, flag_comment, flag_in_comment,
//			flag_transfer_comment, flag_in_one_str, simvol1);
//
//		flag_in_comment_0(simvol1, simvol2, flag_in_str, &flag_in_comment, &flag_continue, flag_in_one_str);
//		if (flag_continue == 1) {
//			flag_continue = 0;
//			continue;
//		}
//
//		flag_in_comment_1(simvol1, simvol2, flag_in_str, &flag_in_comment, flag_in_one_str, flag_comment, file_c);
//
//		flag_comment_0(&flag_comment, simvol1, &flag_continue, file_c);
//		if (flag_continue == 1) {
//			flag_continue = 0;
//			continue;
//		}
//
//		flag_comment_1(simvol1, simvol2, flag_in_str, flag_in_comment, &flag_comment, flag_in_one_str);
//
//		flag_transfer_comment_0(&flag_transfer_comment, simvol1, simvol2, &flag_continue, file_c);//Проблема
//		if (flag_continue == 1) {
//			flag_continue = 0;
//			continue;
//		}
//
//		flag_transfer_comment_1(&flag_comment, simvol1, simvol2, &flag_transfer_comment, &flag_continue, file_c);
//
//		writing(flag_comment, flag_in_comment, flag_transfer_comment, simvol1, file_c, file_wc);//Запись в файл
//
//		seek(file_c);
//	}
//	writing_final_symbol(simvol1, simvol2, flag_comment, flag_in_comment, flag_transfer_comment, file_wc);
//}
//void vivod(splay_tree* base)
//{
//	if (base != NULL)
//	{
//		std::cout << base->data << " ( " << base->po << " )" << std::endl;
//		vivod(base->left);
//		vivod(base->right);
//
//	}
//}
//void check_symbol_in_name(char symbol, int* flag_in_name)
//{
//	if ((symbol >= 'a' && symbol <= 'z') || (symbol >= 'A' && symbol <= 'Z'))
//	{
//		*flag_in_name = 1;
//	}
//	else *flag_in_name = 0;
//}
//void check_minus(char symbol, int* flag_minus)
//{
//	if (symbol == '-')
//	{
//		*flag_minus = 1;
//	}
//}
//void check_down_line(char symbol, int* flag_down_line)
//{
//	if (symbol == '_')
//	{
//		*flag_down_line = 1;
//	}
//
//}
//void check_point(char symbol, int* flag_point)
//{
//	if (symbol == '.' || symbol == '_')
//	{
//		*flag_point = 1;
//	}
//}
//void check_is_number(char symbol, int* flag_number)
//{
//	if (symbol >= '0' && symbol <= '9')
//	{
//		*flag_number = 1;
//	}
//	else
//	{
//		*flag_number = 0;
//	}
//}
//void check_kavichi(char symbol, int* flag_kavicki, int* flag_double_kavik)
//{
//	if (symbol == '\'')
//	{
//		*flag_kavicki = 1;
//	}
//	if (symbol == '\"')
//	{
//		*flag_double_kavik = 1;
//	}
//}
//void all_proverka(int* flag_minus, int* flag_in_name, int* flag_down_line, int* flag_point, int* flag_number, int* flag_kavicki, int* flag_double_kavik, int* flag_unsigned, char symbol)
//{
//	check_symbol_in_name(symbol, flag_in_name);//Проверка, является ли буквой
//	check_minus(symbol, flag_minus);//Проверка, является ли минусом
//	check_down_line(symbol, flag_down_line);//Проверка, является нижним подчеркиванием (_)
//	check_point(symbol, flag_point); //Проверка, является ли точкой
//	check_is_number(symbol, flag_number);//Проверка, является ли числом
//	check_kavichi(symbol, flag_kavicki, flag_double_kavik);//Проверка на кавычки
//}
//void deleting_other(splay_tree* base)
//{
//	FILE* open = fopen("input_1.cpp", "rb");
//	if (!open)
//	{
//		std::cout << "Error to open file" << std::endl;
//		return;
//	}
//	char symbol;
//	int flag_minus = 0;
//	int flag_in_name = 0;
//	int flag_down_line = 0;
//	int flag_point = 0;
//	int flag_number = 0;
//	int flag_kavicki = 0;
//	int flag_double_kavik = 0;
//	int flag_unsigned = 0;
//	int flag_slesh = 0;
//	long long itog = 0;
//	while (!feof(open))
//	{
//		symbol = fgetc(open);
//		check_symbol_in_name(symbol, &flag_in_name);//Проверка, является ли буквой
//		check_minus(symbol, &flag_minus);//Проверка, является ли минусом
//		check_down_line(symbol, &flag_down_line);//Проверка, является нижним подчеркиванием (_)
//		check_point(symbol, &flag_point); //Проверка, является ли точкой
//		check_is_number(symbol, &flag_number);//Проверка, является ли числом
//		check_kavichi(symbol, &flag_kavicki, &flag_double_kavik);//Проверка на кавычки
//		//all_proverka(&flag_minus, &flag_in_name, &flag_down_line, &flag_point, &flag_number, &flag_kavicki, &flag_double_kavik, &flag_unsigned, symbol);
//		if (flag_number == 1 && flag_in_name == 0 && flag_point == 0 && flag_kavicki == 0 && flag_double_kavik == 0)
//		{
//			while (flag_number == 1)
//			{
//				itog = itog * 10;
//				itog += (symbol - '0');
//				symbol = fgetc(open);
//				check_is_number(symbol, &flag_number);
//			}
//			check_symbol_in_name(symbol, &flag_in_name);
//			check_point(symbol, &flag_point);
//			if (flag_point == 1)
//			{
//				symbol = fgetc(open);
//				while (symbol != ' ' && symbol != ';' && symbol != '{' && symbol != '\n' && symbol != '\r' && symbol != '\0' && symbol != '[' && symbol != '=')
//				{
//					symbol = fgetc(open);
//				}
//				flag_point = 0;
//				itog = 0;
//				continue;
//			}
//			if (flag_in_name == 0 && flag_point == 0)
//			{
//				if (flag_minus == 1)
//				{
//					itog = itog * (-1);
//					flag_minus = 0;//Возможно не надо
//					check_minus(symbol, &flag_minus);
//				}
//				base = insert(base, itog);
//			}
//			else
//			{
//				if (flag_point == 1)
//				{
//					symbol = fgetc(open);//Возможно не нужно
//					itog = 0;
//				}
//				else
//				{
//					//check_unsigned(symbol, &flag_unsigned);
//					if (symbol == 'u' || symbol == 'l' || symbol == 'U' || symbol == 'L')
//					{
//						flag_unsigned = 0;
//						if ((symbol == 'u' || symbol == 'U') && flag_minus == 1)
//						{
//							flag_minus = 0;
//							flag_unsigned = 1;
//						}
//						symbol = fgetc(open);
//						check_symbol_in_name(symbol, &flag_in_name);
//						check_point(symbol, &flag_point);
//						if (flag_in_name == 0 && flag_point == 0 && flag_unsigned == 0)
//						{
//							if (flag_minus == 1)
//							{
//								itog = itog * (-1);
//								flag_minus = 0;
//							}
//							base = insert(base, itog);
//						}
//						else
//						{
//							if (symbol == 'l' || symbol == 'L')
//							{
//								symbol = fgetc(open);//Начало
//								check_symbol_in_name(symbol, &flag_in_name);
//								check_point(symbol, &flag_point);
//								if (flag_in_name == 0 && flag_point == 0)
//								{
//									if (flag_minus == 1)
//									{
//										itog = itog * (-1);
//										flag_minus = 0;
//									}
//									base = insert(base, itog);
//								}//Конец, можно заменить функцией
//								else
//								{
//									symbol = fgetc(open);
//									itog = 0;
//								}
//							}
//						}
//					}
//				}
//			}
//
//		}
//		else
//		{
//			if (flag_point == 1)
//			{
//				while (symbol != ' ' && symbol != ';' && symbol != '{' && symbol != '\n' && symbol != '\r' && symbol != '\0' && symbol != '[' && symbol != '=' && symbol != '+' && symbol != '-')
//				{
//					symbol = fgetc(open);
//				}
//				if (flag_point == 1) flag_point = 0;
//				continue;
//			}
//			if (flag_kavicki == 1)
//			{
//				symbol = fgetc(open);
//				while (symbol != '\'' && symbol != '\n' && symbol != '\r' && symbol != '\0' && symbol != '&' && symbol != '[')//Вот здесь и ошибка у нас записано '\''
//					//while (symbol != '\'' && symbol != '\n' && symbol != '\r' && symbol != '\0' && symbol != '&')
//				{
//					symbol = fgetc(open);
//				}
//				flag_kavicki = 0;
//			}
//			if (flag_double_kavik == 1)
//			{
//				symbol = fgetc(open);
//				while (symbol != '\"' && symbol != '\n' && symbol != '\r' && symbol != '\0')
//				{
//					symbol = fgetc(open);
//				}
//				flag_double_kavik = 0;
//			}
//
//		}
//		if (flag_in_name == 1)
//		{
//			while (symbol != '.' && symbol != ' ' && symbol != ';' && symbol != '{' && symbol != '\n' && symbol != '\r' && symbol != '\0' && symbol != '[' && symbol != '=' && symbol != '+' && symbol != '-')
//			{
//				symbol = fgetc(open);
//			}
//			if (flag_in_name == 1) flag_in_name = 0;
//		}
//		itog = 0;
//		if (symbol == ' ' || symbol == '\n' || symbol == '\r' || symbol == '\0')
//		{
//			flag_minus = 0;
//			flag_in_name = 0;
//			flag_down_line = 0;
//			flag_point = 0;
//			flag_number = 0;
//			flag_kavicki = 0;
//			flag_double_kavik = 0;
//			flag_unsigned = 0;
//			flag_slesh = 0;
//		}
//	}
//	//Либо вывод здесь
//	vivod(base);
//	fclose(open);
//}
//
//int main()
//{
//	FILE* file_wc = NULL;
//	FILE* file_c = NULL;
//	file_c = fopen("input.cpp", "rb");
//	file_wc = fopen("input_1.cpp", "wb");
//	long long pomenat;
//	splay_tree* base = NULL;
//	deleting_of_comments(file_c, file_wc);
//	fclose(file_c);
//	fclose(file_wc);
//	deleting_other(base);
//	//Дoработать
//	long long vibor = 0;
//	return 0;
//}
