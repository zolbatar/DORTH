#include <stdio.h>
#include "UI/UISDL.h"
#include "Compiler/Compiler.h"

int main()
{
	Compiler compiler;
//	UISDL ui(compiler);
	//ui.Start();
//	compiler.Compile("STATE . : foo 100 . ; 200 DROP 100 250 + . 100 250 - . 123 .");
//	compiler.Compile("STATE . : STATE .");
	compiler.Compile("CREATE HELLO , HELLO @ .");
	//compiler.Compile(": CONSTANT CREATE , DOES> @ ;");
// 76 CONSTANT TROMBONES
	compiler.Run();
	return 0;
}
