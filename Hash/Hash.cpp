// программа для вычисления хэш-суммы логина и пароля + проверка авторизации

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <iomanip>
#include "md5.h"
#include "MainMD5.h"
#pragma warning(disable : 4996)

#define SALT_SIZE 5 // размер рандомной соли + '\0'
char genersol[] = "ABC@##!?+234243"; // - общая соль
#define size 10 //размер хэш-таблицы
#define length 10 //размер максимального логина и пароля
#define MIN_LEN_PASSWORD 4
#define MIN_LEN_LOGIN 4
#define COUNT_LETTERS_PASSWORD 3

char admin[] = "admin"; //файл с администраторами
char clients[] = "database"; //файл с пользователями

using namespace std;

struct user
{
	char login[length];
	char password[length];
	char hash[64];
	char sol[SALT_SIZE];
	user* next;
};

int admin_menu();
bool admin_check();
int user_menu();
void hashing(char*, user*);
int h(char*);
user** registration(user**);
void exit(user**, char*);
user** write_from_file_to_array(char*, user**);
user** del(user**, user*);
void print(user**);
void FREE(user***);
bool authorization_check(user**, user*, int);
bool check_difficulty(user*);
char* get_letters_from_line(char*);
bool proceed();
bool check_login(user**, char*);
bool check_letters(char*);


