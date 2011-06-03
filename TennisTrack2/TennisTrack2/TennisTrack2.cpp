// TennisTrack2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

/*****************************************************************************************
*  Name    : Fast object tracking using the OpenCV library                               *
*  Author  : Lior Chen <chen.lior@gmail.com>                                             *
*  Notice  : Copyright (c) Jun 2010, Lior Chen, All Rights Reserved                      *
*          :                                                                             *
*  Site    : http://www.lirtex.com                                                       *
*  WebPage : http://www.lirtex.com/robotics/fast-object-tracking-robot-computer-vision   *
*          :                                                                             *
*  Version : 1.0                                                                         *
*  Notes   : By default this code will open the first connected camera.                  *
*          : In order to change to another camera, change                                *
*          : CvCapture* capture = cvCaptureFromCAM( 0 ); to 1,2,3, etc.                  *
*          : Also, the code is currently configured to tracking RED objects.             *
*          : This can be changed by changing the hsv_min and hsv_max vectors             *
*          :                                                                             *
*  License : This program is free software: you can redistribute it and/or modify        *
*          : it under the terms of the GNU General Public License as published by        *
*          : the Free Software Foundation, either version 3 of the License, or           *
*          : (at your option) any later version.                                         *
*          :                                                                             *
*          : This program is distributed in the hope that it will be useful,             *
*          : but WITHOUT ANY WARRANTY; without even the implied warranty of              *
*          : MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
*          : GNU General Public License for more details.                                *
*          :                                                                             *
*          : You should have received a copy of the GNU General Public License           *
*          : along with this program.  If not, see <http://www.gnu.org/licenses/>        *
******************************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <CLEyeMulticam.h>
 

 class CLEyeCameraCapture
{
	CHAR _windowName[256];
	GUID _cameraGUID;
	CLEyeCameraInstance _cam;
	CLEyeCameraColorMode _mode;
	CLEyeCameraResolution _resolution;
	float _fps;
	HANDLE _hThread;
	bool _running;
public:
	CLEyeCameraCapture(LPSTR windowName, GUID cameraGUID, CLEyeCameraColorMode mode, CLEyeCameraResolution resolution, float fps) :
	_cameraGUID(cameraGUID), _cam(NULL), _mode(mode), _resolution(resolution), _fps(fps), _running(false)
	{
		strcpy(_windowName, windowName);
	}
	bool StartCapture()
	{
		_running = true;
		// Start CLEye image capture thread
		_hThread = CreateThread(NULL, 0, &CLEyeCameraCapture::CaptureThread, this, 0, 0);
		if(_hThread == NULL)
		{
			MessageBox(NULL,"Could not create capture thread","CLEyeMulticamTest", MB_ICONEXCLAMATION);
			return false;
		}
		return true;
	}
	void StopCapture()
	{
		if(!_running)	return;
		_running = false;
		WaitForSingleObject(_hThread, 1000);
		
	}
	void IncrementCameraParameter(int param)
	{
		if(!_cam)	return;
		CLEyeSetCameraParameter(_cam, (CLEyeCameraParameter)param, CLEyeGetCameraParameter(_cam, (CLEyeCameraParameter)param)+10);
	}
	void DecrementCameraParameter(int param)
	{
		if(!_cam)	return;
		CLEyeSetCameraParameter(_cam, (CLEyeCameraParameter)param, CLEyeGetCameraParameter(_cam, (CLEyeCameraParameter)param)-10);
	}
	void Run()
	{
		int w, h;
		IplImage *pCapImage;
		PBYTE pCapBuffer = NULL;
		
        // Create camera instance
		_cam = CLEyeCreateCamera(_cameraGUID, _mode, _resolution, _fps);
		
        if(_cam == NULL)		return;
		
        // Get camera frame dimensions
		CLEyeCameraGetFrameDimensions(_cam, w, h);
		
        // Depending on color mode chosen, create the appropriate OpenCV image
		if(_mode == CLEYE_COLOR_PROCESSED || _mode == CLEYE_COLOR_RAW)
			pCapImage = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 4);
		else
			pCapImage = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);

		// Set some camera parameters
		//CLEyeSetCameraParameter(_cam, CLEYE_GAIN, 30);
		//CLEyeSetCameraParameter(_cam, CLEYE_EXPOSURE, 500);
        //CLEyeSetCameraParameter(_cam, CLEYE_AUTO_EXPOSURE, false);
        //CLEyeSetCameraParameter(_cam, CLEYE_AUTO_GAIN, false);
        //CLEyeSetCameraParameter(_cam, CLEYE_AUTO_WHITEBALANCE, false);
        //CLEyeSetCameraParameter(_cam, CLEYE_WHITEBALANCE_RED, 100);
        //CLEyeSetCameraParameter(_cam, CLEYE_WHITEBALANCE_BLUE, 200);
        //CLEyeSetCameraParameter(_cam, CLEYE_WHITEBALANCE_GREEN, 200);

        

		// Start capturing
		CLEyeCameraStart(_cam);

		CvMemStorage* storage = cvCreateMemStorage(0);
		
        IplImage* hsv_frame = cvCreateImage(cvSize(pCapImage->width, pCapImage->height), IPL_DEPTH_8U, 3);
        IplImage* thresholded   = cvCreateImage(cvSize(pCapImage->width, pCapImage->height), IPL_DEPTH_8U, 1);
		IplImage* temp = cvCreateImage(cvSize(pCapImage->width >> 1, pCapImage->height >> 1), IPL_DEPTH_8U, 3);

        // Create a window in which the captured images will be presented
        cvNamedWindow( "Camera" , CV_WINDOW_AUTOSIZE );
        cvNamedWindow( "HSV", CV_WINDOW_AUTOSIZE );
        cvNamedWindow( "EdgeDetection", CV_WINDOW_AUTOSIZE );
 
        
 
        //int hl = 100, hu = 115, sl = 95, su = 135, vl = 115, vu = 200;
        int hl = 5, hu = 75, sl = 40, su = 245, vl = 105, vu = 175;
        

		// image capturing loop
		while(_running)
		{

            // Detect a red ball
            CvScalar hsv_min = cvScalar(hl, sl, vl, 0);
            CvScalar hsv_max = cvScalar(hu, su, vu, 0);

			cvGetImageRawData(pCapImage, &pCapBuffer);
			CLEyeCameraGetFrame(_cam, pCapBuffer);

			cvConvertImage(pCapImage, hsv_frame);

            // Get one frame
            if( !pCapImage )
            {
                    fprintf( stderr, "ERROR: frame is null...\n" );
                    getchar();
                    break;
            }
 
                // Covert color space to HSV as it is much easier to filter colors in the HSV color-space.
                cvCvtColor(pCapImage, hsv_frame, CV_RGB2HSV);
                // Filter out colors which are out of range.
                cvInRangeS(hsv_frame, hsv_min, hsv_max, thresholded);
 
                // Memory for hough circles
                CvMemStorage* storage = cvCreateMemStorage(0);
                // hough detector works better with some smoothing of the image
                cvSmooth( thresholded, thresholded, CV_GAUSSIAN, 9, 9 );
                CvSeq* circles = cvHoughCircles(thresholded, storage, CV_HOUGH_GRADIENT, 2,
                                                thresholded->height/4, 100, 50, 10, 400);
 
                for (int i = 0; i < circles->total; i++)
                {
                    float* p = (float*)cvGetSeqElem( circles, i );
                    //printf("Ball! x=%f y=%f r=%f\n\r",p[0],p[1],p[2] );
                    cvCircle( pCapImage, cvPoint(cvRound(p[0]),cvRound(p[1])),
                                            3, CV_RGB(0,255,0), -1, 8, 0 );
                    cvCircle( pCapImage, cvPoint(cvRound(p[0]),cvRound(p[1])),
                                            cvRound(p[2]), CV_RGB(255,0,0), 3, 8, 0 );
                }
 
                cvShowImage( "Camera", pCapImage ); // Original stream with detected ball overlay
                cvShowImage( "HSV", hsv_frame); // Original stream in the HSV color space
                cvShowImage( "EdgeDetection", thresholded ); // The stream after color filtering
 
                cvReleaseMemStorage(&storage);
 
                // Do not release the frame!
 
                //If ESC key pressed, Key=0x10001B under OpenCV 0.9.7(linux version),
                //remove higher bits using AND operator
                int key = cvWaitKey(10);
                
                

                switch(key){
                    case 'q' : hu += 5; break;
                    case 'Q' : hu -= 5; break;
                    
                    case 'a' : hl -= 5; break;
                    case 'A' : hl += 5; break;
                    
                    case 'w' : su += 5; break;
                    case 'W' : su -= 5; break;
                    
                    case 's' : sl -= 5; break;
                    case 'S' : sl += 5; break;
                    
                    case 'e' : vu += 5; break;
                    case 'E' : vu -= 5; break;
                    
                    case 'd' : vl -= 5; break;
                    case 'D' : vl += 5; break;
                }

                if (key != -1){
                    printf("H: %i, S: %i, V: %i\nH: %i, S: %i, V: %i\n\n", hu, su, vu, hl, sl, vl);
                }
            
 

			
		}
		cvReleaseImage(&temp);
		cvReleaseImage(&pCapImage);

		// Stop camera capture
		CLEyeCameraStop(_cam);
		// Destroy camera object
		CLEyeDestroyCamera(_cam);
		// Destroy the allocated OpenCV image
		cvReleaseImage(&pCapImage);
		_cam = NULL;
	}
	static DWORD WINAPI CaptureThread(LPVOID instance)
	{
		// seed the rng with current tick count and thread id
		srand(GetTickCount() + GetCurrentThreadId());
		// forward thread to Capture function
		CLEyeCameraCapture *pThis = (CLEyeCameraCapture *)instance;
		pThis->Run();
		return 0;
	}
};

int main(int argc, _TCHAR* argv[])
{
	CLEyeCameraCapture *cam = NULL;
	// Query for number of connected cameras
	int numCams = CLEyeGetCameraCount();
	if(numCams == 0)
	{
		printf("No PS3Eye cameras detected\n");
		return -1;
	}
	char windowName[64];
	// Query unique camera uuid
	GUID guid = CLEyeGetCameraUUID(0);
	printf("Camera GUID: [%08x-%04x-%04x-%02x%02x%02x%02x%02x%02x%02x%02x]\n", 
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2],
		guid.Data4[3], guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
	sprintf(windowName, "Face Tracker Window");
	// Create camera capture object
	// Randomize resolution and color mode
	cam = new CLEyeCameraCapture(windowName, guid, CLEYE_COLOR_PROCESSED, CLEYE_VGA, 75);
	
    printf("Starting capture\n");
	cam->StartCapture();

	printf("Use the following keys to change camera parameters:\n"
		"\t'g' - select gain parameter\n"
		"\t'e' - select exposure parameter\n"
		"\t'z' - select zoom parameter\n"
		"\t'r' - select rotation parameter\n"
		"\t'+' - increment selected parameter\n"
		"\t'-' - decrement selected parameter\n");
	// The <ESC> key will exit the program
	CLEyeCameraCapture *pCam = NULL;
	int param = -1, key;
	while((key = cvWaitKey(0)) != 0x1b)
	{
		switch(key)
		{
		case 'g':	case 'G':	printf("Parameter Gain\n");		param = CLEYE_GAIN;		break;
		case 'e':	case 'E':	printf("Parameter Exposure\n");	param = CLEYE_EXPOSURE;	break;
		case 'z':	case 'Z':	printf("Parameter Zoom\n");		param = CLEYE_ZOOM;		break;
		case 'r':	case 'R':	printf("Parameter Rotation\n");	param = CLEYE_ROTATION;	break;
		case '+':	if(cam)		cam->IncrementCameraParameter(param);					break;
		case '-':	if(cam)		cam->DecrementCameraParameter(param);					break;
		}
	}
	cam->StopCapture();
	delete cam;
	return 0;
}