#include <stdio.h>
#include "UI/UISDL.h"
#include "Compiler/Compiler.h"

int main()
{
	Compiler compiler;
//	UISDL ui(compiler);
	//ui.Start();
	compiler.Compile("200 DROP 100 250 + . 100 250 - . 123 .");
	compiler.Run();
	return 0;
}
