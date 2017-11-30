#-------------------------------------------------
#
# Project created by QtCreator 2017-11-15T14:23:51
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = 3DHairstyleSimulator
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        MainWindow.cpp \
    VideoPlayer.cpp \
    FaceDetector.cpp \
    PoseEstimation.cpp \
    Graphics/Base3DModel.cpp

HEADERS += \
        MainWindow.h \
    VideoPlayer.h \
    FaceDetector.h \
    PoseEstimation.h \
    Graphics/Base3DModel.h

FORMS += \
        MainWindow.ui

INCLUDEPATH += C:\OpenCV\opencv-3.2.0\buildx64\install\include \
C:\Dlib\dlib-19.6
LIBS += -LC:\Dlib\dlib-19.6\buildx64 \
-ldlib \
-LC:\OpenCV\opencv-3.2.0\buildx64\install\bin \
-LC:\OpenCV\opencv-3.2.0\buildx64\install\lib \
-lopencv_aruco320 \
-lopencv_bgsegm320 \
-lopencv_bioinspired320 \
-lopencv_calib3d320 \
-lopencv_ccalib320 \
-lopencv_core320 \
-lopencv_datasets320 \
-lopencv_dnn320 \
-lopencv_dpm320 \
-lopencv_face320 \
-lopencv_features2d320 \
-lopencv_flann320 \
-lopencv_fuzzy320 \
-lopencv_highgui320 \
-lopencv_imgcodecs320 \
-lopencv_imgproc320 \
-lopencv_line_descriptor320 \
-lopencv_ml320 \
-lopencv_objdetect320 \
-lopencv_optflow320 \
-lopencv_phase_unwrapping320 \
-lopencv_photo320 \
-lopencv_plot320 \
-lopencv_reg320 \
-lopencv_rgbd320 \
-lopencv_saliency320 \
-lopencv_shape320 \
-lopencv_stereo320 \
-lopencv_stitching320 \
-lopencv_structured_light320 \
-lopencv_superres320 \
-lopencv_surface_matching320 \
-lopencv_text320 \
-lopencv_tracking320 \
-lopencv_video320 \
-lopencv_videoio320 \
-lopencv_videostab320 \
-lopencv_xfeatures2d320 \
-lopencv_ximgproc320 \
-lopencv_xobjdetect320 \
-lopencv_xphoto320
