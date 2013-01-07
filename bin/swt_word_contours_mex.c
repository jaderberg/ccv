// Max Jaderberg 4/1/13

//clang swtwordcontours.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -o swt_word_contours `cat /Users/jaderberg/Work/Utils/ccv_max/lib/.LN` -lm 

//mex swt_word_contours_mex.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -ljpeg -lpng -lz -L/usr/X11/lib -lm -L/opt/local/lib -I/opt/local/include

#include "ccv.h"
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include "mex.h"


void mexFunction(int nlhs, mxArray *plhs[], /* Output variables */
					int nrhs, const mxArray *prhs[]) /* Input variable */
{
        
    
    // Check input variable 1 is a 2D image
    if (mxIsUint8(prhs[0]) && mxGetNumberOfDimensions(prhs[0]) == 2) {
        
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
 
//     printf("%d firstval\n", *(unsigned char *)ccv_get_dense_matrix_cell(image, 0, 0, 0));
//     printf("%d firstval\n", (image)->data.u8[(1) * (image)->step + (1) * CCV_GET_CHANNEL(CCV_8U | CCV_C1) + (0)]);
    
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
	
	ccv_array_t* textlines = ccv_swt_detect_words_contour(image, params);
    
    mxArray* X = mxCreateStructMatrix(1,1,0,0);

    if (textlines)
	{	
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
	} else {
        // just add an empty field
        mxAddField(X, "words");
    }
	ccv_matrix_free(image);
	
	ccv_drain_cache();
       
    nlhs = 1;
    plhs[0] = X;
    
	return;
}

