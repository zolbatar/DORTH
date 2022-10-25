#include "../beebScreen/beebScreen.h"
#include "../rich/keyboard.h"
#include "../beebScreen/beebScreen.h"

char get_key()
{
	unsigned c;
	_swi(OS_ReadC, _OUT(1), &c);
	return c;
}