//Общий принцип работы: из файла переписываем в массив; в конце все записываем в файл
int main()
{
	setlocale(LC_ALL, "Rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	system("color E0");
	int k;
	while (true) 
	{
		system("cls");
		cout << "|=================================================|" << endl;
		cout << "|                 Вход в программу                |" << endl;
		cout << "|=================================================|" << endl;
		cout << "|" << setw(49) << left << "1. Меню администратора" << "|" << endl;
		cout << "|" << setw(49) << left << "2. Меню пользователя" << "|" << endl;
		cout << "|" << setw(49) << left << "0. Выйти из программы" << "|"
			<< "\n|=================================================|" << endl;

		while (!scanf_s("%d", &k))
		{
			rewind(stdin);
			printf("Введите число\n");
		}
		switch (k)
		{
		case 1:
		{
			cout << "Для входа в меню требуется пройти авторизацию\n";
			do
			{
				if (!admin_check()) cout << "Вы не авторизованы\n";
				else
				{
					admin_menu();
					break;
				}
				cout << "Желаете продолжить?\n";
			} while (proceed());
			

			break;
		}
		case 2:
		{
			user_menu();
			break;
		}
		case 0:
			return 0;
		default:
			cout << "Введите пункт меню от 0 до 2 " << endl;
			system("pause");
		}
	}

	return 0;
}

//проверка на админимтратора
bool admin_check()
{
	FILE* f;
	user** ms = (user**)calloc(size, sizeof(user*)); //массив указателей
	ms = write_from_file_to_array(admin, ms);
	user* st; //структура для проверки человека
	if (!(st = (user*)calloc(1, sizeof(user))))
	{
		puts("ошибка при выделении памяти");
		return 0;
	}
	int sum = 0; //хэш-сумма
	rewind(stdin);
	cout << "Введите логин\n";
	cin >> st->login;
	cout << "Введите пароль\n";
	cin >> st->password;
	st->next = nullptr;

	sum += h(st->login);
	sum += h(st->password);
	sum = sum % size; // получение индекса

	bool flg = false;
	if (authorization_check(ms, st, sum)) flg = true;

	FREE(&ms);
	free(st);
	return flg;
}

//меню админа
int admin_menu()
{
	FILE* f;
	user** ms = (user**)calloc(size, sizeof(user*)); //массив указателей
	int k = 0;
	//флаги для очистки памяти
	bool flag = false, //флаг того, что мы только создали новый файл и в том числе икслючения ошибок (удаления и регистрации человека) - чтобы не переписывать из файла которого нет, т.к. изначально 2 флаг false
						// в нашем сеансе скидывали или нет файл
		flag2 = false,//говорит о том что мы из файла еще не переписывали в массив
		flag3 = false; //делали ли вообще что-то с массивом, чтобы его записывать при выходе

	while (1)
	{
		system("cls");
		cout << "|=================================================|" << endl;
		cout << "|                Меню администратора              |" << endl;
		cout << "|=================================================|" << endl;
		cout << "|" << setw(49) << left << "1. Создать базу данных" << "|" << endl;
		cout << "|" << setw(49) << left << "2. Регистрация пользователя" << "|" << endl;
		cout << "|" << setw(49) << left << "3. Удалить пользователя" << "|" << endl;
		cout << "|" << setw(49) << left << "4. Вывести базу данных из файла" << "|" << endl;
		cout << "|" << setw(49) << left << "5. Добавить администратора" << "|" << endl;
		cout << "|" << setw(49) << left << "0. Выход" << "|"
			<< "\n|=================================================|" << endl;

		while (!scanf_s("%d", &k))
		{
			rewind(stdin);
			printf("Введите пункт меню от 0 до 5\n");
		}
		switch (k)
		{
		case 1:
		{
			cout << "Вы точно желаете удалить всех существующих пользователей?\n";
			if (proceed())
			{
				fopen_s(&f, clients, "w+b"); //очистка файла
				fclose(f);
				cout << "Добавить первого пользователя\n";
				ms = registration(ms);
				flag = true;
				flag3 = true;
			}
			break;
		}
		case 2:
		{
			if (flag) ms = registration(ms);
			else
			{
				if (!flag2)
				{
					ms = write_from_file_to_array(clients, ms);
					flag2 = true;
				}
				ms = registration(ms);
			}
			flag3 = true;
			break;
		}
		case 3:
		{
			//вывод всех пользователей
			user** work = (user**)calloc(size, sizeof(user*)); //массив указателей
			work = write_from_file_to_array(clients, work);
			print(work);
			cout << endl << endl;

			cout << "Введите логин и пароль удаляемого пользователя\n";
			user* st = (user*)calloc(1, sizeof(user));
			cout << "Логин:\n";
			cin >> st->login;
			cout << "Пароль:\n";
			cin >> st->password;

			if (flag) ms = del(ms, st);
			else
			{
				if (!flag2)
				{
					ms = write_from_file_to_array(clients, ms);
					flag2 = true;
				}
				ms = del(ms, st);
			}

			flag3 = true;
			free(st);
			FREE(&work);
			break;
		}
		case 4:
		{
			user** work = (user**)calloc(size, sizeof(user*)); //массив указателей
			work = write_from_file_to_array(clients, work);
			print(work);
			cout << endl;

			FREE(&work);
			break;
		}
		case 5:
		{
			user** work = (user**)calloc(size, sizeof(user*)); //массив указателей
			work = write_from_file_to_array(admin, work);
			work = registration(work);
			exit(work, admin);
			FREE(&work);
			break;
		}
		case 0:
		{
			if (flag3)
			{
				exit(ms, clients);
				FREE(&ms);
			}
			else free(ms);
			return 0;
		}
		default:
			cout << "Введите пункт меню от 0 до 5 " << endl;
		}
		cout << endl;
		system("pause");
	}
}

//меню пользователя
int user_menu()
{
	FILE* f;
	int k;

	while (1)
	{
		system("CLS");
		cout << "|=================================================|" << endl;
		cout << "|                 Меню пользователя               |" << endl;
		cout << "|=================================================|" << endl;
		cout << "|" << setw(49) << left << "1. Проверка авторизации" << "|" << endl;
		cout << "|" << setw(49) << left << "2. Регистрация пользователя" << "|" << endl;
		cout << "|" << setw(49) << left << "0. Выход" << "|"
			<< "\n|=================================================|" << endl;
		while (!scanf_s("%d", &k))
		{
			rewind(stdin);
			printf("Введите пункт меню от 0 до 2\n");
		}
		switch (k)
		{
		case 1:
		{
			user** work = (user**)calloc(size, sizeof(user*)); //массив указателей
			work = write_from_file_to_array(clients, work);

			user* st; //структура для проверки человека
			if (!(st = (user*)calloc(1, sizeof(user))))
			{
				puts("ошибка при выделении памяти");
				return 0;
			}
			int sum = 0; //хэш-сумма
			rewind(stdin);
			cout << "Введите логин\n";
			cin >> st->login;
			cout << "Введите пароль\n";
			cin >> st->password;
			st->next = nullptr;

			sum += h(st->login);
			sum += h(st->password);
			sum = sum % size; // получение индекса

			if (authorization_check(work, st, sum)) cout << "Этот пользователь зарегистрирован\n";
			else cout << "Этот пользователь не зарегистрирован\n";

			FREE(&work);
			system("pause");
			break;
		}
		case 2:
		{
			user** ms = (user**)calloc(size, sizeof(user*)); //массив указателей
			ms = write_from_file_to_array(clients, ms);
			ms = registration(ms);
			exit(ms, clients);
			FREE(&ms);
			break;
		}
		case 0: return 0;
		default:
			cout << "Введите пункт меню от 0 до 2 " << endl;
			system("pause");
		}
	}
}

//функция хэширования
void hashing(char* str, user* st)
{
	srand(time(NULL)); //инициализируем генератор случайных чисел
	char* randsol = (char*)calloc(SALT_SIZE, sizeof(char)); //рандомная соль
	for (int i = 0; i < SALT_SIZE; ++i)
	{
		switch (rand() % 3) //генерируем случайное число от 0 до 2
		{
		case 0: //если ноль
			randsol[i] = rand() % 10 + '0'; //вставляем в пароль случайную цифру
			break;
		case 1: //если единица
			randsol[i] = rand() % 26 + 'A'; //вставляем случайную большую букву
			break;
		case 2: //если двойка
			randsol[i] = rand() % 26 + 'a'; //вставляем случайную маленькую букву
		}
	}
	randsol[SALT_SIZE - 1] = '\0'; //записываем в конец строки признак конца строки
	strcpy(st->sol, randsol);

	int lenpassword = strlen(str);
	int lenrand = strlen(randsol);
	int lengener = strlen(genersol);

	//формирование хэша
	char* line = (char*)calloc(lenpassword + lenrand + lengener + 1, sizeof(char));
	strcpy(line, str);  //копируем сначала строку от пользователя
	for (int i = 0; i < lenrand; i++) line[lenpassword +i] = randsol[i]; //затем копируем рандомную соль
	for (int i = 0; i < lengener; i++) line[lenpassword + lenrand + i] = genersol[i]; // и в конце копируем общую соль
	line[lenpassword + lenrand + lengener] = '\0';

	char* hash = (char*)calloc(64, sizeof(char)); //64 т.к. это длина стандартного хэша
	MD5(&hash, &line);
	strcpy(st->hash, hash);

	free(line);
	free(randsol);
	free(hash);
}

// функция получения индекса в хэш таблице
int h(char* word)
{
	int index = 0;
	int i = 0;
	while (word[i] != '\0') index += word[i++] - '0';
	return index % size;
}

// функция добавления нового пользователя и добавления его по индексу хэш-суммы
user** registration(user** ms)
{
	while (1)
	{
		int sum = 0; //хэш-сумма
		user* st; //структура для нового человека
		while (1)
		{
			if (!(st = (user*)calloc(1, sizeof(user))))
			{
				puts("ошибка при выделении памяти");
				return nullptr;
			}
			
			rewind(stdin);
			cout << "Введите логин\n";
			cin >> st->login;
			if (!check_login(ms, st->login)) //проверка на то, есть ли в базе пользователь с таким логином (возвращает true если есть)
			{
				if (check_letters(st->login)) //проверка использованных символов в логине
				{
					rewind(stdin);
					cout << "Введите пароль\n";
					cin >> st->password;
					if (check_letters(st->password)) //проверка использованных символов в пароле
					{
						st->next = nullptr;
						if (check_difficulty(st)) break; //если все нормально выход из цикла
					}
					else
					{
						cout << "Использованы запрещенные символы\n";
						cout << "Разрешены: буквы латинского алфавита и цифры\n\n";
					}
				}
				else
				{
					cout << "Использованы запрещенные символы\n";
					cout << "Разрешены: буквы латинского алфавита и цифры\n\n";
				}
			}
			else
			{
				cout << "Пользователь с таким логином уже существует\n";
				cout << "Пожалуйста, введите другой логин\n";
			}
			free(st);
		}
		
		sum += h(st->login);
		sum += h(st->password);
		sum = sum % size; // получение индекса

		if (!authorization_check(ms, st, sum))
		{
			cout << "Пользователь добавлен\n";

			//находим сумму логина и пароля
			int lenlogin = strlen(st->login);
			int lenpass = strlen(st->password);
			char* line = (char*)calloc(lenlogin + lenpass + 1, sizeof(char));
			for (int i = 0; i < lenlogin; i++) line[i] = st->login[i];
			for (int i = 0; i < lenpass; i++) line[i + lenlogin] = st->password[i];
			line[lenlogin + lenpass] = '\0';

			hashing(line, st); //отправляем сумму хэшироваться
			free(line);

			//добавление в массив
			user* work;
			if (ms[sum])
			{
				work = ms[sum];
				while (work->next)
					work = work->next;
				work->next = st;
			}
			else ms[sum] = st;
			return ms;
		}
		else cout << "Такой пользователь уже существует\nПоменяйте логин или пароль\n\n";
		free(st);
	}
}

//функция записи массива в файл, вызывается только при выходе
void exit(user** ms, char* name)
{
	FILE* f;
	fopen_s(&f, name, "w+b");
	rewind(f);
	user* work; //рабочий указатель
	for (int i = 0; i < size; i++) //цикл заполнения файла \n для отделения структур с одинаковыми индексами (хэш-суммами)
	{
		if (!ms[i]) fprintf(f, "%c", '\n'); //
		else
		{
			work = ms[i];
			while (work)
			{
				fwrite(work->login, sizeof(char), length, f);
				fprintf(f, "%c", '|');
				fwrite(work->password, sizeof(char), length, f);
				fprintf(f, "%c", '|');
				fwrite(work->hash, sizeof(char), 64, f);
				fprintf(f, "%c", '|');
				fwrite(work->sol, sizeof(char), SALT_SIZE, f);
				fprintf(f, "%c", '|');
				if (work->next) fwrite(work->next, sizeof(user*), 1, f);
				else fprintf(f, "%c", '\0');
				fprintf(f, "%c", '|');

				work = work->next;
			}
			fprintf(f, "%c", '\n');
		}
	}
	fclose(f);
}

// функция записи в массив из файла
user** write_from_file_to_array(char* name, user** ms)
{
	FILE* f;
	fopen_s(&f, name, "rb");

	char c;
	int i = 0;
	user* ptr, * work; //рабочий указатель
	fpos_t pos;
	char str[length];

	while (!feof(f))
	{
		fscanf(f, "%c", &c);
		if (c == '\n') i++; //переход к следующему индексу в массиве
		else
		{
			fgetpos(f, &pos); //возвращаем УТПФ на начало записи о структуре
			pos--;
			fsetpos(f, &pos);

			if (!ms[i]) //если в массиве по этому индексу еще нет структуры
			{
				ms[i] = (user*)calloc(1, sizeof(user));
				fread(&ms[i]->login, sizeof(char), length, f);
				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);
				
				fread(&ms[i]->password, sizeof(char), length, f);
				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);
				
				fread(&ms[i]->hash, sizeof(char), 64, f); //считываем хэш из файла
				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);

				fread(&ms[i]->sol, sizeof(char), SALT_SIZE, f); //считываем индивидуальную соль из файла
				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);

				fread(&str, sizeof(char), 1, f); //чисто для того чтобы переместить УТПФ на нужное положение
				if (str[0] != '\0')
				{
					pos += sizeof(user*);
					fsetpos(f, &pos);
				}
				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);

				ms[i]->next = nullptr;
			}
			else
			{
				ptr = (user*)calloc(1, sizeof(user));

				fread(&ptr->login, sizeof(char), length, f);
				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);

				fread(&ptr->password, sizeof(char), length, f);
				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);
				
				fread(&ptr->hash, sizeof(char), 64, f); //считываем хэш из файла
				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);

				fread(&ptr->sol, sizeof(char), SALT_SIZE, f); //считываем индивидуальную соль из файла
				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);

				fread(&str, sizeof(char), 1, f); 
				if (str[0] != '\0')
				{
					pos += sizeof(user*);
					fsetpos(f, &pos);
				}

				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);

				ptr->next = nullptr;

				work = ms[i];
				while (work->next) work = work->next;
				work->next = ptr;
			}
		}
	}

	fclose(f);
	return ms;
}

