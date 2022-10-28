#include <stdio.h>
#include "prompt.h"
#ifdef PITUBE
#include "pitube/fb.h"
bool usePi = false;
#else
#include "rich/UI/UISDL.h"
#endif
#include "compiler/compiler.h"

int main()
{
#ifdef PITUBE
	fb_init();
/*	printf("Welcome to PiTubeDirect Dorth! Created by @thezolbatar.\n");
	fb_string("Hello!",100,100);
	fb_string_square("PiTubeDirect!", 200,200);
	beebScreen_Flip();*/
#endif
	compiler_init();
//	prompt();
	compile("200 DROP 100 250 + . 100 250 - . 123 .");
	return 0;
}
