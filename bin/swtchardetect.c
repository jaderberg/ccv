//clang swtdetect.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -otestccv `cat /Users/jaderberg/Work/Utils/ccv_max/lib/.LN` -lm

#include "ccv.h"
#include <sys/time.h>
#include <ctype.h>


unsigned int get_current_time()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main(int argc, char** argv)
{
	
	ccv_enable_default_cache();
	ccv_dense_matrix_t* image = 0;
	ccv_read(argv[1], &image, CCV_IO_GRAY | CCV_IO_ANY_FILE);
	
	
	unsigned int elapsed_time = get_current_time();
	ccv_swt_param_t params = ccv_swt_default_params;
	if (argc == 3)
		params.scale_invariant = 1;

	ccv_array_t* words = ccv_swt_detect_chars(image, params);
	elapsed_time = get_current_time() - elapsed_time;
	if (words)
	{
		printf("total : %d in time %dms\n", words->rnum, elapsed_time);
		int i;
		for (i = 0; i < words->rnum; i++)
		{
			ccv_rect_t* rect = (ccv_rect_t*)ccv_array_get(words, i);
			printf("%d %d %d %d\n", rect->x, rect->y, rect->width, rect->height);
		}
		
		ccv_array_free(words);
	}
	ccv_matrix_free(image);
	
	ccv_drain_cache();
	return 0;
}

