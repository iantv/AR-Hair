// HeadPose.cpp : Defines the entry point for the console application.
//
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "stdafx.h"

#include <opencv/cv.h>
#include <opencv2/highgui.hpp>
#include <glut.h>
using namespace cv;

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

#if defined(__APPLE__)
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#elif defined(__linux__) || defined(__MINGW32__) || defined(WIN32)
#  include <GL/gl.h>
#  include <GL/glu.h>
#else
#include <gl/GL.h>
#include <glut.h>
#include <gl/GLU.h>
#endif

#include "glm.h"
#include "HeadPos.h"
#include "PoseEstimation.h"

const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

double rot[9] = {0};

void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);
	gluPerspective(40, 1.5, 0.01, 1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int __w=250,__h=250;
bool readFrame;
bool stopReadFrame;

void key(unsigned char key, int x, int y)
{

    switch (key)
    {
    case 27 :
    case 'Q':
    case 'q': 
		exit(0);
		break;
	case 'w':
	case 'W':
		__w++;
		__w = __w%251;
		break;
	case 'h':
	case 'H':
		__h++;
		__h = __h%250;
		break;
	case 't':
	case 'T': {
		readFrame = true;
		break;
	}
	case 'p':
	case 'P':
	{
		stopReadFrame = !stopReadFrame;
		break;
	}
    default:
        break;
    }

    glutPostRedisplay();
}

void idle(void)
{
    glutPostRedisplay();
}

void myGLinit() {
//    glutSetOption ( GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION ) ;

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial ( GL_FRONT, GL_AMBIENT_AND_DIFFUSE );

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	glEnable(GL_LIGHTING);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

#include <ctime>
void display(void)
{	
	Mat frame;
	bool wasReadNewFrame = false;
	clock_t t1 = clock();
	if (capture.isOpened()) {
		if (!stopReadFrame &&/* readFrame &&*/ capture.read(frame)) { // while
			readFrame = false;
			wasReadNewFrame = true;
			if (!frame.empty()) {
				Mat flipFrame;
				cv::flip(frame, flipFrame, 1);
				detect_2d_points(flipFrame);
			} else {
				printf(" --(!) No captured frame -- Break!"); //break;
			}
		}
		int c = waitKey(10);
	}
	
	// draw the image in the back
	int vPort[4]; glGetIntegerv(GL_VIEWPORT, vPort);
	glEnable2D();
	drawOpenCVImageInGL(imgTex);
	glDisable2D();
	
	glClear(GL_DEPTH_BUFFER_BIT); // we want to draw stuff over the image
	glViewport(0, 0, vPort[2], vPort[3]);
	
	glPushMatrix();
	
	gluLookAt(0,0,0,0,0,1,0,-1,0);

	// put the object in the right position in space
	Vec3d tvv(tv[0],tv[1],tv[2]);
	glTranslated(tvv[0], tvv[1], tvv[2]);

	// rotate it
	double _d[16] = {	rot[0],rot[1],rot[2],0,
						rot[3],rot[4],rot[5],0,
						rot[6],rot[7],rot[8],0,
						0,	   0,	  0		,1};
	
	glMultMatrixd(_d);
	
	// draw the 3D head model
	glColor4f(1, 1, 1, 0);
	glmDraw(head_obj, GLM_SMOOTH);
	glColor4f(1, 1, 0.8, 1); // color for hair + alfa == 1
	glmDraw(hair_obj, GLM_SMOOTH | GLM_TEXTURE);

	glPopMatrix();
	
	// restore to looking at complete viewport
	glViewport(0, 0, vPort[2], vPort[3]); 
	
	glutSwapBuffers();
	if (wasReadNewFrame) {
		printf("fps %f ", 1/(float(clock() - t1) / CLOCKS_PER_SEC));
	}
}

void init_opengl(int argc, char** argv) {
	glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 0);
	
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH); // | GLUT_MULTISAMPLE
    glutCreateWindow("Virtual HairStyle");
	
	myGLinit();
	
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
   // glutSpecialFunc(special);
    glutIdleFunc(idle);
}	

int start_opengl() {
    glutMainLoop();
	return 1;
}

void solve_head_pos(Mat& ip, Mat& img) {
	int max_d = MAX(img.rows, img.cols);
	cam_mat = (Mat_<double>(3, 3) << max_d, 0, img.cols / 2.0,
		0, max_d, img.rows / 2.0,
		0, 0, 1.0);
	double _dc[] = { 0,0,0,0 };
	if (!solvePnP(op, ip, cam_mat, Mat(1, 4, CV_64FC1, _dc), rvec, tvec, true, SOLVEPNP_EPNP)) {
		cout << "solvePnP was fail" << endl;
	}
	
	Mat rotM(3, 3, CV_64FC1, rot);
	Rodrigues(rvec, rotM);
	double* _r = rotM.ptr<double>();

	rotM = rotM.t();// transpose to conform with majorness of opengl matrix
}
