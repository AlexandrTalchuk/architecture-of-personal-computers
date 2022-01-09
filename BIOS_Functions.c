//924401-2 Тальчук А.П., Лабораторная работа №6 - Функции BIOS
#include <dos.h>
#include <conio.h>
#include <stdio.h>
#include <string.h>
#include <process.h>
#include <stdlib.h>

//Глобальные переменные для работы программы
volatile int close = 0;//Флаг выхода
union REGS rg;//Регистры BIOS для считывания конфигурации
char input[10];//Строка для ввода чисел
char videoModes[4][40] = { "Не используется", "40х25 c цветной платой", "80х25 c цветной платой","80х25 c монохромной платой" };


//Прототипы функций
void itodec(int i, char* c, int j); //Перевод двухзначного числа в десятичную систему счисления
void itodec2(int i, char* c, int j);//Перевод четрыёх значного числа в десятичную систему счисления
int bcdtoint(char* k);//Переводит число, заданное в двоично-десятичном коде, в тип int, 2бита
int bcdtoint2(char* k); //Переводит число, заданное в двоично-десятичном коде, в тип int, 4 бита
void inttobcd(int i, char* c);//Переводит число в в двоично-десятичный код
void Clear(int i, int u, int count);//Очищает count позиций окна консоли со строки i, столбца u
char* IntToStr(int n);// Функция преобразования числа в строку
void PrintPos(char* str, int line, int column, int attrib); // Функцияя вывода в определёную позицию
void Init(void);//Функция для вывода шапки
void Menu(void);//Функция вывода меню
void PressAnyKey(int i);//Функия ожидания нажатия любой клавиши для выхода в меню
void ConfigInfo(void);//Функция вывода конфигурации системы
void InChar(void);//Функция ввода одного символа без ожидания
void InCharWaiting(void);//Функция ввода одного символа с ожиданием
int InDigit(int j, char* limits, int left, int right);//Функция ввода числа в границах определённых значений
void InStr(void);//Функция ввода строки
void InStrDelay(int del);//Функция ввода строки с ожиданием
void Input(void);//Функция вывода меню для функций ввода
void ShowTime(void);//Функция вывода текущих даты и времени
void SetTime(void);//Функция установки времени
void SetDate(void);//Функция установки даты
void TimeOperations(void);//Функция вывода меню для работы с временем
char* B2Bin(unsigned char bt, char* str);//Функция перевода числа из десятичной системы в двоичную систему
char* B2H2(unsigned char bt, char* str);//Функция перевода числа из десятичной системы в шестнадцатеричную систему
unsigned char B4to1H(unsigned char bt);//Функция перевода четырёх младших бит в шестнадцатеричную систему




//Битовые поля слова конфигурации
typedef struct _HDWCFG
{
	unsigned HddPresent : 1; //0 - Дисковод
	unsigned NpuPresent : 1; //1 - Математический сопроцессор
	unsigned AmountOfRAM : 2; //2-3 - Размер ОЗУ
	unsigned VideoMode : 2; //4-5 - Активный видеорежим
	unsigned NumberOfFdd : 2; //6-7 - Число обнаруженных НГМД
	unsigned DmaPresent : 1; //8 - Наличие контроллера DMA
	unsigned NumberOfCom : 3; //9-11 - Число COM-портов
	unsigned GamePresent : 1; //12 - Игровой адаптер
	unsigned JrComPresent : 1; //13 - Резерв
	unsigned NumberOfLpt : 2; //14-15 - Число LPT-портов
} HDWCFG;

//Хранение битов конфигурации
HDWCFG HdwCfg;

