// Max Jaderberg 11/11/12

// Uses opencv to grab webcam input, then find the words using Stroke Width Transform.

// clang demo_webcam_words.c -I"/opt/local/include/opencv" -I"/opt/local/include" -L"/opt/local/lib" -L"/Users/jaderberg/Work/Utils/ccv_max/lib" -I"/Users/jaderberg/Work/Utils/ccv_max/lib" -lccv -lopencv_core -lopencv_highgui -lopencv_video -lopencv_imgproc -o demo_webcam_words `cat /Users/jaderberg/Work/Utils/ccv_max/lib/.LN` -lm

#include <ccv.h>
#include <cv.h>
#include <highgui.h>

int main(int argc, char *argv[])
{

	// Capture camera
	CvCapture* webcam = cvCreateCameraCapture(0);
	if (!webcam) {
		fprintf(stderr, "ERROR: no webcam found\n");
		return 1;
	}

	// Create window
	char* win_name = "SWT Word Finding";
	cvNamedWindow(win_name, CV_WINDOW_AUTOSIZE);

	// Capture loop
	IplImage* curr_frame = malloc(sizeof(IplImage));
	curr_frame = NULL;
	IplImage* raw_image = malloc(sizeof(IplImage));
	CvSize cvsize;
	

	while (1) {
		raw_image = cvQueryFrame(webcam);
		if (raw_image) {
			cvsize.width = raw_image->width;
			cvsize.height = raw_image->height;
			// convert to mono
			curr_frame = cvCreateImage(cvsize, IPL_DEPTH_8U, 1);
			cvConvertImage(raw_image, curr_frame, 0);
			
			

	
			// convert to ccv
			ccv_dense_matrix_t* curr_frame_ccv = 0;
			ccv_read(curr_frame->imageData, &curr_frame_ccv, CCV_IO_GRAY_RAW, curr_frame->height, curr_frame->width, curr_frame->widthStep);

			// get the words
			ccv_array_t* words = ccv_swt_detect_words(curr_frame_ccv, ccv_swt_default_params);
			ccv_matrix_free(curr_frame_ccv);

			//
			if (words) {
				int i;
				for (i = 0; i < words->rnum; i++) {
					ccv_rect_t* rect = (ccv_rect_t*)ccv_array_get(words, i);
					cvRectangle(raw_image,                    /* the dest image */
				                cvPoint(rect->x, rect->y),        /* top left point */
				                cvPoint(rect->x + rect->width, rect->y + rect->height),       /* bottom right point */
				                cvScalar(0, 0, 255, 0), /* the color; blue */
				                1, CV_AA, 0); 
				}
				ccv_array_free(words);
			}

			


			

			cvShowImage(win_name, raw_image);
		}

		// if ESC is pressed
		if ((cvWaitKey(10) & 255) == 27) break;
	}
}