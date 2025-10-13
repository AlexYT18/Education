#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <malloc.h>
#include <Windows.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string>
#include <math.h>
union ob
{
	char surname[50];//Фамилия
	char name[50];//Имя
	char patronymic[50];//Отчество
};
enum gender 
{
	Женщина,
	Мужчина,
}genders;
typedef struct student
{
	ob fio;
	char surname[50];
	char name[50];
	char patronymic[50];
	int date_of_birthday;
	char institute[100];
	char dorm_number[20];
	gender genders;
	double ID;
	student* put_student;
};
void entering_data(student** student_one)
{
	student* new_student = (student*)malloc(sizeof(student));//создается новый элемент
	if (new_student == NULL) return;
	memset(new_student, 0, sizeof(student));
	fgets(new_student->name , 2, stdin);//Решение проблемы на enter
	printf("--------------------\nИмя:\n--------------------\n");
	fgets(new_student->name, 50, stdin);
	strcpy((*new_student).fio.name,new_student->name);
	new_student->name[strlen(new_student->name) - 1] = 0;
	printf("--------------------\nФамилия:\n--------------------\n");
	fgets(new_student->surname, 50, stdin);
	new_student->surname[strlen(new_student->surname) - 1] = 0;
	printf("--------------------\nОтчество:\n--------------------\n");
	fgets(new_student->patronymic, 50, stdin);
	new_student->patronymic[strlen(new_student->patronymic) - 1] = 0;
	printf("--------------------\nДата Рождения (вводить без точек в формате ДДММГГГГ):\n--------------------\n");
	scanf("%d", &(new_student)->date_of_birthday);
	fgets(new_student->institute, 2, stdin);
	printf("--------------------\nИнститут обучающегося:\n--------------------\n");
	printf("Возможный вариант институтов: \nИКиЗИ; ИКНТ; ГИ и тд.\n");
	fgets(new_student->institute, 100, stdin);
	new_student->institute[strlen(new_student->institute) - 1] = 0;
	printf("--------------------\nНомер общежития:\n--------------------\n");
	fgets(new_student->dorm_number, 20, stdin);
	new_student->dorm_number[strlen(new_student->dorm_number) - 1] = 0;
	printf("--------------------\nПол студента:\n--------------------\n");
	char pol[100] = "Мужчина";
	fgets(pol, 100, stdin);
	if (pol == "Женщина") new_student->genders = Женщина;
	else new_student->genders = Мужчина;
	printf("--------------------\nID студента Политеха:\n--------------------\n");
	fgets(pol, 100, stdin);
	new_student->ID = atof(pol); 
	new_student->put_student = NULL;
	printf("\n--------------------\nВвод данных завершен.\n\n\n");
	*student_one = new_student;
}
void add_to_base_rb_consol(student** zapis)
{
	student* news_student;
	int c = -1;
	if (*zapis == NULL) c = 0;
	entering_data(&news_student);
	if (c == 0)
	{
		*zapis = news_student;
		return;
	}
	else
	{
		news_student->put_student = (*zapis)->put_student;
		(*zapis)->put_student = news_student;
	}
}
void print_base_console(student* vivod)
{
	if (vivod == NULL) return;
	student* itog;
	itog = vivod;
	
	while (itog != NULL)
	{
		printf("ФИО| %s %s %s\n-----------------------\n",
			itog->surname,
			itog->name,
			itog->patronymic);
		printf("Год рождения| %d\n-----------------------\n", 
			itog->date_of_birthday);
		if (itog->genders == Женщина) printf("Пол| Женщина\n-----------------------\n");
		else printf("Пол| Мужчина\n-----------------------\n");
		printf("Институт и номер общежития| %s   %s\n-----------------------\n",
			itog->institute,
			itog->dorm_number);
		printf("ID Студента| %lf\n-----------------------\n\n\n",itog->ID);
		itog = itog->put_student;
	}
	return;
}
student* delete_student(student* base, student* element)
{
	if (base == NULL)
	{
		return base;
	}
	if (base == element)
	{
		student* new_base = (base)->put_student;
		free(base);
		return new_base;
	}
	student* poisk_st;
	poisk_st = base;
	while ((poisk_st->put_student != element) && (poisk_st != NULL)) 
	{
		poisk_st = poisk_st->put_student;
	}
	if (poisk_st == NULL) {
		printf("Error\n");
		atexit(0);
	}
	poisk_st->put_student = element->put_student;
	free(element);
	return base;
}
student* delete_from_base(student* students)
{
	if (students == NULL) { printf("База пуста\n\n\n"); return students; }
	char surname[50];
	char name[50];
	printf("--------------------EXIT FROM POLYTECH--------------------\nВведите данные студента для отчисления:\n");
	fgets(name, 2, stdin);//Решение проблемы на enter
	printf("--------------------\nИмя:\n--------------------\n");
	fgets(name, 50, stdin);
	name[strlen(name) - 1] = 0;
	printf("--------------------\nФамилия:\n--------------------\n");
	fgets(surname, 50, stdin);
	surname[strlen(surname) - 1] = 0;
	student* komsa = students, * new_base = (student*)malloc(sizeof(student));
	while (komsa != NULL)
	{
		if (!strcmp(surname, komsa->surname) && !strcmp(name, komsa->name))
		{
			new_base = delete_student(students, komsa);
			printf("\n--------------------\nОтчислен\n--------------------\n\n\n");
			return new_base;
		}
		komsa = komsa->put_student;
		if (komsa == NULL)
		{
			printf("\n--------------------\nТакого студента нет\n--------------------\n\n");
			return students;
		}
	}
}
void add_to_base_one(student** zapis, char* surname, char* name, char* patronymic, char* date_of_birthday, char* institute, char* dorm_number,char* pol,char* id)
{
	student* new_student = (student*)malloc(sizeof(student));
	memset(new_student, 0, sizeof(student));
	strcpy(new_student->surname, surname);
	strcpy(new_student->name, name);
	strcpy(new_student->patronymic, patronymic);
	new_student->date_of_birthday = atoi(date_of_birthday);
	strcpy(new_student->institute, institute);
	strcpy(new_student->dorm_number, dorm_number);
	if (!strcmp(pol, "Мужчина")) new_student->genders = Мужчина;
	else new_student->genders = Женщина;
	new_student->ID = atof(id);
	new_student->put_student = NULL;
	*zapis = new_student;
}
void add_from_file(student** add_student)
{
	*add_student = NULL;
	char stroka[1000000];
	int c = 0;
	FILE* output = fopen("Based.txt", "rb");
	student* new_student;
	while (fgets(stroka, 1000000, output) != NULL)
	{
		char* surname = strtok(stroka, " ");//Фамилия
		char* name = strtok(NULL, " ");//Имя
		char* patronymic = strtok(NULL, " ");//Отчество
		char* date_of_birthday = strtok(NULL, " ");//Пока сделаем запись и вывод без точек
		char* institute = strtok(NULL, " ");//Институт
		char* dorm_number = strtok(NULL, " ");//Номер общежития
		char* pol = strtok(NULL, " ");
		char* id = strtok(NULL, " ");
		add_to_base_one(&new_student, surname, name, patronymic, date_of_birthday, institute, dorm_number, pol, id);
		new_student->put_student = NULL;
		if (c == 0)
		{
			c++;
			
			*add_student = new_student;
		}
		else
		{
			new_student->put_student = (*add_student)->put_student;
			(*add_student)->put_student = new_student;
		}
	}
	fclose(output);
}
void saving_data(student* vivod)
{
	student* print_student;
	print_student = vivod;
	FILE* based = fopen("Based.txt", "wb");
	while (print_student != NULL)
	{
		char pol[50] = "Мужчина";
		if (print_student->genders == Женщина) strcpy(pol, "Женщина");
		fprintf(based, "%s %s %s %d %s %s %s %lf \r\n",
			print_student->surname,
			print_student->name, 
			print_student->patronymic, 
			print_student->date_of_birthday, 
			print_student->institute, 
			print_student->dorm_number,
			pol,
			print_student->ID);
		print_student = print_student->put_student;
	}
	fclose(based);
	printf("Сохранение завершенно успешно.\n");
}
void print_base_console_one(student* vivod)
{
	student* itog;
	itog = vivod;
	printf("ФИО| %s %s %s\n-----------------------\n",itog->surname,itog->name,itog->patronymic);
	printf("Год рождения| %d\n-----------------------\n",itog->date_of_birthday);
	if (vivod->genders == Женщина) printf("Пол| Женщина\n-----------------------\n");
	else printf("Пол| Мужчина\n-----------------------\n");
	printf("Институт и номер общежития| %s  |  %s\n-----------------------\n",itog->institute,itog->dorm_number);
	printf("ID Студента| %lf\n-----------------------\n\n\n", itog->ID);
}
void poisk_student(student* base, int v)
{
	student* nuxaem;
	nuxaem = base;
	printf("\n--------------------\nВведите ключевое слово: \n--------------------\n");
	char slovo[100];
	int count = 0;
	fgets(slovo, 2, stdin);
	fgets(slovo, 100, stdin);
	printf("--------------------\n");
	slovo[strlen(slovo) - 1] = 0;
	char pol[100];
	while (nuxaem != NULL)
	{
		switch (v)
		{
		case 1:
			if (!strcmp(slovo, nuxaem->surname)) 
			{
				print_base_console_one(nuxaem);
				count++;
			}
			break;
		case 2:
			if (!strcmp(slovo, nuxaem->name))
			{
				print_base_console_one(nuxaem);
				count++;
			}
			break;
		case 3:
			if (!strcmp(slovo, nuxaem->patronymic))
			{
				print_base_console_one(nuxaem);
				count++;
			}
			break;
		case 4:
			if (!strcmp(slovo, nuxaem->institute))
			{
				print_base_console_one(nuxaem);
				count++;
			}
			break;
		case 5:
			if (!strcmp(slovo, nuxaem->dorm_number))
			{
				print_base_console_one(nuxaem);
				count++;
			}
			break;
		case 6:
			strcpy(pol, "Мужчина");
			if (nuxaem->genders == Женщина) strcpy(pol,"Женщина");
			if (!strcmp(slovo,pol))
			{
				print_base_console_one(nuxaem);
				count++;
			}
			break;
		case 7:
			if (atoi(slovo) == nuxaem->date_of_birthday)
			{
				print_base_console_one(nuxaem);
				count++;
			}
			break;
		default:
			break;
		}
		nuxaem = nuxaem->put_student;
	}
	if (count == 0) printf("Студент не обнаружен\n");
}
void poisk_data(student* basa)
{
	if (basa == NULL) return;
	printf("--------------------POISK MENU--------------------\n");
	printf("Поиск по Фамилии(1); Имени(2); Отчеству(3); Институту(4); Номеру общаги(5); Гендеру(6);Дате рождения(7) \n");
	int v = 0;
	scanf("%d", &v);
	if (v < 1 || v > 7) printf("\nТакого варианта поиска нет\n");
	while (v >= 1 && v <= 7)
	{
		printf("\n");
		poisk_student(basa,v);
		break;
	}
	printf("Поиск завершен.\n\n\n");
}
int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);//Учим две строчки, как отче наш
	setlocale(LC_ALL, "RUS");
	int vibor = 0;
	student* new_student = NULL;
	add_from_file(&new_student);//Работает
	while (true)
	{
		vibor = 0;
		//БАЗА ДАННЫХ СТУДЕНТОВ ПРОЖИВАЮЩИХ В ОБЩЕЖИТИЕ
		printf("--------------------MAIN MENU--------------------\n");
		printf("Выбор варианта:\n1 - Добавь студента\n2 - Вывести базу данных всех студентов\n3 - Поиск по определенному критерию\n4 - Отчисление студентов\n5 - Завершение программы и сохранение\n--------------------\nВыбор: ");
		scanf("%d", &vibor);
		if (vibor < 1 || vibor >5)
		{
			printf("Ошибка ввода\n"); 
			return 0;
		}
		switch (vibor)
		{
		case 1:
			add_to_base_rb_consol(&new_student);
			break;
		case 2:
			if (new_student == NULL) break;
			printf("\n\n\n--------------------BASE OF STUDENTS--------------------\n\n");
			print_base_console(new_student);
			break;
		case 3:
			poisk_data(new_student);
			break;
		case 4:
			new_student = delete_from_base(new_student);
			break;
		case 5:
			saving_data(new_student);
			printf("Завершение программы\n");
			free(new_student);
			exit(1);
		default:
			break;
		}
		
	}
	free(new_student);
	return 0;
}