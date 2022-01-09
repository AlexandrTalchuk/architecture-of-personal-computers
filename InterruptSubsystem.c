//924401-2
//Тальчук А.П.
//ЛР 4 Подсистема прерываний
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

//Указатель на функцию старого обработчика прерывания клавиатуры
void interrupt(*old_IRQ_09h) (void);
//Функция обработки прерывания клавиатуры
void interrupt new_IRQ_09h(void);

//Указатель на функцию старого обработчика системного таймера
void interrupt(*old_IRQ_08h) (void);
//Функция обработки прерывания системного таймера
void interrupt new_IRQ_08h(void);

//Указатель на функцию старого обработчика прерывания ошибки деления
void interrupt(*old_IRQ_00h) (void);
//Функция обработки прерывания ошибки деления
void interrupt new_IRQ_00h(void);

//Указатель на функцию старого обработчика прерывания часов реального времени
void interrupt far(*old_IRQ_70h)(void);
//Функция обработки прерывания часов реального времени
void interrupt far new_IRQ_70h(void);

//Функция чтения значений регистров контроллера прерываний
void ReadRegister(void);

//Функция вывода значений регистров контроллера прерываний
void PrintRegister(void);

//Функция очистки указанной строки консоли
void ClearRow(int);

//Функция очистки экрана
void ClearScreen(void);

//Функция получения символа шестнадцатеричного числа для 4 младших бит
char t2hex(unsigned char);

//Функция перевода числа из десятичной в шетснадцатеричную систему
char* b2hex(unsigned char, char*);

//Функция перевода числа из десятичной в двоичную систему
char* b2bin(unsigned char, char*);

//Функция вывода шапки в консоль
void header(void);

//Функция перевода числа из десятичного формата в строку
char* dec2char(short, char*);

//Функция переворачивания строки
char* reverse(char*, int);

//Функция деления на 0
void divide(void);

//вывод в консоль через видеопамять в опред.место, прямое обращение к ней
void printVideoMemory(char* str, int row, int column);

//вывод в консоль через видеопамять
void printVideoMemory1(char* str);

//Функция прокрутки экрана
void ScrollScreen(void);

//Инициализация переменных текущих строки и столбца и флагов программы
unsigned char	y = FIRST_STRING, x = 0, isQuit = 0, is00h = 0, is08h = 0, isReadRegister = 0, is70h = 0, is09h = 0;

//Инициализация переменных, необходимых для работы программы
unsigned char	master_IRR, slave_IRR, master_ISR, slave_ISR, master_IMR, slave_IMR, divisible = 1, divisor = 1, scanCode = 0;
short ticks = 0;
int count = 0;

//Указатель на начало видеобуфера
char far* start = (char far*) 0xb8000000;

//Указатели на символ атрибутов индикаторов прерываний
char far* indicator00h, far* indicator08h, far* indicator09h, far* indicator70h;

