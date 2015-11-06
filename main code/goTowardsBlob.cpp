#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "nasaroboteq.cpp"

#include <iostream>
#include <ctype.h>

//as;gl
using namespace cv;
using namespace std;
Mat hsvImage, hsvOutputImage;
Mat dstHue, dstSat, dstVal;
vector<Mat> hsv_planes;
int h_low=0;         //for orange, =0
int h_high=20;       //for orange, =20
int s_low=50;
int s_high=110;
int v_low=0;
int v_high=255;

/*roboteq.h Definitions*/
int roboteqPort = 6;
int leftChannelNum = 2;//Can be 1 or 2 check physical connection
int rightChannelNum = 1;//Can be 1 or 2 check physical connection
/*joystick.h Definitions*/
struct JoystickData joy0;

void trackbarInit()
{
    namedWindow( "HSV Detection", 1 );
    createTrackbar("Hue High", "HSV Detection", &h_high, 256);
    createTrackbar("Hue Low", "HSV Detection", &h_low, 256);
    createTrackbar("Sat High", "HSV Detection", &s_high, 256);
    createTrackbar("Sat Low", "HSV Detection", &s_low, 256);
    createTrackbar("Value High", "HSV Detection", &v_high, 256);
    createTrackbar("Value Low", "HSV Detection", &v_low, 256);
}

int main( int argc, char** argv )
{
    initRoboteq();//Refer to roboteq.c
    VideoCapture cap;
    if( argc == 1)
    {
        cap.open(argc == 2); //initialize camera: argc==2: laptop webcam; argc==1: external webcam
    }
    if( !cap.isOpened() )
    {
        cout << "Could not initialize capturing...\n";
        return 0;
    }
    trackbarInit();

    while(1) {
    static int lspeed, rspeed;
    Mat frame;
    cap>>frame;
    frame.copyTo(hsvImage);
    split(hsvImage, hsv_planes);
    inRange(hsv_planes[0], Scalar(h_low), Scalar(h_high), dstHue);    //Sets all pixels with Hue value in between the threshold values to 1, everything else to 0
    inRange(hsv_planes[1], Scalar(s_low), Scalar(s_high), dstSat);    //Does the same, but for Saturation values
    inRange(hsv_planes[2], Scalar(v_low), Scalar(v_high), dstVal);
    hsvOutputImage=dstHue&dstSat&dstVal;

    Mat temp;
    hsvOutputImage.copyTo(temp);
    vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	int xPos, yPos;
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE ); //find outlines of blobs
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		double area[numObjects];
		Moments moment[numObjects];
            for (int index = 0; index >= 0; index = hierarchy[index][0]) {
                    moment[index] = moments((cv::Mat)contours[index]); //get pixel intensity moments of blobs
                    area[index] = moment[index].m00;
            }
            int maxArea=area[0];
            int areaIndex=0;
            for (int i=1; i<numObjects; i++){   //finding biggest blob
                if (area[i]>maxArea){
                    maxArea=area[i];
                    areaIndex=i;
                }
            }
            if (maxArea>20*20){     //make sure largest blob is big enough to be significant and not noise
                xPos=moment[areaIndex].m10/area[areaIndex];
                yPos=moment[areaIndex].m01/area[areaIndex];
            }
            cv::circle(hsvOutputImage,cv::Point(xPos,yPos),10,cv::Scalar(0,0,255));  //draw circle over center of largest blob
            //Getting speeds to sent to Roboteq
            int width=frame.cols;
            float halfWidth=width/2;
            float xDiff=xPos-halfWidth;
            //cout << "Difference in X: " << xDiff << "\n";
            if (numObjects>0){
                lspeed=(int)(200+(xDiff/halfWidth)*100);
                rspeed=(int)(200-(xDiff/halfWidth)*100);
            }
            else{
                    lspeed=rspeed=0;
            }
            sendspeed(lspeed, rspeed);

	}

    imshow("HSV_thresholded image", hsvOutputImage);
    //imshow("Image", frame);
    if( waitKey(1) == 27 ) break; // stop capturing by pressing ESC
    }
}



