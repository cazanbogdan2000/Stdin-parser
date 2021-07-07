#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>

// functie primita in cadrul skelet-ului
static int write_stdout(const char *token, int length)
{
	int rc;
	int bytes_written = 0;

	do {
		rc = write(1, token + bytes_written, length - bytes_written);
		if (rc < 0)
			return rc;

		bytes_written += rc;
	} while (bytes_written < length);

	return bytes_written;
}

//functie care, asa cum si spune, transforma tipul integer intr-un
//string; functia este recursiva, pornind de la final si afisand,
//pe rand, cifrele numarului
void integer_to_string(int num, int* number_of_letters)
{
	if(num == 0)
	{
		return;
	}
	//eliminam cate o cifra din numar, urmand sa o afisam la final
	int digit = num % 10;
	if(digit < 0)
		digit *= (-1);
	char* digit_to_print = malloc(sizeof(char));
	digit_to_print[0] = '0' + digit;

	integer_to_string(num / 10, number_of_letters);
	//afisam la finalul recursvitatii !!
	write_stdout(digit_to_print, 1);
	free(digit_to_print);
	(*number_of_letters)++;
}

//asemanator functiei de mai sus, numai ca de data aceasta
//avem un numar natural(unsigned)
void unsigned_to_string(unsigned num, int* number_of_letters)
{
	if(num == 0)
	{
		return;
	}

	int digit = num % 10;
	char* digit_to_print = malloc(sizeof(char));
	digit_to_print[0] = '0' + digit;

	unsigned_to_string(num / 10, number_of_letters);
	write_stdout(digit_to_print, 1);
	free(digit_to_print);
	(*number_of_letters)++;
}

//functie recursiva care formeaza un numar din baza 10 in baza 16, scriindu-l
//direct sub forma de sir de caractere
void integer_to_hexa(unsigned num, int* number_of_letters)
{
	if(num == 0)
	{
		return;
	}
	int digit = num % 16;
	char* digit_to_print = malloc(sizeof(char));

//facem codificarea cifrelor din baza 10 in format de baza 16
	if(digit < 10)
	{
		digit_to_print[0] = '0' + digit;
	}
	else if(digit == 10)
		digit_to_print[0] = 'a';
	else if(digit == 11)
		digit_to_print[0] = 'b';
	else if(digit == 12)
		digit_to_print[0] = 'c';
	else if(digit == 13)
		digit_to_print[0] = 'd';
	else if(digit == 14)
		digit_to_print[0] = 'e';
	else
		digit_to_print[0] = 'f';

	//folsindu-ne de recursivitate, obtinem numarul prin 
	//impartirea la 16
	integer_to_hexa(num / 16, number_of_letters);
	write_stdout(digit_to_print, 1);
	free(digit_to_print);
	(*number_of_letters)++;
	
}

//functie care nu reprezinta nimic altceva in afara unui switch
//pe care se intra in momentul gasirii caracterului '%'
void percent_switch(char cursor, va_list* args, int* nr_letters)
{
	switch (cursor)
	{
		//integer type
		case 'd':
		{
			int integer_argument = va_arg(*args, int);
			if(integer_argument < 0)
			{
				write_stdout("-", 1);
				(*nr_letters)++;
				//num *= (-1);
			}

			integer_to_string(integer_argument, nr_letters);
			break;
		}
		//unsingned integer type
		case 'u':
		{
			unsigned unsigned_argument = va_arg(*args, unsigned);
			unsigned_to_string(unsigned_argument, nr_letters);
			break;
		}
		//hexadecimal type
		case 'x':
		{
			unsigned hexa_argument = va_arg(*args, unsigned);
			integer_to_hexa(hexa_argument, nr_letters);
			break;
		}
		//single character type
		case 'c':
		{
			char char_argument = va_arg(*args, int);
			write_stdout(&char_argument, 1);
			(*nr_letters)++;
			break;
		}
		//string
		case 's':
		{
			char* string_argument = va_arg(*args, char*);
			write_stdout(string_argument, strlen(string_argument));
			(*nr_letters) += strlen(string_argument);
			break;
		}
		//daca gaseste %%, atunci afiseaza doar unul singur
		//similar lui \% in acest caz
		case '%':
		{
			char percent = '%';
			write_stdout(&percent, 1);
			(*nr_letters)++;
			break;
		}
		default:
			break;
	}
}

//functia pe care trebuia efectiv sa o implementam, functie care
//exact aceeasi functionalitate (stiu ca suna repetitiv, dar nu 
//gasesc pe moment un cuvant mai bun :)) )
//Un lucru important de precizat este faptul ca prelucram format-ul
//pe litere, parsandu-l caracter cu caracter
int iocla_printf(const char *format, ...)
{
	//folosim biblioteca stdarg.h pentru prelucrarea parametrilor
	int number_of_letters = 0;
	va_list args;
	va_start(args, format);
	int position = 0;
	char cursor = format[position];
	while(cursor != '\0')
	{
		//daca e un mesaj simplu de afisat
		if(cursor != '\\' && cursor != '%')
		{
			write_stdout(&cursor, 1);
			number_of_letters++;
		}
		else
		{
			//cazul in care se gaseste caracterul '%'
			if(cursor == '%')
			{
				position++;
				cursor = format[position];
				percent_switch(cursor, &args, &number_of_letters);
			}
			//cazul in care se excapeaza backslash
			else if(cursor == '\\')
			{
				write_stdout(&cursor, 1);
				number_of_letters++;
			}

		}
		position++;
		cursor = format[position];
	}
	va_end(args);
	return number_of_letters;
}