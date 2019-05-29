#ifndef TABLE_H
#define TABLE_H

#include <cell/cell.h>
#include <list>

#define MAX_TABLE_HIGHT 10 // [0-9]
#define MAX_TABLE_WIDTH 27 // [a-zA-Z]
#define CELL_REFER_LEN 2 // [a-zA-Z][0-9]


typedef enum 
{
	TOKEN_TYPE_OPERATOR,
	TOKEN_TYPE_NUMBER,
	TOKEN_TYPE_CELL_REF
} TOKEN_TYPE;

typedef struct
{
	TOKEN_TYPE type;
	union {
		long int value;
		char cellRef[CELL_REFER_LEN + 1];
		char operation;
	} token;
} Token;

class Table 
{
	Cell **tableArr;
	int hight;
	int width;
	std::list<Cell *> cellsToCalc; // list of uncalculated cells
	bool CellRefToCell(char *cellRef, Cell **cell); // convert cell reference to cell pointer
	bool ExprToRpn(Cell *cell, std::list<Token>& out); // convert expr to Reverse Polish notation
	bool RpnToValue(Cell *cell, std::list<Token>& rpn); // compute RPN to value if it's possible
	bool ComputeCell(Cell *cell);
public:
	~Table();
	bool Fill();
	bool Calc();
	void Print(int fieldWidth);
};

#endif
