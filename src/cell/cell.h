#ifndef CELL_H
#define CELL_H

#define MAX_CELL_SIZE 32

typedef enum 
{
	CELL_TYPE_NUMBER,
	CELL_TYPE_TEXT,
	CELL_TYPE_EXPR,
	CELL_TYPE_EMPTY,
	CELL_TYPE_UNKNOWN
} CELL_TYPE;

typedef enum
{
	CELL_ERROR_NO,
	CELL_ERROR_NUMBER_FORMAT,
	CELL_ERROR_EXPR_FORMAT,
	CELL_ERROR_CELL_REFER,
	CELL_ERROR_DIV_BY_ZERO,
	CELL_ERROR_NON_PRINTABLE_TEXT
} CELL_ERROR_TYPE;

class Cell 
{
	char outputString[MAX_CELL_SIZE];
public:
	char expression[MAX_CELL_SIZE];
	long int value; // calculated value or nonnegative number
	bool calculated;
	CELL_TYPE type;
	CELL_ERROR_TYPE error;
	const char* ToString();
};

#endif
