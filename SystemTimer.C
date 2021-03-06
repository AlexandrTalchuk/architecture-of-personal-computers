//924401-2,������ �.�.,LR2:���⥬�� ⠩���
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <dos.h>
#include <string.h>
#define TimerClock 1193180//���⮢�� ���� ⠩���
#define READ_PORT_40 0xe2//1110 0010
#define READ_PORT_41 0xe4//1110 0100
#define READ_PORT_42 0xe8//1110 1000
#define Port40 0x40//0100 0000
#define Port41 0x41//0100 0001
#define Port42 0x42//0100 0010
#define Port43 0x43//0100 0011
#define Port61 0x61//0110 0001
#define Dynamic_on 3//0011
#define Dynamic_off 0xfc//1111 1100

void StatusOfPorts(void);//�⥭�� ᫮� ���ﭨ� �������, ���祭�� ॣ���஢
void Call(int frequency, int time);//��⠭���� ���祭�� ���稪� ��ண� ������
void CallMusic(void); //�ந��뢠��� ��모
char* b2bin(unsigned char bin, char* str); //��ॢ�� � ������� ��⥬�
char* b2h2(unsigned char bin, char* str);//��ॢ�� � 16����
unsigned char bit4to1h(unsigned char bin); //��ॢ�� �� 2��� � 16���
void Dynamic(int b);//����祭��/�몫�祭�� ���������
void Name();//�뢮� �����, ������୮� ࠡ���

struct Notes //������� ����� � ���⥫쭮��
{
	unsigned int frequency;
	unsigned int time;
};
void Dynamic(int b)//����祭��/�몫�祭�� ���������
{
	unsigned int dwResult = 0;
	unsigned char t;
	char* str_binary;
	// �����頥� 㪠��⥫� �� �������᪨ �뤥������
	//������� ����� ��� 9 char
	str_binary = (char*)calloc(9, sizeof(char));
	//����祭�� ���祭�� �� 61 ����
	dwResult = inport(Port61);
	if (b)
	{
		//0x03 = 0000 0011. 0 � 1 ��� ࠢ�� 11, �� ����砥� ���
		//������� � ���� ������ ���� ��� ��� ��������
		//ᨣ��� �� 2 ������ ��室�� �� �室
		outport(Port61, dwResult| Dynamic_on);

	}
	else
	{
		//0xFC = 1111 1100, ��� ���
		//� ������ ���� ��� �몫 ��������
		outport(Port61, dwResult & Dynamic_off);

	}
}
void Call(int frequency, int time)//��⠭���� ���祭�� ���稪� ��ண� ������
{
	int value;
	//���᫥��� ����প� ��� ����㧪� � ॣ���� ���稪� ⠩���.
	value = TimerClock / frequency;
	//����㧪� ॣ���� ���稪� ⠩���
	//᭠砫� ������ ���祭�� ����襣� ����
	//����⥫� �����
	printf("%d/%d | ", frequency, time);
	//Port42 = 0100 0010(2 �����, 1 ०��: ���騩 ���⨢�����)
	//�����஢�� ���稪� �������� ������� ���ﭨ� ���稪� ��� ��⠭���� ⠩���
	//��⥬ ���襣�
	outp(Port42, (char)value);
	outp(Port42, (char)(value >> 8));
	delay(time);//����প� �� �६� time
}
char* b2bin(unsigned char bin, char* str)// ��ॢ�� � ������� ��⥬�
{
	int i;
	for (i = 0; i < 8; i++)
	{
		str[i] = (bin & 0x80) ? '1' : '0';//1000 0000
		//�몫�砥� �� ���� �஬� ���襣�,
		//�᫨ �� �� 1, � ��୥��� ���(128), ���� 䮫�(0)
		bin <<= 1;
	}
	str[8] = '\0';
	return str;
}
char* b2h2(unsigned char bin, char* str)//��ॢ�� � 16���
{
	str[0] = bit4to1h(bin >> 4);
	str[1] = bit4to1h(bin);
	str[2] = '\0';
	return str;
}
unsigned char bit4to1h(unsigned char bin)//��ॢ�� �� 2��� � 16���
{
	bin &= 0x0f;//0000 1111
	return (bin < 10) ? ('0' + bin) : ('A' + bin - 10);
}

