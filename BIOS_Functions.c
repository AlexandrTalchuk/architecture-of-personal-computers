//924401-2 ������ �.�., ������ୠ� ࠡ�� �6 - �㭪樨 BIOS
#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <stdlib.h>

//�������� ��६���� ��� ࠡ��� �ணࠬ��
volatile int close = 0;//���� ��室�
union REGS rg;//�������� BIOS ��� ���뢠��� ���䨣��樨
char input[10];//��ப� ��� ����� �ᥫ
char videoModes[4][40] = { "�� �ᯮ������", "40�25 c 梥⭮� ���⮩", "80�25 c 梥⭮� ���⮩","80�25 c �����஬��� ���⮩" };


//���⨯� �㭪権
void itodec(int i, char* c, int j); //��ॢ�� ���姭�筮�� �᫠ � �������� ��⥬� ��᫥���
void itodec2(int i, char* c, int j);//��ॢ�� ������ ���筮�� �᫠ � �������� ��⥬� ��᫥���
int bcdtoint(char* k);//��ॢ���� �᫮, �������� � ����筮-�����筮� ����, � ⨯ int, 2���
int bcdtoint2(char* k); //��ॢ���� �᫮, �������� � ����筮-�����筮� ����, � ⨯ int, 4 ���
void inttobcd(int i, char* c);//��ॢ���� �᫮ � � ����筮-������� ���
void Clear(int i, int u, int count);//��頥� count ����権 ���� ���᮫� � ��ப� i, �⮫�� u
char* IntToStr(int n);// �㭪�� �८�ࠧ������ �᫠ � ��ப�
void PrintPos(char* str, int line, int column, int attrib); // �㭪��� �뢮�� � ��।����� ������
void Init(void);//�㭪�� ��� �뢮�� 蠯��
void Menu(void);//�㭪�� �뢮�� ����
void PressAnyKey(int i);//�㭪�� �������� ������ �� ������ ��� ��室� � ����
void ConfigInfo(void);//�㭪�� �뢮�� ���䨣��樨 ��⥬�
void InChar(void);//�㭪�� ����� ������ ᨬ���� ��� ��������
void InCharWaiting(void);//�㭪�� ����� ������ ᨬ���� � ���������
int InDigit(int j, char* limits, int left, int right);//�㭪�� ����� �᫠ � �࠭��� ��।����� ���祭��
void InStr(void);//�㭪�� ����� ��ப�
void InStrDelay(int del);//�㭪�� ����� ��ப� � ���������
void Input(void);//�㭪�� �뢮�� ���� ��� �㭪権 �����
void ShowTime(void);//�㭪�� �뢮�� ⥪��� ���� � �६���
void SetTime(void);//�㭪�� ��⠭���� �६���
void SetDate(void);//�㭪�� ��⠭���� ����
void TimeOperations(void);//�㭪�� �뢮�� ���� ��� ࠡ��� � �६����
char* B2Bin(unsigned char bt, char* str);//�㭪�� ��ॢ��� �᫠ �� �����筮� ��⥬� � ������� ��⥬�
char* B2H2(unsigned char bt, char* str);//�㭪�� ��ॢ��� �᫠ �� �����筮� ��⥬� � ��⭠������� ��⥬�
unsigned char B4to1H(unsigned char bt);//�㭪�� ��ॢ��� ������ ������ ��� � ��⭠������� ��⥬�




//��⮢� ���� ᫮�� ���䨣��樨
typedef struct _HDWCFG
{
	unsigned HddPresent : 1; //0 - ��᪮���
	unsigned NpuPresent : 1; //1 - ��⥬���᪨� ᮯ�����
	unsigned AmountOfRAM : 2; //2-3 - ������ ���
	unsigned VideoMode : 2; //4-5 - ��⨢�� �����०��
	unsigned NumberOfFdd : 2; //6-7 - ��᫮ �����㦥���� ����
	unsigned DmaPresent : 1; //8 - ����稥 ����஫��� DMA
	unsigned NumberOfCom : 3; //9-11 - ��᫮ COM-���⮢
	unsigned GamePresent : 1; //12 - ��஢�� ������
	unsigned JrComPresent : 1; //13 - �����
	unsigned NumberOfLpt : 2; //14-15 - ��᫮ LPT-���⮢
} HDWCFG;

