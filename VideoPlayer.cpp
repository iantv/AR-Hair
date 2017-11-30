#include "VideoPlayer.h"
#include <QtCore>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d.hpp>
#include <PoseEstimation.h>

VideoPlayer::VideoPlayer(QObject *parent, unsigned int camIdx) : QThread(parent) {
    _stop = true;
    _camIdx = camIdx;
    _poseEstimator = new PoseEstimation();
    _detector = new FaceDetector();
}

VideoPlayer::~VideoPlayer() {
    _mutex.lock();
    _stop = true;
    _capture.release();
    _condition.wakeOne();
    _mutex.unlock();
    this->wait();
    delete _detector;
    delete _poseEstimator;
}

void VideoPlayer::play() {
    if (!this->isRunning()) {
        if (this->isStopped()) {
            _stop = false;
        }
        _capture.open(_camIdx);
        this->start(LowPriority);
    }
}

void VideoPlayer::run() {
    cv::Mat frame;
    while(_capture.read(frame)) {
        cv::Mat rgbFrame;
        cv::cvtColor(frame, rgbFrame, CV_BGR2RGB);
        this->poseEstimate(frame.channels() == 3 ? rgbFrame : frame,
                           frame.channels() == 3 ? QImage::Format_RGB888 : QImage::Format_Indexed8);
        emit this->processedImage(_image);
        this->msleep(10);
    }
}

void VideoPlayer::stop() {
    _stop = true;
}

bool VideoPlayer::isStopped() const {
    return this->_stop;
}

void VideoPlayer::drawBasis(std::vector<cv::Point2d> &imgPoints) {
    int maxD = MAX(_rgbFrame.rows, _rgbFrame.cols);
    cv::Mat camMat = (cv::Mat_<double>(3, 3) << maxD, 0, _rgbFrame.cols / 2.0,
        0, maxD, _rgbFrame.rows / 2.0,
        0, 0, 1.0);
    double distCoeffs[] = { 0,0,0,0 };

    std::vector<cv::Point3d> nose_end_point3D;
    std::vector<cv::Point2d> nose_end_point2D;
    nose_end_point3D.push_back(cv::Point3d(0.0, 0.0, 100.0));
    nose_end_point3D.push_back(cv::Point3d(0.0, 100.0, 0.0));
    nose_end_point3D.push_back(cv::Point3d(-100.0, 0.0, 0.0));

    cv::Mat rvec = _poseEstimator->getRotationVector();
    cv::Mat tvec = _poseEstimator->getTranslationVector();

    cv::projectPoints(nose_end_point3D, rvec, tvec, camMat, cv::Mat(1, 4, CV_64FC1, distCoeffs), nose_end_point2D);
    cv::line(_rgbFrame, imgPoints[1], nose_end_point2D[0], cv::Scalar(255, 0, 0), 2); //blue z
    cv::line(_rgbFrame, imgPoints[1], nose_end_point2D[1], cv::Scalar(0, 255, 0), 2); //green y
    cv::line(_rgbFrame, imgPoints[1], nose_end_point2D[2], cv::Scalar(0, 0, 255), 2); //red x
}
