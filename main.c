#include "fractal.h"

int
main(int argc, char** argv)
{
		int ret;
		struct sstate state;

		if((ret = init(&state)))
				return ret;

		if((ret = mainloop(&state)))
				return ret;

		return finish(&state);
}
