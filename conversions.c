#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gmp.h>
#include <ctype.h>

void dec_to_fib(long int number, char * output, int buffer_size);
void dec_to_hex(char * output, int buffer_size);
void rom_to_dec(char *number, char * output, int buffer_size);


int conversions(char *input_string, int buffer_size)  
{
	char *convert_input;

	// This function will be passed the string
	int ret_value = 0;
	long fvalue;
	mpz_t value;
	mpz_init(value);

	convert_input = input_string;

	printf("input ");
	mpz_out_str(stdout, 10, value);
	printf("\n");

	
	switch(input_string[0])
	{
		case 'F':
			printf("F\n");
			fvalue = strtol(convert_input + 1, NULL, 10);
			if (fvalue > 300)
			{
				// This could be a return error message to the file descriptor.
				strncpy(input_string, "Input value out of range\n", buffer_size);
				break;
			}
			printf("value = [%ld]\n", fvalue);
			dec_to_fib(fvalue, input_string, buffer_size);
			break;
		case 'D':
			printf("D\n");

			for ( ;*convert_input; convert_input++)
			{
				printf(
				if (!isdigit(*convert_input))
				{
					strncpy(input_string, "Input value out of range\n", buffer_size);
					break;
				} 
			}

			printf("value = [%s]\n", input_string);
			dec_to_hex(input_string, buffer_size);
			break;
		case 'R':
			printf("R\n");
			convert_input = convert_input + 1;
			printf("[%s]\n", convert_input);
			if (strlen(convert_input + 1) >= 4)
			{
				// This could be a return error message to the file descriptor.
				strncpy(input_string, "Input value out of range\n", buffer_size);
				break;
			}
			rom_to_dec(convert_input, input_string, buffer_size);
			break;

		default:
			strncpy(input_string, "Input Invalid\n", buffer_size);
			ret_value = -1;
			
	}
	
	//printf("output[%s]\n", input_string);

	return ret_value;
}


void dec_to_fib(long int number, char * output, int buffer_size)
{
	//Receive a number and return a hex string representation.
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
	}
	//printf("Output [%s]\n", output);
	
	mpz_clear(a);
	mpz_clear(b);
	mpz_clear(fib);
	// For case conversion -> use flag as parameter? Defaults to lower case.
}

void dec_to_hex(char * output, int buffer_size)
{
	//char * input_data;
	printf("current output [%s]\n", output);
	//sprintf(output, "0x%llx\n", number);
	printf("after output [%s]\n", output);
	// For case conversion -> use flag as parameter? Defaults to lower case.
}

void rom_to_dec(char *number, char * output, int buffer_size)
{
	int total = 0;
	int is_upper = 1;
	printf("output = [%s]\n", number);
	for (char *p = number; *p ; p++)
	{
		printf("[%c]\n", *p);
		switch(*p)
		{
			case 'i':
				is_upper = 0;
			case 'I':
				total += 1;
				break;
			case 'v':
				is_upper = 0;
			case 'V':
				total += 5;
				break;
			case 'x':
				is_upper = 0;
			case 'X':
				total += 10;
				break;
			case 'l':
				is_upper = 0;
			case 'L':
				total += 50;
				break;
			case 'c':
				is_upper = 0;
			case 'C':
				total += 100;
				break;
			case 'd':
				is_upper = 0;
			case 'D':
				total += 500;
				break;
			case 'm':
				is_upper = 0;
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
	printf("lower case is present %c\n", is_upper == 0 ? 'T':'F');
	snprintf(output, buffer_size, "0x%x\n", total);
}
