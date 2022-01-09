//924401-2
//������ �.�.
//�� 4 �����⥬� ���뢠���
#include <dos.h>
#define MASTER		0x20
#define SLAVE		0xA0
#define IRR		0x0A
#define ISR	        0x0B
#define IMR_MASTER  	0x21
#define IMR_SLAVE       0xA1
#define CYAN		0x30
#define BLACK		0x00
#define SYMBOL_SIZE 	0x02
#define INT00H		0x00
#define INT08H		0x08
#define INT09H		0x09
#define INT70H		0x70
#define STRING_SIZE     0xA0//160
#define FIRST_STRING 	0x0C//12
#define LAST_STRING  	0x18//24
#define QUANTITY_OF_SYMBOLS 0x50 //80

//�����⥫� �� �㭪�� ��ண� ��ࠡ��稪� ���뢠��� ����������
void interrupt(*old_IRQ_09h) (void);
//�㭪�� ��ࠡ�⪨ ���뢠��� ����������
void interrupt new_IRQ_09h(void);

//�����⥫� �� �㭪�� ��ண� ��ࠡ��稪� ��⥬���� ⠩���
void interrupt(*old_IRQ_08h) (void);
//�㭪�� ��ࠡ�⪨ ���뢠��� ��⥬���� ⠩���
void interrupt new_IRQ_08h(void);

//�����⥫� �� �㭪�� ��ண� ��ࠡ��稪� ���뢠��� �訡�� �������
void interrupt(*old_IRQ_00h) (void);
//�㭪�� ��ࠡ�⪨ ���뢠��� �訡�� �������
void interrupt new_IRQ_00h(void);

//�����⥫� �� �㭪�� ��ண� ��ࠡ��稪� ���뢠��� �ᮢ ॠ�쭮�� �६���
void interrupt far(*old_IRQ_70h)(void);
//�㭪�� ��ࠡ�⪨ ���뢠��� �ᮢ ॠ�쭮�� �६���
void interrupt far new_IRQ_70h(void);

//�㭪�� �⥭�� ���祭�� ॣ���஢ ����஫��� ���뢠���
void ReadRegister(void);

//�㭪�� �뢮�� ���祭�� ॣ���஢ ����஫��� ���뢠���
void PrintRegister(void);

//�㭪�� ���⪨ 㪠������ ��ப� ���᮫�
void ClearRow(int);

//�㭪�� ���⪨ �࠭�
void ClearScreen(void);

//�㭪�� ����祭�� ᨬ���� ��⭠����筮�� �᫠ ��� 4 ������ ���
char t2hex(unsigned char);

//�㭪�� ��ॢ��� �᫠ �� �����筮� � ��᭠������� ��⥬�
char* b2hex(unsigned char, char*);

//�㭪�� ��ॢ��� �᫠ �� �����筮� � ������� ��⥬�
char* b2bin(unsigned char, char*);

//�㭪�� �뢮�� 蠯�� � ���᮫�
void header(void);

//�㭪�� ��ॢ��� �᫠ �� �����筮�� �ଠ� � ��ப�
char* dec2char(short, char*);

//�㭪�� ��ॢ��稢���� ��ப�
char* reverse(char*, int);

//�㭪�� ������� �� 0
void divide(void);

//�뢮� � ���᮫� �१ ����������� � ��।.����, ��אַ� ���饭�� � ���
void printVideoMemory(char* str, int row, int column);

//�뢮� � ���᮫� �१ �����������
void printVideoMemory1(char* str);

//�㭪�� �ப��⪨ �࠭�
void ScrollScreen(void);

//���樠������ ��६����� ⥪��� ��ப� � �⮫�� � 䫠��� �ணࠬ��
unsigned char	y = FIRST_STRING, x = 0, isQuit = 0, is00h = 0, is08h = 0, isReadRegister = 0, is70h = 0, is09h = 0;

//���樠������ ��६�����, ����室���� ��� ࠡ��� �ணࠬ��
unsigned char	master_IRR, slave_IRR, master_ISR, slave_ISR, master_IMR, slave_IMR, divisible = 1, divisor = 1, scanCode = 0;
short ticks = 0;
int count = 0;

//�����⥫� �� ��砫� ���������
char far* start = (char far*) 0xb8000000;

//�����⥫� �� ᨬ��� ��ਡ�⮢ �������஢ ���뢠���
char far* indicator00h, far* indicator08h, far* indicator09h, far* indicator70h;

