// Max Jaderberg 4/1/13

//clang swtwordcontours.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -o swt_word_contours `cat /Users/jaderberg/Work/Utils/ccv_max/lib/.LN` -lm 

//mex swt_word_contours_mex.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -lm -ljpeg -lpng -lz -L/usr/X11/lib -lm

#include "ccv.h"
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include "mex.h"

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
			);
		printf("example: %s -s 5 -H 500 myimage.jpg\n", argv[0]);
		return 0;
	}

	// process arguments
	char* image_file = "";
	int size;
	ccv_swt_param_t params = ccv_swt_default_params;
	char ch;
	while ((ch = getopt(argc, argv, "h:H:a:s:t:T:o:r:v:k:g:i:d:I:l:e:b:B:")) != EOF)
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
	
	ccv_dense_matrix_t* out_im = 0;
	out_im = ccv_dense_matrix_renew(out_im, image->rows, image->cols, CCV_C1 | CCV_ALL_DATA_TYPE, CCV_8U | CCV_C1, image->sig);

	ccv_array_t* textlines = ccv_swt_detect_words_contour(image, params);
	elapsed_time = get_current_time() - elapsed_time;
	if (textlines)
	{	
		printf("total : %d in time %dms\n", textlines->rnum, elapsed_time);
        int i, j, k;
		for (i = 0; i < textlines->rnum; i++)
		{
			ccv_textline_t* t = (ccv_textline_t*)ccv_array_get(textlines, i);
			printf("Textline %d (%d letters):\n", i, t->neighbors);
			for (j = 0; j < t->neighbors; j++) {
				// for each letter
				ccv_contour_t* cont = t->letters[j]->contour;
				printf("Contour %d (size=%d):\n", j, cont->size);
				for (k = 0; k < cont->size; k++) {
					// for each point in contour
					ccv_point_t* point = (ccv_point_t*)ccv_array_get(cont->set, k);
					printf("%d %d\n", point->x, point->y);
					(out_im)->data.u8[(point->y) * (out_im)->step + (point->x) * CCV_GET_CHANNEL(CCV_8U | CCV_C1) + (1)] = 255;
				}
				printf("Endcontour %d\n", j);
			}
			printf("Endtextline %d\n", i);
		}
		
		ccv_array_free(textlines);
	}
	ccv_matrix_free(image);

	ccv_write(out_im, strcat(image_file, "-words.png"), 0, CCV_IO_PNG_FILE, 0);
	ccv_matrix_free(out_im);
	
	ccv_drain_cache();
	return 0;
}

void mexFunction(int nlhs, mxArray *plhs[], /* Output variables */
					int nrhs, const mxArray *prhs[]) /* Input variable */
{
	mexPrintf("Hello, from swt detect!\n"); /* Do something interesting */
	return;
}