// функция удаления элемента в массиве
user** del(user** ms, user* st)
{
	int sum = 0;
	sum += h(st->login);
	sum += h(st->password);
	sum = sum % size; // получение индекса

	user* us = ms[sum]; //указатель ставим по хэш-сумме
	user* beforeUs = us;  //указатель за основным указателем
	if (us)
	{
		while (us)
		{
			if (!strcmp(us->login, st->login) && !strcmp(us->password, st->password)) //если совпадает с введенным - удаляем
			{
				if (beforeUs->next == us->next)
				{
					user* kk = us;
					ms[sum] = ms[sum]->next; //перемещаем индекс на стек вниз на 1 элемент
					free(kk);
					cout << "Пользователь удален\n";
				}
				else
				{
					beforeUs->next = us->next; //удаляем из цепи
					free(us);
					cout << "Пользователь удален\n";
				}
				return ms;
			}
			beforeUs = us;
			us = us->next;
		}
	}
	else 
	{
		cout << "Такого пользователя не существует\n";
		return ms;
	}
	return ms;
}

// функция вывода массива
void print(user** ms)
{
	user* work; //рабочий указатель
	cout << "\n|===============================================================================================|"
		<< "\n|                                   *База данных пользователей*                                 |"
		<< "\n|===============================================================================================|"
		<< "\n|  №  |      Логин      |        Пароль        |                  Хэш                  |  Соль  |"
		<< "\n|===============================================================================================|";

	for (int i = 0; i < size; i++) 
	{
		int j = 1;
		if (ms[i])
		{
			work = ms[i];
			while (work)
			{
				cout << "\n| " << setw(4) << left << (j++)
					<< "| " << setw(16) << left << work->login
					<< "| " << setw(21) << left << work->password
					<< "| " << setw(38) << left << work->hash
					<< "| " << setw(7) << left << work->sol << "|";
				work = work->next;
			}
			cout << "\n|-----------------------------------------------------------------------------------------------|";
		}
	}
		
}

