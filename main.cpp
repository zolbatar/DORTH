#include <stdio.h>
#include "UI/UISDL.h"
#include "Compiler/Compiler.h"

Compiler compiler;

int main()
{
//	ui.Start();
	compiler.Compile("200 DROP 100 250 + . 100 250 - . 123 .");
	return 0;
}
