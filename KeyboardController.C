//924401-2
//Тальчук А.П.
//ЛР-3 Контроллер клавиатуры
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
char far* start = (char far*)0xb8000000; // начало видеобуфера
unsigned int count = 0; //количество символов, уже напечатанных
int y = FIRST_STRING; //первая строчка, с которой будет идти заполнение
int x = 0; //для символа
int isQuit = 0;//для прерывания
char* b2bin(unsigned char bt, char* str);
//перевод в 2ичную систему
unsigned char bit4to1h(unsigned char bt);
//тетрада в 16ричную
char* b2h2(unsigned char bt, char* str);
//перевод в 16ичную систему
void ScrollScreen(void);
//прокрутка экрана
void CleanScreen(int row, int counts);
//очистка экрана
void printVideoMemory(char* str, int row, int column);
//вывод в консоль через видеопамять в опред.место, прямое обращение к ней
void printVideoMemory1(char* str);
//вывод в консоль через видеопамять
void ReadStatusRegister(void);
//вывод значений регистров
void ISRStatus();
//Функция вывода статуса регистра обслуживания прерывания(ISR) контроллера прерываний
void IndicatorsBlink(int code);
//мигание
void interrupt OwnInterruptHandler(void);
//собственный обработчик прерываний INT9
void interrupt(*oldInt9h)(void);
//указатель на обработчик INT9, подпрограмма обработки прерывания
void PrintName(void);
//вывод группы, имени, названия работы
void main()
{
	clrscr();//очистка текстового окна
	PrintName();
	oldInt9h = getvect(INT9);
	//сохранение обработчика прерываний, getvect считывает значение
	//вектора с номером 9, как указатель на функцию прерывания
	//возвращает 4-х байтовое значение, хранящееся в векторе прерывания
	//сохранение указателя на старый обработчик
	setvect(INT9, OwnInterruptHandler); //установка нового вектора
	ReadStatusRegister(); //вывод значений регистров
	ISRStatus(); //Вывод слова состояния регистра ISR
	while (!isQuit) //пока не установлен флаг выхода
	{

	}
	setvect(INT9, oldInt9h); //восстановление старого вектора
	return;
}
//вывод группы, имени, названия работы
void PrintName()
{
	char str0[] = { "||=====================================||" };
	char str1[] = { "||      924401-2 Тальчук А.П.          ||" };
	char str2[] = { "||     ЛР-3. Контроллер клавиатуры     ||" };
	char str3[] = { "||=====================================||" };
	char str4[] = { "||     Esc - выход, i - индикаторы     ||" };
	printVideoMemory(str0, 0, 0);
	printVideoMemory(str1, 1, 0);
	printVideoMemory(str2, 2, 0);
	printVideoMemory(str3, 3, 0);
	printVideoMemory(str4, 4, 0);
	//вывод на консоль через видеопамять в опред.место
	//номер группы, ФИО, название лр
}
//моргание индикаторов
void IndicatorsBlink(int code)
{
	int i;
	for (i = 0; i < 10; i++)
	{
		while ((inp(PORT_64) & CHECK_BIT_2) != 0x00);
		//читаем состояние порта + проверка
		outp(PORT_60, INDICATORS_CONTROL);
		//запись в порт команды управления индикаторами,
		//команда EDh для вкл / выкл индикаторов 
		while ((inportb(PORT_64) & CHECK_BIT_2) != 0x00);
		//inportb считывает байт из порта
		outp(PORT_60, code);
		delay(200);
		while ((inp(PORT_64) & CHECK_BIT_2) != 0x00);
		outp(PORT_60, INDICATORS_CONTROL);
		while ((inportb(PORT_64) & CHECK_BIT_2) != 0x00);
		outp(PORT_60, 0);
		delay(200);
	}
}
//собственный обработчик прерываний INT9
//обрабатывает прерывания, вызванные приходом кодов нажатия и отпускания клавиш
void interrupt OwnInterruptHandler()
{
	unsigned char value = 0;
	char st[3];
	int maxCount;
	maxCount = (25 - 10) * QUANTITY_OF_SYMBOLS;//25 всего, первые 10 не в счет
	ReadStatusRegister();
	if (count >= maxCount)
	{
		//от 8 до 23 строчки раб.область
		y = LAST_STRING;//всего 25 строк, от 0 до 24
		x = 0;
		ScrollScreen();
		count = maxCount - QUANTITY_OF_SYMBOLS;
		//-80 из-за добавившейся строчки
	}
	//в ответ на прерывание считываем код сканировния клавиши из порта клавиатуры
	value = inp(PORT_60);
	//получаем длину генерируемого скан-кода, запись командного байта в регистр команд
	if (value == SCAN_ESC) isQuit = 1;
	//изменение знач-я глоб переменной, скан-код esc
	if (value == SCAN_I) IndicatorsBlink(0x07);
	//скан-код нажатия клавиши i
	b2h2(value, st);//перевод в 16ичную систему
	printVideoMemory1(st);//вывод в консоль через видеопамять
	outp(PORT_20, 0x20);
	delay(100);
	ISRStatus();
	//Сброс контроллера прерываний
	//20 h прочитать байт из регистра команд
	//FA подтверждене приема инф-и
}
// вывод в консоль через видеопамять в опред.место
void printVideoMemory(char* str, int row, int column)
{
	int i;
	char far* v;
	v = start + row * STRING_SIZE + column * SYMBOL_SIZE;
	//символ - 2 байта, в строке их 80, т.е. размер одной строки 160
	for (i = 0; str[i] != '\0'; i++)
	{
		*v = str[i];
		v += SYMBOL_SIZE;
	}
}
// вывод в консоль через видеопамять,прямое обращение к видеопамяти
void printVideoMemory1(char* str)
{
	int i;
	char far* v;
	v = start + y * STRING_SIZE + x * SYMBOL_SIZE;
	//символ - 2 байта, в строке их 80, т.е. размер одной строки 160
	for (i = 0; str[i] != '\0'; i++)
	{
		x++;
		*v = str[i];
		v += SYMBOL_SIZE;
		count++;
	}
	if (x >= QUANTITY_OF_SYMBOLS)//т.к. в строку вмещается 80 символов
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
//перевод в 16ичную систему
char* b2h2(unsigned char bt, char* str)
{
	str[0] = bit4to1h(bt >> 4);
	str[1] = bit4to1h(bt);
	str[2] = 0;
	return str;
}
//тетрада в 16ричную
unsigned char bit4to1h(unsigned char bt)
{
	bt &= TETRAD;
	return (bt < 10) ? '0' + bt : 'A' - 10 + bt;
}
//перевод в 2ичную систему
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
//вывод значений регистров
void ReadStatusRegister()
{
	unsigned char t;
	char str[3];
	char str1[9];
	str[0] = '\0';//нуль терминатор
	str1[0] = '\0';
       	t = inp(PORT_64);//регистр состояния клавиатуры
	b2h2(t, str); //перевод в 16ичную систему
	b2bin(t, str1);//перевод в 2ичную систему
	printVideoMemory("Регистр состояния (0x64): ", 5, 0);
	printVideoMemory(str1, 5, 44);//вывод в консоль через видеопамять в опр место
	printVideoMemory(str, 5, 53);
	t = inp(PORT_61);//регистр команд
	b2h2(t, str); //перевод в 16ичную систему
	b2bin(t, str1);//перевод в 2ичную систему
	printVideoMemory("Регистр команд (0x61): ", 6, 0);
	printVideoMemory(str1, 6, 44);//вывод в консоль через видеопамять в опр место
	printVideoMemory(str, 6, 53);
	t = inp(PORT_60);//регистр управления клавиатурой, регистр данных
	//если у него 1 бит = 0, значит в нем что-то записано
	b2h2(t, str); //перевод в 16ичную систему
	b2bin(t, str1);//перевод в 2ичную систему
	printVideoMemory("Регистр данных (0x60): ", 7, 0);
	printVideoMemory(str1, 7, 44);//вывод в консоль через видеопамять в опр место
	printVideoMemory(str, 7, 53);
	ISRStatus();
}
void ISRStatus()//Функция вывода статуса регистра обслуживания прерывания(ISR) контроллера прерываний
{
	unsigned char t;
	char str[3];
	char str1[9];
	str[0] = '\0';//нуль терминатор
	str1[0] = '\0';
	outp(PORT_20, 0x0B); //Запись в порт 20h команды разрешения чтения регистра обслуживания прерываний(ISR) контроллера прерываний
	t = inp(PORT_20); //Считывание значения регистра обслуживания прерывания (ISR)
	b2h2(t, str); //перевод в 16ичную систему
	b2bin(t, str1);//перевод в 2ичную систему
	printVideoMemory("Регистр обслуживаемых прерываний ISR(0x20)", 8, 0);
	printVideoMemory(str1, 8, 44);//вывод в консоль через видеопамять в опр место
	printVideoMemory(str, 8, 53);
}
//прокрутка экрана
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
	CleanScreen(24, QUANTITY_OF_SYMBOLS);//очистка экрана
}
//Очистка экрана
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