// функция очистки памяти под массив
void FREE(user*** ms)
{
	//ОЧИСТКА ПАМЯТИ
	user* runner; //рабочий указатель
	user* behind_runner;
	for (int i = 0; i < size; i++) 
	{
		if ((*ms)[i])
		{
			runner = (*ms)[i];
			while (runner)
			{
				behind_runner = runner;
				runner = runner->next;
				free(behind_runner);
			}
		}
	}
	free(*ms);
}

// функция проверки авторизации пользователя
bool authorization_check(user** ms, user* st, int sum)
{
	//просмотр в массиве по индексу
	user* work; //рабочик указатель для просмотра всех структур
	char* lineofhash = (char*)calloc(64, sizeof(char)); //строка хэша которую будем формировать для сравнения с хэшем переменной work

	int lengener = strlen(genersol);
	int lenpassword = strlen(st->password);
	int lenlogin = strlen(st->login);
	int lenrand = SALT_SIZE;

	//формирование строки с данными
	char* line = (char*)calloc(lenpassword + lenlogin + lenrand + lengener + 1, sizeof(char));
	for (int i = 0; i < lenlogin; i++) line[i] = st->login[i]; //копируем логин и пароль от пользователя
	for (int i = 0; i < lenpassword; i++) line[i + lenlogin] = st->password[i];
	

	if (ms[sum])
	{
		work = ms[sum];
		while (work)
		{
			for (int i = 0; i < lenrand; i++) line[lenpassword + lenlogin + i] = work->sol[i]; //для каждой структуры перезаписываем место с солю просматриваемой структуры
			for (int i = 0; i < lengener; i++) 
				line[lenpassword + lenlogin + lenrand - 1 + i] = genersol[i]; // и в конце копируем общую соль
			line[lenpassword + lenlogin + lenrand + lengener] = '\0';

			MD5(&lineofhash, &line);//получение из строки хэша
			if (!strcmp(work->hash, lineofhash))
				return true;
			else work = work->next;
		}
		return false;
	}
	else return false;
}

