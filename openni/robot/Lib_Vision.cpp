
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

#include "main.h"
#include "./Main_Lib/Thread_Lib.h"  


#include "cv.h"
#include "highgui.h"


#include "./cvblob_lib/cvblob.h"

using namespace cvb;
using namespace openni;


clock_t	start_point, end_point; 
clock_t start_point_frame;
clock_t end_point_frame;




#define SAMPLE_READ_WAIT_TIMEOUT	2000



int Lib_Vision_Debug = 0;


//-- OpenNI 변수 
//
VideoStream depth, color;
Device device;

IplImage *IplImage_depth;
IplImage *IplImage_color;

	
cv::Mat MatImage_depth;
cv::Mat MatImage_color;


VideoFrameRef *frame_ptr;	
VideoFrameRef  frame_depth;
VideoFrameRef  frame_color;	
VideoStream*   streams[] = {&depth, &color};


int CapturedFlag = 0;


double 	process_time;
int 	process_rate;
double  process_time_depth;
double  process_time_color;
double  process_time_frame;




int OpenNI_Init(void);
int OpenNI_Close(void);
int OpenNI_Capture( THREAD_OBJ *pArg );

int Tracking_Color( THREAD_OBJ *pArg );
int Detect_Red( THREAD_OBJ *pArg );



extern int Lib_Motor_PwmLeft;
extern int Lib_Motor_PwmRight;


void *Lib_Vision(void *Arg)
{
	THREAD_OBJ *pArg = (THREAD_OBJ *)Arg;

	printf("Enter Vision \n");


	//pArg->Thread_Ret = Tracking_Color( pArg );
	pArg->Thread_Ret = Detect_Red( pArg );


	printf("Exit Vision Ret : %d\n", pArg->Thread_Ret);

	return (void *)&(pArg->Thread_Ret);
}





int Detect_Red( THREAD_OBJ *pArg )
{
	char 	ch;
	double 	process_time;
	int 	process_rate;

	clock_t start_point;
	clock_t end_point;
		
	// 프레임 포인터
	IplImage *frame;
	IplImage *image;
	IplImage *img_bin;	
	IplImage *img_red;
	IplImage *img_green;
	IplImage *img_blue;
	IplImage *img_dst;
	
	int threadhold_red_value;
	int threadhold_bin_value;
	
	
	image = cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 3);

	img_bin 	= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);	
	img_red 	= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	img_green 	= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	img_blue 	= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	
	img_dst		= cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT), 8, 1);
	
	threadhold_red_value = 120;	// 120
	threadhold_bin_value = 165;	// 220
		

	if( OpenNI_Init() > 0 )
	{
		printf("OpenNI Init Failed\n");

		pArg->Thread_Ret = 0;
	
		return 1;
	}
	else
	{
		printf("OpenNI Init OK\n");
	}


	//-- 알고리즘 실행 
	//
	while ( pArg->Thread_Stop == FALSE )
	{
		start_point = clock();

		if( OpenNI_Capture( pArg ) != TRUE )
		{
			continue;
		}	

		
		// 카메라로 부터 1프레임 이미지를 읽어온다.
		frame = IplImage_color;

		// YCrCb 형태로 변경
		cvCvtColor(frame, image, CV_RGB2YCrCb);
		
		// 채널별로 분리
		cvSplit(image, img_blue, img_green, img_red, NULL);
		cvMaxS(img_red, threadhold_red_value, img_red);
		

		// 이진화
		cvThreshold(img_red, img_bin, threadhold_bin_value, 255, CV_THRESH_BINARY);
		
		// 팽창
		cvDilate(img_bin, img_dst, NULL, 4);
		
		// 침식
		cvErode(img_dst, img_dst, NULL, 4);
		
		// 모멘트 구하기
		CvMoments moments;
		
		int binary = 1;
		
		cvMoments(img_dst, &moments, binary);
		
		
		
		double M;
		int x_order;
		int y_order;
		double cX, cY;
		double m00;
		
		for( y_order=0; y_order<=3; y_order++ )
		{
			for( x_order=0; x_order<=3; x_order++ )
			{
				if( x_order+y_order > 3 )
				{
					continue;
				}
				
				M = cvGetSpatialMoment( &moments, x_order, y_order );
				
				if( x_order == 0 && y_order == 0 )
				{
					m00 = M;
				}
				else if( x_order == 1 && y_order == 0 )
				{
					cX = M;
				}
				else if( x_order == 0 && y_order == 1 )
				{
					cY = M;
				}
			}
		}
		
		cX = cX / m00;
		cY = cY / m00;
		
		cvCircle(frame, cvPoint(cvRound(cX), cvRound(cY)), 20, CV_RGB(0, 0, 255), 2);
		
		int x_offset;
		
		x_offset = cX - (IMG_WIDTH/2);

		Lib_Motor_PwmLeft  =  x_offset;
		Lib_Motor_PwmRight = -x_offset; 
		
		// Labeling
		// 구형 예정
		
		
		//cvSaveImage("/mnt/ramdisk/depth/depth.jpg",IplImage_depth);  
		cvSaveImage("/mnt/ramdisk/color/color.jpg",frame); 

		
		end_point = clock();
		process_time = ((double)(end_point - start_point)/(CLOCKS_PER_SEC/1000));

		if( Lib_Vision_Debug == 1 )
		{
			printf("Exe time : %04f msec  %03.1f frames\r\n", process_time, 1000./process_time ); 
		}
		
	}
	

	// Release All Images and Windows
	cvReleaseImage(&image);
	cvReleaseImage(&img_bin);
	cvReleaseImage(&img_red);
	cvReleaseImage(&img_green);
	cvReleaseImage(&img_blue);
	cvReleaseImage(&img_dst);

	OpenNI_Close();

	return 0;
}