int main()
{
	short seconds = 0;
	unsigned char value;
	char buffer[9];

	//��।������ ��������� ��ਡ�⮢ ᨬ����� �������஢ � ���뢠���
	indicator00h = start + 10 * STRING_SIZE + 28 * 2 + 1;//68
	indicator08h = start + 9 * STRING_SIZE + 28 * 2 + 1;
	indicator70h = start + 9 * STRING_SIZE + 65 * 2 + 1;
	indicator09h = start + 10 * STRING_SIZE + 65 * 2 + 1;

	//�뢮� 蠯�� ���᮫�
	header();
	//���ப������ ������ ���뢠��� ���������� �� �६� ��⠭���� ����� ��ࠡ��稪�� ���뢠���
	disable();

	//����祭�� ����� ���뢠��� � ����஬ 8h
	old_IRQ_08h = getvect(INT08H);
	//�����祭�� ������ ��ࠡ��稪� ���뢠��� INT8h
	setvect(INT08H, new_IRQ_08h);

	//����祭�� ����� ���뢠��� � ����஬ 9h
	old_IRQ_09h = getvect(INT09H);
	//�����祭�� ������ ��ࠡ��稪� ���뢠��� INT9h
	setvect(INT09H, new_IRQ_09h);

	//����祭�� ����� ���뢠��� � ����஬  0h
	old_IRQ_00h = getvect(INT00H);
	//�����祭�� ������ ��ࠡ��稪� ���뢠��� INT0h
	setvect(INT00H, new_IRQ_00h);

	//����祭�� ����� ���뢠��� � ����஬ 70h
	old_IRQ_70h = getvect(INT70H);
	//�����祭�� ������ ��ࠡ��稪� ���뢠��� INT70h
	setvect(INT70H, new_IRQ_70h);

	//���ப������ ����������� ���������� ������ ���뢠���
	enable();

	outp(0x70, 0x0B);//�롮� ����室����� ॣ���� ���ﭨ� �ᮢ ॠ�쭮�� �६���
	value = inp(0x71); //����祭�� ������ �� �⮣� ॣ����
	outp(0x70, 0x0B); //������ �롮� ॣ���� ���ﭨ�
	outp(0x71, value | 0x40); //������ ������ ���祭�� � ����� ॣ����. 40h == 0100 0000b.
	//��� ��� ��⮩ ��� ॣ���� ���ﭨ� 2 �ᮢ ॠ�쭮�� �६��� �⢥���� �� ࠧ�襭�� �맮�� ��ਮ���᪮�� ���뢠���, � ��� �㦭� ��⠭����� � 1

	while (!isQuit)
	{
		if (isReadRegister)
		{
			PrintRegister();//�뢮� ���祭�� ॣ���஢ ����஫��� ���뢠���
			isReadRegister = 0;//���� 䫠�� regReadFlag
		}
		if (is00h)
		{
			printVideoMemory("��������! �ந��諮 ������� �� 0!", 11, 20);
			delay(1000);//�ਮ�⠭���� ࠡ��� �ணࠬ�� �� 1000��
			printVideoMemory("                                 ", 11, 20);
			*(indicator00h) = BLACK;//"�몫�祭��" �������� ���뢠���
			is00h = 0;//���� 䫠��
		}
		if (is09h)
		{
			int maxCount;
			maxCount = (25 - FIRST_STRING) * QUANTITY_OF_SYMBOLS;//25 �ᥣ�, ���� 10 �� � ���
			if (count >= maxCount)
			{
				//�� 8 �� 23 ���窨 ࠡ.�������
				y = LAST_STRING;//�ᥣ� 25 ��ப, �� 0 �� 24
				x = 0;
				ScrollScreen();
				count = maxCount - QUANTITY_OF_SYMBOLS;
				//-80 ��-�� �������襩�� ���窨
			}
			switch (scanCode)
			{
			case 0x01://����-��� ������ ESC
				isQuit = 1; //��⠭���� 䫠�� �����襭�� �ணࠬ��
				break;
			case 0x0B://����-��� ������ 0
				divide();//�맮� ������� �� 0
				break;
			case 0x2E://����-��� ������ c
				header();//�뢮� 蠡���� ���᮫�
				break;
			}
			delay(200);
			*(indicator09h) = BLACK;//"�몫�祭��" �������� ���뢠���
			is09h--;//�����襭�� 䫠��
		}
		if (is08h)
		{
			//ticks 㢥��稢����� �� ������ �맮�� ���뢠��� ��⥬���� ⠩��� � ������� ���祭�� �� ����� 18
			//��� ⮫쪮 ��� �⠭�� ࠢ�� ���, �㦭� �㤥� �뢥�� ����� ᥪ㭤�
			if (ticks == 0)
			{
				printVideoMemory(dec2char(++seconds, buffer), 9, 12);
				seconds %= 999; //��࠭�祭�� �뢮������ ������⢠ ᥪ㭤
			}
			delay(20);
			*(indicator08h) = BLACK;//"�몫�祭��" �������� ���뢠���
			is08h = 0;//���� 䫠��
		}
		if (is70h)
		{
			*(indicator70h) = BLACK;//"�몫�祭��" �������� ���뢠���
			is70h = 0;//���� 䫠��
		}
	}

	disable();//���ப������ ������ ���뢠��� ���������� �� �६� �����饭�� ����� ��ࠡ��稪�� ���뢠���

	setvect(INT00H, old_IRQ_00h);//�����饭�� ��室���� ��ࠡ��稪� ���뢠��� INT0h
	setvect(INT08H, old_IRQ_08h);//�����饭�� ��室���� ��ࠡ��稪� ���뢠��� INT8h
	setvect(INT09H, old_IRQ_09h);//�����饭�� ��室���� ��ࠡ��稪� ���뢠��� INT9h
	setvect(INT70H, old_IRQ_70h);//�����饭�� ��室���� ��ࠡ��稪� ���뢠��� INT70h

	outp(0x70, 0x0B);//����⠭������� ���祭�� ॣ���� ���ﭨ� 2 �ᮢ ॠ�쭮�� �६���
	value = inp(0x71);//����⠭������� �ᮢ ॠ�쭮�� �६���
	outp(0x70, 0x0B);//�롮� ����室����� ॣ���� ���ﭨ� �ᮢ ॠ�쭮�� �६���
	outp(0x71, value & 0xBF);

	enable();//���ப������ ������������� ���������� ������ ���뢠���
	ClearScreen();//���⪠ �࠭�
	return 0;
}



