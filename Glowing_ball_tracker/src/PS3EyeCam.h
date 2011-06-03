#ifndef INC_PS3EYECAM_H
#define INC_PS3EYECAM_H

#include <stdio.h>
#include <windows.h>

#include <cv.h>
#include <highgui.h>

#include "CLEyeMulticam.h"

using namespace cv;

// Sample camera capture class
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

		IplImage *pCapImage;
		PBYTE pCapBuffer;

		static DWORD WINAPI CaptureThread(LPVOID instance)
		{
			// forward thread to Capture function
			CLEyeCameraCapture *pThis = (CLEyeCameraCapture *)instance;
			pThis->Run();
			return 0;
		}

	public:

		CLEyeCameraCapture(LPSTR windowName, GUID cameraGUID, CLEyeCameraColorMode mode, CLEyeCameraResolution resolution, float fps) :
			_cameraGUID(cameraGUID), _cam(NULL), _mode(mode), _resolution(resolution), _fps(fps), _running(false)
		{
			strcpy(_windowName, windowName);
		}

		bool StartCapture();
		void StopCapture();
		void Run();
		
		Vec3f balltoPaint;
		bool is_ball_detected;
		int w, h;

	private:

		int param1, param2, param3;
		int param4, param5, param6;
		int param7, param8;
		int gain, exposure;
		int mode;

		Vec3f ball;
		IplImage *threshold_image, *imgHSV;
		Mat Hough_mat,threshold_image_mat;
		Mat image_roi;
		vector<Vec3f> circles;
		Point center,pos_rectangle_roi;
		Vec3f last_ball1,last_ball2,last_ball3;
		int radius;
		bool using_image_roi;
		DWORD oldTime, newTime;
		Scalar min_color, max_color;
		int rowStep;
		int pixel;
		int real_radiusR, real_radiusL, real_radiusT, real_radiusB;
		float temp;
		int newradius, diffradius;
		float newCX, newCY;
		int diffcenter;
		int xx, yy, ww, hh, offset;
		Rect roi;

		static void switch_param1( int position, void* obj )
		{
			CLEyeCameraCapture* myObj = (CLEyeCameraCapture*) obj; //recast 
			myObj->param1 = position;
		}
		static void switch_param2( int position, void* obj )
		{
			CLEyeCameraCapture* myObj = (CLEyeCameraCapture*) obj; //recast 
			myObj->param2 = position;
		}
		static void switch_param3( int position, void* obj )
		{
			CLEyeCameraCapture* myObj = (CLEyeCameraCapture*) obj; //recast 
			myObj->param3 = position;
		}
		static void switch_param4( int position, void* obj )
		{
			CLEyeCameraCapture* myObj = (CLEyeCameraCapture*) obj; //recast 
			myObj->param4 = position;
		}
		static void switch_param5( int position, void* obj )
		{
			CLEyeCameraCapture* myObj = (CLEyeCameraCapture*) obj; //recast 
			myObj->param5 = position;
		}
		static void switch_param6( int position, void* obj )
		{
			CLEyeCameraCapture* myObj = (CLEyeCameraCapture*) obj; //recast 
			myObj->param6 = position;
		}

		static void switch_param7( int position, void* obj )
		{
			CLEyeCameraCapture* myObj = (CLEyeCameraCapture*) obj; //recast 
			myObj->param7 = position;
		}
		static void switch_param8( int position, void* obj )
		{
			CLEyeCameraCapture* myObj = (CLEyeCameraCapture*) obj; //recast 
			myObj->param8 = position;
		}

		static void switch_gain( int position, void* obj )
		{
			CLEyeCameraCapture* myObj = (CLEyeCameraCapture*) obj; //recast 
			myObj->gain = position;
			CLEyeSetCameraParameter(myObj->_cam, CLEYE_GAIN, position);
		}
		static void switch_exposure( int position, void* obj )
		{
			CLEyeCameraCapture* myObj = (CLEyeCameraCapture*) obj; //recast 
			myObj->exposure = position;
			CLEyeSetCameraParameter(myObj->_cam, CLEYE_EXPOSURE, position);
		}
		static void switch_mode( int position, void* obj )
		{
			CLEyeCameraCapture* myObj = (CLEyeCameraCapture*) obj; //recast 
			myObj->mode = position;
		}	
		
		void setupWindows();
		void setupParams();
		void showFPS();
		void colorFiltering();
		void opening();
		void find_ball();
		void destroyMyImages();

};


#endif//PS3EYECAM_H