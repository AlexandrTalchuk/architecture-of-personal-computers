//924401-2 ������ �.�., ������ୠ� ࠡ�� � 5 ���� ॠ�쭮�� �६���

#include <dos.h>
#include <ctype.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#pragma warn -sus

volatile int x = 45, y = 7; //�������� ��६���� �⮫�殢 � ��ப
volatile int hMax = 25;//���� �㤨�쭨��
volatile int minMax = 60;//������ �㤨�쭨��
volatile int msCounter = 0; //����稪 ����প�
volatile int tik=0; //����稪 �맮�� ���뢠��� ⠩���
volatile char massive[20];  //���ᨢ ��� ����� ������
volatile int nomer = 0;  //������⢮ ������⮢ � ���ᨢ�
volatile int enter = 0;  //��६����� ��� ������ enter
volatile int gran = 0;  //�࠭�� ��� backspace
volatile int flagAlarm = 0; //���� �㤨�쭨��

void printtopos(char *str, int line, int column, int color); //�㭪�� �뢮�� ��ப� �१ ��������� � ��।����� ������
void print(char *str);//�㭪�� �뢮�� ��ப� �१ ����������
void cleanscreen(int line, int counts, int column);//�㭪�� ���⪨ ��࠭�
int BCDToInteger(int bcd);//�㭪�� ��ॢ��� � ⨯ int
unsigned char IntToBCD(int value);//�㭪�� ��ॢ��� � ����筮-������� ���
char* toBinary(unsigned char bt, char* str);//�㭪�� ��ॢ��� �᫠ �� �����筮� � ������� ��⥬�
char Translate(unsigned char bt);// �㭪�� ����祭�� ᨬ���� ��⠭����筮�� �᫠ ��� ������ ������ ���
char* toHex(unsigned char bt, char* str); //�㭪�� ��ॢ��� �᫠ �� �����筮� � ��⭠������� ��⥬�

void inittimer(void);//�㭪�� ���樠����樨 ⠩���
void inittimerstop(void);//�㭪�� ������ ⠩��� �� �⠭�����
void interrupt far(*oldInt70h)(void);//�����⥫� �� ��ࠡ��稪 ���뢠��� �ᮢ ॠ�쭮�� �६���
void interrupt far NewInt70Handler(void);//�㭪�� ��ࠡ�⪨ ���뢠��� �ᮢ ॠ�쭮�� �६���
void interrupt TimerHandler(void);//�㭪�� ��ࠡ�⪨ ���뢠��� ��⥬���� ⠩���
void interrupt(*oldTimerHandler)(void);//�����⥫� �� ��ࠡ��稪 ���뢠��� ��⥬���� ⠩���
void interrupt newInt9(void); //�㭪�� ��ࠡ�⪨ ���뢠��� ����������
void interrupt(*oldInt9)(void);//�����⥫� �� ��ࠡ��稪 ���뢠��� ����������

void cleanmassive(void);//�㭪�� ���⪨ ���ᨢ�
void GetTime(void); //�㭪�� ����祭�� �६���
void SetTime(void);//�㭪�� ��⠭���� �६���
void WaitClockIsFree(void);//�㭪�� �஢��ન �� ����������� ���饭�� � �ᠬ
void correcttime(void);//�㭪�� ।���஢���� �६���
void LockClockUpdate(void);//�㭪�� ����� �� ���������� ⠩���
void UnlockClockUpdate(void);//�㭪�� ࠧ�襭�� �� ���������� ⠩���
void CustomDelay(void); //�㭪�� ����প� �६���
//void CreateAlarm(void); //�㭪�� ������� �㤨�쭨��
//void Sound(int); //�㭪�� ����祭�� ��������
//void SetCount(int, int);//�㭪�� ��� �ந��뢠��� �������
void Status(void);//�㭪�� �뢮�� ���祭�� ॣ���஢
void tostring(unsigned char , char *);//�㭪�� ��ॢ��� � ��ப�
void ISRStatus(void); //�㭪�� �뢮�� ����� ॣ���� ���㦨����� ���뢠��� (ISR) ����஫��� ���뢠���

void tostring(unsigned char temp, char *str)//�㭪�� ��ॢ��� � ��ப�
{
	int i;
	str[8] = 0;
	i = 7;
	while (temp)
	{
		str[i] = '0' + temp % 2;
		temp = temp / 2;
		i--;
	}
	for (; i >= 0; i--)
		str[i] = '0';

}

