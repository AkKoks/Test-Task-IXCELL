#include <iostream>
#include <iomanip>
#include <list>
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <climits>
#include <cstring>
#include <cstdio>
#include <cell/cell.h>
#include <table/table.h>

using namespace std;

Table::~Table()
{
	cellsToCalc.clear();

	if (!tableArr)
		return;

	for(int i = 0; i < hight; i++)
		delete[] tableArr[i];

	delete[] tableArr;
}

bool Table::Fill()
{
	char ch;

	hight = width = 0;
	/* first read size:
	 * <hight>\t<width>
	 */
	cin >> hight;
	if (hight <= 0 ) {
		cerr << "Input data error!" << endl;
		exit(-1);
	}
	/* expect \t */
	cin.get(ch);
	if (ch != '\t') {
		cerr << "Input data error!" << endl;
		exit(-1);
	}
	cin >> width;
	if (width <= 0 ) {
		cerr << "Input data error!" << endl;
		exit(-1);
	}
	/* expect \n */
	cin.get(ch);
	if (ch != '\n') {
		cerr << "Input data error!" << endl;
		exit(-1);
	}

	if (hight > MAX_TABLE_HIGHT ||
		width > MAX_TABLE_WIDTH) {
		cerr << "Max table sizes were exceed. Max hight " << MAX_TABLE_HIGHT << ", max width " << MAX_TABLE_WIDTH << "." << endl;
		exit(-1);
	}
	cout << endl;

	/* allocate mem and fill table from cin */
	tableArr = new Cell*[hight];
	if (!tableArr) {
		cerr << "Memory error!" << endl;
		exit(-1);
	}
	for (int i = 0; i < hight; i++) {
		tableArr[i] = new Cell[width];
		if (!tableArr[i]) {
			cerr << "Memory error!" << endl;
			exit(-1);
		}
		/* try to get cells data from input */
		for (int k = 0; k < width; k++) {
			if (k == width - 1)
				/* the last cell splitter is \n */
				cin.getline(tableArr[i][k].expression, MAX_CELL_SIZE, '\n');
			else
				cin.getline(tableArr[i][k].expression, MAX_CELL_SIZE, '\t');

			/* check stream status */
			if (cin.rdstate() & ios_base::failbit) {
				cerr << "Input data error!" << endl;
				exit(-1);
			}

			tableArr[i][k].calculated = false;
			tableArr[i][k].error = CELL_ERROR_NO;
			/* identify type of cell */
			ch = *tableArr[i][k].expression;
			if (isdigit(ch)) {
				bool is_number = true;
				tableArr[i][k].type = CELL_TYPE_NUMBER;
				/* check number */
				for (char *ptr = tableArr[i][k].expression; *ptr != '\0'; ptr++) {
					if (!isdigit(*ptr)) {
						is_number = false;
						break;
					}
				}
				if (is_number) {
					tableArr[i][k].value = strtol(tableArr[i][k].expression, NULL, 10);
					if (tableArr[i][k].value == LONG_MAX &&	errno == ERANGE) {
						tableArr[i][k].error = CELL_ERROR_NUMBER_FORMAT;
					}
					else {
						tableArr[i][k].calculated = true;
					}
				}
				else {
					tableArr[i][k].error = CELL_ERROR_NUMBER_FORMAT;
				}
			}
			else if (ch == '\'') {
				tableArr[i][k].type = CELL_TYPE_TEXT;
				for (char *p = tableArr[i][k].expression; *p != '\0'; p++) {
					if (!isprint(*p)) {
						tableArr[i][k].error = CELL_ERROR_NON_PRINTABLE_TEXT;
						break;
					}
				}
			}
			else if (ch == '=') {
				tableArr[i][k].type = CELL_TYPE_EXPR;
				/* add a cell to array of uncalc cells */
				cellsToCalc.push_back(&tableArr[i][k]);
			}
			else if (ch == '\0') {
				tableArr[i][k].type = CELL_TYPE_EMPTY;
			}
			else {
				tableArr[i][k].type = CELL_TYPE_UNKNOWN;
				tableArr[i][k].error = CELL_ERROR_EXPR_FORMAT;
			}
#ifdef DEBUG
			printf("[%d][%d]:%s - %d\n", i, k, tableArr[i][k].expression, tableArr[i][k].type);
#endif
		}
	}

	return true;
}

