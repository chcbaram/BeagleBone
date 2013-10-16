
//-- For OpenNI2
//
#include <stdio.h>
#include <OpenNI.h>

#include "OniSampleUtilities.h"

//-- For OpenCV
//
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <time.h>

#include "cv.h"
#include "highgui.h"


#define DEPTH_IMG_WIDTH		320
#define DEPTH_IMG_HEIGHT	240
#define DEPTH_IMG_FPS		30



#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

using namespace openni;


clock_t	start_point, end_point; 
struct 	termios orig_termios;



int main()
{
	IplImage *image;
	IplImage *img_dst;
	
	cv::Mat MatImage;
	
	double 	process_time;
	int 	process_rate;

	clock_t start_point;
	clock_t end_point;
	
	
		
	
	Status rc = OpenNI::initialize();
	if (rc != STATUS_OK)
	{
		printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
		return 1;
	}

	Device device;
	rc = device.open(ANY_DEVICE);
	if (rc != STATUS_OK)
	{
		printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
		return 2;
	}

	VideoStream depth;

	if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
	{
		rc = depth.create(device, SENSOR_DEPTH);
		if (rc != STATUS_OK)
		{
			printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
			return 3;
		}
	}

	VideoMode mode = depth.getVideoMode();
    mode.setResolution(DEPTH_IMG_WIDTH, DEPTH_IMG_HEIGHT);
	mode.setFps(DEPTH_IMG_FPS);
    depth.setVideoMode(mode);
    depth.setMirroringEnabled(false);
      

	rc = depth.start();
	if (rc != STATUS_OK)
	{
		printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
		return 4;
	}

	VideoFrameRef frame;

	//-- OpenCV 초기화 
	//
	//image = cvCreateImage(cvSize(DEPTH_IMG_WIDTH,DEPTH_IMG_HEIGHT), 8, 3);
	
	
	while (!wasKeyboardHit())
	{
		start_point = clock();
		
		
		int changedStreamDummy;
		VideoStream* pStream = &depth;
	
		rc = OpenNI::waitForAnyStream(&pStream, 1, &changedStreamDummy, SAMPLE_READ_WAIT_TIMEOUT);
		if (rc != STATUS_OK)
		{
			printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
			continue;
		}

		rc = depth.readFrame(&frame);
		if (rc != STATUS_OK)
		{
			printf("Read failed!\n%s\n", OpenNI::getExtendedError());
			continue;
		}

		if (frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_1_MM && frame.getVideoMode().getPixelFormat() != PIXEL_FORMAT_DEPTH_100_UM)
		{
			printf("Unexpected frame format\n");
			continue;
		}


		MatImage = cv::Mat(depth.getVideoMode().getResolutionY(),
                           depth.getVideoMode().getResolutionX(),
                           CV_16U,
                           (char *) frame.getData());
        MatImage.convertTo(MatImage, CV_8U, 256.0 / 10000);
		cv::equalizeHist(MatImage, MatImage);
    	bitwise_not(MatImage, MatImage);
                        
		//image = &IplImage(MatImage);      
		//image = &(MatImage.operator IplImage());
		
		//cvSaveImage("/srv/depth.jpg",image);      
		imwrite("/mnt/ramdisk/depth.jpg",MatImage);      


		/*
		cv::Mat depth(240,320,CV_16UC1,(void*)frame.getData(), 2*320);
		cv::Mat depthNorm;
		cv::normalize(depth, depthNorm, 0, 255, CV_MINMAX, CV_8UC1);

		cv::Mat bgrMat,rgbMat(240,320,CV_8UC3,(void*)frame.getData(),3*320);
		
		cv::cvtColor(rgbMat,bgrMat, CV_RGB2BGR);// opencv expects the image in BGR format
		*/
		

		//DepthPixel* pDepth = (DepthPixel*)frame.getData();
		//int middleIndex = (frame.getHeight()+1)*frame.getWidth()/2;


		end_point = clock();
		process_time = ((double)(end_point - start_point)/(CLOCKS_PER_SEC/1000));
		//printf("Exe time : %04f msec  %03.1f frames\r\n", process_time, 1000./process_time );
		printf("H : %d, W : %d  %03.1f frames\n", frame.getHeight(), frame.getWidth(), 1000./process_time);

		//printf("[%08llu] %8d\n", (long long)frame.getTimestamp(), pDepth[middleIndex]);
	}

	depth.stop();
	depth.destroy();
	device.close();
	OpenNI::shutdown();

	// OpenCV 종료
	//
	cvReleaseImage(&image);
	
	return 0;
}