//�㭪�� ��ࠡ�⪨ ���뢠��� ��⥬���� ⠩���
void interrupt new_IRQ_08h(void)
{
	//��⠭���� ���㡮�� 䮭� ᨬ���� ��ਡ�⮢ �������� ���뢠���
	*(indicator08h) = CYAN;
	ReadRegister();
	ticks++;
	ticks %= 18;
	isReadRegister = 1;
	is08h = 1;
	old_IRQ_08h();//�맮� ��ண� ��ࠡ��稪� ���뢠���
}

//�㭪�� ��ࠡ�⪨ ���뢠��� �訡�� �������
void interrupt new_IRQ_00h(void)
{
	//��⠭���� ���㡮�� 䮭� ᨬ���� ��ਡ�⮢ �������� ���뢠���
	*(indicator00h) = CYAN;
	//����⠭������� ���祭�� �������� � ����⥫�
	divisible = 1;
	divisor = 1;
	isReadRegister = 1;
	is00h = 1;
}

//�㭪�� ��ࠡ�⪨ ���뢠��� ����������
void interrupt new_IRQ_09h(void)
{
	unsigned char str[3];
	is09h++; //�����祭�� 䫠�� kbFlag
	isReadRegister = 1; //��⠭���� 䫠�� regReadFlag
	ReadRegister();//�⥭�� ���祭�� ॣ���஢ ����஫��� ���뢠���
	*(indicator09h) = CYAN;//��⠭���� ���㡮�� 䮭� ᨬ���� ��ਡ�⮢ �������� ���뢠���
	scanCode = inp(0x60); //����祭�� ���祭�� ���� 60h. � ������ ��砥 - ���� ᪠�-���� ����⮩ ������
	printVideoMemory1(b2hex(scanCode, str));
	old_IRQ_09h();//�맮� ��ண� ��ࠡ��稪� ���뢠��� ����������
}

