#include "../prompt.h"
#include "UI/UISDL.h"
#include "../compiler/compiler.h"

UISDL ui;

void prompt()
{
	compiler_init();
	ui.Start();
}