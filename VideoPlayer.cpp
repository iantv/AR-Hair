#include "VideoPlayer.h"
#include <QtCore>
#include <opencv2/core/core.hpp>

VideoPlayer::VideoPlayer(QObject *parent, unsigned int camIdx) : QThread(parent) {
    _stop = true;
    _camIdx = camIdx;
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
        _capture.open(0);
        this->start(LowPriority);
    }
}

void VideoPlayer::run() {
    int delay = (1000/_frameRate);
    while(!_stop) {
        if (!_capture.read(_frame)) {
            _stop = true;
        }
        if (_frame.channels()== 3) {
            cv::cvtColor(_frame, _rgbFrame, CV_BGR2RGB);
            cv::flip(_rgbFrame, _rgbFrame, 1);
            _image = QImage((const unsigned char*)(_rgbFrame.data),
                            _rgbFrame.cols, _rgbFrame.rows, QImage::Format_RGB888);
        } else {
            _image = QImage((const unsigned char*)(_frame.data),
                         _frame.cols,_frame.rows,QImage::Format_Indexed8);
        }
        emit this->processedImage(_image);
        cv::waitKey(delay);
    }
}

void VideoPlayer::stop() {
    _stop = true;
}

bool VideoPlayer::isStopped() const {
    return this->_stop;
}