void Status()//�㭪�� �뢮�� ���祭�� ॣ���஢
{
	unsigned char a, b, c, d;
	char buf[9];
	outp(0x70, 0x0A);
	a = inp(0x71);
	tostring(a, buf);
	printtopos("0Ah: ", 7, 20, 11);
	printtopos(buf, 7, 24,13);

	outp(0x70, 0x0B);
	b = inp(0x71);
	tostring(b, buf);
	printtopos("0Bh: ", 7, 44, 11);
	printtopos(buf, 7, 48, 13);

	outp(0x70, 0x0C);
	c = inp(0x71);
	tostring(c, buf);
	printtopos("0Ch: ", 8, 20, 11);
	printtopos(buf, 8, 24, 13);

	outp(0x70, 0x0D);
	d = inp(0x71);
	tostring(d, buf);
	printtopos("0Dh: ", 8, 44, 11);
	printtopos(buf, 8, 48, 13);

	printtopos("���祭�� ������஢",5, 30, 15);

	ISRStatus();
}





void main()
{
	char str[] = { "924401-2 ������ �.�." };
	char str1[] = { "������ୠ� ࠡ�� �5 ���� ॠ�쭮�� �६���"};
	char str0[] = { "1 - ��⠭����� ���� � �६�, 2 - ��⠭����� ����প�  Esc - ��� ��室�" };
	char c;
	clrscr();
	printtopos("������� ���㦨����� ���뢠��� ISR(0x20)", 6,10, 11);
	printtopos(str, 0, 0, 11);
	printtopos(str1, 1, 0, 11);
	printtopos(str0, 3, 0, 15);
	oldTimerHandler = getvect(0x1c);

	while (c != 27)
	{
		inittimer();
	    GetTime();
		Status();
		ISRStatus();
		c = getch();
		inittimerstop();
		switch (c)
		{
		case '1':
			inittimer();
			SetTime();
			break;
	     /*	case '2':
			inittimer();
			CreateAlarm();
			getch();
			cleanscreen(6, 720, 0);
			inittimerstop();
			break;*/
		case '2':
			inittimer();
			CustomDelay();
			break;
		case 27:
			inittimerstop();
			cleanscreen(0, 720, 0);
			return;
		}
		c = 0;
	}
}

/*void SetCount(int freq, int time)//�㭪�� �ந��뢠��� �������
{
	unsigned long int timerClock = 1193180;
	short value;				// ���祭�� ����⥫� �����
	value = timerClock / freq; 	// ����७��� ���� ⠩���
	outp(0x42, (char)value);		// �����뢠�� ���祭�� ����饣� ���� ����⥫� ����� � ���� ��������
	outp(0x42, (char)(value >> 8)); // �����뢠�� ���祭�� ���襣� ���� ����⥫� ����� � ���� ��������
	delay(time);				// ��㧠 �� �६� = time
} */

void WaitClockIsFree()//�㭪�� �஢��ન �� ����������� ���饭�� � �ᠬ
{
	do
	{
		outp(0x70, 0x0A);
	} while (inp(0x71) & 0x80);//�஢�ઠ 7 ���
}

void inittimer() //�㭪�� ���樠����樨 ⠩���
{
	disable();//����� �� ���뢠��� ⠩���
	setvect(0x1c, TimerHandler);//��⠭���� ������ ��ࠡ��稪�
	enable();//����襭�� �� ���뢠���
}
void inittimerstop()//�㭪�� ������ �� �⠭����� ��ࠡ��稪 ⠩���
{
	disable();//����� �� ���뢠��� ⠩���
	setvect(0x1c, oldTimerHandler);   //��⠭���� ��ண� ��ࠡ��稪�
	enable();//����襭�� �� ���뢠���
}


void interrupt TimerHandler() //�㭪�� ��ࠡ�⪨ ���뢠��� ⠩���
{
	char str6[9];
	disable();//����� �� ���뢠��� ⠩���
	tik++;             //�����稢��� ������⢮ ⨪��
	if (!(tik % 18))   //�᫨ ��⭮ 18,� ��諠 1 ᥪ㭤�
	{

		GetTime();     //�뢮� �६��� � ����
	}
	outp(0x20, 0x20);  //���� ����஫��� ���뢠���


	enable();//����襭�� �� ���뢠���
}

