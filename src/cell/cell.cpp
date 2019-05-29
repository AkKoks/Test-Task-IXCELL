#include <cell/cell.h>
#include <cstdio>

const char* Cell::ToString()
{
	if (error == CELL_ERROR_NO) {
		switch (type) {
			case CELL_TYPE_NUMBER:
				snprintf(outputString, MAX_CELL_SIZE, "%li", value);
				break;
			case CELL_TYPE_TEXT:
				snprintf(outputString, MAX_CELL_SIZE, "%s", expression + 1);	
				break;
			case CELL_TYPE_EXPR:
				if (calculated)
					snprintf(outputString, MAX_CELL_SIZE, "%li", value);
				else
					snprintf(outputString, MAX_CELL_SIZE, "#NotCalculatedExpr");
				break;
			case CELL_TYPE_EMPTY:
				snprintf(outputString, MAX_CELL_SIZE, "%s", expression);
				break;
			case CELL_TYPE_UNKNOWN:
				snprintf(outputString, MAX_CELL_SIZE, "#UnknownCellType");
				break;
			default:
				break;
		}
	}
	else {
		switch (error) {
			case CELL_ERROR_NUMBER_FORMAT:
				snprintf(outputString, MAX_CELL_SIZE, "#WrongNumberFormat");
				break;
			case CELL_ERROR_EXPR_FORMAT:
				snprintf(outputString, MAX_CELL_SIZE, "#WrongExprFomat");
				break;
			case CELL_ERROR_CELL_REFER:
				snprintf(outputString, MAX_CELL_SIZE, "#WrongCellRefer");
				break;
			case CELL_ERROR_DIV_BY_ZERO:
				snprintf(outputString, MAX_CELL_SIZE, "#DivByZero");
				break;
			case CELL_ERROR_NON_PRINTABLE_TEXT:
				snprintf(outputString, MAX_CELL_SIZE, "#NonPrintableText");
				break;
			default:
				break;
		}
	}

	return outputString;

}