bool Table::CellRefToCell(char *cellRef, Cell **cellPtrPtr = NULL)
{
	/* if outgoing pointer is NULL - just check for correct refer */
	int x,y;
	/* get column offset */
	y = isupper(*cellRef) ? (*cellRef) - 'A' : (*cellRef) - 'a';
	/* get line offset */
	x = atoi(cellRef + 1) - 1;
#ifdef DEBUG
	printf("Cell refer is %s, line offset %d, column offset %d\n", cellRef, x, y);
#endif
	if (x < 0 || y < 0 || x >= hight || y >= width)
		return false;

	if (cellPtrPtr) {
		*cellPtrPtr = &tableArr[x][y];
#ifdef DEBUG
		printf("Found cell type is %d, cell exrp: %s, %s, cell value %li\n", 
			(*cellPtrPtr)->type, 
			(*cellPtrPtr)->expression, 
			(*cellPtrPtr)->calculated ? "calculated" : "not calculated",
			(*cellPtrPtr)->value);
#endif
	}


	return true;
}

bool Table::ExprToRpn(Cell *cell, list<Token>& out)
{
	char *endPtr, *ptr;
	Token tokenValue;
	list<Token> stack;
	bool ret = true;



	ptr = cell->expression + 1; // ignore '='
	if (*ptr == '\0') {
		cell->error = CELL_ERROR_EXPR_FORMAT;
		return false;
	}

	while (*ptr) {
		/* case number */
		if (isdigit(*ptr)) {
			tokenValue.type = TOKEN_TYPE_NUMBER;
			tokenValue.token.value = strtol(ptr, &endPtr, 10);
			if (tokenValue.token.value == LONG_MAX && errno == ERANGE) {
				/* incorrect number format */
				cell->error = CELL_ERROR_NUMBER_FORMAT;
				ret = false;
				break;
			}
			/* add to outgoing list */
			out.push_back(tokenValue);
			ptr = endPtr;
		}
		/* case cell reference */
		else if (isalpha(*ptr)) {
			if (!isdigit(*(ptr+1))) {
				/* incorrect cell refer format */
				cell->error = CELL_ERROR_EXPR_FORMAT;
				ret = false;
				break;
			}
			tokenValue.type = TOKEN_TYPE_CELL_REF;
			memset(tokenValue.token.cellRef, 0, CELL_REFER_LEN + 1);
			memcpy(tokenValue.token.cellRef, ptr, CELL_REFER_LEN);
			/* check cell refer correctness */
			if (!CellRefToCell(tokenValue.token.cellRef)) {
				cell->error = CELL_ERROR_CELL_REFER;
				ret = false;
				break;
			}
			/* add to outgoing list */
			out.push_back(tokenValue);
			ptr += CELL_REFER_LEN;
		}
		/* case operator */
		else if ((*ptr) == '+' || (*ptr) == '-' || (*ptr) == '*' || (*ptr) == '/') {			
			tokenValue.type = TOKEN_TYPE_OPERATOR;
			tokenValue.token.operation = (*ptr);
			if (stack.empty()) {
				stack.push_front(tokenValue);
			}
			else {
				Token top = stack.front();
				/* operators have same priority
				 * in this case we should pop
				 * operator from stack, add to
				 * outgoing list and push new
				 * operator into stack
				 */
				out.push_back(top);
				stack.pop_front();
				stack.push_front(tokenValue);
			}
			ptr++;
		}
		/* incorrect expression format */
		else {
			ret = false;
			cell->error = CELL_ERROR_EXPR_FORMAT;
			break;
		}
	}

	/* pop all operators from stack to outgoing list */
	while(!stack.empty()) {
		Token tok = stack.front();
		if (tok.type != TOKEN_TYPE_OPERATOR) {
			ret = false;
			cell->error = CELL_ERROR_EXPR_FORMAT;
			break;
		}
		out.push_back(tok);
		stack.pop_front();
	}

	return ret;
}

