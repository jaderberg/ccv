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
	if (argc==1) {
		printf("usage: %s [options] image_file\n", argv[0]);
		printf("available options [default]:\n%s", \
			"	-h    min height [8]\n"
			"	-H    max height [300]\n"
			"	-a    min area [38]\n"
			"	-s    canny size (must be an odd number) [3]\n"
			"	-t    canny low thresh [124]\n"
			"	-T    canny high thresh [204]\n"
			"	-o    letter occlude thresh (cannot occlude more than -o other letters) [3]\n"
			"	-r    maximum aspect ratio of a letter [8]\n"
			"	-v    max inner class variance for grouping letters [0.83]\n"
			"	-k    max thickness variance for grouping letters [1.5]\n"
			"	-g    max height variance for grouping letters [1.7]\n"
			"	-i    max intensity variance for grouping letters [31]\n"
			"	-d    max distance variance for grouping letters [2.9]\n"
			"	-I    max intersect variance for grouping letters [1.3]\n"
			"	-l    max letters threshold for grouping letters [3]\n"
			"	-e    max elongate variance for grouping letters [1.9]\n"
			"	-b    breakdown textlines to words [1]\n"
			"	-B    breakdown threshold [1.0]\n"
			"\t-S\tscale invariant [0]\n"
			);
		printf("example: %s -s 5 -H 500 myimage.jpg\n", argv[0]);
		return 0;
	}

	// process arguments
	char* image_file = "";
	int size;
	ccv_swt_param_t params = ccv_swt_default_params;
	char ch;
	while ((ch = getopt(argc, argv, "h:H:a:s:t:T:o:r:v:k:g:i:d:I:l:e:b:B:S:")) != EOF)
		switch(ch) {
			case 'h':
				params.min_height = atoi(optarg);
				continue;
			case 'H':
				params.max_height = atoi(optarg);
				continue;
			case 'a':
				params.min_area = atoi(optarg);
				continue;
			case 's':
				size = atoi(optarg);
				if (size % 2 == 0) {
					fprintf(stderr, "ERROR: canny size option (-s) must be an odd number (%d given)\n", size);
					return 1;
				}
				params.size = size;
				continue;
			case 't':
				params.low_thresh = atoi(optarg);
				continue;
			case 'T':
				params.high_thresh = atoi(optarg);
				continue;
			case 'o':
				params.letter_occlude_thresh = atoi(optarg);
				continue;
			case 'r':
				params.aspect_ratio = atof(optarg);
				continue;
			case 'v':
				params.std_ratio = atof(optarg);
				continue;
			case 'k':
				params.thickness_ratio = atof(optarg);
				continue;
			case 'g':
				params.height_ratio = atof(optarg);
				continue;
			case 'i':
				params.intensity_thresh = atoi(optarg);
				continue;
			case 'd':
				params.distance_ratio = atof(optarg);
				continue;
			case 'I':
				params.intersect_ratio = atof(optarg);
				continue;
			case 'l':
				params.letter_thresh = atoi(optarg);
				continue;
			case 'e':
				params.elongate_ratio = atof(optarg);
				continue;
			case 'b':
				params.breakdown = atoi(optarg);
				continue;
			case 'B':
				params.breakdown_ratio = atof(optarg);
				continue;
			case 'S':
				params.scale_invariant = atoi(optarg);
				continue;
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

