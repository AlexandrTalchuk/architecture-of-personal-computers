//924401-2 Тальчук А.П., Лабораторная работа № 5 Часы реального времени

#include <dos.h>
#include <ctype.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#pragma warn -sus

volatile int x = 45, y = 7; //Глобальные переменные столбцов и строк
volatile int hMax = 25;//Часы будильника
volatile int minMax = 60;//Минуты будильника
volatile int msCounter = 0; //Счётчик задержки
volatile int tik=0; //Счётчик вызова прерываний таймера
volatile char massive[20];  //Массив для ввода данных
volatile int nomer = 0;  //Количество элементов в массиве
volatile int enter = 0;  //Переменная для клавиши enter
volatile int gran = 0;  //Граница для backspace
volatile int flagAlarm = 0; //Флаг будильника

void printtopos(char *str, int line, int column, int color); //Функция вывода строки через видеобуфер в определённую позицию
void print(char *str);//Функция вывода строки через видеообуфер
void cleanscreen(int line, int counts, int column);//Функция очистки экрана
int BCDToInteger(int bcd);//Функция перевода в тип int
unsigned char IntToBCD(int value);//Функция перевода в двоично-десятичный код
char* toBinary(unsigned char bt, char* str);//Функция перевода числа из десятичной в двоичную систему
char Translate(unsigned char bt);// Функция получения символа шестандцатиричного числа для четырёх младших бит
char* toHex(unsigned char bt, char* str); //Функция перевода числа из десятичной в шестнадцатеричную систему

void inittimer(void);//Функция инициализации таймера
void inittimerstop(void);//Функция замены таймера на стандартный
void interrupt far(*oldInt70h)(void);//Указатель на обработчик прерываний часов реального времени
void interrupt far NewInt70Handler(void);//Функция обработки прерываний часов реального времени
void interrupt TimerHandler(void);//Функция обработки прерываний системного таймера
void interrupt(*oldTimerHandler)(void);//Указатель на обработчик прерываний системного таймера
void interrupt newInt9(void); //Функция обработки прерывания клавиатуры
void interrupt(*oldInt9)(void);//Указатель на обработчик прерывания клавиатуры

void cleanmassive(void);//Функция очистки массива
void GetTime(void); //Функция получения времени
void SetTime(void);//Функция установки времени
void WaitClockIsFree(void);//Функция провекрки на возможность обращения к часам
void correcttime(void);//Функция редактирования времени
void LockClockUpdate(void);//Функция запрета на обновление таймера
void UnlockClockUpdate(void);//Функция разрешения на обновление таймера
void CustomDelay(void); //Функция задержки времени
//void CreateAlarm(void); //Функция задания будильника
//void Sound(int); //Функция включения динамика
//void SetCount(int, int);//Функция для проигрывания мелодии
void Status(void);//Функция вывода значений регистров
void tostring(unsigned char , char *);//Функция перевода в строку
void ISRStatus(void); //Функция вывода статуса регистра обслуживания прерывания (ISR) контроллера прерываний

void tostring(unsigned char temp, char *str)//Функция перевода в строку
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

void Status()//Функция вывода значений регистров
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

	printtopos("Значения Регистров",5, 30, 15);

	ISRStatus();
}





