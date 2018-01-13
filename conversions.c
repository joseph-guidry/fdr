#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <ctype.h>

void dec_to_fib(long int number, char * output, int buffer_size, int is_upper);
void dec_to_hex(char * output, int buffer_size, int is_upper);
void rom_to_dec(char *number, char * output, int buffer_size, int is_upper);

void convert_to_upper(char * string)
{
	char *p;
	for (p = string + 2; *p; p++)
	{
		if ( isalpha(*p))
		{
			*p = toupper(*p);
		}
	}
}

int conversions(char *input_string, int buffer_size)  
{
	char *convert_input;

	// This function will be passed the string
	int ret_value = 0, is_upper = 1;
	long fvalue;
	mpz_t value;
	mpz_init(value);

	convert_input = input_string + 1;

	switch(input_string[0])
	{
		case 'f':
			is_upper = 0;
		case 'F':
			// Ensure there is a number to use. 
			if (!isdigit(input_string[1]))
			{
				strncpy(input_string, "Invalid Input", buffer_size);
				break;
			}
			fvalue = strtol(convert_input, NULL, 10);
			if (fvalue > 300)
			{
				strncpy(input_string, "Input value out of range", buffer_size);
				break;
			}
			dec_to_fib(fvalue, input_string, buffer_size, is_upper);
			break;
		case 'd':
			is_upper = 0;
		case 'D':
			for (;*convert_input; convert_input++)
			{
				if (!isdigit(*convert_input))
				{	//Ensure all values are digits before operation.
					strncpy(input_string, "Invalid Input", buffer_size);
					return ret_value;
				} 
			}
			dec_to_hex(input_string, buffer_size, is_upper);
			break;
		case 'r':
			is_upper = 0;
		case 'R':
			if (strlen(convert_input + 1) >= 15)
			{
				strncpy(input_string, "Input value out of range", buffer_size);
				break;
			}
			rom_to_dec(convert_input, input_string, buffer_size, is_upper);
			break;
		default:
			strncpy(input_string, "Input Invalid", buffer_size);
			ret_value = -1;
	}

	return ret_value;
}

void dec_to_fib(long int number, char * output, int buffer_size, int is_upper)
{
	//Receive a number and return the fibonacci number in hexadecimal.
	char * fib_number;
	mpz_t fib;
	/* Initialize variable */
	mpz_init(fib);
	/* Set variable */
	mpz_fib_ui(fib, number);

	fib_number = mpz_get_str(NULL, 16, fib);
	snprintf(output, buffer_size, "0x%s", fib_number);
	free(fib_number);
	if (is_upper == 1)
	{ 	// Convert to Upper if Input is all Capitol letters
		convert_to_upper(output);
	}
	
	mpz_clear(fib);
}

void dec_to_hex(char * output, int buffer_size, int is_upper)
{
	mpz_t big_num;
	mpz_t input_num;
	mpz_init(big_num);
	mpz_init(input_num);

	mpz_ui_pow_ui(big_num, 10, 20);
	mpz_set_str(input_num, output + 1, 10);

	if (mpz_cmp(input_num, big_num) > 0 )
	{
		/* The number is bigger than 10^20; */
		strncpy(output, "Input value out of range\n", buffer_size);
	}
	else
	{
		/* The input Number is smaller than 10^20 */
		char * input_data = mpz_get_str(NULL, 16, input_num);
		sprintf(output, "0x%s", input_data);
		free(input_data);
		if (is_upper == 1)
		{ 	// Convert to Upper if Input is all Capitol letters
			convert_to_upper(output);
		}
	}
	mpz_clear(big_num);
	mpz_clear(input_num);
}

/* 
	The Roman Numerals are lower/upper version as found on wikipedia.
	This is without subtractive notation.
	For example: 
		IIII = 4 ( Without substractive notation )
		IV   = 4 ( With substractive notation )
*/
void rom_to_dec(char *numeral, char * output, int buffer_size, int is_upper)
{
	int total = 0;
	char *p;
	for (p = numeral; *p ; p++)
	{
		switch(*p)
		{
			case 'i':
			case 'I':
				total += 1;
				break;
			case 'v':
			case 'V':
				total += 5;
				break;
			case 'x':
			case 'X':
				total += 10;
				break;
			case 'l':
			case 'L':
				total += 50;
				break;
			case 'c':
			case 'C':
				total += 100;
				break;
			case 'd':
			case 'D':
				total += 500;
				break;
			case 'm':
			case 'M':
				total += 1000;
				break;
			default:
				snprintf(output, buffer_size, "Invalid Input: %c\n", *p);
				total = -1;
				return;
		}
	}
	if ( total > 4000 )
		snprintf(output, buffer_size, "Value is to Large: [%s]", numeral);
		
	else
	{
		snprintf(output, buffer_size, "0x%x", total);
		if (is_upper == 1)
		{ 	// Convert to Upper if Input is all Capitol letters
			convert_to_upper(output);
		}
	}
}
