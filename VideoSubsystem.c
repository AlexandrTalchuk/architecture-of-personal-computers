
//924401-2 ������ �.�.
//��-7 ����� ����������⥬�
#include <dos.h>
#include <stdio.h>
#include <conio.h>

void PrntPos(char* _str, int _posX, int _posY); //�뢮� ��ப� �� �࠭ � 㪠������ ����
void CGA(void); // �㭪�� ���ᮢ�� ஬��
void EGA(void); // �㭪�� ���ᮢ�� ���ᥪ������ �����
void Clear(int _posY); // �㭪�� ���⪨ �࠭�

int main(void)
{
	char input;
	while (1)
	{
		Clear(0);
		PrntPos("||-----------------------------------||", 0, 0);
		PrntPos("||     924401-2 ������ �.�,         ||", 0, 1);
		PrntPos("||    LR7. ����� ����������⥬�    ||", 0, 2);
		PrntPos("||-----------------------------------||", 0, 3);
		PrntPos("����祭 ⥪�⮢� ०�� (AL = 0x03)", 0, 5);
		PrntPos("1 - ����� CGA (AL = 0x04, 320x200)", 0, 5); // Color Graphics Adapter - 梥⭮� ����. ������
		PrntPos("2 - ����� EGA (AL = 0x0D, 320x200)", 0, 6); // Enhanced Graphics Adapter - ����. ����. ������
		PrntPos("3 - ��室", 0, 7);
		input = getch();
		switch (input) {
		case '1': CGA(); break;
		case '2': EGA(); break;
		case '3': return 0;
		default: break;
		}
	}
}

void PrntPos(char* _str, int _posX, int _posY)
{
	int i;
	char far* memp;
	memp = (char far*)0xb8000000 + _posY * 160 + _posX * 2;
	for (i = 0; _str[i] != '\0'; i = i + 1)
	{
		*memp = _str[i];
		memp++;
		*memp = 7;
		memp++;
	}
}


void Clear(int _posY)
{
	int i, j;
	char far* memp;
	memp = (char far*)0xB8000000;
	for (i = _posY; i < 25; i = i + 1)
	{
		for (j = 0; j < 80; j = j + 1)
		{
			memp = (char far*)0xB8000000 + i * 160 + j * 2;
			*memp = ' ';
			memp++;
			*memp = 7;
		}
	}

}


void CGA(void)
{
	union REGS inregs, outregs;
	// �ਬ�砭��: ��ࠬ���� inregs � outregs ����� 㪠�뢠�� �� ���� � �㦥 ��������.
		// �ணࠬ��, ��⠢����� �� �몥 �, �������� � ���뢠��� MS-DOS ���筮 � ������� ⠪�� �㭪権, ��� intdos, int86, intdosx � �.�.
		//������� �ᮡ�������� ��ꥤ������ ���� �, �� ��� ������� �� �������� ����⮢
		//�뤥����� ���� � � �� ������� �����, �.�. ��� ��४�뢠����.
	unsigned char color, j, p = 1, i;
	color = 2; // ����� 梥� �����
	inregs.h.ah = 0x0;
	inregs.h.al = 0x4;//���室�� � ०�� �GA
	int86(0x10, &inregs, &outregs);
	inregs.h.ah = 0x0B;
	inregs.h.bh = 0x0; //��⠭�������� 梥� 䮭�
	inregs.h.bl = 0x01; //����� ��⠭����������� 梥� 䮭�
	int86(0x10, &inregs, &outregs);
	while (!kbhit()) { // ���� �� �㤥� ����� �����-���� ������
	// �뢮��� ���ᨢ �祪
		inregs.h.ah = 0x0C;
		inregs.h.al = (unsigned char)(color); // ��⠭�������� 梥� �����
		for (j = 78; j > 0; j--) {
			inregs.x.cx = 0x65; // x-���न��� �窨
			inregs.x.dx = 0x54 + j; // y-���न��� �窨
			int86(0x10, &inregs, &outregs);

			inregs.x.cx = 0xDF; // x-���न��� �窨
			inregs.x.dx = 0x54 + j; // y-���न��� �窨
			int86(0x10, &inregs, &outregs);
		}
		for (j = 185; j > 0; j--) {
			inregs.x.cx = 0x45 + j; // x-���न��� �窨
			inregs.x.dx = 0x54; // y-���न��� �窨
			int86(0x10, &inregs, &outregs);
		}
		for (j = 122; j > 0; j--) {
			inregs.x.cx = 0x65 + j; // x-���न��� �窨
			inregs.x.dx = 0xA2; // y-���न��� �窨
			int86(0x10, &inregs, &outregs);

		}
		for (j = 38; j > 0; j--) {
			inregs.x.cx = 0xA1 + j * 2.4; // x-���न��� �窨
			inregs.x.dx = 0x2D + j; // y-���न��� �窨
			int86(0x10, &inregs, &outregs);

			inregs.x.cx = 0xA3 - j * 2.4; // x-���न��� �窨
			inregs.x.dx = 0x2D + j; // y-���न��� �窨
			int86(0x10, &inregs, &outregs);
		}
		delay(300); // ��⠭�������� ����প�
		color++;
		if (color == 4) color = 2;
	}
	inregs.h.ah = 0x0;
	inregs.h.al = 0x3;//���室 � ⥪�⮢� ०��
	int86(0x10, &inregs, &outregs);
}


void EGA(void) { //०�� EGA
	union REGS inregs, outregs;
	unsigned char i, j, color;
	color = 7;// ����� 梥� �����
	inregs.h.ah = 0x00;
	inregs.h.al = 0x0D;//��⠭���� ०��� EGA
	int86(0x10, &inregs, &outregs);
	inregs.h.ah = 0x0B;
	inregs.h.bh = 0x00; //������� 梥� 䮭�
	inregs.h.bl = 0x05;//����� ��⠭����������� 梥� 䮭�
	int86(0x10, &inregs, &outregs);
	while (!kbhit()) { // ���� �� �㤥� ����� �����-���� ������
	// �뢮��� ���ᨢ �祪
		inregs.h.ah = 0x0C;
		inregs.h.al = (unsigned char)(color); // ��⠭�������� 梥� �����
		for (j = 38; j > 0; j--) {
			inregs.x.cx = 0xA1 + j * 2.4; // x-���न��� �窨
			inregs.x.dx = 0x2D + j; // y-���न��� �窨
			int86(0x10, &inregs, &outregs);

			inregs.x.cx = 0xA3 - j * 2.4; // x-���न��� �窨
			inregs.x.dx = 0x2D + j; // y-���न��� �窨
			int86(0x10, &inregs, &outregs);
		}
		for (j = 38; j > 0; j--) {
			inregs.x.cx = 0xA1 + j * 2.4; // x-���न��� �窨
			inregs.x.dx = 0x7C - j; // y-���न��� �窨
			int86(0x10, &inregs, &outregs);

			inregs.x.cx = 0xA3 - j * 2.4; // x-���न��� �窨
			inregs.x.dx = 0x7C - j; // y-���न��� �窨
			int86(0x10, &inregs, &outregs);
		}
		delay(200); // ��⠭�������� ����প�
		color++;
		if (color == 15) color = 1;
		//else color = 12;
	}
	inregs.h.ah = 0x00;
	inregs.h.al = 0x03;//���室 � ⥪�⮢� ०��
	int86(0x10, &inregs, &outregs);
}