int Tracking_Color( THREAD_OBJ *pArg )
{
	if( OpenNI_Init() > 0 )
	{
		printf("OpenNI Init Failed\n");

		pArg->Thread_Ret = 0;
	
		return 1;
	}
	else
	{
		printf("OpenNI Init OK\n");
	}


	//-- 알고리즘 실행 
	//
	while ( pArg->Thread_Stop == FALSE )
	{
		if( OpenNI_Capture( pArg ) == TRUE )
		{
			//printf("Captured\n");
		}	
	}


	OpenNI_Close();

	return 0;
}




int OpenNI_Capture( THREAD_OBJ *pArg )
{
	Status rc;
	int readyStream;

	start_point = clock();

	
	while ( pArg->Thread_Stop == FALSE )
	{
		readyStream = -1;

		rc = OpenNI::waitForAnyStream(streams, 2, &readyStream, SAMPLE_READ_WAIT_TIMEOUT);
		if (rc != STATUS_OK)
		{
			printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
			return FALSE;
		}

		
		switch (readyStream)
		{
		case 0:
			// Depth
			rc = depth.readFrame(&frame_depth);
			CapturedFlag |= (1<<0);

			if (rc != STATUS_OK)
			{
				printf("Read depth failed!\n%s\n", OpenNI::getExtendedError());
				continue;
			}			

			frame_ptr = &frame_depth;
			break;
		case 1:
			// Color
			rc = color.readFrame(&frame_color);
			CapturedFlag |= (1<<1);
			if (rc != STATUS_OK)
			{
				printf("Read color failed!\n%s\n", OpenNI::getExtendedError());
				continue;
			}			

			frame_ptr = &frame_color;
			break;
		default:
			printf("Unxpected stream\n");
			continue;
			break;
		}
		

		switch (frame_ptr->getVideoMode().getPixelFormat())
		{
		case PIXEL_FORMAT_DEPTH_1_MM:
		case PIXEL_FORMAT_DEPTH_100_UM:
			//rc = depth.readFrame(&frame);
			//printf("Depth Captured %d\n", Cnt++);
			//continue;
			break;

		case PIXEL_FORMAT_RGB888:
			//printf("Color Captured %d\n", Cnt++ );
			//continue;
			break;
		default:
			printf("Unknown format\n");
			continue;
			break;
		}



		if( readyStream == 0 )
		{
			MatImage_depth = cv::Mat(depth.getVideoMode().getResolutionY(),
	                           depth.getVideoMode().getResolutionX(),
	                           CV_16U,
	                           (char *) frame_depth.getData());
	        MatImage_depth.convertTo(MatImage_depth, CV_8U, 255.0/2000.0); 

			IplImage_depth->imageData = (char *) MatImage_depth.data;   
		}
		else
		{
			const openni::RGB888Pixel* imageBuffer = (const openni::RGB888Pixel*)frame_color.getData();

   			MatImage_color.create(frame_color.getHeight(), frame_color.getWidth(), CV_8UC3);
			memcpy( MatImage_color.data, imageBuffer, 3*frame_color.getHeight()*frame_color.getWidth()*sizeof(uint8_t) );

			cv::cvtColor(MatImage_color,MatImage_color,CV_BGR2RGB); //this will put colors right	

			IplImage_color->imageData = (char *) MatImage_color.data;  
		}


		if( CapturedFlag == 0x03 )
		{	
			//Object_Tracking(IplImage_color, labelImg, segmentated, morphKernel);

			//cvSaveImage("/mnt/ramdisk/depth/depth.jpg",IplImage_depth);  
			//cvSaveImage("/mnt/ramdisk/color/color.jpg",IplImage_color);  

			//imwrite("/mnt/ramdisk/depth/depth.jpg",MatImage_depth);      
			//imwrite("/mnt/ramdisk/color/color.jpg",MatImage_color); 

			CapturedFlag = 0;
			//Sleep(200);	

			end_point_frame = clock();
			process_time_frame = ((double)(end_point_frame - start_point_frame)/(CLOCKS_PER_SEC/1000));

			if( Lib_Vision_Debug == 1 )
			{
				/*
				printf(" %3.1f frames, D : %03.1f, C : %03.1f \n", 1000./process_time_frame,
																   1000./process_time_depth,
																   1000./process_time_color );
				*/
			}

			start_point_frame = clock();

			break;
		}


	}

	return TRUE;
}