bool Table::RpnToValue(Cell *cell, list<Token>& rpn)
{
	list<Token> stack;
	Token curTok, args[2];
	long int x[2];
	Token resultTok;

	/* Compute RPN: if curTok is value - push into stack.
	 * If curTok is operator - pop values from 
	 * stack, compute and push result into stack.
	 */

	while (!rpn.empty()) {
		curTok = rpn.front();
		rpn.pop_front();
		if (curTok.type == TOKEN_TYPE_OPERATOR) {
			if (stack.size() < 2) {
				/* `+` and `-` can be unary operators */
				if ((curTok.token.operation != '-' &&
						curTok.token.operation != '+') ||
					stack.empty()) 
				{
					/* expression format error */
					cell->error = CELL_ERROR_EXPR_FORMAT;
					return false;
				}
				else {
					args[0].type = TOKEN_TYPE_NUMBER;
					args[0].token.value = 0;
					args[1] = stack.front();
					stack.pop_front();
				}
			}
			else {
				/* pop args from stack */
				args[1] = stack.front();
				stack.pop_front();
				args[0] = stack.front();
				stack.pop_front();
			}
			/* get values from args */
			for (int i = 0; i < 2; i++) {
				if (args[i].type == TOKEN_TYPE_NUMBER) {
					x[i] = args[i].token.value;
				}
				else if (args[i].type == TOKEN_TYPE_CELL_REF) {
					Cell *outCellPtr;
					if (!CellRefToCell(args[i].token.cellRef, &outCellPtr)) {
						cell->error = CELL_ERROR_CELL_REFER;
						return false;
					}
					/* if cell wasn't calculated - we cant compute all expression */
					if (!outCellPtr->calculated) {
						return false;
					}
					else {
						x[i] = outCellPtr->value;
					}
				}
			}
			/* do operation */
			switch (curTok.token.operation) {
				case '+':
					x[0]+=x[1];
					break;
				case '-':
					x[0]-=x[1];
					break;
				case '*':
					x[0]*=x[1];
					break;
				case '/':
					if (x[1] == 0) {
						/* div by zero */
						cell->error = CELL_ERROR_DIV_BY_ZERO;
						return false;
					}
					x[0]/=x[1];
					break;
				default:
					cerr << "Something went wrong" << endl;
					exit(-1);
					break;
			}
			resultTok.type = TOKEN_TYPE_NUMBER;
			resultTok.token.value = x[0];
			stack.push_front(resultTok);
		}
		else if (curTok.type == TOKEN_TYPE_NUMBER ||
			curTok.type == TOKEN_TYPE_CELL_REF) 
		{
			stack.push_front(curTok);
		}
	}

	/* in the end we must have only result in stack */
	if (stack.size() != 1) {
		cell->error = CELL_ERROR_EXPR_FORMAT;
		return false;
	}

	resultTok = stack.front();

	if (resultTok.type == TOKEN_TYPE_NUMBER) {
		cell->value = resultTok.token.value;
	}
	/* in case where we have only cell ref in expr */
	else if (resultTok.type == TOKEN_TYPE_CELL_REF) {
		Cell *outCellPtr;
		if (!CellRefToCell(resultTok.token.cellRef, &outCellPtr)) {
			cell->error = CELL_ERROR_CELL_REFER;
			return false;
		}
		if (outCellPtr->calculated) {
			cell->value = outCellPtr->value;

		}
		else {
			return false;
		}
	}

	cell->calculated = true;

	return true;
}

bool Table::ComputeCell(Cell *cell)
{
	list<Token> out;
	bool ret;

#ifdef DEBUG
	cout << "Calculate Cell: " << cell->expression << ", type is " << cell->type << endl;
#endif
	if (cell->type != CELL_TYPE_EXPR || cell->calculated)
		return true;

	if (cell->error != CELL_ERROR_NO)
		return false;

	/* parsing via RPN */
	if (!(ret = ExprToRpn(cell, out)))
		return ret;

#ifdef DEBUG
	cout << "RPN: ";
	for (list<Token>::iterator it = out.begin(); it != out.end(); advance(it, 1)) {
		if ((*it).type == TOKEN_TYPE_NUMBER)
			cout << (*it).token.value << " ";
		else if ((*it).type == TOKEN_TYPE_OPERATOR)
			cout << (*it).token.operation << " ";
		else if ((*it).type == TOKEN_TYPE_CELL_REF)
			cout << (*it).token.cellRef << " ";
	}
	cout << endl;
#endif

	if (!(ret = RpnToValue(cell, out)))
		return ret;

	return ret;
}

bool Table::Calc()
{
	bool op_succeed = true;
	bool res;
	list<Cell *>::iterator it = cellsToCalc.begin();
	list<Cell *>::iterator it_next = it;

	/* Run over all uncalc cells and try to calc it.
	 * If a cell successfuly calc-ed - delete it and 
	 * mark an operation succeed.
	 * Repeat the loop until the list has elements
	 * and we have succeed operations.
	 */
	while(!cellsToCalc.empty() && op_succeed) {
		it = cellsToCalc.begin();
		op_succeed = false;
		/* run over list elems */
		while (it != cellsToCalc.end()) {
			/* member next iterator */
			it_next = it, advance(it_next, 1);
			/* try calculate */
			res = ComputeCell(*it);
			if (res)
				cellsToCalc.erase(it);
			op_succeed |= res;
			it = it_next;
		}
	}

	if (!cellsToCalc.empty())
		return false;

	return true;
}

void Table::Print(int fieldWidth = MAX_CELL_SIZE)
{
	for (int i = 0; i < hight; i++) {
		for (int k = 0; k < width; k++) {
			cout << left << setw(fieldWidth) << tableArr[i][k].ToString();
		}
		cout << endl;
	}
}
