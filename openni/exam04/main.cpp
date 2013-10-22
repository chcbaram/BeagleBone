
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


#include "./cvblob_lib/cvblob.h"

using namespace cvb;


#define DEPTH_IMG_WIDTH		320		//(320)
#define DEPTH_IMG_HEIGHT	240		//(240)
#define DEPTH_IMG_FPS		30

#define COLOR_IMG_WIDTH		320		//(320)
#define COLOR_IMG_HEIGHT	240		//(240)
#define COLOR_IMG_FPS		30


#define SAMPLE_READ_WAIT_TIMEOUT 2000 //2000ms

using namespace openni;


clock_t	start_point, end_point; 
struct 	termios orig_termios;


//-- OpenNI 변수 
//
VideoStream depth, color;
Device device;


int OpenNI_Init(void);
int Object_Tracking(IplImage *frame, IplImage *labelImg, IplImage *segmentated, IplConvKernel* morphKernel);





int main()
{
	IplImage *IplImage_depth;
	IplImage *IplImage_color;

	
	cv::Mat MatImage_depth;
	cv::Mat MatImage_color;
	

	double 	process_time;
	int 	process_rate;

	clock_t start_point;
	clock_t end_point;
	
	Status rc;

	int Cnt = 0;
	int CapturedFlag;
	
	if( OpenNI_Init() > 0 )
	{
		printf("OpenNI Init Failed\n");
		return 1;
	}
	else
	{
		printf("OpenNI Init OK\n");
	}

	IplImage_depth = cvCreateImage(cvSize(DEPTH_IMG_WIDTH,DEPTH_IMG_HEIGHT), IPL_DEPTH_8U, 1);
	IplImage_color = cvCreateImage(cvSize(COLOR_IMG_WIDTH,COLOR_IMG_HEIGHT), IPL_DEPTH_8U, 3);



	IplImage *labelImg=NULL;
    IplImage *segmentated = NULL;
	IplConvKernel* morphKernel = cvCreateStructuringElementEx(5, 5, 1, 1, CV_SHAPE_RECT, NULL);


	VideoFrameRef *frame_ptr;	
	VideoFrameRef frame_depth;
	VideoFrameRef frame_color;	
	VideoStream*  streams[] = {&depth, &color};

	//-- OpenCV 초기화 
	//
	//image = cvCreateImage(cvSize(DEPTH_IMG_WIDTH,DEPTH_IMG_HEIGHT), 8, 3);
	
	CapturedFlag = 0;

	while (!wasKeyboardHit())
	{
		int readyStream = -1;

		start_point = clock();
		

		//VideoStream* pStream = &depth;
		//VideoStream* pStream = &color;


	

		rc = OpenNI::waitForAnyStream(streams, 2, &readyStream, SAMPLE_READ_WAIT_TIMEOUT);
		//rc = OpenNI::waitForAnyStream(&pStream, 1, &readyStream, SAMPLE_READ_WAIT_TIMEOUT);
		if (rc != STATUS_OK)
		{
			printf("Wait failed! (timeout is %d ms)\n%s\n", SAMPLE_READ_WAIT_TIMEOUT, OpenNI::getExtendedError());
			continue;
		}

	
		//rc = depth.readFrame(&frame);
		//rc = color.readFrame(&frame);


		
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
			printf("Depth Captured %d\n", Cnt++);
			//continue;
			break;

		case PIXEL_FORMAT_RGB888:
			/*
			pColor = (RGB888Pixel*)frame.getData();
			printf("[%08llu] 0x%02x%02x%02x\n", (long long)frame.getTimestamp(),
				pColor[middleIndex].r&0xff,
				pColor[middleIndex].g&0xff,
				pColor[middleIndex].b&0xff);
			*/
			//rc = color.readFrame(&frame);
			printf("Color Captured %d\n", Cnt++ );
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
	        //MatImage.convertTo(MatImage, CV_8U, 256.0 / 0xFFFF);
	        MatImage_depth.convertTo(MatImage_depth, CV_8U, 255.0/2000.0);
			//cv::equalizeHist(MatImage, MatImage);
	    	//bitwise_not(MatImage, MatImage);
	                        
			//image = &IplImage(MatImage);      
			//image = &(MatImage.operator IplImage());
			
			//cvSaveImage("/srv/depth.jpg",image);      
			//imwrite("/mnt/ramdisk/depth/depth.jpg",MatImage);   

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

			cvSaveImage("/mnt/ramdisk/depth/depth.jpg",IplImage_depth);  
			cvSaveImage("/mnt/ramdisk/color/color.jpg",IplImage_color);  

			//imwrite("/mnt/ramdisk/depth/depth.jpg",MatImage_depth);      
			//imwrite("/mnt/ramdisk/color/color.jpg",MatImage_color); 

			CapturedFlag = 0;
			//Sleep(200);		
		}
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

		if( readyStream == 0 )
		{
			printf("H : %d, W : %d  %03.1f frames \n", frame_depth.getHeight(), frame_depth.getWidth(), 1000./process_time );
		}
		else
		{
			printf("H : %d, W : %d  %03.1f frames \n", frame_color.getHeight(), frame_color.getWidth(), 1000./process_time );
		}
		//printf("[%08llu] %8d\n", (long long)frame.getTimestamp(), pDepth[middleIndex]);
	}

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
	//mode_depth.setFps(DEPTH_IMG_FPS);
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
	//mode_color.setFps(COLOR_IMG_FPS);
    color.setVideoMode(mode_color);
    color.setMirroringEnabled(false);

    printf("Ver 0.1\n");
	
	rc = color.start();
	if (rc != STATUS_OK)
	{
		printf("Couldn't start the color stream\n%s\n", OpenNI::getExtendedError());
		return 4;
	}
	

	return 0;
}





int Object_Tracking(IplImage *frame, IplImage *labelImg, IplImage *segmentated, IplConvKernel* morphKernel)
{
	CvTracks tracks;
	CvBlobs blobs;
	unsigned int blobNumber = 0;
	bool quit = false;
	CvSize imgSize = cvGetSize(frame);

	segmentated = cvCreateImage(imgSize, 8, 1);

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

    labelImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_LABEL, 1);

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