//�㭪�� ��ࠡ�⪨ ���뢠��� �ᮢ ॠ�쭮�� �६���
void interrupt far new_IRQ_70h(void)
{
	*(indicator70h) = CYAN;//��⠭���� ���㡮�� 䮭� ᨬ���� ��ਡ�⮢ �������� ���뢠���
	is70h = 1;//��⠭���� 䫠�� rtcFlag
	ReadRegister();//�⥭�� ���祭�� ॣ���஢ ����஫��� ���뢠���
	outp(0x70, 0x0C);//�롮� ॣ���� ���ﭨ� 3 �ᮢ ॠ�쭮�� �६���
	inp(0x71); //�⥭�� ��࠭���� ॣ���� � ���㫥���� 7 ���, �⢥��饣� �� ���뢠��� IRQ8
	old_IRQ_70h();//�맮� ��ண� ��ࠡ��稪� ���뢠��� �ᮢ ॠ�쭮�� �६���
}

//�㭪�� �뢮�� 蠯�� ���᮫�
void header(void)
{
	ClearScreen();//���⪠ �࠭�
	printVideoMemory("|  924401-2 ������ �.�.    | ����稥 ������:        0 - ������� �� 0        |", 0, 0); //�뢮� ��ப� �� ���᮫�
	printVideoMemory("|�� �4 �����⥬� ���뢠���| C - ���⪠ ���᮫�     ESC - ��室 �� �ணࠬ��|", 1, 0);
	printVideoMemory("|___________________________|_________________________________________________|", 2, 0);
	printVideoMemory("|     �������    |           MASTER          |              SLAVE             |", 3, 0);
	printVideoMemory("|       IRR      | 0x21                      | 0xA0                           |", 4, 0);
	printVideoMemory("|       ISR      | 0x20                      | 0x0A                           |", 5, 0);
	printVideoMemory("|       IMR      | 0x20                      | 0xA1                           |", 6, 0);
	printVideoMemory("|       0x60     |                           |                                |", 7, 0);
	printVideoMemory("|________________|___________________________|________________________________|", 8, 0);
	printVideoMemory("|    ������       (INT08h):   |  ���뢠��� RTC        (INT70h):              |", 9, 0);
	printVideoMemory("|    ������� �� 0 (INT00h):   |  ���뢠��� ���������� (INT09h):              |", 10, 0);
}

//�㭪�� �뢮�� ���祭�� ॣ���஢ ����஫��� ���뢠���
void PrintRegister()
{
	char buffer[9];
	//�뢮� ��⭠����筮�� ���祭�� �᫠ � ���᮫�, � ��।����� ������ �१ �����������
	printVideoMemory(b2hex(master_IRR, buffer), 4, 25);
	//�뢮� ����筮�� ���祭�� �᫠ � ���᮫�, � ��।����� ������ �१ �����������
	printVideoMemory(b2bin(master_IRR, buffer), 4, 29);

	printVideoMemory(b2hex(slave_IRR, buffer), 4, 56);
	printVideoMemory(b2bin(slave_IRR, buffer), 4, 60);
	printVideoMemory(b2hex(master_ISR, buffer), 5, 25);
	printVideoMemory(b2bin(master_ISR, buffer), 5, 29);
	printVideoMemory(b2hex(slave_ISR, buffer), 5, 56);
	printVideoMemory(b2bin(slave_ISR, buffer), 5, 60);
	printVideoMemory(b2hex(master_IMR, buffer), 6, 25);
	printVideoMemory(b2bin(master_IMR, buffer), 6, 29);
	printVideoMemory(b2hex(slave_IMR, buffer), 6, 56);
	printVideoMemory(b2bin(slave_IMR, buffer), 6, 60);
	printVideoMemory(b2hex(scanCode, buffer), 7, 25);
	printVideoMemory(b2bin(scanCode, buffer), 7, 29);
}

//�㭪�� ���⪨ 㪠������ ��ப�
void ClearRow(int row)
{
	short i = 0;
	char far* v = start + row * STRING_SIZE;//�����⥫� �� ᨬ���, � ���ண� �㤥� ���� ���⪠
	for (; i < 80; i++)
	{
		*v = ' ';//���������� �祩�� ᨬ����� �஡��
		*(v + 1) = 0;//��饭�� ������ ��ਡ�⮢
		v += 2;
	}
}

//�㭪�� ���⪨ �࠭�
void ClearScreen()
{
	unsigned char i;
	for (i = 0; i <= 24; i++)//�맮� �㭪樨 ���⪨ 㪠������ ��ப� ��� ��� ��ப ���᮫�
		ClearRow(i);
	y = FIRST_STRING; //���� ���祭�� ⥪�饩 ��ப�
	x = 0; //���� ���祭�� ⥪�饣� �⮫��
}

