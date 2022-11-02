#include <stdio.h>
#include "UI/UISDL.h"
#include "Compiler/Compiler.h"

int main()
{
	Compiler compiler;
//	UISDL ui(compiler);
	//ui.Start();
//	compiler.Compile(": TEST 100 . ; : VARIABLE CREATE 0 , ; TEST");
	compiler.Compile(": TEST 100 . ; TEST");
//	compiler.Compile(": TEST 100 . ; : VARIABLE CREATE 0 , ; VARIABLE ORANGES");
//	compiler.Compile("STATE . : STATE .");
//	compiler.Compile("CREATE HELLO 100 , HELLO @ HEX.");
//	compiler.Compile("CREATE HELLO 100 , HELLO @ HEX.");
	//compiler.Compile(": CONSTANT CREATE , DOES> @ ;");
// 76 CONSTANT TROMBONES
	compiler.Run();
	return 0;
}