void interrupt far NewInt70Handler(void)//�㭪�� ��ࠡ�⪨ ���뢠��� ��ᮢ ॠ�쭮�� �६���
{
	msCounter++;
	outp(0x70,0x0C);
	inp(0x71);

	outp(0x20, 0x20);
	outp(0xA0, 0x20);
}
void ISRStatus()//�㭪�� �뢮�� ����� ॣ���� ���㦨����� ���뢠��� (ISR) ����஫��� ���뢠���
{
	unsigned char value;
	char str[3];
	char str1[9];

	outp(0x20, 0x0B); //������ � ���� 20h ������� ࠧ�襭�� �⥭�� ॣ���� ���㦨����� ���뢠��� (ISR) ����஫��� ���뢠���
	value = inp(0x20); //���뢠��� ���祭�� ॣ���� ���㦨����� ���뢠��� (ISR)
	printtopos(toHex(value, str), 6, 60, 13); //�뢮� ��⭠����筮� � ����筮� �� ᫮�� ���ﭨ� ॣ���� ISR

	printtopos(toBinary(value, str1), 6, 51, 13);
}
char Translate(unsigned char bt)// �㭪�� ����祭�� ᨬ���� ��⠭����筮�� �᫠ ��� ������ ������ ���
{
	return (bt < 10) ? '0' + bt : 'A' - 10 + bt;
}
char* toHex(unsigned char bt, char* str)//�㭪�� ��ॢ��� �᫠ �� �����筮� � ��⭠������� ��⥬�
{
	str[2] = 0;
	str[1] = Translate(bt & 0x0F);//bt & 0x0F ? ��᪠, ����祭�� ᨬ���� ��⭠����筮�� �᫠ ��� 4 ������ ���
	str[0] = Translate(bt >> 4);//����⮢� ᤢ�� ��ࠢ� �� 4 ���
	return str;
}
char* toBinary(unsigned char bt, char* str)//�㭪�� ��ॢ��� �᫠ �� �����筮� � ������� ��⥬�
{
	int i;
	for (i = 7; i >= 0; i--)
	{
		str[i] = (bt & 1) ? '1' : '0';//�᫨ १���� ��������� ��᪨ �� ����, � �����뢠���� ?1?, ���� ?0?
		bt = bt >> 1;//����⮢� ᤢ�� ��ࠢ� �� 1 ���
	}
	str[8] = '\0';
	return str;
}
void GetTime()//�㭪�� ����祭�� �६���
{
	unsigned char value;
	char str6[9];
	int minutes, hours,second;
	int i = 0;
    //	int frequencyArray[][2] = {{600, 100}, {1, 50}, {600, 100}, {1, 50}, {600, 100}, {1 , 50}, {600, 100}, {1, 50}};
	printtopos("����饥 �६�: ", 20, 45, 11);
	WaitClockIsFree(); // �஢�ઠ, �᢮�������� �� ���
	outp(0x70, 0x04); // ���뢠��� ������ ��� �ᮢ
	value = inp(0x71);
	itoa((BCDToInteger(value)), str6, 10);
	hours = BCDToInteger(value);
	if (BCDToInteger(value) < 10)
	{
		printtopos("0", 20, 59,13);
		printtopos(str6, 20, 60,13);
	}
	else {
		printtopos(str6, 20, 59,13);
	}
	printtopos(":", 20, 61,13);
	if (BCDToInteger(value) > 23) {
		correcttime();
		printtopos("00", 20, 61,13);
	}

	WaitClockIsFree();
	outp(0x70, 0x02); // ���뢠��� ������ ��� �����
	value = inp(0x71);
	itoa((BCDToInteger(value)), str6, 10);
	minutes = BCDToInteger(value);
	if (BCDToInteger(value) < 10)
	{
		printtopos("0", 20, 62,13);
		printtopos(str6, 20, 63,13);
	}
	else {
		printtopos(str6, 20, 62,13);
	}
	printtopos(":", 20, 64,13);

	WaitClockIsFree();
	outp(0x70, 0x00); // ���뢠��� ������ ��� ᥪ㭤
	value = inp(0x71);
	itoa((BCDToInteger(value)), str6, 10);
	if (BCDToInteger(value) < 10)
	{
		printtopos("0", 20, 65,13);
		printtopos(str6, 20, 66,13);
	}
	else {
		printtopos(str6, 20, 65,13);
	}

	printtopos("������ ���:", 21, 45,11);
	WaitClockIsFree();
	outp(0x70, 0x07); // ���뢠��� ������ � ���
	value = inp(0x71);
	itoa((BCDToInteger(value)), str6, 10);
	if (BCDToInteger(value) < 10)
	{
		printtopos("0", 21, 58,13);
		printtopos(str6, 21, 59,13);
	}
	else {
		printtopos(str6, 21, 58,13);
	}
	printtopos(".", 21, 60, 13);

	WaitClockIsFree();
	outp(0x70, 0x08); // ���뢠��� ������ � �����
	value = inp(0x71);
	itoa((BCDToInteger(value)), str6, 10);
	if (BCDToInteger(value) < 10)
	{
		printtopos("0", 21, 61,13);
		printtopos(str6, 21, 62,13);
	}
	else {
		printtopos(str6, 21, 61,13);
	}
	printtopos(".", 21, 63,13);

	WaitClockIsFree();
	outp(0x70, 0x09); // ���뢠��� ������ � ����
	value = inp(0x71);
	itoa((BCDToInteger(value)), str6, 10);
	if (BCDToInteger(value) < 10)
	{
		printtopos("200", 21, 64, 13);
		printtopos(str6, 21, 67,13);
	}
	else {
		printtopos("20", 21, 64,13);
		printtopos(str6, 21, 66,13);
	}

	WaitClockIsFree();
	outp(0x70, 0x06); // ���뢠��� ������ � ��� ������
	value = inp(0x71);
	cleanscreen(2, 9, 26);
	cleanscreen(21,11,69);
	switch (BCDToInteger(value))
	{
	case 1:printtopos("����ᥭ�", 21, 69, 11);  break;
	case 2:printtopos("�������쭨�", 21, 69, 11);  break;
	case 3:printtopos("��୨�", 21, 69, 11);  break;
	case 4:printtopos("�।�", 21, 69, 11);   break;
	case 5:printtopos("��⢥�", 21, 69, 11);  break;
	case 6:printtopos("��⭨�", 21, 69, 11);   break;
	case 7:printtopos("�㡡��", 21, 69, 11);   break;
	default:printtopos("����।��� ���� ������", 21, 26, 11);  break;
	}

      /*	if (minutes == minMax && hMax == hours && flagAlarm) {
		printtopos("�㤨�쭨� �ࠡ�⠫", 10, 0, 3);
		printtopos("�㤨�쭨� �ࠡ�⠫", 10, 20, 14);
		printtopos("�㤨�쭨� �ࠡ�⠫", 10, 40, 7);
		printtopos("�㤨�쭨� �ࠡ�⠫", 10, 60, 10);
		Sound(1);
		for (i = 0; i < 8; i++)
		{
			SetCount(frequencyArray[i][0], frequencyArray[i][1]); //���ந�������� ��㪠 �㤨�쭨��
			if (i == 7) break;
		}
		flagAlarm = 0;
		Sound(0);
		cleanscreen(10,80,0);
	}*/

}