// функция проверки сложности пароля и логина
bool check_difficulty(user* st)
{
	char c;
	if (strlen(st->login) >= MIN_LEN_LOGIN && strlen(st->password) >= MIN_LEN_PASSWORD)
	{
		char* line = (char*)calloc(length, sizeof(char));
		line = get_letters_from_line(st->password);
		if (strlen(line) >= COUNT_LETTERS_PASSWORD) return true;
		else
		{
			cout << "Вы ввели слишком простой пароль\n";
			cout << "Желаете продолжить? (y or n)\n";
			cin >> c;
			if (c == 'y') return true;
			else return false;
		}
	}
	else
	{
		cout << "Вы ввели слишком короткий логин или пароль\n";
		cout << "Желаете продолжить? (y or n)\n";
		if (proceed()) return true;
		else return false;
	}
}

// функция копирования в новую строку только букв из str
char* get_letters_from_line(char* str)
{
	char* line = (char*)calloc(length, sizeof(char));
	int j = 0;
	for (int i = 0; str[i]; i++)
	{
		if (((str[i] - 'A') >= 0 && (str[i] - 'A') < 32) || ((str[i] - 'a') >= 0 && (str[i] - 'a') < 32))
			line[j++] = str[i];
	}
	line[j] = '\0';
	return line;
}

