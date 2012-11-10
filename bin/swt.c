// Max Jaderberg 9/11/12
// Outputs the SWT of an image
// Follows the structure of https://github.com/liuliu/ccv/blob/unstable/bin/swtdetect.c and https://github.com/liuliu/ccv/blob/unstable/lib/ccv_swt.c

// To compile:
// clang swt.c -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -o swt `cat /Users/jaderberg/Work/Utils/ccv_max/lib/.LN` -lm

#include <ccv.h>

int main(int arc, char** argv)
{
	// read in image
	ccv_dense_matrix_t* image = 0;
	ccv_read(argv[1], &image, CCV_IO_GRAY | CCV_IO_ANY_FILE);

	// create SWT matrix
	ccv_dense_matrix_t* swt = 0;
	ccv_swt_param_t params = ccv_swt_default_params;

	params.direction = CCV_BRIGHT_TO_DARK;

	ccv_swt(image, &swt, CCV_8U, params);

	ccv_write(swt, argv[2], 0, CCV_IO_JPEG_FILE, 0);
    return 0;
}