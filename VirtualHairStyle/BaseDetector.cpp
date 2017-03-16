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

void BaseDetector::resetDetectedFlag(bool value) {
	_isDetected = value;
}

FaceDetector::FaceDetector() : BaseDetector()
{
	_cascadeName = "haarcascades/haarcascade_frontalface_alt.xml";
	_trackerName = "MEDIANFLOW";
	_tracker = Tracker::create(_trackerName);
	_interval = FACE_DETECTION_INTERVAL;
}

FaceDetector::~FaceDetector()
{

}

bool FaceDetector::try_detect(Mat im, Mat frame_gray, Size minSize, Size maxSize)
{
	_status = FAIL_STATUS;
	if (_isDetected && (_isDetected = _tracker->update(im, _detectedRect))) {
		_status = TRACKING_STATUS;
		return _isDetected;
	}
	_status = DETECT_STATUS;
	return _isDetected = detect_and_tracker(im, frame_gray);
}

bool FaceDetector::detect_and_tracker(Mat im, Mat frame_gray, Size minSize, Size maxSize)
{
	vector<Rect> faces;
	_cascade.detectMultiScale(frame_gray, faces, 1.3, 5, 0 | CASCADE_SCALE_IMAGE, minSize, maxSize);
	if (faces.size() == 0) return false;
	_detectedRect = bestCandidate(faces);
	_tracker = Tracker::create(_trackerName);
	_tracker->init(im, _detectedRect);
	return true;
}

FaceDetectionStatus FaceDetector::getStatus() {
	return _status;
}

FaceElementsDetector::FaceElementsDetector() : BaseDetector()
{

}

FaceElementsDetector::~FaceElementsDetector()
{

}

bool FaceElementsDetector::try_detect(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize, Size maxSize)
{
	if (_isDetected && (_isDetected = _tracker->update(im, _detectedRect))) {
		return _isDetected;
	}
	return _isDetected = detect_and_tracker(im, frame_gray, faceRect);
}

bool FaceElementsDetector::detect_and_tracker(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize, Size maxSize)
{
	vector<Rect> detectedObjects;
	_cascade.detectMultiScale(frame_gray, detectedObjects, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, minSize, maxSize);
	if (!detectedObjects.size()) return false;
	Rect2d tRect = bestCandidate(detectedObjects, faceRect);
	_detectedRect = Rect2d(tRect.x + faceRect.x, tRect.y + faceRect.y, tRect.width, tRect.height);
	_tracker = Tracker::create(_trackerName);
	_tracker->init(im, _detectedRect);
	return true;
}

NoseDetector::NoseDetector() : FaceElementsDetector()
{
	_cascadeName = "haarcascades/haarcascade_mcs_nose.xml";
	_trackerName = "KCF";
	_tracker = Tracker::create(_trackerName);
	_interval = NOSE_DETECTION_INTERVAL;
}

NoseDetector::~NoseDetector()
{

}

bool NoseDetector::try_detect(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize, Size maxSize)
{
	return FaceElementsDetector::try_detect(im, frame_gray, faceRect, minSize, maxSize);
}

bool NoseDetector::detect_and_tracker(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize, Size maxSize)
{
	return FaceElementsDetector::detect_and_tracker(im, frame_gray, faceRect, minSize, maxSize);
}

Rect2d NoseDetector::bestCandidate(std::vector<Rect> &candidates, Rect2d faceRect)
{
	for (size_t i = 0; i < candidates.size(); i++) {
		if ((candidates[i].y + candidates[i].height*0.5 > faceRect.height * 0.25) &&
			(candidates[i].y + candidates[i].height*0.5 < faceRect.height * 0.75)) {
			return candidates[i];
		}
	}
}

MouthDetector::MouthDetector() : FaceElementsDetector()
{
	_cascadeName = "haarcascades/haarcascade_mcs_mouth.xml";
	_trackerName = "KCF";
	_tracker = Tracker::create(_trackerName);
	_interval = MOUTH_DETECTION_INTERVAL;
}

MouthDetector::~MouthDetector()
{

}

bool MouthDetector::try_detect(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize, Size maxSize)
{
	return FaceElementsDetector::try_detect(im, frame_gray,  faceRect, minSize, maxSize);
}

bool MouthDetector::detect_and_tracker(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize, Size maxSize)
{
	return FaceElementsDetector::detect_and_tracker(im, frame_gray, faceRect, minSize, maxSize);
}

Rect2d MouthDetector::bestCandidate(std::vector<Rect> &candidates, Rect2d faceRect)
{
	for (size_t i = 0; i < candidates.size(); i++) {
		if (candidates[i].y + candidates[i].height*0.5 > faceRect.height * 0.65) {
			return candidates[i];
		}
	}
}

PairFaceElementsDetector::PairFaceElementsDetector() : FaceElementsDetector()
{

}

PairFaceElementsDetector::~PairFaceElementsDetector()
{

}

bool PairFaceElementsDetector::try_detect_pair(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize, Size maxSize)
{
	if (_isDetected &&
		(_isDetected = _leftTracker->update(im, _detectedLeftRect) && _rightTracker->update(im, _detectedRightRect)))
		return _isDetected;
	return _isDetected = detect_and_tracker(im, frame_gray, faceRect);
}

bool PairFaceElementsDetector::is_left(Rect eye, Rect face)
{
	return eye.x + eye.width*0.5 < face.width*0.5;
}

bool PairFaceElementsDetector::detect_and_tracker(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize, Size maxSize)
{
	vector<Rect> detectedObjects;
	_cascade.detectMultiScale(frame_gray, detectedObjects, 1.1, 3, 0 | CASCADE_SCALE_IMAGE, minSize, maxSize);
	for (size_t j = 0; j < detectedObjects.size(); j++) {
		add_point(im, faceRect, detectedObjects[j]);
		if (_isDetected) break;
	}
	return _isDetected;
}

void PairFaceElementsDetector::add_point(cv::Mat im, Rect face, Rect elem)
{
	if (elem.y + elem.height*0.5 > face.height * 0.5) return;
	if (is_left(elem, face) && !_isDetectedLeft) {
		_isDetectedLeft = true;
		_detectedLeftRect = Rect2d(elem.x + face.x, elem.y + face.y, elem.width, elem.height);
		_leftTracker = Tracker::create(_trackerName);
		_leftTracker->init(im, _detectedLeftRect);
	} else if (!is_left(elem, face) && !_isDetectedRight) {
		_isDetectedRight = true;
		_detectedRightRect = Rect2d(elem.x + face.x, elem.y + face.y, elem.width, elem.height);
		_rightTracker = Tracker::create(_trackerName);
		_rightTracker->init(im, _detectedRightRect);
	}
	_isDetected = _isDetectedLeft && _isDetectedRight;
}

void PairFaceElementsDetector::resetDetectedFlag(bool value) {
	_isDetected = _isDetectedLeft = _isDetectedRight = value;
}

EyesDetector::EyesDetector() : PairFaceElementsDetector()
{
	_cascadeName = "haarcascades/haarcascade_eye.xml";
	_trackerName = "KCF";
	_leftTracker = Tracker::create(_trackerName);
	_rightTracker = Tracker::create(_trackerName);
	_interval = EYES_DETECTION_INTERVAL;
}

EyesDetector::~EyesDetector()
{

}

bool EyesDetector::try_detect_pair(Mat im, Mat frame_gray, Rect2d faceRect, Size minSize, Size maxSize)
{
	return PairFaceElementsDetector::try_detect_pair(im, frame_gray, faceRect, minSize, maxSize);
}