// функция для проверки желания продолжить
bool proceed()
{
	char* str = (char*)calloc(10, sizeof(char));
	cin >> str;
	if (!strcmp(str, "yes") || !strcmp(str, "Yes") || !strcmp(str, "y") || !strcmp(str, "Y")
		|| !strcmp(str, "да") || !strcmp(str, "Да") || !strcmp(str, "д") || !strcmp(str, "Д")
		|| !strcmp(str, "+") || !strcmp(str, "Конечно") || !strcmp(str, "конечно"))
	{
		return true;
	}
	else return false;
}

// функция проверки наличия такого же логина в базе
bool check_login(user** ms, char* str)
{
	for(int i=0; i<size; i++)
		if (ms[i])
		{
			user* work = ms[i];
			while (work)
			{
				if (!strcmp(work->login, str)) return true;
				work = work->next;
			}
		}
	return false;
}

// функция проверки использованных символов (можно только англ и цифры)
bool check_letters(char* str)
{
	for (int i = 0; str[i]; i++)
	{
		if (!(((str[i] - 'A') >= 0 && (str[i] - 'A') < 32)
			|| ((str[i] - 'a') >= 0 && (str[i] - 'a') < 32)
			|| ((str[i] - '0') >= 0 && (str[i] - '0') < 10)))
			return false;	
	}
	return true; //все нормально
}