void main()
{
	char str[] = { "924401-2 Тальчук А.П." };
	char str1[] = { "Лабораторная работа №5 Часы реального времени"};
	char str0[] = { "1 - Установить дату и время, 2 - Установить задержку  Esc - для выхода" };
	char c;
	clrscr();
	printtopos("Регистр обслуживания прерывания ISR(0x20)", 6,10, 11);
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

/*void SetCount(int freq, int time)//Функция проигрывания мелодии
{
	unsigned long int timerClock = 1193180;
	short value;				// значения делителя частоты
	value = timerClock / freq; 	// внутренняя частота таймера
	outp(0x42, (char)value);		// записываем значение младщего байта делителя частоты в порт динамика
	outp(0x42, (char)(value >> 8)); // записываем значение старшего байта делителя частоты в порт динамика
	delay(time);				// Пауза на время = time
} */

void WaitClockIsFree()//Функция провекрки на возможность обращения к часам
{
	do
	{
		outp(0x70, 0x0A);
	} while (inp(0x71) & 0x80);//Проверка 7 бита
}

void inittimer() //Функция инициализации таймера
{
	disable();//Запрет на прерывание таймера
	setvect(0x1c, TimerHandler);//Установка нового обработчика
	enable();//Разрешение на прерывание
}
void inittimerstop()//Функция замены на стандартный обработчик таймера
{
	disable();//Запрет на прерывание таймера
	setvect(0x1c, oldTimerHandler);   //Установка старого обработчика
	enable();//Разрешение на прерывание
}


void interrupt TimerHandler() //Функция обработки прерывания таймера
{
	char str6[9];
	disable();//Запрет на прерывание таймера
	tik++;             //Увеличиваем количество тиков
	if (!(tik % 18))   //если кратно 18,то прошла 1 секунда
	{

		GetTime();     //Вывод времени и даты
	}
	outp(0x20, 0x20);  //Сброс контроллера прерывания


	enable();//Разрешение на прерывание
}

void interrupt far NewInt70Handler(void)//Функция обработки прерываний Часов реального времени
{
	msCounter++;
	outp(0x70,0x0C);
	inp(0x71);

	outp(0x20, 0x20);
	outp(0xA0, 0x20);
}
void ISRStatus()//Функция вывода статуса регистра обслуживания прерывания (ISR) контроллера прерываний
{
	unsigned char value;
	char str[3];
	char str1[9];

	outp(0x20, 0x0B); //Запись в порт 20h команды разрешения чтения регистра обслуживания прерываний (ISR) контроллера прерываний
	value = inp(0x20); //Считывание значения регистра обслуживания прерывания (ISR)
	printtopos(toHex(value, str), 6, 60, 13); //Вывод шестнадцатеричной и двоичной форм слова состояния регистра ISR

	printtopos(toBinary(value, str1), 6, 51, 13);
}
char Translate(unsigned char bt)// Функция получения символа шестандцатиричного числа для четырёх младших бит
{
	return (bt < 10) ? '0' + bt : 'A' - 10 + bt;
}
char* toHex(unsigned char bt, char* str)//Функция перевода числа из десятичной в шестнадцатеричную систему
{
	str[2] = 0;
	str[1] = Translate(bt & 0x0F);//bt & 0x0F ? маска, получение символа шестнадцатеричного числа для 4 младших бит
	str[0] = Translate(bt >> 4);//Побитовый сдвиг вправо на 4 бита
	return str;
}
char* toBinary(unsigned char bt, char* str)//Функция перевода числа из десятичной в двоичную систему
{
	int i;
	for (i = 7; i >= 0; i--)
	{
		str[i] = (bt & 1) ? '1' : '0';//Если результат наложения маски не ноль, то записывается ?1?, иначе ?0?
		bt = bt >> 1;//Побитовый сдвиг вправо на 1 бит
	}
	str[8] = '\0';
	return str;
}
void GetTime()//Функция получения времени
{
	unsigned char value;
	char str6[9];
	int minutes, hours,second;
	int i = 0;
    //	int frequencyArray[][2] = {{600, 100}, {1, 50}, {600, 100}, {1, 50}, {600, 100}, {1 , 50}, {600, 100}, {1, 50}};
	printtopos("Текущее время: ", 20, 45, 11);
	WaitClockIsFree(); // Проверка, освободились ли часы
	outp(0x70, 0x04); // Считывание данных для часов
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
	outp(0x70, 0x02); // Считывание данных для минут
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
	outp(0x70, 0x00); // Считывание данных для секунд
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

	printtopos("Текущая дата:", 21, 45,11);
	WaitClockIsFree();
	outp(0x70, 0x07); // Считывание данных о дне
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
	outp(0x70, 0x08); // Считывание данных о месяце
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
	outp(0x70, 0x09); // Считывание данных о годе
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
	outp(0x70, 0x06); // Считывание данных о дне недели
	value = inp(0x71);
	cleanscreen(2, 9, 26);
	cleanscreen(21,11,69);
	switch (BCDToInteger(value))
	{
	case 1:printtopos("Воскресенье", 21, 69, 11);  break;
	case 2:printtopos("Понедельник", 21, 69, 11);  break;
	case 3:printtopos("Вторник", 21, 69, 11);  break;
	case 4:printtopos("Среда", 21, 69, 11);   break;
	case 5:printtopos("Четверг", 21, 69, 11);  break;
	case 6:printtopos("Пятница", 21, 69, 11);   break;
	case 7:printtopos("Суббота", 21, 69, 11);   break;
	default:printtopos("Неопределён день недели", 21, 26, 11);  break;
	}

      /*	if (minutes == minMax && hMax == hours && flagAlarm) {
		printtopos("Будильник сработал", 10, 0, 3);
		printtopos("Будильник сработал", 10, 20, 14);
		printtopos("Будильник сработал", 10, 40, 7);
		printtopos("Будильник сработал", 10, 60, 10);
		Sound(1);
		for (i = 0; i < 8; i++)
		{
			SetCount(frequencyArray[i][0], frequencyArray[i][1]); //Воспроизведение звука будильника
			if (i == 7) break;
		}
		flagAlarm = 0;
		Sound(0);
		cleanscreen(10,80,0);
	}*/

}

/*void Sound(int b) //Функция включения/отключения динамика
{
	unsigned long int dwResult = 0;
	// Читаем состояние порта
	dwResult = inport(0x61);
	if (b != 0)
	{
		dwResult |= 0x03;
		//Записываем значение в порт
		outport(0x61, dwResult);
	}
	else
	{
		dwResult &= 0xFC;
		outport(0x61, dwResult);
	}
} */

void correcttime()//Функция редактирования времени
{
	int hours = 0, weekday = 8, monthday = 32, month = 13, year = 21;
	int kolday[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	unsigned char value;

	WaitClockIsFree();
	outp(0x70, 0x07); // День
	value = inp(0x71);
	monthday = BCDToInteger(value);

	WaitClockIsFree();
	outp(0x70, 0x08); // Месяц
	value = inp(0x71);
	month = BCDToInteger(value);

	WaitClockIsFree();
	outp(0x70, 0x09); // Год
	value = inp(0x71);
	year = BCDToInteger(value);
	year = year + 2000;
	if (year % 4 != 0 || year % 100 == 0 && year % 400 != 0)
	{
	}
	else
		kolday[1] = 29;

	WaitClockIsFree();
	outp(0x70, 0x06); // День недели
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

	LockClockUpdate();//Блокирование прерываний часов реального времени

	outp(0x70, 0x04);
	outp(0x71, IntToBCD(hours));//Установка значения часа

	outp(0x70, 0x06);
	outp(0x71, IntToBCD(weekday));//Установка значения дня недели

	outp(0x70, 0x07);
	outp(0x71, IntToBCD(monthday));//Установка значения дня месяца

	outp(0x70, 0x08);
	outp(0x71, IntToBCD(month));//Установка значения месяца

	outp(0x70, 0x09);
	outp(0x71, IntToBCD(year));//Установка значения года

	UnlockClockUpdate();//Разрешение прерываний часов реального времени

}

void CustomDelay()//Функция установки задержки
{
	int delayPeriod = 0;
	unsigned char value;
	printtopos("Установите задержку в диапазоне 1500-10000 мс ", 10, 0, 15);
	cleanmassive();
	disable();
	oldInt9 = getvect(9);    //Прерывание клавиатуры
	setvect(9, newInt9);
	enable();
	x = 46;
	gran = x;
	y = 10;
	while (delayPeriod < 1500 || delayPeriod>10000)//Ввод задержки
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
	outp(0xA1, value & 0xFE);//Часы заняты

	outp(0x70, 0x0B);
	value = inp(0x0B);
	outp(0x70, 0x0B);
	outp(0x71, value & 0x40);//Вызов периодического прерывания
	printtopos("Выполняется задержка...",11,0,11);
	msCounter = 0;//Сброс счётчика прерываний Часов реального времени
	while (msCounter != delayPeriod);
	printtopos("Задержка завершена!", 12, 0,11);

	setvect(0x70, oldInt70h);
	WaitClockIsFree();
	setvect(0x1c, TimerHandler);
	correcttime();
	printtopos("Нажмите любую клавишу для продолжения...", 13, 20, 15);
	getch();
	cleanscreen(7, 720, 0);
	inittimerstop();
}

void interrupt newInt9()  //Функция обработки прерывания клавиатуры
{
	char str6[9];
	unsigned char value = 0;
	ISRStatus();
	value = inp(0x60);    //Получение значения из порта 60h
	switch (value) {
	case 0x02:  //Если 1
	{
		itoa(1, str6, 10);
		print(str6);
		massive[nomer] = '1';
		nomer++;
		break;
	}
	case 0x03:   //Если 2
	{
		itoa(2, str6, 10);
		print(str6);
		massive[nomer] = '2';
		nomer++;
		break;
	}
	case 0x04:   //Если 3
		{
			itoa(3, str6, 10);
			print(str6);
			massive[nomer] = '3';
			nomer++;
			break;
		}
	case 0x05:   //Если 4
		{
			itoa(4, str6, 10);
			print(str6);
			massive[nomer] = '4';
			nomer++;
			break;
		}
		case 0x06:   //Если 5
		{
			itoa(5, str6, 10);
			print(str6);
			massive[nomer] = '5';
			nomer++;
			break;
		}
		case 0x07:   //Если 6
		{
			itoa(6, str6, 10);
			print(str6);
			massive[nomer] = '6';
			nomer++;
			break;
		}
		case 0x08:   //Если 7
		{
			itoa(7, str6, 10);
			print(str6);
			massive[nomer] = '7';
			nomer++;
			break;
		}
		case 0x09:   //Если 8
		{
			itoa(8, str6, 10);
			print(str6);
			massive[nomer] = '8';
			nomer++;
			break;
		}
		case 0x0A:   //Если 9
		{
			itoa(9, str6, 10);
			print(str6);
			massive[nomer] = '9';
			nomer++;
			break;
		}
		case 0x0B:   //Если 0
		{
			itoa(0, str6, 10);
			print(str6);
			massive[nomer] = '0';
			nomer++;
			break;
		}
		case 0x0E:   //Если backspace
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
		case 0x1C:   //Если Enter
		{
			enter = 1;
			break;
		}
	}
	delay(100);
	outp(0x20, 0x20);   //Сброс контроллера прерывания
	ISRStatus();

}

void SetTime()//Функция установки времени
{
	int hours = 25, minutes = 60, seconds = 60, weekDay = 8, monthDay = 32, month = 13, year = 21;
	int kolday[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	char str[3];
	cleanmassive();
	disable();
	oldInt9 = getvect(9);    //Прерывание клавиатуры
	setvect(9, newInt9);
	enable();
	x = 21;
	gran = x;
	y = 10;
	printtopos("Введите часы(0-23): ", 10, 0, 15);

	while (hours < 0 || hours>23)//Установка часов
	{

		if (enter == 1)
		{
			hours = atoi(massive);
			enter = 0;
		}
	}

	cleanmassive();
	printtopos("Введите минуты(0-59): ", 11, 0, 15);
	x = 23;
	gran = x;
	y++;
	while (minutes < 0 || minutes>59)//Установка минут
	{

		if (enter == 1)
		{
			minutes = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	printtopos("Введите секунды(0-59): ", 12, 0, 15);
	x = 25;
	gran = x;
	y++;
	while (seconds < 0 || seconds>59)//Установка секунд
	{

		if (enter == 1)
		{
			seconds = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	printtopos("Введите год(2000-2021): ", 13, 0, 15);
	x = 25;
	gran = x;
	y++;
	while (year < 2000 || year>2021)//Установка года
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
	printtopos("Введите месяц(1-12): ", 14, 0, 15);
	x = 24;
	gran = x;
	y++;
	while (month < 1 || month>12)//Установка месяца
	{

		if (enter == 1)
		{
			month = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	itoa(kolday[month - 1], str, 10);
	printtopos("Введите день месяца 1- ", 15, 0, 15);
	printtopos(str, 15, 23,15);
	x = 29;
	gran = x;
	y++;
	while (monthDay < 1 || monthDay>kolday[month - 1])//Установка дня
	{

		if (enter == 1)
		{
			monthDay = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	printtopos("Введите номер дня недели(1-7): ", 16, 0, 15);
	x = 29;
	gran = x;
	y++;
	while (weekDay < 1 || weekDay>7)//Установка дня недели
	{
		if (enter == 1)
		{
			weekDay = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();

	setvect(9, oldInt9);

	LockClockUpdate();//Запрет на обновление

	outp(0x70, 0x04);//Установка часов
	outp(0x71, IntToBCD(hours));

	outp(0x70, 0x02);//Установка минут
	outp(0x71, IntToBCD(minutes));

	outp(0x70, 0x00);//Установка секунд
	outp(0x71, IntToBCD(seconds));

	outp(0x70, 0x06);//Установка дня в недели
	outp(0x71, IntToBCD(weekDay));

	outp(0x70, 0x07);//Установка дня
	outp(0x71, IntToBCD(monthDay));

	outp(0x70, 0x08);//Установка месяца
	outp(0x71, IntToBCD(month));

	outp(0x70, 0x09);//Установка года
	outp(0x71, IntToBCD(year));

	UnlockClockUpdate();//Разрешение обновления
	printtopos("Нажмите любую клавишу для продолжения...", 17, 0,15);
	getch();
	cleanscreen(10, 720, 0);
	inittimerstop();//Остановка таймера
}

/*void CreateAlarm()//Функция создания будильника
{
	hMax = 25, minMax = 60;
	cleanmassive();
	disable();
	oldInt9 = getvect(9);    //Прерывание клавиатуры
	setvect(9, newInt9);
	enable();
	x = 21;
	gran = x;
	y = 10;
	printtopos("Ввдеите часы(0-23): ", 10, 0, 15);

	while (hMax < 0 || hMax>23) //Установка часов будильника
	{
		if (enter == 1)
		{
			hMax = atoi(massive);
			enter = 0;
		}
	}

	cleanmassive();
	printtopos("Введите минуты(0-59): ", 11, 0, 15);
	x = 23;
	gran = x;
	y++;
	while (minMax < 0 || minMax>59)//Установка минут будильника
	{
		if (enter == 1)
		{
			minMax = atoi(massive);
			enter = 0;
		}
	}
	cleanmassive();
	setvect(9, oldInt9);
	printtopos("Нажмите любую клавишу для продолжения...", 12, 0,4);
	getch();
	cleanscreen(10, 800, 0);
	flagAlarm = 1;
} */

void printtopos(char *str, int line, int column, int color)//Функция вывода строки через видеобуфер в определённую позицию
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

void print(char *str)//Функция вывода строки через видеобуфер
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

void cleanscreen(int line, int counts, int column)//Функция очистки экрана
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

int BCDToInteger(int bcd)//Функция перевода в тип int
{
	return bcd % 16 + bcd / 16 * 10;//Перевод в тип int
}

unsigned char IntToBCD(int value)//Функция перевода в двоично-десятичный формат
{
	return (unsigned char)((value / 10) << 4) | (value % 10);//Перевод в тип BCD
}

void LockClockUpdate()//Функция запрета на обновления часов реального времени
{
	unsigned char value;
	WaitClockIsFree();//Проверка часов
	outp(0x70, 0x0B);
	value = inp(0x71);// установка 1 в 7 бите
	value |= 0x80;       //Запрет обновления часов
	outp(0x70, 0x0B);
	outp(0x71, value);
}

void UnlockClockUpdate()//Функция разрешения обновления часов реального времени
{
	unsigned char value;
	WaitClockIsFree();
	outp(0x70, 0x0B);
	value = inp(0x71); // установка 0 в 7 бите
	value -= 0x80;        //Разрешение на обновление часов
	outp(0x70, 0x0B);
	outp(0x71, value);
}

void cleanmassive()//Функция очистки массива
{
	int i;
	for (i = 0; i < 20; i++) {
		massive[i] = '\0';
	}
	nomer = 0;
}