int OpenNI_Init(void)
{
	Status rc;


	//-- OpenNI 초기화 
	//
	rc = OpenNI::initialize();
	if (rc != STATUS_OK)
	{
		printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
		return 1;
	}

	
	rc = device.open(ANY_DEVICE);
	if (rc != STATUS_OK)
	{
		printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
		return 2;
	}


	if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
	{
		rc = depth.create(device, SENSOR_DEPTH);
		if (rc != STATUS_OK)
		{
			printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
			return 3;
		}
	}

	if (device.getSensorInfo(SENSOR_COLOR) != NULL)
	{
		rc = color.create(device, SENSOR_COLOR);
		if (rc != STATUS_OK)
		{
			printf("Couldn't create color stream\n%s\n", OpenNI::getExtendedError());
		}
	}

	device.setDepthColorSyncEnabled(true);


	//-- Start 
	//
	VideoMode mode_depth = depth.getVideoMode();

    mode_depth.setResolution(DEPTH_IMG_WIDTH, DEPTH_IMG_HEIGHT);
	mode_depth.setFps(DEPTH_IMG_FPS);
    depth.setVideoMode(mode_depth);
    depth.setMirroringEnabled(false);
      
	rc = depth.start();
	if (rc != STATUS_OK)
	{
		printf("Couldn't start the depth stream\n%s\n", OpenNI::getExtendedError());
		return 4;
	}

	VideoMode mode_color = color.getVideoMode();

    mode_color.setResolution(COLOR_IMG_WIDTH, COLOR_IMG_HEIGHT);
	mode_color.setFps(COLOR_IMG_FPS);
    color.setVideoMode(mode_color);
    color.setMirroringEnabled(false);

 	
	rc = color.start();
	if (rc != STATUS_OK)
	{
		printf("Couldn't start the color stream\n%s\n", OpenNI::getExtendedError());
		return 4;
	}
	


	IplImage_depth = cvCreateImage(cvSize(DEPTH_IMG_WIDTH,DEPTH_IMG_HEIGHT), IPL_DEPTH_8U, 1);
	IplImage_color = cvCreateImage(cvSize(COLOR_IMG_WIDTH,COLOR_IMG_HEIGHT), IPL_DEPTH_8U, 3);


	CapturedFlag = 0;

	return 0;
}





