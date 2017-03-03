#include "BaseDetector.h"
#include <vector>

using namespace std;

BaseDetector::BaseDetector()
{
	_isDetected == false;
	_frame_count = 0;
}

BaseDetector::~BaseDetector()
{

}

int BaseDetector::load_cascade()
{
	if (!_cascade.load(_cascadeName)) {
		printf("--(!)Error loading %s", _cascadeName);
		return 0;
	};
	return 1;
}

FaceDetector::FaceDetector() : BaseDetector()
{
	_cascadeName = "haarcascades/haarcascade_frontalface_alt.xml";
	_tracker = Tracker::create("MEDIANFLOW");
}

FaceDetector::~FaceDetector()
{

}

bool FaceDetector::try_detect(Mat im, Mat frame_gray, Rect2d *rect, Size minSize, Size maxSize)
{
	if (_frame_count == FACE_DETECTION_INTERVAL) {
		_frame_count = 0;
		_isDetected = false;
	}
	if (!_isDetected) {
		if (!detect_and_tracker(im, frame_gray)) return false;
	} else {
		_isDetected = _tracker->update(im, _detectedRect);
		if (!_isDetected && (!detect_and_tracker(im, frame_gray))) return false;
	}
	*rect = _detectedRect;
	_frame_count++;
	return true;
}

bool FaceDetector::detect_and_tracker(Mat im, Mat frame_gray, Size minSize, Size maxSize)
{
	vector<Rect> faces;
	_cascade.detectMultiScale(frame_gray, faces, 1.3, 5, 0 | CASCADE_SCALE_IMAGE, minSize, maxSize);
	if (faces.size() == 0) return false;
	_detectedRect = faces[0];
	_tracker = Tracker::create("MEDIANFLOW");
	_isDetected = _tracker->init(im, _detectedRect);
	return true;
}

FaceElementsDetector::FaceElementsDetector() : BaseDetector()
{

}

FaceElementsDetector::~FaceElementsDetector()
{

}

bool FaceElementsDetector::try_detect(Mat im, Mat frame_gray, Rect2d *rect, Size minSize, Size maxSize)
{
	if (_frame_count == _interval) {
		_frame_count = 0;
		_isDetected = false;
	}
	if (!_isDetected) {
		if (!detect_and_tracker(im, frame_gray)) return false;
	} else {
		_isDetected = _tracker->update(im, _detectedRect);
		if (!_isDetected && (!detect_and_tracker(im, frame_gray))) return false;
	}
	*rect = _detectedRect;
	_frame_count++;
}

bool FaceElementsDetector::detect_and_tracker(Mat im, Mat frame_gray, Size minSize, Size maxSize)
{
	vector<Rect> detectedObjects;
	_cascade.detectMultiScale(frame_gray, detectedObjects, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, minSize, maxSize);
	if (!detectedObjects.size()) return false;
	_isDetected = true;
	_detectedRect = detectedObjects[0];
	_tracker = Tracker::create("KCF");
	_isDetected = _tracker->init(im, _detectedRect);
	return true;
}

NoseDetector::NoseDetector() : FaceElementsDetector()
{
	_cascadeName = "haarcascades/haarcascade_mcs_nose.xml";
	_tracker = Tracker::create("KCF");
	_interval = NOSE_DETECTION_INTERVAL;
}

NoseDetector::~NoseDetector()
{

}

bool NoseDetector::try_detect(Mat im, Mat frame_gray, Rect2d *rect, Size minSize, Size maxSize)
{
	return FaceElementsDetector::try_detect(im, frame_gray, rect, minSize, maxSize);
}

bool NoseDetector::detect_and_tracker(Mat im, Mat frame_gray, Size minSize, Size maxSize)
{
	return FaceElementsDetector::detect_and_tracker(im, frame_gray, minSize, maxSize);
}

MouthDetector::MouthDetector() : FaceElementsDetector()
{
	_cascadeName = "haarcascades/haarcascade_mcs_mouth.xml";
	_tracker = Tracker::create("KCF");
	_interval = MOUTH_DETECTION_INTERVAL;
}

MouthDetector::~MouthDetector()
{

}

bool MouthDetector::try_detect(Mat im, Mat frame_gray, Rect2d *rect, Size minSize, Size maxSize)
{
	return FaceElementsDetector::try_detect(im, frame_gray, rect, minSize, maxSize);
}

bool MouthDetector::detect_and_tracker(Mat im, Mat frame_gray, Size minSize, Size maxSize)
{
	return FaceElementsDetector::detect_and_tracker(im, frame_gray, minSize, maxSize);
}

PairFaceElementsDetector::PairFaceElementsDetector() : FaceElementsDetector()
{

}

PairFaceElementsDetector::~PairFaceElementsDetector()
{

}

EyesDetector::EyesDetector() : PairFaceElementsDetector()
{
	_cascadeName = "haarcascades/haarcascade_eye.xml";
	_leftTracker = Tracker::create("KCF");
	_rightTracker = Tracker::create("KCF");
}

EyesDetector::~EyesDetector()
{

}

bool EyesDetector::try_detect_pair(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize, Size maxSize)
{
	if (_frame_count == EYES_DETECTION_INTERVAL) {
		_frame_count = 0;
		_isDetected = _isDetectedLeft = _isDetectedRight = false;
	}
	
	if (!_isDetected) {
		vector<Rect> eyes;
		_cascade.detectMultiScale(frame_gray, eyes, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, minSize, maxSize);
		for (size_t j = 0; j < eyes.size(); j++) {
			add_eye_point(im, faceRect, eyes[j]);
			if (_isDetectedLeft && _isDetectedRight) break;
		}
		if (eyes.size() < 2) return false;
		_isDetected = _isDetectedLeft && _isDetectedRight;
	} else {
		if (_isDetectedLeft = _leftTracker->update(im, _detectedLeftRect))
			add_eye_point(im, faceRect, _detectedLeftRect);
		if (_isDetectedRight = _rightTracker->update(im, _detectedRightRect))
			add_eye_point(im, faceRect, _detectedRightRect);
	}
	_frame_count++;
	return true;
}

bool EyesDetector::detect_and_tracker(Mat im, Mat frame_gray, Size minSize, Size maxSize)
{
	return true;
}

void EyesDetector::add_eye_point(cv::Mat im, Rect face, Rect eye)
{
	if (eye.y + eye.height*0.5 > face.height * 0.5) return;
	if (is_left_eye(eye, face) && !_isDetectedLeft) {
		_isDetectedLeft = true;
		_detectedLeftRect = eye;
		_leftTracker = Tracker::create("KCF");
		_leftTracker->init(im, _detectedLeftRect);
	} else if (!is_left_eye(eye, face) && !_isDetectedRight) {
		_isDetectedRight = true;
		_detectedRightRect = eye;
		_rightTracker = Tracker::create("KCF");
		_rightTracker->init(im, _detectedRightRect);
	}
}

bool EyesDetector::is_left_eye(Rect eye, Rect face)
{
	return eye.x + eye.width*0.5 < face.width*0.5;
}