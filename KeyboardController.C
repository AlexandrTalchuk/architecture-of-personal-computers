//924401-2
//������ �.�.
//��-3 ����஫��� ����������
#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <stdlib.h>
#define TETRAD 0x0F
#define INT9 9
#define CHECK_BIT_8 0x80
#define PORT_60 0x60
#define PORT_61 0x61
#define PORT_64 0x64
#define PORT_20 0x20
#define SCAN_ESC 0x01
#define SCAN_I 0x17
#define SYMBOL_SIZE 2
#define QUANTITY_OF_SYMBOLS 80
#define STRING_SIZE 160
#define FIRST_STRING 10
#define LAST_STRING 24
#define INDICATORS_CONTROL 0xED
#define CHECK_BIT_2 0x02
char far* start = (char far*)0xb8000000; // ��砫� ���������
unsigned int count = 0; //������⢮ ᨬ�����, 㦥 �����⠭���
int y = FIRST_STRING; //��ࢠ� ���窠, � ���ன �㤥� ��� ����������
int x = 0; //��� ᨬ����
int isQuit = 0;//��� ���뢠���
char* b2bin(unsigned char bt, char* str);
//��ॢ�� � 2���� ��⥬�
unsigned char bit4to1h(unsigned char bt);
//��ࠤ� � 16����
char* b2h2(unsigned char bt, char* str);
//��ॢ�� � 16���� ��⥬�
void ScrollScreen(void);
//�ப��⪠ ��࠭�
void CleanScreen(int row, int counts);
//���⪠ ��࠭�
void printVideoMemory(char* str, int row, int column);
//�뢮� � ���᮫� �१ ����������� � ��।.����, ��אַ� ���饭�� � ���
void printVideoMemory1(char* str);
//�뢮� � ���᮫� �१ �����������
void ReadStatusRegister(void);
//�뢮� ���祭�� ॣ���஢
void ISRStatus();
//�㭪�� �뢮�� ����� ॣ���� ���㦨����� ���뢠���(ISR) ����஫��� ���뢠���
void IndicatorsBlink(int code);
//�������
void interrupt OwnInterruptHandler(void);
//ᮡ�⢥��� ��ࠡ��稪 ���뢠��� INT9
void interrupt(*oldInt9h)(void);
//㪠��⥫� �� ��ࠡ��稪 INT9, ����ணࠬ�� ��ࠡ�⪨ ���뢠���
void PrintName(void);
//�뢮� ��㯯�, �����, �������� ࠡ���
void main()
{
	clrscr();//���⪠ ⥪�⮢��� ����
	PrintName();
	oldInt9h = getvect(INT9);
	//��࠭���� ��ࠡ��稪� ���뢠���, getvect ���뢠�� ���祭��
	//����� � ����஬ 9, ��� 㪠��⥫� �� �㭪�� ���뢠���
	//�����頥� 4-� ���⮢�� ���祭��, �࠭�饥�� � ����� ���뢠���
	//��࠭���� 㪠��⥫� �� ���� ��ࠡ��稪
	setvect(INT9, OwnInterruptHandler); //��⠭���� ������ �����
	ReadStatusRegister(); //�뢮� ���祭�� ॣ���஢
	ISRStatus(); //�뢮� ᫮�� ���ﭨ� ॣ���� ISR
	while (!isQuit) //���� �� ��⠭����� 䫠� ��室�
	{

	}
	setvect(INT9, oldInt9h); //����⠭������� ��ண� �����
	return;
}
//�뢮� ��㯯�, �����, �������� ࠡ���
void PrintName()
{
	char str0[] = { "||=====================================||" };
	char str1[] = { "||      924401-2 ������ �.�.          ||" };
	char str2[] = { "||     ��-3. ����஫��� ����������     ||" };
	char str3[] = { "||=====================================||" };
	char str4[] = { "||     Esc - ��室, i - ���������     ||" };
	printVideoMemory(str0, 0, 0);
	printVideoMemory(str1, 1, 0);
	printVideoMemory(str2, 2, 0);
	printVideoMemory(str3, 3, 0);
	printVideoMemory(str4, 4, 0);
	//�뢮� �� ���᮫� �१ ����������� � ��।.����
	//����� ��㯯�, ���, �������� ��
}
//��࣠��� �������஢
void IndicatorsBlink(int code)
{
	int i;
	for (i = 0; i < 10; i++)
	{
		while ((inp(PORT_64) & CHECK_BIT_2) != 0x00);
		//�⠥� ���ﭨ� ���� + �஢�ઠ
		outp(PORT_60, INDICATORS_CONTROL);
		//������ � ���� ������� �ࠢ����� �������ࠬ�,
		//������� EDh ��� ��� / �몫 �������஢ 
		while ((inportb(PORT_64) & CHECK_BIT_2) != 0x00);
		//inportb ���뢠�� ���� �� ����
		outp(PORT_60, code);
		delay(200);
		while ((inp(PORT_64) & CHECK_BIT_2) != 0x00);
		outp(PORT_60, INDICATORS_CONTROL);
		while ((inportb(PORT_64) & CHECK_BIT_2) != 0x00);
		outp(PORT_60, 0);
		delay(200);
	}
}
//ᮡ�⢥��� ��ࠡ��稪 ���뢠��� INT9
//��ࠡ��뢠�� ���뢠���, �맢���� ��室�� ����� ������ � ���᪠��� ������
void interrupt OwnInterruptHandler()
{
	unsigned char value = 0;
	char st[3];
	int maxCount;
	maxCount = (25 - 10) * QUANTITY_OF_SYMBOLS;//25 �ᥣ�, ���� 10 �� � ���
	ReadStatusRegister();
	if (count >= maxCount)
	{
		//�� 8 �� 23 ���窨 ࠡ.�������
		y = LAST_STRING;//�ᥣ� 25 ��ப, �� 0 �� 24
		x = 0;
		ScrollScreen();
		count = maxCount - QUANTITY_OF_SYMBOLS;
		//-80 ��-�� �������襩�� ���窨
	}
	//� �⢥� �� ���뢠��� ���뢠�� ��� ᪠��஢��� ������ �� ���� ����������
	value = inp(PORT_60);
	//����砥� ����� ������㥬��� ᪠�-����, ������ ���������� ���� � ॣ���� ������
	if (value == SCAN_ESC) isQuit = 1;
	//��������� ����-� ���� ��६�����, ᪠�-��� esc
	if (value == SCAN_I) IndicatorsBlink(0x07);
	//᪠�-��� ������ ������ i
	b2h2(value, st);//��ॢ�� � 16���� ��⥬�
	printVideoMemory1(st);//�뢮� � ���᮫� �१ �����������
	outp(PORT_20, 0x20);
	delay(100);
	ISRStatus();
	//���� ����஫��� ���뢠���
	//20 h ������ ���� �� ॣ���� ������
	//FA ���⢥ত��� �ਥ�� ���-�
}
// �뢮� � ���᮫� �१ ����������� � ��।.����
void printVideoMemory(char* str, int row, int column)
{
	int i;
	char far* v;
	v = start + row * STRING_SIZE + column * SYMBOL_SIZE;
	//ᨬ��� - 2 ����, � ��ப� �� 80, �.�. ࠧ��� ����� ��ப� 160
	for (i = 0; str[i] != '\0'; i++)
	{
		*v = str[i];
		v += SYMBOL_SIZE;
	}
}
// �뢮� � ���᮫� �१ �����������,��אַ� ���饭�� � ����������
void printVideoMemory1(char* str)
{
	int i;
	char far* v;
	v = start + y * STRING_SIZE + x * SYMBOL_SIZE;
	//ᨬ��� - 2 ����, � ��ப� �� 80, �.�. ࠧ��� ����� ��ப� 160
	for (i = 0; str[i] != '\0'; i++)
	{
		x++;
		*v = str[i];
		v += SYMBOL_SIZE;
		count++;
	}
	if (x >= QUANTITY_OF_SYMBOLS)//�.�. � ��ப� ���頥��� 80 ᨬ�����
	{
		y++;
		x = 0;
	}
	else
	{
		x++;
		*v = ' ';
		v++;
		count++;
	}
}
//��ॢ�� � 16���� ��⥬�
char* b2h2(unsigned char bt, char* str)
{
	str[0] = bit4to1h(bt >> 4);
	str[1] = bit4to1h(bt);
	str[2] = 0;
	return str;
}
//��ࠤ� � 16����
unsigned char bit4to1h(unsigned char bt)
{
	bt &= TETRAD;
	return (bt < 10) ? '0' + bt : 'A' - 10 + bt;
}
//��ॢ�� � 2���� ��⥬�
char* b2bin(unsigned char bt, char* str)
{
	char* pbuf = str;
	int i;
	pbuf[8] = '\0';
	for (i = 0; i < 8; i++)
	{
		*pbuf = (bt & CHECK_BIT_8) ? '1' : '0';
		pbuf++;
		bt = bt << 1;
	}
	return str;
}
//�뢮� ���祭�� ॣ���஢
void ReadStatusRegister()
{
	unsigned char t;
	char str[3];
	char str1[9];
	str[0] = '\0';//��� �ନ����
	str1[0] = '\0';
       	t = inp(PORT_64);//ॣ���� ���ﭨ� ����������
	b2h2(t, str); //��ॢ�� � 16���� ��⥬�
	b2bin(t, str1);//��ॢ�� � 2���� ��⥬�
	printVideoMemory("������� ���ﭨ� (0x64): ", 5, 0);
	printVideoMemory(str1, 5, 44);//�뢮� � ���᮫� �१ ����������� � ��� ����
	printVideoMemory(str, 5, 53);
	t = inp(PORT_61);//ॣ���� ������
	b2h2(t, str); //��ॢ�� � 16���� ��⥬�
	b2bin(t, str1);//��ॢ�� � 2���� ��⥬�
	printVideoMemory("������� ������ (0x61): ", 6, 0);
	printVideoMemory(str1, 6, 44);//�뢮� � ���᮫� �१ ����������� � ��� ����
	printVideoMemory(str, 6, 53);
	t = inp(PORT_60);//ॣ���� �ࠢ����� ��������ன, ॣ���� ������
	//�᫨ � ���� 1 ��� = 0, ����� � ��� ��-� ����ᠭ�
	b2h2(t, str); //��ॢ�� � 16���� ��⥬�
	b2bin(t, str1);//��ॢ�� � 2���� ��⥬�
	printVideoMemory("������� ������ (0x60): ", 7, 0);
	printVideoMemory(str1, 7, 44);//�뢮� � ���᮫� �१ ����������� � ��� ����
	printVideoMemory(str, 7, 53);
	ISRStatus();
}
void ISRStatus()//�㭪�� �뢮�� ����� ॣ���� ���㦨����� ���뢠���(ISR) ����஫��� ���뢠���
{
	unsigned char t;
	char str[3];
	char str1[9];
	str[0] = '\0';//��� �ନ����
	str1[0] = '\0';
	outp(PORT_20, 0x0B); //������ � ���� 20h ������� ࠧ�襭�� �⥭�� ॣ���� ���㦨����� ���뢠���(ISR) ����஫��� ���뢠���
	t = inp(PORT_20); //���뢠��� ���祭�� ॣ���� ���㦨����� ���뢠��� (ISR)
	b2h2(t, str); //��ॢ�� � 16���� ��⥬�
	b2bin(t, str1);//��ॢ�� � 2���� ��⥬�
	printVideoMemory("������� ���㦨������ ���뢠��� ISR(0x20)", 8, 0);
	printVideoMemory(str1, 8, 44);//�뢮� � ���᮫� �१ ����������� � ��� ����
	printVideoMemory(str, 8, 53);
}
//�ப��⪠ ��࠭�
void ScrollScreen()
{
	int i, j;
	char far* v = start + FIRST_STRING * STRING_SIZE;
	char far* tmp = start + (FIRST_STRING + 1) * STRING_SIZE;
	int row = FIRST_STRING;
	for (i = row; i < LAST_STRING; i++)
	{
		for (j = 0; j < QUANTITY_OF_SYMBOLS; j++)
		{
			*v = *tmp;
			v += SYMBOL_SIZE;
			tmp += SYMBOL_SIZE;
		}
	}
	CleanScreen(24, QUANTITY_OF_SYMBOLS);//���⪠ ��࠭�
}
//���⪠ ��࠭�
void CleanScreen(int row, int counts)
{
	int i;
	int column = 0;
	char far* v;
	for (i = 0; i < counts; i++)
	{
		v = start + row * STRING_SIZE + column * SYMBOL_SIZE;
		column++;
		*v = ' ';
		v++;
	}
}