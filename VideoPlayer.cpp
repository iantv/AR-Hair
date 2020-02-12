#include "VideoPlayer.h"
#include <QtCore>
#include <opencv2/core/core.hpp>
#include <opencv2/calib3d.hpp>
#include <PoseEstimation.h>

VideoPlayer::VideoPlayer(QObject *parent, unsigned int camIdx) : QThread(parent) {
    _camIdx = camIdx;
    _poseEstimator = new PoseEstimation();
    _detector = new FaceDetector();
    qRegisterMetaType<cv::Mat>();
}

VideoPlayer::~VideoPlayer() {
    _mutex.lock();
    _capture.release();
    _condition.wakeOne();
    _mutex.unlock();
    this->wait();
    delete _detector;
    delete _poseEstimator;
}

void VideoPlayer::play() {
    _capture.open(_camIdx);
    this->start(LowPriority);
}

void VideoPlayer::run() {
    cv::Mat frame;
    while(_capture.read(frame)) {
        cv::Mat rgbFrame;
        cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);
        this->poseEstimate(frame.channels() == 3 ? rgbFrame : frame,
                           frame.channels() == 3 ? QImage::Format_RGB888 : QImage::Format_Indexed8);
        emit this->processedImage(_image, _poseEstimator->getRotationMatrix(), _poseEstimator->getTranslationVector());
        this->msleep(30);
    }
}

void VideoPlayer::poseEstimate(cv::Mat &frame, QImage::Format format) {
    std::vector<cv::Point2d> imgPoints;
    _detector->find2DKeyPoints(frame, imgPoints);
    if (imgPoints.size() == 8) {
        _poseEstimator->solve(frame, imgPoints);
    }
    _image = QImage((const unsigned char*)(frame.data), frame.cols, frame.rows, format);
}
