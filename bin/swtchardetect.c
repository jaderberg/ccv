// Max Jaderberg 10/11/12
// e.g. ./swt_char_detect img.jpg
//		./swt_char_detect -i img.jpg

//clang swtchardetect.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -o swt_char_detect `cat /Users/jaderberg/Work/Utils/ccv_max/lib/.LN` -lm



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
	int interval = 0;
	int scale_invariant = 0;
	int size = 0;
	double low_thresh = 0.0;
	double high_thresh = 0.0;
	int max_height = 0;
	int min_height = 0;
	char ch;
	while ((ch = getopt(argc, argv, "n:is:l:h:M:m:")) != EOF)
		switch(ch) {
			case 'n':
				interval = atoi(optarg);
				break;
			case 'i':
				scale_invariant = 1;
				break;
			case 's':
				size = atoi(optarg);
				break;
			case 'l':
				low_thresh = atof(optarg);
				break;
			case 'h':
				high_thresh = atof(optarg);
				break;
			case 'M':
				max_height = atoi(optarg);
				break;
			case 'm':
				min_height = atoi(optarg);
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

	// Params that are relevant:
	// interval (n=1), scale_invariant (i=0), size (s=3), low_thresh (l=124), high_thresh (h=204), max_height (M=300), min_height (m=8)
	if (interval)
		params.interval = interval;
	if (scale_invariant)
		params.scale_invariant = 1;
	if (size)
		params.size = size;
	if (low_thresh)
		params.low_thresh = low_thresh;
	if (high_thresh)
		params.high_thresh = high_thresh;
	if (max_height)
		params.max_height = max_height;
	if (min_height)
		params.min_height = min_height;

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

