// Max Jaderberg 18/2/13

//clang swt_word_contours_sysmex.c -o swt_word_contours_sysmex

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mex.h"

#define DATA_SIZE 128
#define FILENAME_SIZE 512
#define CMD_SIZE 512
#define OPT_SIZE 32

void mexFunction(int nlhs, mxArray *plhs[], /* Output variables */
					int nrhs, const mxArray *prhs[]) /* Input variable */
{
	FILE* pf;
	char data[DATA_SIZE];
	char input_fn[FILENAME_SIZE];
	char cmd_str[CMD_SIZE];
	char cmd_buff[CMD_SIZE];
	char opt_buff[OPT_SIZE];

	if (!nrhs) {
		mexErrMsgTxt("Error: input 1 must be a filename string");
		return;
	}

	// Check input variable 1 is a string
    if (mxIsChar(prhs[0])) {
        
    } else {
        mexErrMsgTxt("Error: input 1 is not a filename string");
    }

    // Read input variable 1 in
    mxGetString(prhs[0], input_fn, FILENAME_SIZE);

    sprintf(cmd_str, "~/bin/swt_word_contours_sysend");

    // Read in the parameter options to cmd_str
    int n;
    for (n = 1; n < nrhs; n+=2) {
    	strcpy(cmd_buff, cmd_str);
    	mxGetString(prhs[n], opt_buff, OPT_SIZE);
    	sprintf(cmd_str, "%s -%s", cmd_buff, opt_buff);
    	strcpy(cmd_buff, cmd_str);
		mxGetString(prhs[n+1], opt_buff, OPT_SIZE);
    	sprintf(cmd_str, "%s %s", cmd_buff, opt_buff);
    }

    // Append filename
    strcpy(cmd_buff, cmd_str);
    sprintf(cmd_str, "%s %s", cmd_buff, input_fn);

    mexPrintf("CMD: %s\n", cmd_str);

    // RUN SWT DETECTION
	pf = popen(cmd_str, "r");

 	if(!pf){
		mexErrMsgTxt("Error: could not open pipe for output");
		return ;
    }
 
    // Parse result and populate return structure
    mxArray* X;

    char subbuff[64];
    int n_words;
    int n_word = 0;
    int n_chars;
    int n_char;
    int n_cont;
    double x, y, ind;
    double* pr;


    while(fgets(data, DATA_SIZE, pf) != NULL)
	{
		// this is where you parse into a matlab thing

		// numwords
		strcpy(subbuff, "numwords: ");
		if (strncmp(data, subbuff, strlen(subbuff)) == 0) {
			sscanf(data, "numwords: %d", &n_words);
			X = mxCreateCellMatrix(1, n_words);
		}

		if (!n_words)
			continue;

		// Textline
		strcpy(subbuff, "Textline");
		if (strncmp(data, subbuff, strlen(subbuff)) == 0) {
			// start a new word
			mxArray* Word = mxCreateStructMatrix(1,1,0,0);
			mxAddField(Word, "rect");
			mxAddField(Word, "chars");
			mxArray* Chars;
			mxArray* Rect = mxCreateDoubleMatrix(1, 4, mxREAL);
			double minw_x = 9999999;
			double minw_y = 9999999;
			double maxw_x = 0;
			double maxw_y = 0;
			n_char = 0;
			// parse word data
			while(fgets(data, DATA_SIZE, pf) != NULL) {
				// Endtextline
				strcpy(subbuff, "Endtextline");
				if (strncmp(data, subbuff, strlen(subbuff)) == 0)
					break;
				// numchars
				strcpy(subbuff, "numchars: ");
				if (strncmp(data, subbuff, strlen(subbuff)) == 0) {
					sscanf(data, "numchars: %d", &n_chars);
					Chars = mxCreateCellMatrix(1, n_chars);
					continue;
				}
				// Character
				strcpy(subbuff, "Character");
				if (strncmp(data, subbuff, strlen(subbuff)) == 0) {
					// Create the char struct
					mxArray* Char = mxCreateStructMatrix(1,1,0,0);
					mxAddField(Char, "rect");
					mxAddField(Char, "center");
					mxAddField(Char, "S");
					mxArray* Charrect = mxCreateDoubleMatrix(1, 4, mxREAL);
					double min_x = 9999999;
					double min_y = 9999999;
					double max_x = 0;
					double max_y = 0;
					mxArray* Charcenter = mxCreateDoubleMatrix(1, 2, mxREAL);
					// parse character data
					while(fgets(data, DATA_SIZE, pf) != NULL) {
						// Endtextline
						strcpy(subbuff, "Endcharacter");
						if (strncmp(data, subbuff, strlen(subbuff)) == 0)
							break;
						// Contour
						strcpy(subbuff, "Contour");
						if (strncmp(data, subbuff, strlen(subbuff)) == 0) {
							// read how long the contour will be
							if (fgets(data, DATA_SIZE, pf) != NULL) {
								strcpy(subbuff, "numcont: ");
								if (strncmp(data, subbuff, strlen(subbuff)) == 0) {
									sscanf(data, "numcont: %d", &n_cont);
								}
								// create the contour matrix
								mxArray* Cont = mxCreateDoubleMatrix(n_cont, 1, mxREAL);
								pr = mxGetPr(Cont);
								double last_good_ind = 1;
								// read in the contour data
								while(fgets(data, DATA_SIZE, pf) != NULL) {
									// Endcontour
									strcpy(subbuff, "Endcontour");
									if (strncmp(data, subbuff, strlen(subbuff)) == 0) {
										break;
									} else {
										// read the point
										sscanf(data, "%lf %lf %lf", &x, &y, &ind);
										*pr = ind + 1; // +1 for matlab indexing
										pr++;
										// form character rectangle
										min_x = fmin(min_x, x);
										min_y = fmin(min_y, y);
										max_x = fmax(max_x, x);
										max_y = fmax(max_y, y);
									}
								}
								mxSetField(Char, 0, "S", Cont);
								// create the rectangle
								pr = mxGetPr(Charrect);
								pr[0] = min_x + 1;
								pr[1] = min_y + 1;
								pr[2] = max_x - min_x;
								pr[3] = max_y - min_y;
								mxSetField(Char, 0, "rect", Charrect);
								// create the center
								pr = mxGetPr(Charcenter);
								pr[0] = round((max_x - min_x)/2) + 1;
								pr[1] = round((max_y - min_y)/2) + 1;
								mxSetField(Char, 0, "center", Charcenter);
								// form word rectangle
								minw_x = fmin(minw_x, min_x);
								minw_y = fmin(minw_y, min_y);
								maxw_x = fmax(maxw_x, max_x);
								maxw_y = fmax(maxw_y, max_y);
							}
						}
					} // charwhile
					mxSetCell(Chars, n_char, Char);
					n_char++;
				}
			} // wordwhile
			mxSetField(Word, 0, "chars", Chars);
			// create rectangle
			pr = mxGetPr(Rect);
			pr[0] = minw_x + 1;
			pr[1] = minw_y + 1;
			pr[2] = maxw_x - minw_x;
			pr[3] = maxw_y - minw_y;
			mxSetField(Word, 0, "rect", Rect);

			mxSetCell(X, n_word, Word);
			n_word++;
		}


	}
 
    if (pclose(pf) != 0)
    	mexErrMsgTxt("Error: failed to close pipe");


    plhs[0] = X;
    nlhs = 1;
	return;
}









int main(int argc, char** argv) {

	FILE* pf;
	char data[DATA_SIZE];

	pf = popen("swt_word_contours_sysend /Users/jaderberg/Desktop/PICTs0057.JPG", "r");

 	if(!pf){
		fprintf(stderr, "Could not open pipe for output.\n");
		return 1;
    }
 

    while(fgets(data, 80, pf) != NULL)
	{
		// this is where you parse into a matlab thing
		 printf ("%s", data);
	}
 
    if (pclose(pf) != 0)
    	fprintf(stderr," Error: Failed to close command stream \n");

	return 0;
}
