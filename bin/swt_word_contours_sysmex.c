// Max Jaderberg 18/2/13

//clang swt_word_contours_sysmex.c -o swt_word_contours_sysmex

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

	pf = popen(cmd_str, "r");

 	if(!pf){
		mexErrMsgTxt("Error: could not open pipe for output");
		return ;
    }
 

    while(fgets(data, 80, pf) != NULL)
	{
		// this is where you parse into a matlab thing
		mexPrintf("%s", data);;
	}
 
    if (pclose(pf) != 0)
    	mexErrMsgTxt("Error: failed to close pipe");

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