int main()
{
	short seconds = 0;
	unsigned char value;
	char buffer[9];

	//Определение положения атрибутов символов индикаторов и прерываний
	indicator00h = start + 10 * STRING_SIZE + 28 * 2 + 1;//68
	indicator08h = start + 9 * STRING_SIZE + 28 * 2 + 1;
	indicator70h = start + 9 * STRING_SIZE + 65 * 2 + 1;
	indicator09h = start + 10 * STRING_SIZE + 65 * 2 + 1;

	//Вывод шапки консоли
	header();
	//Макрокоманда делает прерывания невозможным на время установки новых обработчиков прерываний
	disable();

	//Получение вектора прерывания с номером 8h
	old_IRQ_08h = getvect(INT08H);
	//Назначение нового обработчика прерывания INT8h
	setvect(INT08H, new_IRQ_08h);

	//Получение вектора прерывания с номером 9h
	old_IRQ_09h = getvect(INT09H);
	//Назначение нового обработчика прерывания INT9h
	setvect(INT09H, new_IRQ_09h);

	//Получение вектора прерывания с номером  0h
	old_IRQ_00h = getvect(INT00H);
	//Назначение нового обработчика прерывания INT0h
	setvect(INT00H, new_IRQ_00h);

	//Получение вектора прерывания с номером 70h
	old_IRQ_70h = getvect(INT70H);
	//Назначение нового обработчика прерывания INT70h
	setvect(INT70H, new_IRQ_70h);

	//Макрокоманда возобновляет возможности появления прерываний
	enable();

	outp(0x70, 0x0B);//Выбор необходимого регистра состояния часов реального времени
	value = inp(0x71); //Получение данных из этого регистра
	outp(0x70, 0x0B); //Повторный выбор регистра состояния
	outp(0x71, value | 0x40); //Запись нового значения в данный регистр. 40h == 0100 0000b.
	//Так как шестой бит регистра состояния 2 часов реального времени отвезает за разрешение вызова периодического прерывания, то его нужно установить в 1

	while (!isQuit)
	{
		if (isReadRegister)
		{
			PrintRegister();//Вывод значений регистров контроллера прерываний
			isReadRegister = 0;//Сброс флага regReadFlag
		}
		if (is00h)
		{
			printVideoMemory("Внимание! Произошло деление на 0!", 11, 20);
			delay(1000);//Приостановка работы программы на 1000мс
			printVideoMemory("                                 ", 11, 20);
			*(indicator00h) = BLACK;//"Выключение" индикатора прерывания
			is00h = 0;//Сброс флага
		}
		if (is09h)
		{
			int maxCount;
			maxCount = (25 - FIRST_STRING) * QUANTITY_OF_SYMBOLS;//25 всего, первые 10 не в счет
			if (count >= maxCount)
			{
				//от 8 до 23 строчки раб.область
				y = LAST_STRING;//всего 25 строк, от 0 до 24
				x = 0;
				ScrollScreen();
				count = maxCount - QUANTITY_OF_SYMBOLS;
				//-80 из-за добавившейся строчки
			}
			switch (scanCode)
			{
			case 0x01://Скан-код клавиши ESC
				isQuit = 1; //Установка флага завершения программы
				break;
			case 0x0B://Скан-код клавиши 0
				divide();//Вызов деления на 0
				break;
			case 0x2E://Скан-код клавиши c
				header();//Вывод шаблона консоли
				break;
			}
			delay(200);
			*(indicator09h) = BLACK;//"Выключение" индикатора прерывания
			is09h--;//Уменьшение флага
		}
		if (is08h)
		{
			//ticks увеличивается при каждом вызове прерывания системного таймера и берётся значение по модулю 18
			//Как только оно станет равным нулю, нужно будет вывести новую секунду
			if (ticks == 0)
			{
				printVideoMemory(dec2char(++seconds, buffer), 9, 12);
				seconds %= 999; //Ограничение выводимого количества секунд
			}
			delay(20);
			*(indicator08h) = BLACK;//"Выключение" индикатора прерывания
			is08h = 0;//Сброс флага
		}
		if (is70h)
		{
			*(indicator70h) = BLACK;//"Выключение" индикатора прерывания
			is70h = 0;//Сброс флага
		}
	}

	disable();//Макрокоманда делает прерывания невозможным на время возвращения старых обработчиков прерываний

	setvect(INT00H, old_IRQ_00h);//Возвращение исходного обработчика прерывания INT0h
	setvect(INT08H, old_IRQ_08h);//Возвращение исходного обработчика прерывания INT8h
	setvect(INT09H, old_IRQ_09h);//Возвращение исходного обработчика прерывания INT9h
	setvect(INT70H, old_IRQ_70h);//Возвращение исходного обработчика прерывания INT70h

	outp(0x70, 0x0B);//Восстановление значения регистра состояния 2 часов реального времени
	value = inp(0x71);//Восстановление часов реального времени
	outp(0x70, 0x0B);//Выбор необходимого регистра состояния часов реального времени
	outp(0x71, value & 0xBF);

	enable();//Макрокоманда возобновления возможности появления прерываний
	ClearScreen();//Очистка экрана
	return 0;
}



