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
			printf("F\n");
			fvalue = strtol(convert_input, NULL, 10);
			if (fvalue > 300)
			{
				strncpy(input_string, "Input value out of range\n", buffer_size);
				break;
			}
			dec_to_fib(fvalue, input_string, buffer_size, is_upper);
			break;
		case 'd':
			is_upper = 0;
		case 'D':
			printf("D\n");
			for (;*convert_input; convert_input++)
			{
				printf("character = %c \n", *convert_input);
				if (!isdigit(*convert_input))
				{	//Ensure all values are digits before operation.
					strncpy(input_string, "Invalid Input\n", buffer_size);
					return ret_value;
				} 
			}
			printf("value = [%s]\n", input_string);
			dec_to_hex(input_string, buffer_size, is_upper);
			break;
		case 'r':
			is_upper = 0;
		case 'R':
			printf("R\n");
			printf("[%s]\n", convert_input);
			if (strlen(convert_input + 1) >= 15)
			{
				strncpy(input_string, "Input value out of range\n", buffer_size);
				break;
			}
			rom_to_dec(convert_input, input_string, buffer_size, is_upper);
			break;
		default:
			strncpy(input_string, "Input Invalid\n", buffer_size);
			ret_value = -1;
	}
	return ret_value;
}

void dec_to_fib(long int number, char * output, int buffer_size, int is_upper)
{
	//Receive a number and return the fibonacci number in hexadecimal.
	char * fib_number;
	printf("input [%ld]\n", number);

	mpz_t a, b, fib;

	/* Initialize variable */
	mpz_init(a);
	mpz_init(b);
	mpz_init(fib);

	/* Set variable */
	mpz_set_ui (a, 1);
	mpz_set_ui (b, 1);
	mpz_set_ui (fib, 0);

	if (number == 0)	
	{
		snprintf(output, sizeof(char),"0x%x\n", '0');
	}
	else if (number == 1)
	{
		snprintf(output, sizeof(char), "0x%x\n", '1');
	}
	else if (number == 2)
	{
		snprintf(output, sizeof(char), "0x%x\n", '2');
	}
	else
	{
		mpz_init(fib);
		for(unsigned int count = 2; count < number; count++)
		{
	
			mpz_add(fib, a, b);
			mpz_set(a, b);
			mpz_set(b, fib);
		}
		fib_number = mpz_get_str(NULL, 16, fib);
		snprintf(output, buffer_size, "0x%s\n", fib_number);
		free(fib_number);
		if (is_upper == 1)
		{ 	// Convert to Upper if Input is all Capitol letters
			convert_to_upper(output);
		}
	}
	
	mpz_clear(a);
	mpz_clear(b);
	mpz_clear(fib);
	// For case conversion -> use flag as parameter? Defaults to lower case.
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
		printf("current output [%s]\n", output);
		sprintf(output, "0x%s\n", input_data);
		printf("after output [%s]\n", output);
		free(input_data);
		if (is_upper == 1)
		{ 	// Convert to Upper if Input is all Capitol letters
			convert_to_upper(output);
		}
	}
	mpz_clear(big_num);
	mpz_clear(input_num);
}

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
				printf("Error\n");
				snprintf(output, buffer_size, "Invalid Input: %c\n", *p);
				total = -1;
				return;
		}
	}
	if ( total > 4000 )
		snprintf(output, buffer_size, "Value is to Large: [%s]\n", numeral);
		
	else
	{
		snprintf(output, buffer_size, "0x%x\n", total);
		if (is_upper == 1)
		{ 	// Convert to Upper if Input is all Capitol letters
			convert_to_upper(output);
		}
	}
}
