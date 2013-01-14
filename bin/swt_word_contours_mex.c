// Max Jaderberg 4/1/13

//clang swtwordcontours.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -o swt_word_contours `cat /Users/jaderberg/Work/Utils/ccv_max/lib/.LN` -lm 

//mex swt_word_contours_mex.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -ljpeg -lpng -lz -L/usr/X11/lib -lm -L/opt/local/lib -I/opt/local/include

//for cluster:
//mex swt_word_contours_mex.c -L"/users/max/Work/Utils/ccv_max/lib" -I"/users/max/Work/Utils/ccv_max/lib" -lccv -ljpeg -lpng -lz -L/usr/X11/lib -I/usr/include -L/usr/lib64 -lm -f /users/max/Work/Utils/ccv_max/bin/gnu89mexopts.sh

#include "ccv.h"
#include <ctype.h>
#include <unistd.h>
#include "mex.h"

int main(int argc, char** argv) {
    return 0;
}

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
    int cont_rows = 10000;
    int cont_cols = 500;
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
            case 'M':
                cont_rows = atoi(optarg);
                continue;
            case 'N':
                cont_cols = atoi(optarg);
		}
    }
        

    ccv_enable_default_cache();
	
    // preallocate memory for cont data
    mxArray* Cont_cell = mxCreateCellMatrix(cont_cols, 1);
    for (n = 0; n < cont_cols; n++) {
        mxArray* cont_mat = mxCreateDoubleMatrix(0, 0, mxREAL);
        mxSetM(cont_mat, cont_rows);
        mxSetN(cont_mat, 1);
        mxSetData(cont_mat, mxMalloc(sizeof(double)*cont_rows*1));
        mxSetCell(Cont_cell, n, cont_mat);
    }
    // preallocate for rects
    mxArray* rectangles = mxCreateDoubleMatrix(0, 0, mxREAL);
    mxSetM(rectangles, 4);
    mxSetN(rectangles, cont_cols);
    mxSetData(rectangles, mxMalloc(sizeof(double)*cont_cols*4));
    double* rectangles_pr = mxGetPr(rectangles);
    // preallocate for centers
    mxArray* centers = mxCreateDoubleMatrix(0, 0, mxREAL);
    mxSetM(centers, 2);
    mxSetN(centers, cont_cols);
    mxSetData(centers, mxMalloc(sizeof(double)*cont_cols*2));
    double* centers_pr = mxGetPr(centers);
    
	ccv_array_t* textlines = ccv_swt_detect_words_contour(image, params);
    
    mxArray* X = mxCreateStructMatrix(1,1,0,0);
    mxAddField(X, "words");
    mxAddField(X, "contours");
    mxAddField(X, "rectangles");
    mxAddField(X, "centers");
    mxAddField(X, "success");
    int success = 1;
    
    if (textlines)
	{	
        int i, j, k;
        // copy all the cont data into matlab mem
        int n_cont = 0;
        int sizes[cont_cols];
        ccv_contour_t* cont;
        ccv_textline_t* t;
        ccv_point_t* point;
        mxArray* temp_cont;
        double* cont_pr;
        for (i = 0; i < textlines->rnum; i++) {
            t = (ccv_textline_t*)ccv_array_get(textlines, i);
            for (j = 0; j < t->neighbors; j++) {
                if (n_cont >= cont_cols)
                    mexErrMsgTxt("Not enough cache columns (increase 'N' option)");
                cont = t->letters[j]->contour;
                sizes[n_cont] = cont->size;
                // read contour
                temp_cont = mxGetCell(Cont_cell, n_cont);
                mxSetM(temp_cont, cont->size);
                cont_pr = mxGetPr(temp_cont);
                for (k = 0; k < cont->size; k++) {
                    if (k >= cont_rows)
                        mexErrMsgTxt("Not enough cache rows (increase 'M' option)");
                    point = (ccv_point_t*)ccv_array_get(cont->set, k);
                    int index = point->y + rows*point->x;
                    cont_pr[k] = index + 1; // for matlab +1
                }
                // read rectangle
                rectangles_pr[0 + n_cont*4] = t->letters[j]->rect.x + 1;
                rectangles_pr[1 + n_cont*4] = t->letters[j]->rect.y + 1;
                rectangles_pr[2 + n_cont*4] = t->letters[j]->rect.width;
                rectangles_pr[3 + n_cont*4] = t->letters[j]->rect.height;
                // read center
                centers_pr[0 + n_cont*2] = t->letters[j]->center.x + 1;
                centers_pr[1 + n_cont*2] = t->letters[j]->center.y + 1;
                n_cont++;
            }
        }
        mxSetM(Cont_cell, n_cont);
        mxSetN(rectangles, n_cont);
        mxSetN(centers, n_cont);
        n_cont = 0;
        
        mxArray* Words = mxCreateCellMatrix(textlines->rnum,1); // holds all the word structs
        

        // ------- For each word
		for (i = 0; i < textlines->rnum; i++)
		{
            mxArray* Word = mxCreateStructMatrix(1,1,0,0); // represents a word          
            mxAddField(Word, "rect");
            mxAddField(Word, "chars");
            
            t = (ccv_textline_t*)ccv_array_get(textlines, i);
            
            
            // populate the rect field
            mxArray* rect = mxCreateNumericMatrix(1, 4, mxDOUBLE_CLASS, mxREAL);
            double* rect_pr = mxGetPr(rect);
            rect_pr[0] = t->rect.x + 1;
            rect_pr[1] = t->rect.y + 1;
            rect_pr[2] = t->rect.width;
            rect_pr[3] = t->rect.height;
            mxSetField(Word, 0, "rect", rect);
            
            // populate the chars field
            mxArray* Chars = mxCreateCellMatrix(t->neighbors,1); // holds the character structs
            // ------ For each letter
			for (j = 0; j < t->neighbors; j++) {  
                mxArray* Char = mxCreateStructMatrix(1,1,0,0); // represents a character
                mxAddField(Char, "rect");
                mxAddField(Char, "center");
                mxAddField(Char, "S");
                mxAddField(Char, "size");
                mxAddField(Char, "id");
                
                // populate the rect field
//                 mxArray* char_rect = mxCreateDoubleMatrix(1, 4, mxREAL);
//                 double* char_rect_pr = mxGetPr(char_rect);
//                 char_rect_pr[0] = t->letters[j]->rect.x + 1;
//                 char_rect_pr[1] = t->letters[j]->rect.y + 1;
//                 char_rect_pr[2] = t->letters[j]->rect.width;
//                 char_rect_pr[3] = t->letters[j]->rect.height;
//                 mxSetField(Char, 0, "rect", char_rect);
                
                // populate the center field
//                 mxArray* char_center = mxCreateDoubleMatrix(1, 2, mxREAL);
//                 double* char_center_pr = mxGetPr(char_center);
//                 char_center_pr[0] = t->letters[j]->center.x + 1;
//                 char_center_pr[1] = t->letters[j]->center.y + 1;
//                 mxSetField(Char, 0, "center", char_center);
                         
                // populate the S field
                int cont_size = sizes[n_cont];
//                 mxSetField(Char, 0, "S", mxCreateSharedDataCopy(mxGetCell(Cont_cell, n_cont)));
       
                // populate the size field
                mxArray* sz = mxCreateDoubleMatrix(1, 1, mxREAL);
                double* sz_pr = mxGetPr(sz);
                sz_pr[0] = cont_size;
                mxSetField(Char, 0, "size", sz);
                
                // populate id field
                mxArray* id = mxCreateDoubleMatrix(1, 1, mxREAL);
                double* id_pr = mxGetPr(id);
                id_pr[0] = n_cont + 1;
                mxSetField(Char, 0, "id", id);
                n_cont++;

                // append to the Chars cell array
                mxSetCell(Chars, j, Char);
			}
            
            mxSetField(Word, 0, "chars", Chars);
            
            // append to the Words cell array
            mxSetCell(Words, i, Word);
		}
		       
        mxSetField(X, 0, "words", Words);
        mxSetField(X, 0, "contours", Cont_cell);
        mxSetField(X, 0, "rectangles", rectangles);
        mxSetField(X, 0, "centers", centers);
        
        ccv_array_free(textlines);
	}
    
	ccv_matrix_free(image);
	
	ccv_drain_cache();
       
    
    mxArray* succ = mxCreateDoubleMatrix(1, 1, mxREAL);
    double* succ_pr = mxGetPr(succ);
    succ_pr[0] = success;
    mxSetField(X, 0, "success", succ);
    
    plhs[0] = X;
    
	return;
}

