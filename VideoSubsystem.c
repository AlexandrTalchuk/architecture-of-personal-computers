
//924401-2 Тальчук А.П.
//ЛР-7 Работа видеоподсистемы
#include <dos.h>
#include <stdio.h>
#include <conio.h>

void PrntPos(char* _str, int _posX, int _posY); //вывод строки на экран в указанном месте
void CGA(void); // функция прорисовки ромба
void EGA(void); // функция прорисовки пересекающихся линий
void Clear(int _posY); // функция очистки экрана

int main(void)
{
	char input;
	while (1)
	{
		Clear(0);
		PrntPos("||-----------------------------------||", 0, 0);
		PrntPos("||     924401-2 Тальчук А.П,         ||", 0, 1);
		PrntPos("||    LR7. Работа видеоподсистемы    ||", 0, 2);
		PrntPos("||-----------------------------------||", 0, 3);
		PrntPos("Включен текстовый режим (AL = 0x03)", 0, 5);
		PrntPos("1 - Режим CGA (AL = 0x04, 320x200)", 0, 5); // Color Graphics Adapter - цветной графич. адаптер
		PrntPos("2 - Режим EGA (AL = 0x0D, 320x200)", 0, 6); // Enhanced Graphics Adapter - улучш. графич. адаптер
		PrntPos("3 - Выход", 0, 7);
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
	// Примечание: параметры inregs и outregs могут указывать на одну и туже структуру.
		// Программы, составленные на языке С, обращаются к прерываниям MS-DOS обычно с помощью таких функций, как intdos, int86, intdosx и т.д.
		//Главной особенностью объединения является то, что для каждого из объявленных элементов
		//выделяется одна и та же область памяти, т.е. они перекрываются.
	unsigned char color, j, p = 1, i;
	color = 2; // номер цвета линии
	inregs.h.ah = 0x0;
	inregs.h.al = 0x4;//переходим в режим СGA
	int86(0x10, &inregs, &outregs);
	inregs.h.ah = 0x0B;
	inregs.h.bh = 0x0; //устанавливаем цвет фона
	inregs.h.bl = 0x01; //номер устанавливаемого цвета фона
	int86(0x10, &inregs, &outregs);
	while (!kbhit()) { // пока не будет нажата какая-либо клавиша
	// выводим массив точек
		inregs.h.ah = 0x0C;
		inregs.h.al = (unsigned char)(color); // устанавливаем цвет линии
		for (j = 78; j > 0; j--) {
			inregs.x.cx = 0x65; // x-координата точки
			inregs.x.dx = 0x54 + j; // y-координата точки
			int86(0x10, &inregs, &outregs);

			inregs.x.cx = 0xDF; // x-координата точки
			inregs.x.dx = 0x54 + j; // y-координата точки
			int86(0x10, &inregs, &outregs);
		}
		for (j = 185; j > 0; j--) {
			inregs.x.cx = 0x45 + j; // x-координата точки
			inregs.x.dx = 0x54; // y-координата точки
			int86(0x10, &inregs, &outregs);
		}
		for (j = 122; j > 0; j--) {
			inregs.x.cx = 0x65 + j; // x-координата точки
			inregs.x.dx = 0xA2; // y-координата точки
			int86(0x10, &inregs, &outregs);

		}
		for (j = 38; j > 0; j--) {
			inregs.x.cx = 0xA1 + j * 2.4; // x-координата точки
			inregs.x.dx = 0x2D + j; // y-координата точки
			int86(0x10, &inregs, &outregs);

			inregs.x.cx = 0xA3 - j * 2.4; // x-координата точки
			inregs.x.dx = 0x2D + j; // y-координата точки
			int86(0x10, &inregs, &outregs);
		}
		delay(300); // устанавливаем задержку
		color++;
		if (color == 4) color = 2;
	}
	inregs.h.ah = 0x0;
	inregs.h.al = 0x3;//переход в текстовый режим
	int86(0x10, &inregs, &outregs);
}


void EGA(void) { //режим EGA
	union REGS inregs, outregs;
	unsigned char i, j, color;
	color = 7;// номер цвета линии
	inregs.h.ah = 0x00;
	inregs.h.al = 0x0D;//установка режима EGA
	int86(0x10, &inregs, &outregs);
	inregs.h.ah = 0x0B;
	inregs.h.bh = 0x00; //задание цвета фона
	inregs.h.bl = 0x05;//номер устанавливаемого цвета фона
	int86(0x10, &inregs, &outregs);
	while (!kbhit()) { // пока не будет нажата какая-либо клавиша
	// выводим массив точек
		inregs.h.ah = 0x0C;
		inregs.h.al = (unsigned char)(color); // устанавливаем цвет линии
		for (j = 38; j > 0; j--) {
			inregs.x.cx = 0xA1 + j * 2.4; // x-координата точки
			inregs.x.dx = 0x2D + j; // y-координата точки
			int86(0x10, &inregs, &outregs);

			inregs.x.cx = 0xA3 - j * 2.4; // x-координата точки
			inregs.x.dx = 0x2D + j; // y-координата точки
			int86(0x10, &inregs, &outregs);
		}
		for (j = 38; j > 0; j--) {
			inregs.x.cx = 0xA1 + j * 2.4; // x-координата точки
			inregs.x.dx = 0x7C - j; // y-координата точки
			int86(0x10, &inregs, &outregs);

			inregs.x.cx = 0xA3 - j * 2.4; // x-координата точки
			inregs.x.dx = 0x7C - j; // y-координата точки
			int86(0x10, &inregs, &outregs);
		}
		delay(200); // устанавливаем задержку
		color++;
		if (color == 15) color = 1;
		//else color = 12;
	}
	inregs.h.ah = 0x00;
	inregs.h.al = 0x03;//переход в текстовый режим
	int86(0x10, &inregs, &outregs);
}
