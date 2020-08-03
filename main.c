#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AX 0
#define BX 1
#define CX 2
#define DX 3

// EMULATOR
// Nathanael Key

struct Stack 
{
	int* stack;
	int i;
};

// ATTN: it might be more efficient to return a struct with file len, max line, etc...
// or just use realloc to wing it on the fly

// get the number of lines in file
int get_file_len (char* filename)
{
	FILE* file_pointer;
	file_pointer = fopen(filename, "r");
	if (file_pointer == NULL)
		return -1;

	// line count
	int i = 0;
	char c;
	while (c != EOF)
	{
		c = getc(file_pointer);
		if (c == '\n')
			++i;
	}

	fclose(file_pointer);

	return i;
}

// get the number of chars in file's longest line
int get_file_buffer_len (char* filename)
{
	FILE* file_pointer;
	file_pointer = fopen(filename, "r");

	if (file_pointer == NULL)
		return -1;

	// max line length
	int i = 0;
	int x = 0;
	char c;
	while (c != EOF)
	{
		c = getc(file_pointer);
		if (c == '\n')
		{
			if (x > i)
				i = x;
			x = 0;
		} else {
			x += 1;
		}
	}

	fclose(file_pointer);

	return i;
}

char** get_src (char* filename, int file_len, int buffer_len)
{
	FILE* file_pointer;
	file_pointer = fopen(filename, "r");
	if (file_pointer == NULL)
		exit(-1);

	char** src = malloc(file_len * sizeof(char*));
	for (int i = 0; i < file_len; ++i)
		src[i] = calloc(buffer_len, sizeof(char));

	// line number
	int i = 0;
	// char number
	int j = 0;
	for (char c = getc(file_pointer); c != EOF; c = getc(file_pointer))
	{
		if (c == '\n')
		{
			src[i][j + 1] = '\0';
			++i;
			j = 0;
		} else {
			src[i][j] = c;
			++j;
		}	
	}
	
	fclose(file_pointer);
	return src;
}

// convert operand (potential register) to data
int to_data (char* op, int* registers)
{
	if (!strcmp(op, "ax"))
		return registers[AX];
	else if (!strcmp(op, "bx"))
		return registers[BX];
	else if (!strcmp(op, "cx"))
		return registers[CX];
	else if (!strcmp(op, "dx"))
		return registers[DX];
	else
		return atoi(op);
}

// mov [data/reg] [reg]
void mov (char* op_a, char* op_b, int* registers)
{
	if (!strcmp(op_b, "ax"))
		registers[AX] = to_data(op_a, registers);
	else if (!strcmp(op_b, "bx"))
		registers[BX] = to_data(op_a, registers);
	else if (!strcmp(op_b, "cx"))
		registers[CX] = to_data(op_a, registers);
	else if (!strcmp(op_b, "dx"))
		registers[DX] = to_data(op_a, registers);
}

// push [data/reg]
void push (char* op, int* registers, struct Stack stack)
{
	stack.stack[stack.i] = to_data(op, registers);
	++stack.i;
}

// pop [reg]
void pop (char* op, int* registers, struct Stack stack)
{
	if (!strcmp(op, "ax"))
		registers[AX] = stack.stack[--stack.i];
	else if (!strcmp(op, "bx"))
		registers[BX] = stack.stack[--stack.i];
	else if (!strcmp(op, "cx"))
		registers[CX] = stack.stack[--stack.i];
	else if (!strcmp(op, "dx"))
		registers[DX] = stack.stack[--stack.i];
	else
		return;
	
	stack.stack[stack.i] = 0;
}

void run_src (char** src, int file_len, int* registers, struct Stack stack)
{
	char* tok;
	for (int i = 0; i < file_len; ++i)
	{
		tok = strtok(src[i], " ");
		printf("\t%s\n", tok);
		if (!strcmp(tok, "mov"))
		{
			char* data = strtok(NULL, " ");
			char* reg = strtok(NULL, " ");
			mov(data, reg, registers);
		}
		else if (!strcmp(tok, "push"))
			push(strtok(NULL, " "), registers, stack);
		else if (!strcmp(tok, "pop"))
			pop(strtok(NULL, " "), registers, stack);
		else
			continue;		
	}
}

void free_src (char** src, int file_len)
{
	for (int i = 0; i < file_len; ++i)
		free(src[i]);

	free(src);
}

void print_reg (int* registers)
{
	printf("ax: %d\n", registers[0]);
	printf("bx: %d\n", registers[1]);
	printf("cx: %d\n", registers[2]);
	printf("dx: %d\n", registers[3]);
}

void print_stack(struct Stack stack)
{
	printf("{");
	for (int i = 0; i <= stack.i; ++i)
		printf("%d, ", stack.stack[stack.i]);

	printf("}\n");
}

int main ()
{
	char* filename = "src";
	int file_len = get_file_len(filename);
	int buffer_len = get_file_buffer_len(filename);
	char** src = get_src("src", file_len, buffer_len);
	
	struct Stack stack;
	stack.stack = calloc(10, sizeof(int));
	stack.i = 0;

	int* registers = calloc(4, sizeof(int));
	
	run_src(src, file_len, registers, stack);

	print_reg(registers);	
	print_stack(stack);

	free(registers);
	free(stack.stack);
	free_src(src, file_len);
	
	return 0;
}
