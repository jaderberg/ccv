// Max Jaderberg 9/11/12
// Outputs the SWT of an image
// Follows the structure of https://github.com/liuliu/ccv/blob/unstable/bin/swtdetect.c and https://github.com/liuliu/ccv/blob/unstable/lib/ccv_swt.c

// To compile:
// clang swt.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -o swt `cat /Users/jaderberg/Work/Utils/ccv_max/lib/.LN` -lm

#include <ccv.h>

int main(int argc, char** argv)
{
	// process arguments
	char* image_file = "";
	char* out_file = "";
	int dark_to_bright = 1;
	char ch;
	while ((ch = getopt(argc, argv, "r")) != EOF)
		switch(ch) {
			case 'r':
				dark_to_bright = 0;
				break;
		}
	argc -= optind;
	argv += optind;
	image_file = argv[0];
	out_file = argv[1];


	// read in image
	ccv_dense_matrix_t* image = 0;
	ccv_read(image_file, &image, CCV_IO_GRAY | CCV_IO_ANY_FILE);

	// create SWT matrix
	ccv_dense_matrix_t* swt = 0;
	ccv_swt_param_t params = ccv_swt_default_params;

	if (dark_to_bright) {
		params.direction = CCV_DARK_TO_BRIGHT;
		fprintf(stdout, "Computing dark to bright SWT\n");
	}
	else {
		params.direction = CCV_BRIGHT_TO_DARK;
		fprintf(stdout, "Computing bright to dark SWT\n");
	}

	ccv_swt(image, &swt, CCV_8U, params);
	ccv_matrix_free(image);

	// put in visual range
	ccv_dense_matrix_t* vis_swt = 0;
	ccv_visualize(swt, &vis_swt, 0);
	ccv_matrix_free(swt);
	
	char* suffix = strrchr(out_file, '.');
	if (strncmp(suffix, ".png", 4) == 0) {
		ccv_write(vis_swt, out_file, 0, CCV_IO_PNG_FILE, 0);
	} else {
		ccv_write(vis_swt, out_file, 0, CCV_IO_JPEG_FILE, 0);
	}
	ccv_matrix_free(vis_swt);

    return 0;
}