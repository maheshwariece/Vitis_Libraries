#include "common/xf_headers.hpp"
#include <stdlib.h>
#include <ap_int.h>
#include <stdio.h>
#include "xf_flip_config.h"
#include "xcl2.hpp"
#include "xf_opencl_wrap.hpp"

using namespace std;
int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <INPUT IMAGE PATH > \n", argv[0]);
        return EXIT_FAILURE;
    }

    cv::Mat in_img, out_img, diff;
    cv::Mat in_gray, out_hls;

// Reading in the images:
#if GRAY
    in_gray = cv::imread(argv[1], 0);
#else
    in_gray = cv::imread(argv[1], 1);
#endif

    if (in_gray.data == NULL) {
        printf("ERROR: Cannot open image %s\n ", argv[1]);
        return EXIT_FAILURE;
    }

    int direction = -1;

// Allocate memory for the output images:
#if GRAY
    out_img.create(in_gray.rows, in_gray.cols, CV_8UC1);
    out_hls.create(in_gray.rows, in_gray.cols, CV_8UC1);
#else
    out_img.create(in_gray.rows, in_gray.cols, CV_8UC3);
    out_hls.create(in_gray.rows, in_gray.cols, CV_8UC3);
#endif

    int height = in_gray.rows;
    int width = in_gray.cols;

// Opencv Flip Function
#if HOR
#if VER
    // Both Horizontal and vertical flip
    cv::flip(in_gray, out_img, -1);
#else
    // horizontal flip
    cv::flip(in_gray, out_img, 1);
    direction = 1;
#endif
#else
    // vertical flip
    cv::flip(in_gray, out_img, 0);
    direction = 0;
#endif

    ////////////////////	HLS TOP function call	/////////////////

    /////////////////////////////////////// CL ////////////////////////
    (void)cl_kernel_mgr::registerKernel("flip_accel", "krnl_flip", XCLIN(in_gray), XCLOUT(out_hls), XCLIN(height),
                                        XCLIN(width), XCLIN(direction));
    cl_kernel_mgr::exec_all();
    /////////////////////////////////////// end of CL ////////////////////////

    // Compute absolute difference image
    cv::absdiff(out_img, out_hls, diff);
    // Save the difference image
    cv::imwrite("diff.jpg", diff);

    imwrite("out_img.jpg", out_img);
    imwrite("out_hls.jpg", out_hls);

    printf("Error analysis");
    float err_per;
    xf::cv::analyzeDiff(diff, 0, err_per);

    if (err_per > 0.0f) {
        fprintf(stderr, "ERROR: Test Failed.\n ");
        return 1;
    }
    std::cout << "Test Passed " << std::endl;
    return 0;
}