//�㭪�� ������� �� 0
void divide(void)
{
	divisor = 0;//��᢮���� ����⥫� 0
	divisible /= divisor;//������� �� 0
}

//�㭪�� ��ॢ��� �᫠ �� �����筮�� �ଠ� � ��ப�
char* dec2char(short value, char* buffer)
{
	int i = 0;
	while (value != 0)
	{
		buffer[i++] = value % 10 + 48;//����祭�� ᨬ���� �� �����筮�� �᫠
		value /= 10;
	}
	buffer[i] = '\0';
	return reverse(buffer, i);//��ॢ��� ��ப�
}

//�㭪�� ��ॢ��� ��ப�
char* reverse(char* str, int length)
{
	int left = 0, right = length - 1;
	char temp;

	for (; left < right; left++, right--)
	{
		temp = str[left]; //����⠭���� ���祭�� str[left] � str[right]
		str[left] = str[right];
		str[right] = temp;
	}
	return str;
}

//�㭪�� �⥭�� ���祭�� ॣ���஢ ����஫��� ���뢠���
void ReadRegister()
{
	outp(MASTER, IRR);//�롮� �㦭��� ॣ���� ����஫��� ���뢠���
	master_IRR = inp(MASTER);//������ ���祭�� ॣ����
	outp(SLAVE, IRR);
	slave_IRR = inp(SLAVE);
	outp(MASTER, ISR);
	master_ISR = inp(MASTER);
	outp(SLAVE, ISR);
	slave_ISR = inp(SLAVE);
	master_IMR = inp(IMR_MASTER);//�⥭�� ��᪨ ����饣� ����஫��� ���뢠���
	slave_IMR = inp(IMR_SLAVE); //�⥭�� ��᪨ �������� ����஫��� ���뢠���
}

//�㭪�� ����祭�� ᨬ���� ��⭠����筮�� �᫠ ��� 4 ������ ���
char t2hex(unsigned char value)
{
	value &= 0x0F;//��᪠ 
	return (value < 10) ? '0' + value : 'A' - 10 + value;
}

//�㭪�� ��ॢ��� �᫠ �� �����筮� � ��⭠������� ��⥬�
char* b2hex(unsigned char value, char* str)
{
	str[0] = t2hex(value >> 4);
	str[1] = t2hex(value);//����祭�� ᨬ���� ��⭠����筮�� �᫠ ��� 4 ������ ���
	str[2] = '\0';
	return str;
}

//�㭪�� ��ॢ��� �᫠ �� �����筮� � ������� ��⥬�
char* b2bin(unsigned char value, char* str)
{
	unsigned char i = 0;
	for (; i < 8; i++)
	{
		str[i] = value & 0x80 ? '1' : '0';
		value = value << 1; //����⮢� ᤢ�� ����� �� 1 ���
	}
	str[8] = '\0';
	return str;
}

// �뢮� � ���᮫� �१ ����������� � ��।������� ����, ��אַ� ���饭�� � ����������
void printVideoMemory(char* str, int row, int column)
{
	int i;
	char far* v;
	v = start + row * STRING_SIZE + column * SYMBOL_SIZE;
	//ᨬ��� - 2 ����, � ��ப� �� 80, �.�. ࠧ��� ����� ��ப� 160

	for (i = 0; str[i] != '\0'; i++)
	{
		*v = str[i];
		*(v + 1) = 0x07; //��⠭���� ��ਡ�� ᨬ���� (7h = 00000111b).
		//���� �� ���, ��⠭������ � 1, �⢥��� �� ����, ����� � ᨭ�� ���������� 梥� ᮮ⢥�⢥���
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
		*(v + 1) = 0x07; //��⠭���� ��ਡ�� ᨬ���� (7h = 00000111b).
		//���� �� ���, ��⠭������ � 1, �⢥��� �� ����, ����� � ᨭ�� ���������� 梥� ᮮ⢥�⢥���
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
		*(v + 1) = 0;//��饭�� ������ ��ਡ�⮢
		v += 2;//v++
		count++;
	}
}

//�ப��⪠ �࠭�
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
	ClearRow(24);//���⪠ �࠭�
}