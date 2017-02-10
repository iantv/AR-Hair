#include "PoseEstimation.h"
#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

string window_name = "Capture - Face detection";

int main(int argc, const char** argv) {
	VideoCapture capture;
	Mat frame;

	if (load_cascades() == -1) return -1;

	capture.open(0);
	/*capture.open("C:/Users/iantv/Downloads/Video/linfina.mp4");
	capture.set(CV_CAP_PROP_POS_MSEC, 5500);*/

	if (capture.isOpened()) {
		//bool r = capture.read(frame);
		while (capture.read(frame)) {
			flip(frame, frame, 1);
			if (!frame.empty()) {
				detect2dpoints(frame);
			}
			else {
				printf(" --(!) No captured frame -- Break!"); break;
			}

			int c = waitKey(10);
			if ((char)c == 'c') { break; }
		}
	}
	return 0;
}