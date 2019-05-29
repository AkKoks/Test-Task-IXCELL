#include <table/table.h>
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[]) 
{

	Table *tbl = new Table();

	if (!tbl) {
		std::cerr<<"Memory error!"<<std::endl;
		exit(-1);
	}

	tbl->Fill();
	tbl->Calc();
	tbl->Print(25);

	delete tbl;

	return 0;
}