/*void Sound(int b) //�㭪�� ����祭��/�⪫�祭�� ��������
{
	unsigned long int dwResult = 0;
	// ��⠥� ���ﭨ� ����
	dwResult = inport(0x61);
	if (b != 0)
	{
		dwResult |= 0x03;
		//�����뢠�� ���祭�� � ����
		outport(0x61, dwResult);
	}
	else
	{
		dwResult &= 0xFC;
		outport(0x61, dwResult);
	}
} */

void correcttime()//�㭪�� ।���஢���� �६���
{
	int hours = 0, weekday = 8, monthday = 32, month = 13, year = 21;
	int kolday[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	unsigned char value;

	WaitClockIsFree();
	outp(0x70, 0x07); // ����
	value = inp(0x71);
	monthday = BCDToInteger(value);

	WaitClockIsFree();
	outp(0x70, 0x08); // �����
	value = inp(0x71);
	month = BCDToInteger(value);

	WaitClockIsFree();
	outp(0x70, 0x09); // ���
	value = inp(0x71);
	year = BCDToInteger(value);
	year = year + 2000;
	if (year % 4 != 0 || year % 100 == 0 && year % 400 != 0)
	{
	}
	else
		kolday[1] = 29;

	WaitClockIsFree();
	outp(0x70, 0x06); // ���� ������
	value = inp(0x71);
	weekday = BCDToInteger(value);

	weekday++;
	if (weekday > 7)
		weekday = 1;
	monthday++;
	if (monthday > kolday[month - 1]) {
		monthday = 1;
		month++;
		if (month > 12)
		{
			month = 1;
			year++;
		}
	}
	year = year - 2000;

	LockClockUpdate();//�����஢���� ���뢠��� �ᮢ ॠ�쭮�� �६���

	outp(0x70, 0x04);
	outp(0x71, IntToBCD(hours));//��⠭���� ���祭�� ��

	outp(0x70, 0x06);
	outp(0x71, IntToBCD(weekday));//��⠭���� ���祭�� ��� ������

	outp(0x70, 0x07);
	outp(0x71, IntToBCD(monthday));//��⠭���� ���祭�� ��� �����

	outp(0x70, 0x08);
	outp(0x71, IntToBCD(month));//��⠭���� ���祭�� �����

	outp(0x70, 0x09);
	outp(0x71, IntToBCD(year));//��⠭���� ���祭�� ����

	UnlockClockUpdate();//����襭�� ���뢠��� �ᮢ ॠ�쭮�� �६���

}

void CustomDelay()//�㭪�� ��⠭���� ����প�
{
	int delayPeriod = 0;
	unsigned char value;
	printtopos("��⠭���� ����প� � ��������� 1500-10000 �� ", 10, 0, 15);
	cleanmassive();
	disable();
	oldInt9 = getvect(9);    //���뢠��� ����������
	setvect(9, newInt9);
	enable();
	x = 46;
	gran = x;
	y = 10;
	while (delayPeriod < 1500 || delayPeriod>10000)//���� ����প�
	{

		if (enter == 1)
		{
			delayPeriod = atoi(massive);
			enter = 0;
		}
	}
	setvect(9, oldInt9);
	cleanmassive();
	setvect(0x1c, oldTimerHandler);
	disable();
	oldInt70h = getvect(0x70);
	setvect(0x70, NewInt70Handler);
	enable();

	value = inp(0xA1);
	outp(0xA1, value & 0xFE);//���� ������

	outp(0x70, 0x0B);
	value = inp(0x0B);
	outp(0x70, 0x0B);
	outp(0x71, value & 0x40);//�맮� ��ਮ���᪮�� ���뢠���
	printtopos("�믮������ ����প�...",11,0,11);
	msCounter = 0;//���� ����稪� ���뢠��� ��ᮢ ॠ�쭮�� �६���
	while (msCounter != delayPeriod);
	printtopos("����প� �����襭�!", 12, 0,11);

	setvect(0x70, oldInt70h);
	WaitClockIsFree();
	setvect(0x1c, TimerHandler);
	correcttime();
	printtopos("������ ���� ������� ��� �த�������...", 13, 20, 15);
	getch();
	cleanscreen(7, 720, 0);
	inittimerstop();
}

void interrupt newInt9()  //�㭪�� ��ࠡ�⪨ ���뢠��� ����������
{
	char str6[9];
	unsigned char value = 0;
	ISRStatus();
	value = inp(0x60);    //����祭�� ���祭�� �� ���� 60h
	switch (value) {
	case 0x02:  //�᫨ 1
	{
		itoa(1, str6, 10);
		print(str6);
		massive[nomer] = '1';
		nomer++;
		break;
	}
	case 0x03:   //�᫨ 2
	{
		itoa(2, str6, 10);
		print(str6);
		massive[nomer] = '2';
		nomer++;
		break;
	}
	case 0x04:   //�᫨ 3
		{
			itoa(3, str6, 10);
			print(str6);
			massive[nomer] = '3';
			nomer++;
			break;
		}
	case 0x05:   //�᫨ 4
		{
			itoa(4, str6, 10);
			print(str6);
			massive[nomer] = '4';
			nomer++;
			break;
		}
		case 0x06:   //�᫨ 5
		{
			itoa(5, str6, 10);
			print(str6);
			massive[nomer] = '5';
			nomer++;
			break;
		}
		case 0x07:   //�᫨ 6
		{
			itoa(6, str6, 10);
			print(str6);
			massive[nomer] = '6';
			nomer++;
			break;
		}
		case 0x08:   //�᫨ 7
		{
			itoa(7, str6, 10);
			print(str6);
			massive[nomer] = '7';
			nomer++;
			break;
		}
		case 0x09:   //�᫨ 8
		{
			itoa(8, str6, 10);
			print(str6);
			massive[nomer] = '8';
			nomer++;
			break;
		}
		case 0x0A:   //�᫨ 9
		{
			itoa(9, str6, 10);
			print(str6);
			massive[nomer] = '9';
			nomer++;
			break;
		}
		case 0x0B:   //�᫨ 0
		{
			itoa(0, str6, 10);
			print(str6);
			massive[nomer] = '0';
			nomer++;
			break;
		}
		case 0x0E:   //�᫨ backspace
		{
			if (x != gran) {
				x--;
				print(" ");
				x--;
				massive[nomer] = '\0';
				nomer--;
				massive[nomer] = '\0';
			}
			break;
		}
		case 0x1C:   //�᫨ Enter
		{
			enter = 1;
			break;
		}
	}
	delay(100);
	outp(0x20, 0x20);   //���� ����஫��� ���뢠���
	ISRStatus();

}

void SetTime()//�㭪�� ��⠭���� �६���
{
	int hours = 25, minutes = 60, seconds = 60, weekDay = 8, monthDay = 32, month = 13, year = 21;
	int kolday[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	char str[3];
	cleanmassive();
	disable();
	oldInt9 = getvect(9);    //���뢠��� ����������
	setvect(9, newInt9);
	enable();
	x = 21;
	gran = x;
	y = 10;
	printtopos("������ ���(0-23): ", 10, 0, 15);

	while (hours < 0 || hours>23)//��⠭���� �ᮢ
	{

		if (enter == 1)
		{
			hours = atoi(massive);
			enter = 0;
		}
	}

	cleanmassive();
	printtopos("������ ������(0-59): ", 11, 0, 15);
	x = 23;
	gran = x;
	y++;
	while (minutes < 0 || minutes>59)//��⠭���� �����
	{

		if (enter == 1)
		{
			minutes = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	printtopos("������ ᥪ㭤�(0-59): ", 12, 0, 15);
	x = 25;
	gran = x;
	y++;
	while (seconds < 0 || seconds>59)//��⠭���� ᥪ㭤
	{

		if (enter == 1)
		{
			seconds = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	printtopos("������ ���(2000-2021): ", 13, 0, 15);
	x = 25;
	gran = x;
	y++;
	while (year < 2000 || year>2021)//��⠭���� ����
	{
		if (enter == 1)
		{
			year = atoi(massive);
			enter = 0;
		}
	}
	if (year % 4 != 0 || year % 100 == 0 && year % 400 != 0)
	{
	}
	else
		kolday[1] = 29;
	year = year - 2000;
	cleanmassive();
	printtopos("������ �����(1-12): ", 14, 0, 15);
	x = 24;
	gran = x;
	y++;
	while (month < 1 || month>12)//��⠭���� �����
	{

		if (enter == 1)
		{
			month = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	itoa(kolday[month - 1], str, 10);
	printtopos("������ ���� ����� 1- ", 15, 0, 15);
	printtopos(str, 15, 23,15);
	x = 29;
	gran = x;
	y++;
	while (monthDay < 1 || monthDay>kolday[month - 1])//��⠭���� ���
	{

		if (enter == 1)
		{
			monthDay = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	printtopos("������ ����� ��� ������(1-7): ", 16, 0, 15);
	x = 29;
	gran = x;
	y++;
	while (weekDay < 1 || weekDay>7)//��⠭���� ��� ������
	{
		if (enter == 1)
		{
			weekDay = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();

	setvect(9, oldInt9);

	LockClockUpdate();//����� �� ����������

	outp(0x70, 0x04);//��⠭���� �ᮢ
	outp(0x71, IntToBCD(hours));

	outp(0x70, 0x02);//��⠭���� �����
	outp(0x71, IntToBCD(minutes));

	outp(0x70, 0x00);//��⠭���� ᥪ㭤
	outp(0x71, IntToBCD(seconds));

	outp(0x70, 0x06);//��⠭���� ��� � ������
	outp(0x71, IntToBCD(weekDay));

	outp(0x70, 0x07);//��⠭���� ���
	outp(0x71, IntToBCD(monthDay));

	outp(0x70, 0x08);//��⠭���� �����
	outp(0x71, IntToBCD(month));

	outp(0x70, 0x09);//��⠭���� ����
	outp(0x71, IntToBCD(year));

	UnlockClockUpdate();//����襭�� ����������
	printtopos("������ ���� ������� ��� �த�������...", 17, 0,15);
	getch();
	cleanscreen(10, 720, 0);
	inittimerstop();//��⠭���� ⠩���
}

/*void CreateAlarm()//�㭪�� ᮧ����� �㤨�쭨��
{
	hMax = 25, minMax = 60;
	cleanmassive();
	disable();
	oldInt9 = getvect(9);    //���뢠��� ����������
	setvect(9, newInt9);
	enable();
	x = 21;
	gran = x;
	y = 10;
	printtopos("������ ���(0-23): ", 10, 0, 15);

	while (hMax < 0 || hMax>23) //��⠭���� �ᮢ �㤨�쭨��
	{
		if (enter == 1)
		{
			hMax = atoi(massive);
			enter = 0;
		}
	}

	cleanmassive();
	printtopos("������ ������(0-59): ", 11, 0, 15);
	x = 23;
	gran = x;
	y++;
	while (minMax < 0 || minMax>59)//��⠭���� ����� �㤨�쭨��
	{
		if (enter == 1)
		{
			minMax = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	setvect(9, oldInt9);
	printtopos("������ ���� ������� ��� �த�������...", 12, 0,4);
	getch();
	cleanscreen(10, 800, 0);
	flagAlarm = 1;
} */

void printtopos(char *str, int line, int column, int color)//�㭪�� �뢮�� ��ப� �१ ��������� � ��।����� ������
{
	int i;
	char far* start = (char far*)0xb8000000;
	char far *v;
	for (i = 0; str[i] != '\0'; i++)
	{
		v = start + line * 160 + column * 2;
		column++;
		*v = str[i];
		v++;
		*v = color;
	}
}

void print(char *str)//�㭪�� �뢮�� ��ப� �१ ���������
{
	int i;
	char far* start = (char far*)0xb8000000;
	char far *v;
	for (i = 0; str[i] != '\0'; i++)
	{
		v = start + y * 160 + x * 2;
		x++;
		*v = str[i];
		v++;
		*v = 0x0F;
	}
}

void cleanscreen(int line, int counts, int column)//�㭪�� ���⪨ ��࠭�
{
	int i;
	char far* start = (char far*)0xb8000000;
	char far *v;
	for (i = 0; i < counts; i++)
	{
		v = start + line * 160 + column * 2;
		column++;
		*v = ' ';
		v++;
	}
}

int BCDToInteger(int bcd)//�㭪�� ��ॢ��� � ⨯ int
{
	return bcd % 16 + bcd / 16 * 10;//��ॢ�� � ⨯ int
}

unsigned char IntToBCD(int value)//�㭪�� ��ॢ��� � ����筮-������� �ଠ�
{
	return (unsigned char)((value / 10) << 4) | (value % 10);//��ॢ�� � ⨯ BCD
}

void LockClockUpdate()//�㭪�� ����� �� ���������� �ᮢ ॠ�쭮�� �६���
{
	unsigned char value;
	WaitClockIsFree();//�஢�ઠ �ᮢ
	outp(0x70, 0x0B);
	value = inp(0x71);// ��⠭���� 1 � 7 ���
	value |= 0x80;       //����� ���������� �ᮢ
	outp(0x70, 0x0B);
	outp(0x71, value);
}

void UnlockClockUpdate()//�㭪�� ࠧ�襭�� ���������� �ᮢ ॠ�쭮�� �६���
{
	unsigned char value;
	WaitClockIsFree();
	outp(0x70, 0x0B);
	value = inp(0x71); // ��⠭���� 0 � 7 ���
	value -= 0x80;        //����襭�� �� ���������� �ᮢ
	outp(0x70, 0x0B);
	outp(0x71, value);
}

void cleanmassive()//�㭪�� ���⪨ ���ᨢ�
{
	int i;
	for (i = 0; i < 20; i++) {
		massive[i] = '\0';
	}
	nomer = 0;
}