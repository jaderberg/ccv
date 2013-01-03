// Max Jaderberg 2/1/13

//clang swtwordcontours.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -o swt_word_contours `cat /Users/jaderberg/Work/Utils/ccv_max/lib/.LN` -lm -lopencv_core -lopencv_highgui -I/opt/local/include -L/opt/local/lib

#include "ccv.h"
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>


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
	int min_height = 0;
	int min_area = 0;
	char ch;
	while ((ch = getopt(argc, argv, "a:h:i")) != EOF)
		switch(ch) {
			case 'i':
				scale_invariant = 1;
				printf("Scale invariant\n");
				break;
			case 'h':
				min_height = atoi(optarg);
				printf("Min height %d\n", min_height);
				continue;
			case 'a':
				min_area = atoi(optarg);
				printf("Min area %d\n", min_area);
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

	ccv_swt_param_t params = ccv_swt_default_params;
	if (scale_invariant)
		params.scale_invariant = 1;
	if (min_height)
		params.min_height = min_height;
	if (min_area)
		params.min_area = min_area;
	params.breakdown = 1;

	IplImage* in_im = cvLoadImage(image_file, 0);
	IplImage* out_im = cvCreateImage(cvGetSize(in_im), IPL_DEPTH_8U, 1);
	CvScalar pixel;

	ccv_array_t* textlines = ccv_swt_detect_words_contour(image, params);
	elapsed_time = get_current_time() - elapsed_time;
	if (textlines)
	{	
		printf("total : %d in time %dms\n", textlines->rnum, elapsed_time);
		for (int i = 0; i < textlines->rnum; i++)
		{
			ccv_textline_t* t = (ccv_textline_t*)ccv_array_get(textlines, i);
			printf("Textline %d (%d letters):\n", i, t->neighbors);
			for (int j = 0; j < t->neighbors; j++) {
				// for each letter
				ccv_contour_t* cont = t->letters[j]->contour;
				printf("Contour %d (size=%d):\n", j, cont->size);
				for (int j = 0; j < cont->size; j++) {
					// for each point in contour
					ccv_point_t* point = (ccv_point_t*)ccv_array_get(cont->set, j);
					printf("%d %d\n", point->x, point->y);
					pixel.val[0] = 255;
					pixel.val[1] = 255;
					pixel.val[2] = 255;
					cvSet2D(out_im, point->y, point->x, pixel); 
				}
				printf("Endcontour %d\n", j);
			}
			printf("Endtextline %d\n", i);
		}
		
		ccv_array_free(textlines);
	}
	ccv_matrix_free(image);

	cvSaveImage(strcat(image_file, "-words.png"), out_im, 0);
	cvReleaseImage(&in_im);
	cvReleaseImage(&out_im);
	
	ccv_drain_cache();
	return 0;
}