//Функция обработки прерывания системного таймера
void interrupt new_IRQ_08h(void)
{
	//Установка голубого фона символа атрибутов индикатора прерываний
	*(indicator08h) = CYAN;
	ReadRegister();
	ticks++;
	ticks %= 18;
	isReadRegister = 1;
	is08h = 1;
	old_IRQ_08h();//Вызов старого обработчика прерываний
}

//Функция обработки прерывания ошибки деления
void interrupt new_IRQ_00h(void)
{
	//Установка голубого фона символа атрибутов индикатора прерываний
	*(indicator00h) = CYAN;
	//Восстановление значений делимого и делителя
	divisible = 1;
	divisor = 1;
	isReadRegister = 1;
	is00h = 1;
}

//Функция обработки прерывания клавиатуры
void interrupt new_IRQ_09h(void)
{
	unsigned char str[3];
	is09h++; //Увеличение флага kbFlag
	isReadRegister = 1; //Установка флага regReadFlag
	ReadRegister();//Чтение значений регистров контроллера прерываний
	*(indicator09h) = CYAN;//Установка голубого фона символа атрибутов индикатора прерываний
	scanCode = inp(0x60); //Получение значения порта 60h. В данном случае - байт скан-кода нажатой клавиши
	printVideoMemory1(b2hex(scanCode, str));
	old_IRQ_09h();//Вызов старого обработчика прерываний клавиатуры
}

//Функция обработки прерывания часов реального времени
void interrupt far new_IRQ_70h(void)
{
	*(indicator70h) = CYAN;//Установка голубого фона символа атрибутов индикатора прерываний
	is70h = 1;//Установка флага rtcFlag
	ReadRegister();//Чтение значений регистров контроллера прерываний
	outp(0x70, 0x0C);//Выбор регистра состояния 3 часов реального времени
	inp(0x71); //Чтение выбранного регистра с обнулением 7 бита, отвечающего за прерывание IRQ8
	old_IRQ_70h();//Вызов старого обработчика прерываний часов реального времени
}

//Функция вывода шапки консоли
void header(void)
{
	ClearScreen();//Очистка экрана
	printVideoMemory("|  924401-2 Тальчук А.П.    | Горячие клавиши:        0 - Деление на 0        |", 0, 0); //Вывод строки на консоль
	printVideoMemory("|ЛР №4 Подсистема прерываний| C - Очистка консоли     ESC - Выход из программы|", 1, 0);
	printVideoMemory("|___________________________|_________________________________________________|", 2, 0);
	printVideoMemory("|     Регистр    |           MASTER          |              SLAVE             |", 3, 0);
	printVideoMemory("|       IRR      | 0x21                      | 0xA0                           |", 4, 0);
	printVideoMemory("|       ISR      | 0x20                      | 0x0A                           |", 5, 0);
	printVideoMemory("|       IMR      | 0x20                      | 0xA1                           |", 6, 0);
	printVideoMemory("|       0x60     |                           |                                |", 7, 0);
	printVideoMemory("|________________|___________________________|________________________________|", 8, 0);
	printVideoMemory("|    Таймер       (INT08h):   |  Прерывание RTC        (INT70h):              |", 9, 0);
	printVideoMemory("|    Деление на 0 (INT00h):   |  Прерывание клавиатуры (INT09h):              |", 10, 0);
}

