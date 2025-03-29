#include "../internal.h"

using namespace rnd;

long int random()
{
	seed = (seed * 1664525UL) + 1013904223;
	return seed;
}