int main(void)
{
	int key;
	Clear(0, 0, 25 * 80);//Очищаем экран
	Init();//Выводим надпись
	Menu();//Пункты меню
	while (!close)//Пока не закрыт
	{
		rg.h.ah = 0;//Помещаем в регистр AH значение 0
		int86(0x16, &rg, &rg);//Выполняем прерывание 16H
		key = rg.h.ah;//В регистре AH - скан-код нажатой клавиши
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

unsigned char B4to1H(unsigned char bt)//Функция перевода четырёх младших бит в шестнадцатеричную систему
{
	bt &= 0x0F;
	return (bt <= 9) ? '0' + bt : 'A' + (bt - 10);
}

char* B2H2(unsigned char bt, char* str)//Функция перевода числа из десятичной системы в шестнадцатеричную систему
{
	str[0] = B4to1H(bt >> 4);
	str[1] = B4to1H(bt);
	str[2] = '\0';
	return str;
}

char* B2Bin(unsigned char bt, char* str)//Функция перевода числа из десятичной системы в двоичную систему
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
//Перевод числа i в десятичную систему счисления в строку
void itodec(int i, char* c, int j)
{
	c[j] = '0' + ((i % 100) / 10);
	c[j + 1] = '0' + (i % 10);
}

//Перевод числа i в десятичную систему счисления в строку
void itodec2(int i, char* c, int j)
{
	c[j] = '0' + ((i % 10000) / 1000);
	c[j + 1] = '0' + ((i % 1000) / 100);
	c[j + 2] = '0' + ((i % 100) / 10);
	c[j + 3] = '0' + (i % 10);
}
//Переводит число k, заданное в двоично-десятичном коде в тип int
int bcdtoint(char* k)
{
	return (*k) % 16 + (*k) / 16 * 10;
}
//Переводит число k, заданное в двоично-десятичном коде в тип int
int bcdtoint2(char* k)
{
	return bcdtoint(k + 1) * 100 + bcdtoint(k);
}
//Переводит число i в двоично-десятичный код
void inttobcd(int i, char* c)
{
	*c = (i % 10) + ((i / 10) << 4);
}
//Очищает count позиций окна консоли со строки i, столбца u
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
// Функция преобразования числа в строку
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
//Выводит на экран текст str цвета attrib, начиная со строки line, столбца column
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
//Заголовок с названием работы и фио
void Init(void)
{
	PrintPos("924401-2 Тальчук А.П.", 0, 30, 11);
	PrintPos("Лабораторная работа №6 - Функции BIOS", 1, 22, 11);
}
//Вывод меню и очистка окна с выводом
void Menu(void)
{
	Clear(3, 0, 22 * 80);//Очистка рабочей области
	PrintPos("----------------------------------------МЕНЮ------------------------------------", 3, 0, 10);
	PrintPos("1) Вывести конфигурацию ПК (INT 11h, INT 12h)", 5, 0, 10);
	PrintPos("2) Ввод (INT 16h)", 6, 0, 10);
	PrintPos("3) Операции с датой и временем (INT 1Ah)", 7, 0, 10);
	PrintPos("Еsc) Выход", 8, 0, 10);
}
//Ожидание нажатия любой клавиши и возврат в меню
//Вывод сообщения в строку под номером i
void PressAnyKey(int i)
{
	PrintPos("Нажмите любую клавишу...", i, 0, 4);
	rg.h.ah = 0;//Помещаем в регистр AH значение 0
	int86(0x16, &rg, &rg);//Вызываем прерывание 16h
	Menu();
}
//Информация об установленном оборудовании
//и оперативной памяти компьютера
void ConfigInfo(void)
{
	unsigned char str[9];
	char c[2];
	unsigned uword = 0;
	Clear(3, 0, 22 * 80);
	PrintPos("(Прерывание Int 11h)", 3, 0, 11);
	//Вызов прерывания INT 11h для получения
	//слова конфигурации компьютера
	rg.h.ah = 0;
	int86(0x11, &rg, &rg);
	// Получение слова конфигурации и сохранение
	//его в структуре HdwCfg
	PrintPos("Слово Конфигурации",4,0,14);
	PrintPos(B2Bin(rg.h.ah,str),4,26,11);
	PrintPos(B2Bin(rg.h.al,str),4,34,11);
	PrintPos(B2H2(rg.h.ah,str),4,20,11);
	PrintPos(B2H2(rg.h.al, str), 4, 22, 11);
	uword = (unsigned int)rg.x.ax;
	memcpy(&HdwCfg, &uword, 2);
	//Вывод на экран конфигурации
	if (HdwCfg.HddPresent)
		PrintPos("Накопитель на магнитном жестком диске установлен", 5, 0, 1);
	else
		PrintPos("Накопитель на магнитном жестком диске не установлен", 5, 0, 14);
	if (HdwCfg.NpuPresent)
		PrintPos("Арифметический процессор установлен", 6, 0, 14);
	else
		PrintPos("Арифметический процессор не установлен", 6, 0, 14);
	c[0] = HdwCfg.AmountOfRAM + '0';
	PrintPos("Количество банков оперативной памяти на системной плате:", 7, 0, 14);
	PrintPos(c, 7, 59, 11); PrintPos("    ",7,60,0);
	PrintPos("Начальный режим видеоадаптера:", 8, 0, 14);
	PrintPos(videoModes[HdwCfg.VideoMode], 8, 32, 14);
	c[0] = (HdwCfg.NumberOfFdd + 1) + '0';
	PrintPos("Количество установленных накопителей на гибких магнитных дисках:", 9, 0,14);
	PrintPos(c, 9, 66, 11);PrintPos("    ",9,67,0);
	if (HdwCfg.DmaPresent)
		PrintPos("Контроллер прямого доступа к памяти DMA установлен", 10, 0, 14);
	else
		PrintPos("Контроллер прямого доступа к памяти DMA не установлен", 10, 0,14);
	c[0] = HdwCfg.NumberOfCom + '0';
	PrintPos("Количество установленных асинхронных последвательных портов:", 11, 0, 14);
	PrintPos(c, 11, 63, 11);PrintPos("    ",11,64,0);
	if (HdwCfg.GamePresent)
		PrintPos("Игровой порт установлен", 12, 0, 14);
        else
	PrintPos("Игровой порт нe установлен", 12, 0, 14);
	if (HdwCfg.JrComPresent)
		PrintPos("Последовательный порт установлен", 13, 0, 14);
	else
		PrintPos("Последовательный порт не установлен", 13, 0, 14);
	c[0] = HdwCfg.NumberOfLpt + '0';
	PrintPos("Количество установленных параллельных адаптеров:", 14, 0, 14);
	PrintPos(c, 14, 50, 11); PrintPos("    ",14,51,0);
	//Вызов прерывания INT 12h для определения
	//объема основной оперативной памяти компьютера
	rg.h.ah = 0x00;
	int86(0x12, &rg, &rg);
	PrintPos("(Прерывание Int 12h)", 16, 0, 11);
	PrintPos("Объем основной оперативной памяти компьютера:", 17, 0, 14);
	PrintPos(IntToStr(rg.x.ax),17,47,11 );
	PrintPos("KБайт", 17, 51, 11);
	PressAnyKey(18);
}
//Ввод одного символа без ожидания
void InChar(void)
{
	char c[2];
	Clear(3, 0, 22 * 80);
	//menu = 0;
	c[1] = 0;
	PrintPos("(Прерывание Int 16h)", 3, 0, 11);
	PrintPos("Введите символ:", 4, 0, 5);
	//Вызов прерывания INT 16h
	rg.h.ah = 0;
	int86(0x16, &rg, &rg);
	c[0] = (char)rg.h.al;
	PrintPos("Вы ввели символ ", 6, 0, 5);
	PrintPos(c, 6, 17, 15);
	PrintPos("Со скан-кодом ", 7, 0, 5);
	PrintPos(IntToStr(rg.h.ah), 7, 17, 11);
	PressAnyKey(9);
}
//Ввод одного символа с ожиданием нажатия enter
void InCharWaiting(void
)
{
	int done = 0, inp = 0;
	char c[2];
	int code;
	Clear(3, 0, 22 * 80);
	//menu = 0;
	c[1] = 0;
	PrintPos("(Прерывание Int 16h)", 3, 0, 11);
	PrintPos("Введите символ:", 4, 0, 5);
	//Вызов прерывания INT 16h
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
	PrintPos("Вы ввели символ ", 6, 0, 5);
	PrintPos(c, 6, 17, 11);
	PrintPos("Со скан-кодом ", 7, 0, 5);
	PrintPos(IntToStr(code), 7, 17, 11);
	PressAnyKey(9);
}
//Ввод числа в определённых границах
int InDigit(int j, char* limits, int left, int right)
{
	char input[80];
	int i = 0, k = 0, digit = 0;
	PrintPos("Введите число", j, 0, 5);
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
		for (k = 0; k < i; k++)//преобразование в int
		{
			digit = digit * 10 + (input[k] - '0');
		}
	} while (digit<left || digit> right);
	return digit;
}
//ввод строки
void InStr(void)
{
	char str[80];
	int i = 0;
	Clear(3, 0, 22 * 80);
	PrintPos("Введите строку:", 3, 0, 5);
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
	PrintPos("Введенная строка:", 6, 0, 5);
	PrintPos(str, 7, 0, 11);
	PressAnyKey(9);
}
//ввод строки с ожиданием del
void InStrDelay(int del)
{
	char str[80];
	int i = 0;
	Clear(3, 0, 22 * 80);
	PrintPos("Введите строку:", 3, 0, 5);
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
	PrintPos("Введенная строка:", 6, 0, 5);
	PrintPos(str, 7, 0, 11);
	PressAnyKey(9);
}
//менюс пунктами ввода
void Input(void)
{
	int k = 0, d = 0;
	Clear(3, 0, 22 * 80);
	PrintPos("1) Ввод символа без ожидания", 3, 0, 10);
	PrintPos("2) Ввод символа с ожиданием", 4, 0, 10);
	PrintPos("3) Ввод строки без ожидания", 5, 0, 10);
	PrintPos("4) Ввод строки с ожиданием", 6, 0, 10);
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
		PrintPos("Введите задержку", 3, 0, 14);
		d = InDigit(4, "от 1500 до 10000", 1500, 10000);
		InStrDelay(d);
		break;
	default:
		Menu();
		break;
	}
}
//просмотр текущих даты и времени
void ShowTime(void)
{
	char str[20];
	int year = 0, month = 0, day = 0, secs = 0, mins = 0, hrs = 0;
	rg.h.ah = 0x04;//Считывание даты с часов реального времени
	int86(0x1a, &rg, &rg);
	day = bcdtoint(&(rg.h.dl));
	month = bcdtoint(&(rg.h.dh));
	year = bcdtoint2(&(rg.h.cl));
	rg.h.ah = 0x02;
	int86(0x1a, &rg, &rg);
	hrs = bcdtoint(&(rg.h.ch));
	mins = bcdtoint(&(rg.h.cl));
	secs = bcdtoint(&(rg.h.dh));
	PrintPos("Текущая дата:", 5, 40, 14);
	itodec2(year, str, 0);
	str[4] = '.';
	itodec(month, str, 5);
	str[7] = '.';
	itodec(day, str, 8);
	str[10] = '\0';
	PrintPos(str, 5, 55, 15);
	PrintPos("Текущее время:", 6, 40, 14);
	itodec(hrs, str, 0);
	str[2] = ':';
	itodec(mins, str, 3);
	str[5] = ':';
	itodec(secs, str, 6);
	str[8] = '\0';
	PrintPos(str, 6, 56, 15);
}
//Установка времени
void SetTime(void)
{
	int i = 0, j = 0, k = 0;
	PrintPos("Часы:", 10, 0, 14);
	i = InDigit(11, "от 0 дo 23", 0, 23);
	PrintPos("Минуты:", 13, 0, 14);
	j = InDigit(14, "от 0 до 59", 0, 59);
	PrintPos("Секунды:", 16, 0, 14);
	k = InDigit(17, "от 0 до 59", 0, 59);
	rg.h.ah = 0x03;
	inttobcd(i, &(rg.h.ch));//часы
	inttobcd(j, &(rg.h.cl));//минуты
	inttobcd(k, &(rg.h.dh));//секунды
	int86(0x1a, &rg, &rg);
	ShowTime();
}
//Установка даты
void SetDate(void)
{
	int i = 0, j = 0, k = 0;
	PrintPos("Год:", 10, 0, 14);
	i = InDigit(11, "от 2001 до 2021", 2001, 2021);
	if (i < 2000)//определение столетия
	{
		i = i - 1900;
		inttobcd(19, &(rg.h.ch));
	}
	else
	{
		i = i - 2000;
		inttobcd(20, &(rg.h.ch));
	}
	PrintPos("Месяц:", 13, 0, 14);
	j = InDigit(14, "от 1 до 12", 0, 12);
	PrintPos("День:", 16, 0, 14);
	k = InDigit(17, " ", 0, 31);
	rg.h.ah = 0x05;
	inttobcd(k, &(rg.h.dl));//день
	inttobcd(j, &(rg.h.dh));//месяц
	inttobcd(i, &(rg.h.cl));//год
	int86(0x1a, &rg, &rg);
	ShowTime();
}
//вывод меню с опрациями над временем
void TimeOperations(void)
{
	int k = 0, j = 0;
	Clear(3, 0, 22 * 80);
	PrintPos("1) Просмотр текущего времени", 3, 0, 3);
	PrintPos("2) Установить текущее время", 4, 0, 3);
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
		PrintPos("1) Установить дату", 8, 0, 3);
		PrintPos("2) Установить время", 9, 0, 3);
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