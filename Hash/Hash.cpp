// ��������� ��� ���������� ���-����� ������ � ������ + �������� �����������

#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <iomanip>
#include "md5.h"
#include "MainMD5.h"
#pragma warning(disable : 4996)

#define SALT_SIZE 5 // ������ ��������� ���� + '\0'
char genersol[] = "ABC@##!?+234243"; // - ����� ����
#define size 10 //������ ���-�������
#define length 10 //������ ������������� ������ � ������
#define MIN_LEN_PASSWORD 4
#define MIN_LEN_LOGIN 4
#define COUNT_LETTERS_PASSWORD 3

char admin[] = "admin"; //���� � ����������������
char clients[] = "database"; //���� � ��������������

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


//����� ������� ������: �� ����� ������������ � ������; � ����� ��� ���������� � ����
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
		cout << "|                 ���� � ���������                |" << endl;
		cout << "|=================================================|" << endl;
		cout << "|" << setw(49) << left << "1. ���� ��������������" << "|" << endl;
		cout << "|" << setw(49) << left << "2. ���� ������������" << "|" << endl;
		cout << "|" << setw(49) << left << "0. ����� �� ���������" << "|"
			<< "\n|=================================================|" << endl;

		while (!scanf_s("%d", &k))
		{
			rewind(stdin);
			printf("������� �����\n");
		}
		switch (k)
		{
		case 1:
		{
			cout << "��� ����� � ���� ��������� ������ �����������\n";
			do
			{
				if (!admin_check()) cout << "�� �� ������������\n";
				else
				{
					admin_menu();
					break;
				}
				cout << "������� ����������?\n";
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
			cout << "������� ����� ���� �� 0 �� 2 " << endl;
			system("pause");
		}
	}

	return 0;
}

//�������� �� ��������������
bool admin_check()
{
	FILE* f;
	user** ms = (user**)calloc(size, sizeof(user*)); //������ ����������
	ms = write_from_file_to_array(admin, ms);
	user* st; //��������� ��� �������� ��������
	if (!(st = (user*)calloc(1, sizeof(user))))
	{
		puts("������ ��� ��������� ������");
		return 0;
	}
	int sum = 0; //���-�����
	rewind(stdin);
	cout << "������� �����\n";
	cin >> st->login;
	cout << "������� ������\n";
	cin >> st->password;
	st->next = nullptr;

	sum += h(st->login);
	sum += h(st->password);
	sum = sum % size; // ��������� �������

	bool flg = false;
	if (authorization_check(ms, st, sum)) flg = true;

	FREE(&ms);
	free(st);
	return flg;
}

