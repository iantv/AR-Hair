#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H
#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <FaceDetector.h>
class PoseEstimation;

class VideoPlayer : public QThread
{
    Q_OBJECT
public:
    explicit VideoPlayer(QObject *parent = nullptr, unsigned int camIdx = 0);
    ~VideoPlayer();
    void play();
    void stop();
    bool isStopped() const;
protected:
    void run();
signals:
    void processedImage(const QImage &image);
private:
    void drawBasis(std::vector<cv::Point2d> &imgPoints);
    bool _stop;
    unsigned int _camIdx;
    QMutex _mutex;
    QWaitCondition _condition;
    cv::Mat _frame;
    cv::VideoCapture _capture;
    cv::Mat _rgbFrame;
    QImage _image;
    FaceDetector *_detector;
    PoseEstimation *_poseEstimator;
};

#endif // VIDEOPLAYER_H
