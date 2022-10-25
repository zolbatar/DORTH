#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include "console.h"
#include "../../beebScreen/beebScreen.h"

uint8_t* console;
unsigned size;

void init_console(unsigned width, unsigned height)
{
	size = width * height;
	console = malloc(size);
	for (unsigned i = 0; i < size; i++)
		console[i] = rand();
}

void console_draw()
{
}

void console_printf()
{

}