//�࠭���� ��⮢ ���䨣��樨
HDWCFG HdwCfg;

int main(void)
{
	int key;
	Clear(0, 0, 25 * 80);//��頥� �࠭
	Init();//�뢮��� �������
	Menu();//�㭪�� ����
	while (!close)//���� �� ������
	{
		rg.h.ah = 0;//����頥� � ॣ���� AH ���祭�� 0
		int86(0x16, &rg, &rg);//�믮��塞 ���뢠��� 16H
		key = rg.h.ah;//� ॣ���� AH - ᪠�-��� ����⮩ ������
		switch (key)
		{
		case 1:
			close = 1;
			break;
		case 2:
			ConfigInfo();
			break;
		case 3:
			Input();
			break;
		case 4:
			TimeOperations();
			break;
		default:
			break;
		}
	}
	return 0;
}

unsigned char B4to1H(unsigned char bt)//�㭪�� ��ॢ��� ������ ������ ��� � ��⭠������� ��⥬�
{
	bt &= 0x0F;
	return (bt <= 9) ? '0' + bt : 'A' + (bt - 10);
}

char* B2H2(unsigned char bt, char* str)//�㭪�� ��ॢ��� �᫠ �� �����筮� ��⥬� � ��⭠������� ��⥬�
{
	str[0] = B4to1H(bt >> 4);
	str[1] = B4to1H(bt);
	str[2] = '\0';
	return str;
}

char* B2Bin(unsigned char bt, char* str)//�㭪�� ��ॢ��� �᫠ �� �����筮� ��⥬� � ������� ��⥬�
{
	int i;
	for (i = 0; i < 8; i++)
	{
		str[i] = (bt & 0x80) ? '1' : '0';
		bt <<= 1;
	}
	str[8] = '\0';
	return str;
}
//��ॢ�� �᫠ i � �������� ��⥬� ��᫥��� � ��ப�
void itodec(int i, char* c, int j)
{
	c[j] = '0' + ((i % 100) / 10);
	c[j + 1] = '0' + (i % 10);
}

