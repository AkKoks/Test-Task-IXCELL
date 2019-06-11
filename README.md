# Test-Task-IXCELL

A little Excel
The program is a simple console application who can process table cells like Excel but with simple syntax. Every cell may contain:
- Nothing
- Nonnegative number
- The lines of the text beginning with a symbol '
- Expressions beginning whit a symbol =. May contain nonnegative numbers, cell references and simple mathematic operations. Brackets are not allowed, all operations have same priority

expression ::= '=' term {operation term}*
term ::= cell_reference | nonnegative_number
cell_reference ::= [A-Za-z][0-9] --
operation ::= '+' | '-' | '*' | '/'
text ::= '\'' {printable_character}

Input and output
The program gets description of the table from stdin, processes and prints result into stdout. The input data contains text table, cells split by tabulation. 
The first line contain couple of numbers - hight and width of table. Then there are lines with data of the table. The output contain only processed information or error messages.
Example of table data:
5	4
12	=C2	3	'Sample
=A1+B1C1/5	=A2B1	=B3-C3	'Spread
'Test	=4-3	5	'Sheet
=-3+5	=d3+3	=-A1+12
=z1+1	3441455437663566337563	=a11+a1	=2/D4

Building
The project uses g++ compiler. Exec build.sh script. The binary file will be in bin/release/.

Executing
You should give text data with table into stdin.
Example usage:
cat test.txt | bin/release/ixcell

