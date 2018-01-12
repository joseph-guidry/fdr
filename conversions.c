#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void dec_to_fib(unsigned number, char * output);
void dec_to_hex(unsigned long long number, char * output);
void rom_to_dec(char *number, char * output);

int conversions(char *input_string)  // Signal Handler?????
{
	char *convert_input;

	// This function will be passed the string
	//strcpy(input_string, "No new input");
	unsigned fvalue = 0;
	unsigned long long dvalue = 0;

	printf(" dvalue size %lu \n", sizeof(dvalue));

		/* ADD checking!!! */

	convert_input = input_string;

	printf("Here [%s]\n", input_string);
	
	switch(input_string[0])
	{
		case 'F':
			printf("F\n");
			fvalue = strtol(convert_input + 1, NULL, 10);
			if (fvalue > 300)
			{
				// This could be a return error message to the file descriptor.
				perror("Input value out of range");
			}
			printf("value = [%u]\n", fvalue);
			dec_to_fib(fvalue, input_string);
			break;
		case 'D':
			printf("D\n");

			dvalue = strtoull (convert_input + 1, NULL, 10);
			if (dvalue > 1000000000000000000 ) // This number may need GMP?
			{
				// This could be a return error message to the file descriptor.
				perror("Input value out of range");
			}

			printf("value = [%llu]\n", dvalue);
			dec_to_hex(dvalue, input_string);
			break;
		case 'R':
			printf("R\n");
			convert_input = convert_input + 1;
			printf("[%s]\n", convert_input);
			if (strlen(convert_input + 1) > 4)
			{
				// This could be a return error message to the file descriptor.
				perror("Input value out of range");
			}
			rom_to_dec(convert_input, input_string);
			break;
		default:
			printf("Invalid Input\n");
	}

	printf("output[%s]\n", input_string);

	return 0;
}


void dec_to_fib(unsigned number, char * output /*case flag yes = 1? */)
{
	//Receive a number and return a hex string representation.
	//printf("input [%d]\n", number);

	int a = 1, b = 2 , c;

	for(unsigned int count = 2; count < number; count++)
	{
		c = a + b;
		a = b;
		b = c;
	}
	sprintf(output, "0x%x\n", c);
	//printf("Output [%s]\n", output);

	// For case conversion -> use flag as parameter? Defaults to lower case.
}

void dec_to_hex(unsigned long long number, char * output)
{
	printf("current output [%s]\n", output);
	sprintf(output, "0x%llx\n", number);
	printf("after output [%s]\n", output);
	// For case conversion -> use flag as parameter? Defaults to lower case.
}

void rom_to_dec(char *number, char * output)
{
	int total = 0;
	printf("output = [%s]\n", number);
	for (char *p = number; *p ; p++)
	{
		printf("[%c]\n", *p);
		switch(*p)
		{
			case 'I':
			case 'i':
					// Use lower case letters
					total += 1;
					break;
			case 'V':
			case 'v':
					total += 5;
					break;
			case 'X':
			case 'x':
					total += 10;
					break;
			case 'L':
			case 'l':
					total += 50;
					break;

			case 'C':
			case 'c':
				total += 100;
				break;
			case 'D':
			case 'd':
				total += 500;
				break;
			case 'M':
			case 'm':
				total += 1000;
				break;
			default:
				printf("Error\n");
				total = -1;
				return;
		}
	}

	sprintf(output,"0x%x\n", total);
}