//��ॢ�� �᫠ i � �������� ��⥬� ��᫥��� � ��ப�
void itodec2(int i, char* c, int j)
{
	c[j] = '0' + ((i % 10000) / 1000);
	c[j + 1] = '0' + ((i % 1000) / 100);
	c[j + 2] = '0' + ((i % 100) / 10);
	c[j + 3] = '0' + (i % 10);
}
//��ॢ���� �᫮ k, �������� � ����筮-�����筮� ���� � ⨯ int
int bcdtoint(char* k)
{
	return (*k) % 16 + (*k) / 16 * 10;
}
//��ॢ���� �᫮ k, �������� � ����筮-�����筮� ���� � ⨯ int
int bcdtoint2(char* k)
{
	return bcdtoint(k + 1) * 100 + bcdtoint(k);
}
//��ॢ���� �᫮ i � ����筮-������� ���
void inttobcd(int i, char* c)
{
	*c = (i % 10) + ((i / 10) << 4);
}
//��頥� count ����権 ���� ���᮫� � ��ப� i, �⮫�� u
void Clear(int i, int u, int count)
{
	int j;
	char far* prev, far* next;
	for (j = 0; j < count; j++)
	{
		prev = (char far*)0xb8000000 + i * 160 + u * 2;
		next = prev + 1;
		*prev = ' ';
		*next = 7;
		u++;
	}
}
// �㭪�� �८�ࠧ������ �᫠ � ��ப�
char* IntToStr(int n)
{
	char s[40], t, * temp;
	int i, k;
	int sign = 0;
	i = 0;
	k = n;
	if (k < 0)
	{
		sign = 1;
		k = -k;
	}
	do {
		t = k % 10;
		k = k / 10;
		s[i] = t | 0x30;
		i++;
	} while (k > 0);
	if (sign == 1)
	{
		s[i] = '-';
		i++;
	}
	temp = malloc(sizeof(char) * i);
	k = 0;
	i--;
	while (i >= 0) {
		temp[k] = s[i];
		i--; k++;
	}
	temp[k] = '\0';
	return(temp);
}
//�뢮��� �� �࠭ ⥪�� str 梥� attrib, ��稭�� � ��ப� line, �⮫�� column
void PrintPos(char* str, int line, int column, int attrib)
{
	int i;
	char far* start = (char far*)0xb8000000;
	char far* v;
	for (i = 0; str[i] != '\0'; i++)
	{
		v = start + line * 160 + column * 2;
		column++;
		*v = str[i];
		v++;
		*v = attrib;
	}
}
//��������� � ��������� ࠡ��� � 䨮
void Init(void)
{
	PrintPos("924401-2 ������ �.�.", 0, 30, 11);
	PrintPos("������ୠ� ࠡ�� �6 - �㭪樨 BIOS", 1, 22, 11);
}
//�뢮� ���� � ���⪠ ���� � �뢮���
void Menu(void)
{
	Clear(3, 0, 22 * 80);//���⪠ ࠡ�祩 ������
	PrintPos("----------------------------------------����------------------------------------", 3, 0, 10);
	PrintPos("1) �뢥�� ���䨣���� �� (INT 11h, INT 12h)", 5, 0, 10);
	PrintPos("2) ���� (INT 16h)", 6, 0, 10);
	PrintPos("3) ����樨 � ��⮩ � �६���� (INT 1Ah)", 7, 0, 10);
	PrintPos("�sc) ��室", 8, 0, 10);
}
//�������� ������ �� ������ � ������ � ����
//�뢮� ᮮ�饭�� � ��ப� ��� ����஬ i
void PressAnyKey(int i)
{
	PrintPos("������ ���� �������...", i, 0, 4);
	rg.h.ah = 0;//����頥� � ॣ���� AH ���祭�� 0
	int86(0x16, &rg, &rg);//��뢠�� ���뢠��� 16h
	Menu();
}
//���ଠ�� �� ��⠭�������� ����㤮�����
//� ����⨢��� ����� ��������
void ConfigInfo(void)
{
	unsigned char str[9];
	char c[2];
	unsigned uword = 0;
	Clear(3, 0, 22 * 80);
	PrintPos("(���뢠��� Int 11h)", 3, 0, 11);
	//�맮� ���뢠��� INT 11h ��� ����祭��
	//᫮�� ���䨣��樨 ��������
	rg.h.ah = 0;
	int86(0x11, &rg, &rg);
	// ����祭�� ᫮�� ���䨣��樨 � ��࠭����
	//��� � ������� HdwCfg
	PrintPos("����� ���䨣��樨",4,0,14);
	PrintPos(B2Bin(rg.h.ah,str),4,26,11);
	PrintPos(B2Bin(rg.h.al,str),4,34,11);
	PrintPos(B2H2(rg.h.ah,str),4,20,11);
	PrintPos(B2H2(rg.h.al, str), 4, 22, 11);
	uword = (unsigned int)rg.x.ax;
	memcpy(&HdwCfg, &uword, 2);
	//�뢮� �� �࠭ ���䨣��樨
	if (HdwCfg.HddPresent)
		PrintPos("������⥫� �� �����⭮� ���⪮� ��᪥ ��⠭�����", 5, 0, 1);
	else
		PrintPos("������⥫� �� �����⭮� ���⪮� ��᪥ �� ��⠭�����", 5, 0, 14);
	if (HdwCfg.NpuPresent)
		PrintPos("��䬥��᪨� ������ ��⠭�����", 6, 0, 14);
	else
		PrintPos("��䬥��᪨� ������ �� ��⠭�����", 6, 0, 14);
	c[0] = HdwCfg.AmountOfRAM + '0';
	PrintPos("������⢮ ������ ����⨢��� ����� �� ��⥬��� ����:", 7, 0, 14);
	PrintPos(c, 7, 59, 11); PrintPos("    ",7,60,0);
	PrintPos("��砫�� ०�� �����������:", 8, 0, 14);
	PrintPos(videoModes[HdwCfg.VideoMode], 8, 32, 14);
	c[0] = (HdwCfg.NumberOfFdd + 1) + '0';
	PrintPos("������⢮ ��⠭�������� ������⥫�� �� ������ �������� ��᪠�:", 9, 0,14);
	PrintPos(c, 9, 66, 11);PrintPos("    ",9,67,0);
	if (HdwCfg.DmaPresent)
		PrintPos("����஫��� ��אַ�� ����㯠 � ����� DMA ��⠭�����", 10, 0, 14);
	else
		PrintPos("����஫��� ��אַ�� ����㯠 � ����� DMA �� ��⠭�����", 10, 0,14);
	c[0] = HdwCfg.NumberOfCom + '0';
	PrintPos("������⢮ ��⠭�������� �ᨭ�஭��� ��᫥���⥫��� ���⮢:", 11, 0, 14);
	PrintPos(c, 11, 63, 11);PrintPos("    ",11,64,0);
	if (HdwCfg.GamePresent)
		PrintPos("��஢�� ���� ��⠭�����", 12, 0, 14);
        else
	PrintPos("��஢�� ���� �e ��⠭�����", 12, 0, 14);
	if (HdwCfg.JrComPresent)
		PrintPos("��᫥����⥫�� ���� ��⠭�����", 13, 0, 14);
	else
		PrintPos("��᫥����⥫�� ���� �� ��⠭�����", 13, 0, 14);
	c[0] = HdwCfg.NumberOfLpt + '0';
	PrintPos("������⢮ ��⠭�������� ��ࠫ������ �����஢:", 14, 0, 14);
	PrintPos(c, 14, 50, 11); PrintPos("    ",14,51,0);
	//�맮� ���뢠��� INT 12h ��� ��।������
	//��ꥬ� �᭮���� ����⨢��� ����� ��������
	rg.h.ah = 0x00;
	int86(0x12, &rg, &rg);
	PrintPos("(���뢠��� Int 12h)", 16, 0, 11);
	PrintPos("��ꥬ �᭮���� ����⨢��� ����� ��������:", 17, 0, 14);
	PrintPos(IntToStr(rg.x.ax),17,47,11 );
	PrintPos("K����", 17, 51, 11);
	PressAnyKey(18);
}
//���� ������ ᨬ���� ��� ��������
void InChar(void)
{
	char c[2];
	Clear(3, 0, 22 * 80);
	//menu = 0;
	c[1] = 0;
	PrintPos("(���뢠��� Int 16h)", 3, 0, 11);
	PrintPos("������ ᨬ���:", 4, 0, 5);
	//�맮� ���뢠��� INT 16h
	rg.h.ah = 0;
	int86(0x16, &rg, &rg);
	c[0] = (char)rg.h.al;
	PrintPos("�� ����� ᨬ��� ", 6, 0, 5);
	PrintPos(c, 6, 17, 15);
	PrintPos("�� ᪠�-����� ", 7, 0, 5);
	PrintPos(IntToStr(rg.h.ah), 7, 17, 11);
	PressAnyKey(9);
}
//���� ������ ᨬ���� � ��������� ������ enter
void InCharWaiting(void
)
{
	int done = 0, inp = 0;
	char c[2];
	int code;
	Clear(3, 0, 22 * 80);
	//menu = 0;
	c[1] = 0;
	PrintPos("(���뢠��� Int 16h)", 3, 0, 11);
	PrintPos("������ ᨬ���:", 4, 0, 5);
	//�맮� ���뢠��� INT 16h
	while (!done)
	{
		rg.h.ah = 0;
		int86(0x16, &rg, &rg);
		if
			(rg.h.ah == 14 && inp)//backspace
		{
			Clear(4, 17, 1);
			inp = 0;
		}
		else if (rg.h.ah == 28 && inp) {
			done = 1;
		}
		else if (!inp && rg.h.ah != 28) {
			c[0] = (char)rg.h.al;
			code = rg.h.ah;
			PrintPos(c, 4, 17, 14);
			inp = 1;
		}
	}
	PrintPos("�� ����� ᨬ��� ", 6, 0, 5);
	PrintPos(c, 6, 17, 11);
	PrintPos("�� ᪠�-����� ", 7, 0, 5);
	PrintPos(IntToStr(code), 7, 17, 11);
	PressAnyKey(9);
}
//���� �᫠ � ��।����� �࠭���
int InDigit(int j, char* limits, int left, int right)
{
	char input[80];
	int i = 0, k = 0, digit = 0;
	PrintPos("������ �᫮", j, 0, 5);
	PrintPos(limits,j, 16, 14);
	j++;
	do
	{
		digit = 0;
		i = 0;
		input[i] = '\0';
		Clear(j, 0, 80);
		while (1)
		{
			rg.h.ah = 0;
			int86(0x16, &rg, &rg);
			if (rg.h.ah == 14 && i > 0)//backspace
			{
				i--;
				input[i] = '\0';
				Clear(j, i, 1);
			}
			else if (rg.h.ah == 28 && i > 0)//enter
			{
				break;
			}
			else if (i < 80 && (rg.h.ah >= 2 && rg.h.ah <= 11))
			{
				input[i] = (char)rg.h.al;
				input[i + 1] = '\0';
				PrintPos(input, j, 0, 15);
				i++;
			}
		}
		for (k = 0; k < i; k++)//�८�ࠧ������ � int
		{
			digit = digit * 10 + (input[k] - '0');
		}
	} while (digit<left || digit> right);
	return digit;
}
//���� ��ப�
void InStr(void)
{
	char str[80];
	int i = 0;
	Clear(3, 0, 22 * 80);
	PrintPos("������ ��ப�:", 3, 0, 5);
	while (1)
	{
		rg.h.ah = 0;
		int86(0x16, &rg, &rg);
		if (rg.h.ah == 14 && i > 0)//backspace
		{
			i--;
			str[i] = '\0';
			Clear(4, i, 1);
		}
		else if (rg.h.ah == 28 && i > 0)
		{
			break;
		}
		else if (i < 80 && rg.h.ah != 28)
		{
			str[i] = (char)rg.h.al;
			str[i + 1] = '\0';
			PrintPos(str, 4, 0, 15);
			i++;
		}
	}
	PrintPos("��������� ��ப�:", 6, 0, 5);
	PrintPos(str, 7, 0, 11);
	PressAnyKey(9);
}
//���� ��ப� � ��������� del
void InStrDelay(int del)
{
	char str[80];
	int i = 0;
	Clear(3, 0, 22 * 80);
	PrintPos("������ ��ப�:", 3, 0, 5);
	while (1)
	{
		rg.h.ah = 0;
		int86(0x16, &rg, &rg);
		if (rg.h.ah == 14 && i > 0)//backspace
		{
			i--;
			str[i] ='\0';
			Clear(4, i, 1);
		}
		else if (rg.h.ah == 28 && i > 0)
		{
			break;
		}
		else if (i < 80 && rg.h.ah != 28)
		{
			str[i] = (char)rg.h.al;
			str[i + 1] = '\0';
			delay(del);
			PrintPos(str, 4, 0, 15);
			i++;
		}
	}
	PrintPos("��������� ��ப�:", 6, 0, 5);
	PrintPos(str, 7, 0, 11);
	PressAnyKey(9);
}
//����� �㭪⠬� �����
void Input(void)
{
	int k = 0, d = 0;
	Clear(3, 0, 22 * 80);
	PrintPos("1) ���� ᨬ���� ��� ��������", 3, 0, 10);
	PrintPos("2) ���� ᨬ���� � ���������", 4, 0, 10);
	PrintPos("3) ���� ��ப� ��� ��������", 5, 0, 10);
	PrintPos("4) ���� ��ப� � ���������", 6, 0, 10);
	rg.h.ah = 0;
	int86(0x16, &rg, &rg);
	k = rg.h.ah;
	switch (k) {
	case 2:
		InChar();
		break
			;
	case 3:
		InCharWaiting();
		break
			;
	case 4:
		InStr();
		break;
	case 5:
		Clear(3, 0, 22 * 80);
		PrintPos("������ ����প�", 3, 0, 14);
		d = InDigit(4, "�� 1500 �� 10000", 1500, 10000);
		InStrDelay(d);
		break;
	default:
		Menu();
		break;
	}
}
//��ᬮ�� ⥪��� ���� � �६���
void ShowTime(void)
{
	char str[20];
	int year = 0, month = 0, day = 0, secs = 0, mins = 0, hrs = 0;
	rg.h.ah = 0x04;//���뢠��� ���� � �ᮢ ॠ�쭮�� �६���
	int86(0x1a, &rg, &rg);
	day = bcdtoint(&(rg.h.dl));
	month = bcdtoint(&(rg.h.dh));
	year = bcdtoint2(&(rg.h.cl));
	rg.h.ah = 0x02;
	int86(0x1a, &rg, &rg);
	hrs = bcdtoint(&(rg.h.ch));
	mins = bcdtoint(&(rg.h.cl));
	secs = bcdtoint(&(rg.h.dh));
	PrintPos("������ ���:", 5, 40, 14);
	itodec2(year, str, 0);
	str[4] = '.';
	itodec(month, str, 5);
	str[7] = '.';
	itodec(day, str, 8);
	str[10] = '\0';
	PrintPos(str, 5, 55, 15);
	PrintPos("����饥 �६�:", 6, 40, 14);
	itodec(hrs, str, 0);
	str[2] = ':';
	itodec(mins, str, 3);
	str[5] = ':';
	itodec(secs, str, 6);
	str[8] = '\0';
	PrintPos(str, 6, 56, 15);
}
//��⠭���� �६���
void SetTime(void)
{
	int i = 0, j = 0, k = 0;
	PrintPos("����:", 10, 0, 14);
	i = InDigit(11, "�� 0 �o 23", 0, 23);
	PrintPos("������:", 13, 0, 14);
	j = InDigit(14, "�� 0 �� 59", 0, 59);
	PrintPos("���㭤�:", 16, 0, 14);
	k = InDigit(17, "�� 0 �� 59", 0, 59);
	rg.h.ah = 0x03;
	inttobcd(i, &(rg.h.ch));//���
	inttobcd(j, &(rg.h.cl));//������
	inttobcd(k, &(rg.h.dh));//ᥪ㭤�
	int86(0x1a, &rg, &rg);
	ShowTime();
}
//��⠭���� ����
void SetDate(void)
{
	int i = 0, j = 0, k = 0;
	PrintPos("���:", 10, 0, 14);
	i = InDigit(11, "�� 2001 �� 2021", 2001, 2021);
	if (i < 2000)//��।������ �⮫���
	{
		i = i - 1900;
		inttobcd(19, &(rg.h.ch));
	}
	else
	{
		i = i - 2000;
		inttobcd(20, &(rg.h.ch));
	}
	PrintPos("�����:", 13, 0, 14);
	j = InDigit(14, "�� 1 �� 12", 0, 12);
	PrintPos("����:", 16, 0, 14);
	k = InDigit(17, " ", 0, 31);
	rg.h.ah = 0x05;
	inttobcd(k, &(rg.h.dl));//����
	inttobcd(j, &(rg.h.dh));//�����
	inttobcd(i, &(rg.h.cl));//���
	int86(0x1a, &rg, &rg);
	ShowTime();
}
//�뢮� ���� � ����ﬨ ��� �६����
void TimeOperations(void)
{
	int k = 0, j = 0;
	Clear(3, 0, 22 * 80);
	PrintPos("1) ��ᬮ�� ⥪�饣� �६���", 3, 0, 3);
	PrintPos("2) ��⠭����� ⥪�饥 �६�", 4, 0, 3);
	rg.h.ah = 0;
	int86(0x16, &rg, &rg);
	k = rg.h.ah;
	switch (k)
	{
	case 2:
		ShowTime();
		PressAnyKey(7);
		break;
	case 3:
		ShowTime();
		PrintPos("1) ��⠭����� ����", 8, 0, 3);
		PrintPos("2) ��⠭����� �६�", 9, 0, 3);
		rg.h.ah = 0;
		int86(0x16, &rg, &rg);
		j = rg.h.ah;
		if (j == 2)
			SetDate();
		else if (j == 3)
			SetTime();
		PressAnyKey(20);
	default:
		Menu();
		break;
	}
}