//���� ������
int admin_menu()
{
	FILE* f;
	user** ms = (user**)calloc(size, sizeof(user*)); //������ ����������
	int k = 0;
	//����� ��� ������� ������
	bool flag = false, //���� ����, ��� �� ������ ������� ����� ���� � � ��� ����� ���������� ������ (�������� � ����������� ��������) - ����� �� ������������ �� ����� �������� ���, �.�. ���������� 2 ���� false
						// � ����� ������ ��������� ��� ��� ����
		flag2 = false,//������� � ��� ��� �� �� ����� ��� �� ������������ � ������
		flag3 = false; //������ �� ������ ���-�� � ��������, ����� ��� ���������� ��� ������

	while (1)
	{
		system("cls");
		cout << "|=================================================|" << endl;
		cout << "|                ���� ��������������              |" << endl;
		cout << "|=================================================|" << endl;
		cout << "|" << setw(49) << left << "1. ������� ���� ������" << "|" << endl;
		cout << "|" << setw(49) << left << "2. ����������� ������������" << "|" << endl;
		cout << "|" << setw(49) << left << "3. ������� ������������" << "|" << endl;
		cout << "|" << setw(49) << left << "4. ������� ���� ������ �� �����" << "|" << endl;
		cout << "|" << setw(49) << left << "5. �������� ��������������" << "|" << endl;
		cout << "|" << setw(49) << left << "0. �����" << "|"
			<< "\n|=================================================|" << endl;

		while (!scanf_s("%d", &k))
		{
			rewind(stdin);
			printf("������� ����� ���� �� 0 �� 5\n");
		}
		switch (k)
		{
		case 1:
		{
			cout << "�� ����� ������� ������� ���� ������������ �������������?\n";
			if (proceed())
			{
				fopen_s(&f, clients, "w+b"); //������� �����
				fclose(f);
				cout << "�������� ������� ������������\n";
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
			//����� ���� �������������
			user** work = (user**)calloc(size, sizeof(user*)); //������ ����������
			work = write_from_file_to_array(clients, work);
			print(work);
			cout << endl << endl;

			cout << "������� ����� � ������ ���������� ������������\n";
			user* st = (user*)calloc(1, sizeof(user));
			cout << "�����:\n";
			cin >> st->login;
			cout << "������:\n";
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
			user** work = (user**)calloc(size, sizeof(user*)); //������ ����������
			work = write_from_file_to_array(clients, work);
			print(work);
			cout << endl;

			FREE(&work);
			break;
		}
		case 5:
		{
			user** work = (user**)calloc(size, sizeof(user*)); //������ ����������
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
			cout << "������� ����� ���� �� 0 �� 5 " << endl;
		}
		cout << endl;
		system("pause");
	}
}

//���� ������������
int user_menu()
{
	FILE* f;
	int k;

	while (1)
	{
		system("CLS");
		cout << "|=================================================|" << endl;
		cout << "|                 ���� ������������               |" << endl;
		cout << "|=================================================|" << endl;
		cout << "|" << setw(49) << left << "1. �������� �����������" << "|" << endl;
		cout << "|" << setw(49) << left << "2. ����������� ������������" << "|" << endl;
		cout << "|" << setw(49) << left << "0. �����" << "|"
			<< "\n|=================================================|" << endl;
		while (!scanf_s("%d", &k))
		{
			rewind(stdin);
			printf("������� ����� ���� �� 0 �� 2\n");
		}
		switch (k)
		{
		case 1:
		{
			user** work = (user**)calloc(size, sizeof(user*)); //������ ����������
			work = write_from_file_to_array(clients, work);

			user* st; //��������� ��� �������� ��������
			if (!(st = (user*)calloc(1, sizeof(user))))
			{
				puts("������ ��� ��������� ������");
				return 0;
			}
			int sum = 0; //���-�����
			rewind(stdin);
			cout << "������� �����\n";
			cin >> st->login;
			cout << "������� ������\n";
			cin >> st->password;
			st->next = nullptr;

			sum += h(st->login);
			sum += h(st->password);
			sum = sum % size; // ��������� �������

			if (authorization_check(work, st, sum)) cout << "���� ������������ ���������������\n";
			else cout << "���� ������������ �� ���������������\n";

			FREE(&work);
			system("pause");
			break;
		}
		case 2:
		{
			user** ms = (user**)calloc(size, sizeof(user*)); //������ ����������
			ms = write_from_file_to_array(clients, ms);
			ms = registration(ms);
			exit(ms, clients);
			FREE(&ms);
			break;
		}
		case 0: return 0;
		default:
			cout << "������� ����� ���� �� 0 �� 2 " << endl;
			system("pause");
		}
	}
}

//������� �����������
void hashing(char* str, user* st)
{
	srand(time(NULL)); //�������������� ��������� ��������� �����
	char* randsol = (char*)calloc(SALT_SIZE, sizeof(char)); //��������� ����
	for (int i = 0; i < SALT_SIZE; ++i)
	{
		switch (rand() % 3) //���������� ��������� ����� �� 0 �� 2
		{
		case 0: //���� ����
			randsol[i] = rand() % 10 + '0'; //��������� � ������ ��������� �����
			break;
		case 1: //���� �������
			randsol[i] = rand() % 26 + 'A'; //��������� ��������� ������� �����
			break;
		case 2: //���� ������
			randsol[i] = rand() % 26 + 'a'; //��������� ��������� ��������� �����
		}
	}
	randsol[SALT_SIZE - 1] = '\0'; //���������� � ����� ������ ������� ����� ������
	strcpy(st->sol, randsol);

	int lenpassword = strlen(str);
	int lenrand = strlen(randsol);
	int lengener = strlen(genersol);

	//������������ ����
	char* line = (char*)calloc(lenpassword + lenrand + lengener + 1, sizeof(char));
	strcpy(line, str);  //�������� ������� ������ �� ������������
	for (int i = 0; i < lenrand; i++) line[lenpassword +i] = randsol[i]; //����� �������� ��������� ����
	for (int i = 0; i < lengener; i++) line[lenpassword + lenrand + i] = genersol[i]; // � � ����� �������� ����� ����
	line[lenpassword + lenrand + lengener] = '\0';

	char* hash = (char*)calloc(64, sizeof(char)); //64 �.�. ��� ����� ������������ ����
	MD5(&hash, &line);
	strcpy(st->hash, hash);

	free(line);
	free(randsol);
	free(hash);
}

// ������� ��������� ������� � ��� �������
int h(char* word)
{
	int index = 0;
	int i = 0;
	while (word[i] != '\0') index += word[i++] - '0';
	return index % size;
}

// ������� ���������� ������ ������������ � ���������� ��� �� ������� ���-�����
user** registration(user** ms)
{
	while (1)
	{
		int sum = 0; //���-�����
		user* st; //��������� ��� ������ ��������
		while (1)
		{
			if (!(st = (user*)calloc(1, sizeof(user))))
			{
				puts("������ ��� ��������� ������");
				return nullptr;
			}
			
			rewind(stdin);
			cout << "������� �����\n";
			cin >> st->login;
			if (!check_login(ms, st->login)) //�������� �� ��, ���� �� � ���� ������������ � ����� ������� (���������� true ���� ����)
			{
				if (check_letters(st->login)) //�������� �������������� �������� � ������
				{
					rewind(stdin);
					cout << "������� ������\n";
					cin >> st->password;
					if (check_letters(st->password)) //�������� �������������� �������� � ������
					{
						st->next = nullptr;
						if (check_difficulty(st)) break; //���� ��� ��������� ����� �� �����
					}
					else
					{
						cout << "������������ ����������� �������\n";
						cout << "���������: ����� ���������� �������� � �����\n\n";
					}
				}
				else
				{
					cout << "������������ ����������� �������\n";
					cout << "���������: ����� ���������� �������� � �����\n\n";
				}
			}
			else
			{
				cout << "������������ � ����� ������� ��� ����������\n";
				cout << "����������, ������� ������ �����\n";
			}
			free(st);
		}
		
		sum += h(st->login);
		sum += h(st->password);
		sum = sum % size; // ��������� �������

		if (!authorization_check(ms, st, sum))
		{
			cout << "������������ ��������\n";

			//������� ����� ������ � ������
			int lenlogin = strlen(st->login);
			int lenpass = strlen(st->password);
			char* line = (char*)calloc(lenlogin + lenpass + 1, sizeof(char));
			for (int i = 0; i < lenlogin; i++) line[i] = st->login[i];
			for (int i = 0; i < lenpass; i++) line[i + lenlogin] = st->password[i];
			line[lenlogin + lenpass] = '\0';

			hashing(line, st); //���������� ����� ������������
			free(line);

			//���������� � ������
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
		else cout << "����� ������������ ��� ����������\n��������� ����� ��� ������\n\n";
		free(st);
	}
}

//������� ������ ������� � ����, ���������� ������ ��� ������
void exit(user** ms, char* name)
{
	FILE* f;
	fopen_s(&f, name, "w+b");
	rewind(f);
	user* work; //������� ���������
	for (int i = 0; i < size; i++) //���� ���������� ����� \n ��� ��������� �������� � ����������� ��������� (���-�������)
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

// ������� ������ � ������ �� �����
user** write_from_file_to_array(char* name, user** ms)
{
	FILE* f;
	fopen_s(&f, name, "rb");

	char c;
	int i = 0;
	user* ptr, * work; //������� ���������
	fpos_t pos;
	char str[length];

	while (!feof(f))
	{
		fscanf(f, "%c", &c);
		if (c == '\n') i++; //������� � ���������� ������� � �������
		else
		{
			fgetpos(f, &pos); //���������� ���� �� ������ ������ � ���������
			pos--;
			fsetpos(f, &pos);

			if (!ms[i]) //���� � ������� �� ����� ������� ��� ��� ���������
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
				
				fread(&ms[i]->hash, sizeof(char), 64, f); //��������� ��� �� �����
				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);

				fread(&ms[i]->sol, sizeof(char), SALT_SIZE, f); //��������� �������������� ���� �� �����
				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);

				fread(&str, sizeof(char), 1, f); //����� ��� ���� ����� ����������� ���� �� ������ ���������
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
				
				fread(&ptr->hash, sizeof(char), 64, f); //��������� ��� �� �����
				fgetpos(f, &pos);
				pos++;
				fsetpos(f, &pos);

				fread(&ptr->sol, sizeof(char), SALT_SIZE, f); //��������� �������������� ���� �� �����
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

// ������� �������� �������� � �������
user** del(user** ms, user* st)
{
	int sum = 0;
	sum += h(st->login);
	sum += h(st->password);
	sum = sum % size; // ��������� �������

	user* us = ms[sum]; //��������� ������ �� ���-�����
	user* beforeUs = us;  //��������� �� �������� ����������
	if (us)
	{
		while (us)
		{
			if (!strcmp(us->login, st->login) && !strcmp(us->password, st->password)) //���� ��������� � ��������� - �������
			{
				if (beforeUs->next == us->next)
				{
					user* kk = us;
					ms[sum] = ms[sum]->next; //���������� ������ �� ���� ���� �� 1 �������
					free(kk);
					cout << "������������ ������\n";
				}
				else
				{
					beforeUs->next = us->next; //������� �� ����
					free(us);
					cout << "������������ ������\n";
				}
				return ms;
			}
			beforeUs = us;
			us = us->next;
		}
	}
	else 
	{
		cout << "������ ������������ �� ����������\n";
		return ms;
	}
	return ms;
}

// ������� ������ �������
void print(user** ms)
{
	user* work; //������� ���������
	cout << "\n|===============================================================================================|"
		<< "\n|                                   *���� ������ �������������*                                 |"
		<< "\n|===============================================================================================|"
		<< "\n|  �  |      �����      |        ������        |                  ���                  |  ����  |"
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

// ������� ������� ������ ��� ������
void FREE(user*** ms)
{
	//������� ������
	user* runner; //������� ���������
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

// ������� �������� ����������� ������������
bool authorization_check(user** ms, user* st, int sum)
{
	//�������� � ������� �� �������
	user* work; //������� ��������� ��� ��������� ���� ��������
	char* lineofhash = (char*)calloc(64, sizeof(char)); //������ ���� ������� ����� ����������� ��� ��������� � ����� ���������� work

	int lengener = strlen(genersol);
	int lenpassword = strlen(st->password);
	int lenlogin = strlen(st->login);
	int lenrand = SALT_SIZE;

	//������������ ������ � �������
	char* line = (char*)calloc(lenpassword + lenlogin + lenrand + lengener + 1, sizeof(char));
	for (int i = 0; i < lenlogin; i++) line[i] = st->login[i]; //�������� ����� � ������ �� ������������
	for (int i = 0; i < lenpassword; i++) line[i + lenlogin] = st->password[i];
	

	if (ms[sum])
	{
		work = ms[sum];
		while (work)
		{
			for (int i = 0; i < lenrand; i++) line[lenpassword + lenlogin + i] = work->sol[i]; //��� ������ ��������� �������������� ����� � ���� ��������������� ���������
			for (int i = 0; i < lengener; i++) 
				line[lenpassword + lenlogin + lenrand - 1 + i] = genersol[i]; // � � ����� �������� ����� ����
			line[lenpassword + lenlogin + lenrand + lengener] = '\0';

			MD5(&lineofhash, &line);//��������� �� ������ ����
			if (!strcmp(work->hash, lineofhash))
				return true;
			else work = work->next;
		}
		return false;
	}
	else return false;
}

// ������� �������� ��������� ������ � ������
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
			cout << "�� ����� ������� ������� ������\n";
			cout << "������� ����������? (y or n)\n";
			cin >> c;
			if (c == 'y') return true;
			else return false;
		}
	}
	else
	{
		cout << "�� ����� ������� �������� ����� ��� ������\n";
		cout << "������� ����������? (y or n)\n";
		if (proceed()) return true;
		else return false;
	}
}

// ������� ����������� � ����� ������ ������ ���� �� str
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

// ������� ��� �������� ������� ����������
bool proceed()
{
	char* str = (char*)calloc(10, sizeof(char));
	cin >> str;
	if (!strcmp(str, "yes") || !strcmp(str, "Yes") || !strcmp(str, "y") || !strcmp(str, "Y")
		|| !strcmp(str, "��") || !strcmp(str, "��") || !strcmp(str, "�") || !strcmp(str, "�")
		|| !strcmp(str, "+") || !strcmp(str, "�������") || !strcmp(str, "�������"))
	{
		return true;
	}
	else return false;
}

// ������� �������� ������� ������ �� ������ � ����
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

// ������� �������� �������������� �������� (����� ������ ���� � �����)
bool check_letters(char* str)
{
	for (int i = 0; str[i]; i++)
	{
		if (!(((str[i] - 'A') >= 0 && (str[i] - 'A') < 32)
			|| ((str[i] - 'a') >= 0 && (str[i] - 'a') < 32)
			|| ((str[i] - '0') >= 0 && (str[i] - '0') < 10)))
			return false;	
	}
	return true; //��� ���������
}



