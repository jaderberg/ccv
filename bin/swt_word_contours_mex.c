// Max Jaderberg 4/1/13

//clang swtwordcontours.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -o swt_word_contours `cat /Users/jaderberg/Work/Utils/ccv_max/lib/.LN` -lm 

//mex swt_word_contours_mex.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -ljpeg -lpng -lz -L/usr/X11/lib -lm -L/opt/local/lib -I/opt/local/include

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



int main(unsigned char* pr, int argc, char** argv)
{
	// process arguments
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

    printf("%d size\n", params.size);
    return 0;
    
// 	ccv_enable_default_cache();
// 	ccv_dense_matrix_t* image = 0;
// 	ccv_read(image_file, &image, CCV_IO_GRAY | CCV_IO_ANY_FILE);
// 
// 	if (image==0) {
// 		fprintf(stderr, "ERROR: image could not be read\n");
// 		return 1;
// 	}
// 	
// 	unsigned int elapsed_time = get_current_time();
// 	
// 	ccv_dense_matrix_t* out_im = 0;
// 	out_im = ccv_dense_matrix_renew(out_im, image->rows, image->cols, CCV_C1 | CCV_ALL_DATA_TYPE, CCV_8U | CCV_C1, image->sig);
// 
// 	ccv_array_t* textlines = ccv_swt_detect_words_contour(image, params);
// 	elapsed_time = get_current_time() - elapsed_time;
// 	if (textlines)
// 	{	
// 		printf("total : %d in time %dms\n", textlines->rnum, elapsed_time);
//         int i, j, k;
// 		for (i = 0; i < textlines->rnum; i++)
// 		{
// 			ccv_textline_t* t = (ccv_textline_t*)ccv_array_get(textlines, i);
// 			printf("Textline %d (%d letters):\n", i, t->neighbors);
// 			for (j = 0; j < t->neighbors; j++) {
// 				// for each letter
// 				ccv_contour_t* cont = t->letters[j]->contour;
// 				printf("Contour %d (size=%d):\n", j, cont->size);
// 				for (k = 0; k < cont->size; k++) {
// 					// for each point in contour
// 					ccv_point_t* point = (ccv_point_t*)ccv_array_get(cont->set, k);
// 					printf("%d %d\n", point->x, point->y);
// 					(out_im)->data.u8[(point->y) * (out_im)->step + (point->x) * CCV_GET_CHANNEL(CCV_8U | CCV_C1) + (1)] = 255;
// 				}
// 				printf("Endcontour %d\n", j);
// 			}
// 			printf("Endtextline %d\n", i);
// 		}
// 		
// 		ccv_array_free(textlines);
// 	}
// 	ccv_matrix_free(image);
// 
// 	ccv_write(out_im, strcat(image_file, "-words.png"), 0, CCV_IO_PNG_FILE, 0);
// 	ccv_matrix_free(out_im);
// 	
// 	ccv_drain_cache();
// 	return 0;
}

void mexFunction(int nlhs, mxArray *plhs[], /* Output variables */
					int nrhs, const mxArray *prhs[]) /* Input variable */
{
	mexPrintf("Hello, from swt detect!\n"); /* Do something interesting */
    
    printf("%d input variables\n", nrhs);
    
    
    // Check input variable 1 is a 2D image
    if (mxIsUint8(prhs[0]) && mxGetNumberOfDimensions(prhs[0]) == 2) {
        printf("Input 1 is a grayscale image\n");
    } else {
        mexErrMsgTxt("Error: input 1 is not a grayscale image");
    }
    
    // Get image data
    unsigned char *pr; 
    mwSize total_num_of_elements, index; 
    pr = (unsigned char *)mxGetData(prhs[0]);
    total_num_of_elements = mxGetNumberOfElements(prhs[0]);
    int rows, cols;
    rows = mxGetM(prhs[0]);
    cols = mxGetN(prhs[0]);
    
    // Image data to ccv matrix
    ccv_dense_matrix_t* image = ccv_dense_matrix_new(rows, cols, CCV_8U | CCV_C1, 0, 0);
    int i, j;
    for (i = 0; i < rows; i++)
        for (j=0; j < cols; j++) 
            (image)->data.u8[(i) * (image)->step + (j) * CCV_GET_CHANNEL(CCV_8U | CCV_C1) + (0)] = pr[i + rows*j];
 
    printf("%d firstval\n", *(unsigned char *)ccv_get_dense_matrix_cell(image, 0, 0, 0));
    printf("%d firstval\n", (image)->data.u8[(1) * (image)->step + (1) * CCV_GET_CHANNEL(CCV_8U | CCV_C1) + (0)]);
    
    // sort out option pairs
    int n;
    int size;
	ccv_swt_param_t params = ccv_swt_default_params;
	char ch;
    char buff1[8];
    char optarg[16];
    for (n = 1; n < nrhs; n+=2) {
        mxGetString(prhs[n], buff1, 8);
        ch = buff1[0];
        mxGetString(prhs[n+1], optarg, 16);
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
					mexErrMsgTxt("ERROR: canny size option (-s) must be an odd number");
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
    }
        
    ccv_enable_default_cache();
	
	unsigned int elapsed_time = get_current_time();
	
	ccv_dense_matrix_t* out_im = ccv_dense_matrix_new(rows, cols, CCV_8U | CCV_C1, 0, 0);
    for (i = 0; i < rows; i++)
        for (j=0; j < cols; j++) 
            (out_im)->data.u8[(i) * (out_im)->step + (j) * CCV_GET_CHANNEL(CCV_8U | CCV_C1) + (0)] = 0;
 

	ccv_array_t* textlines = ccv_swt_detect_words_contour(image, params);
	elapsed_time = get_current_time() - elapsed_time;
	if (textlines)
	{	
		printf("total : %d in time %dms\n", textlines->rnum, elapsed_time);
        int i, j, k;
		for (i = 0; i < textlines->rnum; i++)
		{
			ccv_textline_t* t = (ccv_textline_t*)ccv_array_get(textlines, i);
// 			printf("Textline %d (%d letters):\n", i, t->neighbors);
			for (j = 0; j < t->neighbors; j++) {
				// for each letter
				ccv_contour_t* cont = t->letters[j]->contour;
// 				printf("Contour %d (size=%d):\n", j, cont->size);
				for (k = 0; k < cont->size; k++) {
					// for each point in contour
					ccv_point_t* point = (ccv_point_t*)ccv_array_get(cont->set, k);
// 					printf("%d %d\n", point->x, point->y);
					(out_im)->data.u8[(point->y) * (out_im)->step + (point->x) * CCV_GET_CHANNEL(CCV_8U | CCV_C1) + (0)] = 255;
				}
// 				printf("Endcontour %d\n", j);
			}
// 			printf("Endtextline %d\n", i);
		}
		
		ccv_array_free(textlines);
	}
	ccv_matrix_free(image);

	ccv_write(out_im, "words.png", 0, CCV_IO_PNG_FILE, 0);
	ccv_matrix_free(out_im);
	
	ccv_drain_cache();
    
    
	return;
}

