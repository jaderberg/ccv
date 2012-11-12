// Max Jaderberg 12/11/12
// e.g. ./swt_char_contours img.jpg
//		./swt_char_contours -i img.jpg

//clang swtcharcontours.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -o swt_char_contours `cat /Users/jaderberg/Work/Utils/ccv_max/lib/.LN` -lm


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

	ccv_array_t* letters = ccv_swt_detect_chars_contour(image, params);
	elapsed_time = get_current_time() - elapsed_time;
	if (letters)
	{	
		printf("total : %d in time %dms\n", letters->rnum, elapsed_time);
		int i;
		for (i = 0; i < letters->rnum; i++)
		{
			// for each letter
			ccv_contour_t* cont = *(ccv_contour_t**)ccv_array_get(letters, i);
			printf("Contour %d (size=%d):\n", i, cont->size);
			for (int j = 0; j < cont->size; j++) {
				// for each point in contour
				ccv_point_t* point = (ccv_point_t*)ccv_array_get(cont->set, j);
				printf("%d %d\n", point->x, point->y);
			}
			printf("End contour %d\n", i);
		}
		
		ccv_array_free(letters);
	}
	ccv_matrix_free(image);
	
	ccv_drain_cache();
	return 0;
}