int OpenNI_Close(void)
{
	depth.stop();
	depth.destroy();
	color.stop();
	color.destroy();


	device.close();
	OpenNI::shutdown();

	// OpenCV 종료
	//
	cvReleaseImage(&IplImage_depth);
	cvReleaseImage(&IplImage_color);	

	return 0;
}





#if 0
int Object_Tracking(IplImage *frame, IplImage *labelImg, IplImage *segmentated, IplConvKernel* morphKernel)
{
	CvTracks tracks;
	CvBlobs blobs;
	unsigned int blobNumber = 0;
	bool quit = false;
	CvSize imgSize = cvGetSize(frame);

	//segmentated = cvCreateImage(imgSize, 8, 1);

	printf("Step 1 %d %d\n", imgSize.width, imgSize.height );

  // Detecting red pixels:
    // (This is very slow, use direct access better...)
	for (unsigned int j=0; j<imgSize.height; j++)
		for (unsigned int i=0; i<imgSize.width; i++)
		{
			CvScalar c = cvGet2D(frame, j, i);

			double b = ((double)c.val[0])/255.;
			double g = ((double)c.val[1])/255.;
			double r = ((double)c.val[2])/255.;
			unsigned char f = 255*((r>0.2+g)&&(r>0.2+b));
			//	unsigned char f = 255*((g>0.2+b)&&(g>0.2+r));

			cvSet2D(segmentated, j, i, CV_RGB(f, f, f));
		}

	printf("Step 2\n");

	cvMorphologyEx(segmentated, segmentated, NULL, morphKernel, CV_MOP_OPEN, 1);

	printf("Step 3\n");

    //cvShowImage("segmentated", segmentated);


	printf("Step 4\n");

    unsigned int result = cvLabel(segmentated, labelImg, blobs);

	printf("Step 5\n");

    cvFilterByArea(blobs, 500, 1000000);

	printf("Step 6\n");

    cvRenderBlobs(labelImg, blobs, frame, frame, CV_BLOB_RENDER_BOUNDING_BOX);

	printf("Step 7\n");

    cvUpdateTracks(blobs, tracks, 200., 5);

	printf("Step 8\n");

    cvRenderTracks(tracks, frame, frame, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX);

	printf("Step 9\n");


//    cvShowImage("red_object_tracking", frame);

	cvReleaseBlobs(blobs);
  
  
	return 0;
}






void calculateHistogram(float* pHistogram, int histogramSize, const openni::VideoFrameRef& frame)
{
	const openni::DepthPixel* pDepth = (const openni::DepthPixel*)frame.getData();
	// Calculate the accumulative histogram (the yellow display...)
	memset(pHistogram, 0, histogramSize*sizeof(float));
	int restOfRow = frame.getStrideInBytes() / sizeof(openni::DepthPixel) - frame.getWidth();
	int height = frame.getHeight();
	int width = frame.getWidth();

	unsigned int nNumberOfPoints = 0;
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x, ++pDepth)
		{
			if (*pDepth != 0)
			{
				pHistogram[*pDepth]++;
				nNumberOfPoints++;
			}
		}
		pDepth += restOfRow;
	}
	for (int nIndex=1; nIndex<histogramSize; nIndex++)
	{
		pHistogram[nIndex] += pHistogram[nIndex-1];
	}
	if (nNumberOfPoints)
	{
		for (int nIndex=1; nIndex<histogramSize; nIndex++)
		{
			pHistogram[nIndex] = (256 * (1.0f - (pHistogram[nIndex] / nNumberOfPoints)));
		}
	}
}
#endif