void StatusOfPorts()//�⥭�� ᫮� ���ﭨ� �������, ���祭�� ॣ���஢
{
	short sh;
	unsigned char ch;
	char* str;
	//�����頥� 㪠��⥫� ���������᪨ �뤥����� ���ᨢ
	//��� 9 �ᥫ ⨯� char
	str = (char*)calloc(9, sizeof(char));
	printf("���� 61h ");
	ch = inport(0x61);
	//��ॢ�� � 2���� ��⥬�
	b2bin(ch, str);
	printf("| ������: %s\t", str);
	//��ॢ�� � 16����
	b2h2(ch, str);
	printf("| ���⭠������: %s\n", str);
	//0 �����
	printf("����� 0 ");
	//�⥭�� ᫮�����ﭨ� 0 ������
	outp(0x43, READ_PORT_40);//������� �⥭��, � � 6,7 ���� =1
	//��� 4 =0 ������ ������� ��� ������
	ch = inp(0x40);
	//��ॢ�� � 2���� ��⥬�
	b2bin(ch, str);
	printf("| ������: %s\t", str);
	//��ॢ�� � 16���� ��⥬�
	b2h2(ch, str);
	printf("| ���⭠������: %s\n", str);
	//�⥭�� ⥪�饣� ���ﭨ� ॣ���� 0 ������
	outp(0x43, 0x00);//�믮������ ������� �����஢�� ���稪� ��� ��⠭.⠩���
	//5 � 4 ���� ������ ����=0, 0-3=�����, 6 � 7 ��। �����
	//�⥭�� ॣ���� 0 ������
	sh = inp(0x40) | (inp(0x40) << 8);
	printf("������� 0: ");
	//��ॢ�� � 2���� ��⥬�
	printf("| ������: %s.", b2bin(sh >> 8, str));
	printf("%s\t", b2bin(sh, str));
	//��ॢ�� � 16���� ��⥬�
	printf("| ���⭠������: %s", b2h2(sh >> 8, str));
	printf("%s\n", b2h2(sh, str));
	//1 �����
	printf("����� 1 ");
	//�⥭�� ᫮�����ﭨ� 1 ������
	outp(0x43, READ_PORT_41);
	ch = inp(0x41);
	//��ॢ�� � 2���� ��⥬�
	b2bin(ch, str);
	printf("| ������: %s\t", str);
	//��ॢ�� � 16���� ��⥬�
	b2h2(ch, str);
	printf("| ���⭠������: %s\n", str);
	//�⥭�� ॣ���� 1 ������
	// �믮������ ������� �����஢�� ���稪� ��� ��⠭.⠩���
	outp(0x43, 0x40);//0�40= 01000000
	sh = inp(0x41) | (inp(0x41) << 8);
	printf("������� 1: ");
	//��ॢ�� � 2���� ��⥬�
	printf("| ������: %s.", b2bin(sh >> 8, str));
	printf("%s\t", b2bin(sh, str));
	printf("| ���⭠������: %s", b2h2(sh >> 8, str));
	printf("%s\n", b2h2(sh, str));
	//2 �����
	printf("����� 2 ");
	//�⥭�� ᫮�����ﭨ� 2 ������
	outp(0x43, READ_PORT_42);
	ch = inp(0x42);
	//��ॢ�� � 2���� ��⥬�
	b2bin(ch, str);
	printf("| ������: %s\t", str);
	//��ॢ�� � 16���� ��⥬�
	b2h2(ch, str);
	printf("| ���⭠������: %s\n", str);
	//�⥭�� ॣ���� 2 ������
	// �믮������ ������� �����஢�� ���稪� ��� ��⠭.⠩���
	outp(0x43, 0x80);//0�80=10000000
	sh = inp(0x42) | (inp(0x42) << 8);
	printf("������� 2: ");
	//��ॢ�� � 2���� ��⥬�
	printf("| ������: %s.", b2bin(sh >> 8, str));
	printf("%s\t", b2bin(sh, str));
	//��ॢ�� � 16���� ��⥬�
	printf("| ���⭠������: %s", b2h2(sh >> 8, str));
	printf("%s\n", b2h2(sh, str));
	free(str);
}
void CallMusic(void) //�ந��뢠��� ��모
{
	int i = 0;
	struct Notes notes[] =
	{

		{932, 370}, {20000,30}, {932, 370}, {20000,30}, {932, 370}, {20000,30}, {932, 370}, {20000,30}, {932, 370}, {20000,30}, {987, 300}, {20000,30}, {1244, 300}, {20000,30},
		{932, 370}, {20000,30}, {932, 370}, {20000,30}, {932, 370}, {20000,30}, {932, 370}, {20000,30}, {932, 370}, {20000,30}, {987, 300}, {20000,30}, {1244, 300}, {20000,30},
		{1396,370},	{20000,30}, {1396,370}, {20000,30}, {1661,370}, {20000,30},
		{1479,370},	{20000,30}, {1396,370}, {20000,30},
		{1244,300}, {20000,30}, {1244,300},	{20000,30}, {1691,370}, {20000,30},
		{1497,370}, {20000,30}, {1396,370}, {20000,30},
		{1244,300},	{20000,30}, {1244,300}, {20000,30},
		{932, 370}, {20000,30}, {987, 370}, {20000,30}, {1244, 300}, {20000,30},
		{932, 370}, {20000,30}, {932, 370},	{20000,30}, {932, 370},  {20000,30}, {987, 300}, {20000,30}, {1244, 300},{20000,30},
		{932, 370}, {20000,30}, {932, 370}, {20000,30}, {932, 370},  {20000,30}, {1244, 300},{20000,30}, {932, 1000}, {0,0}

	};
	for (i = 0; notes[i].frequency != 0 && notes[i].time != 0; i++) //���ந�������� ���
	{
		Call(notes[i].frequency, notes[i].time);

	}
	printf("\n");
}
void Name()//�뢮� �����, ������୮� ࠡ���
{
	system("cls");
	printf("|--��㯯� 924401-2, ������ �.�.--|\n");
	printf("|---------------��2---------------|\n");
	printf("|---------���⥬�� ������--------|\n");

}
int main()
{
	system("cls");
	while (1)                                																																																													
	{
		Name();
		printf("\n1: �ந���� �������\n2: �뢥�� ᫮�� ���ﭨ� ������� � ���祭�� ॣ��஢\n0: ��室\n\n");
		switch (getch()) {
		case '1':
			Name();
			Dynamic(1);//����祭�� ��������
			StatusOfPorts();
			CallMusic();//�ந��뢠��� ��모
			Dynamic(0);//�몫�祭�� ��������
			StatusOfPorts();
			system("pause");
			break;
		case '2':
			Name();
			StatusOfPorts();//�뢮� ᫮�� ���ﭨ� ������� � ���祭�� ॣ���஢
			system("pause");
			break;
		case '0':
			system("pause");
			return 0;
		}
	}
}


