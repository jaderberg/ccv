// Max Jaderberg 10/11/12
// e.g. ./swt_word_detect img.jpg
//		./swt_word_detect -i img.jpg

//clang swtdetect.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -o swt_word_detect `cat /Users/jaderberg/Work/Utils/ccv_max/lib/.LN` -lm

#include "ccv.h"
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>


unsigned int get_current_time()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int main(int argc, char** argv)
{
	// process arguments
	char* image_file = "";
	int scale_invariant = 0;
	char ch;
	while ((ch = getopt(argc, argv, "i")) != EOF)
		switch(ch) {
			case 'i':
				scale_invariant = 1;
				break;
		}
	argc -= optind;
	argv += optind;
	image_file = argv[0];

	ccv_enable_default_cache();
	ccv_dense_matrix_t* image = 0;
	ccv_read(image_file, &image, CCV_IO_GRAY | CCV_IO_ANY_FILE);

	if (image==0) {
		fprintf(stderr, "ERROR: image could not be read\n");
		return 1;
	}
	
	unsigned int elapsed_time = get_current_time();

	ccv_swt_param_t params = ccv_swt_default_params;
	if (scale_invariant)
		params.scale_invariant = 1;

	ccv_array_t* words = ccv_swt_detect_words(image, params);
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

