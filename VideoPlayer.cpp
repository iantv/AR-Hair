#include "VideoPlayer.h"
#include <QtCore>
#include <opencv2/core/core.hpp>

VideoPlayer::VideoPlayer(QObject *parent, unsigned int camIdx) : QThread(parent) {
    _stop = true;
    _camIdx = camIdx;
    _detector = new FaceDetector();
}

VideoPlayer::~VideoPlayer() {
    _mutex.lock();
    _stop = true;
    _capture.release();
    _condition.wakeOne();
    _mutex.unlock();
    this->wait();
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
    while(!_stop) {
        if (!_capture.read(_frame)) {
            _stop = true;
        }
        if (_frame.channels()== 3) {
            cv::Mat temp;
            cv::cvtColor(_frame, temp, CV_BGR2RGB);
            cv::flip(temp, _rgbFrame, 1);

            std::vector<cv::Point2d> imgPoints;
            _detector->find2DKeyPoints(_rgbFrame, imgPoints);

            _image = QImage((const unsigned char*)(_rgbFrame.data),
                            _rgbFrame.cols, _rgbFrame.rows, QImage::Format_RGB888);
        } else {
            _image = QImage((const unsigned char*)(_frame.data),
                         _frame.cols,_frame.rows,QImage::Format_Indexed8);
        }
        emit this->processedImage(_image);
        cv::waitKey(30);
    }
}

void VideoPlayer::stop() {
    _stop = true;
}

bool VideoPlayer::isStopped() const {
    return this->_stop;
}