//Функция вывода значений регистров контроллера прерываний
void PrintRegister()
{
	char buffer[9];
	//Вывод шестнадцатеричного значения числа в консоль, в определённую позицию через видеопамять
	printVideoMemory(b2hex(master_IRR, buffer), 4, 25);
	//Вывод двоичного значения числа в консоль, в определённую позицию через видеопамять
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

//Функция очистки указанной строки
void ClearRow(int row)
{
	short i = 0;
	char far* v = start + row * STRING_SIZE;//Указатель на символ, с которого будет начата очистка
	for (; i < 80; i++)
	{
		*v = ' ';//Заполнение ячейки символом пробел
		*(v + 1) = 0;//Очищение данных атрибутов
		v += 2;
	}
}

//Функция очистки экрана
void ClearScreen()
{
	unsigned char i;
	for (i = 0; i <= 24; i++)//Вызов функции очистки указанной строки для всех строк консоли
		ClearRow(i);
	y = FIRST_STRING; //Сброс значения текущей строки
	x = 0; //Сброс значения текущего столбца
}

//Функция деления на 0
void divide(void)
{
	divisor = 0;//Присвоение делителю 0
	divisible /= divisor;//Деление на 0
}

//Функция перевода числа из десятичного формата в строку
char* dec2char(short value, char* buffer)
{
	int i = 0;
	while (value != 0)
	{
		buffer[i++] = value % 10 + 48;//Получение символа из десятичного числа
		value /= 10;
	}
	buffer[i] = '\0';
	return reverse(buffer, i);//Переворот строки
}

//Функция перевотора строки
char* reverse(char* str, int length)
{
	int left = 0, right = length - 1;
	char temp;

	for (; left < right; left++, right--)
	{
		temp = str[left]; //Перестановка значений str[left] и str[right]
		str[left] = str[right];
		str[right] = temp;
	}
	return str;
}

//Функция чтения значений регистров контроллера прерываний
void ReadRegister()
{
	outp(MASTER, IRR);//Выбор нужного регистра контроллера прерываний
	master_IRR = inp(MASTER);//Запись значения регистра
	outp(SLAVE, IRR);
	slave_IRR = inp(SLAVE);
	outp(MASTER, ISR);
	master_ISR = inp(MASTER);
	outp(SLAVE, ISR);
	slave_ISR = inp(SLAVE);
	master_IMR = inp(IMR_MASTER);//Чтение маски ведущего контроллера прерываний
	slave_IMR = inp(IMR_SLAVE); //Чтение маски ведомого контроллера прерываний
}

//Функция получения символа шестнадцатеричного числа для 4 младших бит
char t2hex(unsigned char value)
{
	value &= 0x0F;//Маска 
	return (value < 10) ? '0' + value : 'A' - 10 + value;
}

//Функция перевода числа из десятичной в шестнадцатеричную систему
char* b2hex(unsigned char value, char* str)
{
	str[0] = t2hex(value >> 4);
	str[1] = t2hex(value);//Получение символа шестнадцатеричного числа для 4 младших бит
	str[2] = '\0';
	return str;
}

//Функция перевода числа из десятичной в двоичную систему
char* b2bin(unsigned char value, char* str)
{
	unsigned char i = 0;
	for (; i < 8; i++)
	{
		str[i] = value & 0x80 ? '1' : '0';
		value = value << 1; //Побитовый сдвиг влево на 1 бит
	}
	str[8] = '\0';
	return str;
}

// вывод в консоль через видеопамять в определенное место, прямое обращение к видеопамяти
void printVideoMemory(char* str, int row, int column)
{
	int i;
	char far* v;
	v = start + row * STRING_SIZE + column * SYMBOL_SIZE;
	//символ - 2 байта, в строке их 80, т.е. размер одной строки 160

	for (i = 0; str[i] != '\0'; i++)
	{
		*v = str[i];
		*(v + 1) = 0x07; //Установка атрибута символа (7h = 00000111b).
		//Первые три бита, установлены в 1, отвечают за красный, зелёный и синий компоненты цвета соотвественно
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
		*(v + 1) = 0x07; //Установка атрибута символа (7h = 00000111b).
		//Первые три бита, установлены в 1, отвечают за красный, зелёный и синий компоненты цвета соотвественно
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
		*(v + 1) = 0;//Очищение данных атрибутов
		v += 2;//v++
		count++;
	}
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
	ClearRow(24);//очистка экрана
}