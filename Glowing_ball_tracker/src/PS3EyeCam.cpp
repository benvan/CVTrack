#include "stdafx.h"
#include "SocketConnector.h"
#include "Ball.h"
#include "PS3EyeCam.h"

using namespace std;

CRITICAL_SECTION criticalSection;
Ball* ball;

void createSocket( void* parameter ){
    SocketConnector socketConnector(criticalSection, ball);
}

void CLEyeCameraCapture::setupWindows()
{
	is_ball_detected = false;

	param1 = 17;//h min
	param2 = 22;//s min
	param3 = 51;//v min
	param4 = 57;//h max
	param5 = 94;//s max
	param6 = 108;//v max

	param7 = 1;//erode
	param8 = 3;//dilate

	gain = 0;
	exposure = 511;
	mode = 0;

	w=0; h=0;

	cvNamedWindow("calibration_window", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("mask_window", CV_WINDOW_AUTOSIZE);

	cvCreateTrackbar2("minH","calibration_window",&param1,181,&CLEyeCameraCapture::switch_param1,this);		
	cvCreateTrackbar2("minS","calibration_window",&param2,256,&CLEyeCameraCapture::switch_param2,this);		
	cvCreateTrackbar2("minV","calibration_window",&param3,256,&CLEyeCameraCapture::switch_param3,this);		
	cvCreateTrackbar2("maxH","calibration_window",&param4,181,&CLEyeCameraCapture::switch_param4,this);		
	cvCreateTrackbar2("maxS","calibration_window",&param5,256,&CLEyeCameraCapture::switch_param5,this);		
	cvCreateTrackbar2("maxV","calibration_window",&param6,256,&CLEyeCameraCapture::switch_param6,this);		

	cvCreateTrackbar2("erode","calibration_window",&param7,10,&CLEyeCameraCapture::switch_param7,this);		
	cvCreateTrackbar2("dilate","calibration_window",&param8,10,&CLEyeCameraCapture::switch_param8,this);		

	cvCreateTrackbar2("Gain",_windowName,&gain,79,&CLEyeCameraCapture::switch_gain,this);		
	cvCreateTrackbar2("Exposure",_windowName,&exposure,511,&CLEyeCameraCapture::switch_exposure,this);

	cvCreateTrackbar2("mode",_windowName,&mode,5,&CLEyeCameraCapture::switch_mode,this);
}

bool CLEyeCameraCapture::StartCapture()
{
	cvNamedWindow(_windowName, CV_WINDOW_AUTOSIZE);

	////////////////////////////////////////////////
	setupWindows();
	////////////////////////////////////////////////

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

void CLEyeCameraCapture::StopCapture()
{
	if(!_running)	return;
	_running = false;
	WaitForSingleObject(_hThread, 500);
	cvDestroyWindow(_windowName);

	/////////////////////////////////
	cvDestroyWindow("calibration_window");
	cvDestroyWindow("mask_window");
	/////////////////////////////////
}

void CLEyeCameraCapture::setupParams()
{
	threshold_image = cvCreateImage(cvGetSize(pCapImage),IPL_DEPTH_8U,1);
    contour_image = cvCreateImage(cvGetSize(pCapImage),IPL_DEPTH_8U,1);
    imgHSV = cvCreateImage(cvGetSize(pCapImage), IPL_DEPTH_8U, 3);
	image_roi = Mat(w,h,CV_8UC1,Scalar(0,0,0));
    storageContours = cvCreateMemStorage(0);
	circles.resize(1); //sino peta en debug
	radius=9999;
	oldTime=0; newTime=0;
}

void CLEyeCameraCapture::colorFiltering()
{
	cvCvtColor( pCapImage, imgHSV, CV_BGR2HSV);
	min_color = Scalar(param1,param2,param3,0);
	max_color = Scalar(param4,param5,param6,0);
	cvInRangeS(imgHSV, min_color, max_color, threshold_image);
}

void CLEyeCameraCapture::opening()
{
	cvErode(threshold_image,threshold_image,0,param7);
	cvDilate(threshold_image,threshold_image,0,param8);
}

void CLEyeCameraCapture::find_ball()
{
	
	
	
    cvCopyImage(threshold_image, contour_image);

	cvFindContours(contour_image ,storageContours,&contours,sizeof(CvContour),CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
	CvSeq* tmpcontours=contours;

    double regMax = 0;
    CvPoint pt1,pt2;

	for( ; tmpcontours != 0; tmpcontours = tmpcontours->h_next ){

			// Approximates polygonal curves with desired precision
			CvSeq* result = cvApproxPoly(tmpcontours, sizeof(CvContour), storageContours, CV_POLY_APPROX_DP, cvContourPerimeter(tmpcontours)*0.01, 0);
				
			double reg = fabs(cvContourArea(result, CV_WHOLE_SEQ));

//          check if contour is big enough
			if( reg < (1000) ) { 
					//cout << "skip" << endl;
                    //printf("Contour is too small : %i\n", reg);
					continue;
						
			}

            if (reg > regMax){
                regMax = reg;
                CvRect rectEst = cvBoundingRect( tmpcontours, 0 );

                pt1.x = rectEst.x;
                pt1.y = rectEst.y;
                pt2.x = rectEst.x+ rectEst.width;
                pt2.y = rectEst.y+ rectEst.height;
            }
				
	}

    if (regMax != 0){
        int thickness =1 ;
        cvRectangle( pCapImage, pt1, pt2, CV_RGB(255,255,255 ), thickness );

        int hor = pt2.x - pt1.x;
        int vert = pt2.y - pt1.y;
        int size = max(hor,vert);

        int x = pt1.x + (hor / 2);
        int y = pt1.y + (vert / 2);

        // Make output readable
        fixed(cout);
        showpos(cout);
        cout.precision(2);

        cout << "Ball: ";
        cout << x << ",";
        cout << y << ",";
        cout << size << ") ";
        cout << endl;

        EnterCriticalSection(&criticalSection);
        ball->x  = x;
        ball->y  = y;
        ball->size  = size;
        LeaveCriticalSection(&criticalSection);

    }
    
	

	/*printf("X: %f\n",ball[0]);
	printf("Y: %f\n",ball[1]);
	printf("radius: %f\n",ball[2]);
	printf("\n");

	printf("X_filtered: %f\n",balltoPaint[0]);
	printf("Y_filtered: %f\n",balltoPaint[1]);
	printf("radius_filtered: %f\n",balltoPaint[2]);
	printf("\n");*/
	
	

	cvFlip(threshold_image,threshold_image,1);
	cvFlip(pCapImage,pCapImage,1);

	cvShowImage("mask_window", contour_image);
	cvShowImage("calibration_window",threshold_image);
	cvShowImage(_windowName, pCapImage);
}

void CLEyeCameraCapture::destroyMyImages()
{
	cvReleaseImage(&threshold_image);
    cvReleaseImage(&contour_image);
	cvReleaseImage(&imgHSV);
}

void CLEyeCameraCapture::showFPS()
{
	newTime = timeGetTime();
	float dt = newTime - oldTime;
	float fps = 1.0f / dt * 1000;
	oldTime = newTime;
	printf("fps: %0.f\n",fps);
	printf("dt: %0.f\n",dt);
	printf("\n");
}

void CLEyeCameraCapture::Run()
{
	pCapBuffer = NULL;

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
	CLEyeSetCameraParameter(_cam, CLEYE_GAIN, gain);
	CLEyeSetCameraParameter(_cam, CLEYE_EXPOSURE, exposure);

	// Start capturing
	CLEyeCameraStart(_cam);
	cvGetImageRawData(pCapImage, &pCapBuffer);

	//////////////////////////////////////
	setupParams();
	//////////////////////////////////////

	// image capturing loop
	while(_running)
	{
		CLEyeCameraGetFrame(_cam, pCapBuffer);

		////////////
		showFPS();
		////////////

		//////////////////////////////////////
		colorFiltering();
		opening();
		find_ball();
		//////////////////////////////////////
	}

	// Stop camera capture
	CLEyeCameraStop(_cam);

	// Destroy camera object
	CLEyeDestroyCamera(_cam);

	// Destroy the allocated OpenCV image
	cvReleaseImage(&pCapImage);

	//////////////////////////////////
	destroyMyImages();
	//////////////////////////////////

	_cam = NULL;
}

// Main program entry point
int main(int argc, char* argv[])
{
	CLEyeCameraCapture *cam = NULL;
	if(CLEyeGetCameraCount() == 0){printf("No PS3Eye cameras detected\n");	return -1;}

	// Create camera capture object
	cam = new CLEyeCameraCapture("Camera Window", CLEyeGetCameraUUID(0), CLEYE_COLOR_RAW, CLEYE_VGA, 60);
	//cam = new CLEyeCameraCapture("Camera Window", CLEyeGetCameraUUID(0), CLEYE_COLOR_RAW, CLEYE_QVGA, 125);

    InitializeCriticalSection(&criticalSection);

    ball = new Ball();
    ball->x = 0.5f;
    ball->y = 40.0f;
    ball->size = 40.0f;

    _beginthread(createSocket,0,NULL);
    
	cam->StartCapture();

	// The <ESC> key will exit the program
	int key;
	while((key = cvWaitKey(10)) != 0x1b){}

	
	printf("Stopping capture on camera\n");
	cam->StopCapture();
	delete cam;

	return 0;
}
