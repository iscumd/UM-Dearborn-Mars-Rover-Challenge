#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "nasaroboteq.cpp"

#include <iostream>
#include <ctype.h>
#define MAX_SPEED (200)
#define MIN_AREA (10*10)
#define MAX_AREA (90*90)

using namespace cv;
using namespace std;

int h_low=0;         //hue is the color. For orange, =0
int h_high=20;       //for orange, =20
int s_low=160;      //saturation is the amount of a color
int s_high=256;
int v_low=130;      //value is the shininess of the color
int v_high=255;

/*roboteq.h Definitions*/
int roboteqPort = 6;
int leftChannelNum = 2;//Can be 1 or 2 check physical connection
int rightChannelNum = 1;//Can be 1 or 2 check physical connection
/*joystick.h Definitions*/
struct JoystickData joy0;

struct blob {
int xPos;
int yPos;
int maxArea;
};

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

void morphOps(Mat &thresh)
{
    Mat erodeElement=getStructuringElement(MORPH_RECT, Size(3,3));
    Mat dilateElement=getStructuringElement(MORPH_RECT, Size(6,6));

    erode(thresh,thresh,erodeElement);
    erode(thresh,thresh,erodeElement);

    dilate(thresh,thresh,dilateElement);
    dilate(thresh,thresh,dilateElement);
}

blob blobCenter(Mat temp) {
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    blob blobPos;
    findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE ); //find outlines of blobs
    if (hierarchy.size() > 0)   //Checks to see if at least one blob is detected
    {
        int numObjects = hierarchy.size();
        //cout << "Number of objects found is: "<< numObjects << "\n";
        double area[numObjects];
        Moments moment[numObjects];
        for (int index = 0; index >= 0; index = hierarchy[index][0])
        {
            moment[index] = moments((cv::Mat)contours[index]); //get pixel intensity moments of blobs
            area[index] = moment[index].m00;
        }
        blobPos.maxArea=area[0];
        int areaIndex=0;
        for (int i=1; i<numObjects; i++)    //look through all the blobs found and get the area and index of biggest one
        {
            if (area[i]>blobPos.maxArea)
            {
                blobPos.maxArea=area[i];
                areaIndex=i;
            }
        }
        cout << "Max Area is: "<< blobPos.maxArea << "\n";
        if (blobPos.maxArea>MIN_AREA)      //make sure largest blob is big enough to be significant and not noise
        {
            blobPos.xPos=moment[areaIndex].m10/area[areaIndex];
            blobPos.yPos=moment[areaIndex].m01/area[areaIndex];
        }
    }
    else
    {
        blobPos.xPos=-8;
        blobPos.yPos=-8;
        cout<<"No blobs detected\n";
    }
    return(blobPos);
}

int main( int argc, char** argv )
{
    initRoboteq();//Refer to roboteq.c
    VideoCapture cap;
    blob blobPosition;
    Mat frame, temp;
    Mat bgrImage, hsvImage, hsvOutputImage;
//    Mat dstHue, dstSat, dstVal;
//    vector<Mat> hsv_planes;
    static int lspeed, rspeed;
    int x,y;
    int mArea;
    bool useMorphOps=true;
//    if( argc == 1)
//    {
//        cap.open(argc == 2); //initialize camera: argc==2: laptop webcam; argc==1 or argc==3: external webcam
//    }
//    if( !cap.isOpened() )
//    {
//        cout << "Could not initialize capturing...\n";
//        return 0;
//    }
    cap.open(0); //0:default web cam location
    trackbarInit();

    int fin=1;
    while(fin==1)
    {
        cap>>frame;
        frame.copyTo(bgrImage);
        cvtColor(bgrImage, hsvImage, COLOR_BGR2HSV);
        split(hsvImage, hsv_planes);
//        inRange(hsv_planes[0], Scalar(h_low), Scalar(h_high), dstHue);    //Sets all pixels with Hue value in between the threshold values to 1, everything else to 0
//        inRange(hsv_planes[1], Scalar(s_low), Scalar(s_high), dstSat);    //Does the same, but for Saturation values
//        inRange(hsv_planes[2], Scalar(v_low), Scalar(v_high), dstVal);
//        hsvOutputImage=dstHue&dstSat&dstVal;
        inRange(hsvImage,Scalar(h_low,s_low,v_low), Scalar(h_high,s_high,v_high), hsvOutputImage);

        if(useMorphOps) morphOps(hsvOutputImage);

        hsvOutputImage.copyTo(temp);
        blobPosition=blobCenter(temp);
        if(blobPosition.xPos>=0 && blobPosition.yPos>=0)
        {
            x=blobPosition.xPos;
            y=blobPosition.yPos;
            mArea=blobPosition.maxArea;
            //cv::circle(frame,cv::Point(xPos,yPos),10,cv::Scalar(0,0,255));  //draw circle over center of largest blob
            cv::circle(hsvOutputImage,cv::Point(x,y),10,cv::Scalar(0,0,255));  //draw circle over center of largest blob
            //Getting speeds to sent to Roboteq
            int width=frame.cols;
            float halfWidth=width/2;
            float xDiff=x-halfWidth;
            //cout << "Difference in X: " << xDiff << "\n";
            if (mArea<MAX_AREA) //Check to see if the robot is too close to home base
            {                     //If it isn't, then move as usual
                lspeed=(int)(MAX_SPEED-(xDiff/halfWidth)*MAX_SPEED);
                rspeed=(int)(MAX_SPEED+(xDiff/halfWidth)*MAX_SPEED);
            }
            else    //If it is at the home base
            {       //Stop the robot and end the program
                lspeed=rspeed=0;
                fin=0;
                cout<<"Robot has reached home base"<<"\n";
            }
        }
        else
        {
            lspeed=rspeed=0;
        }
        //cout<<"X Position: "<<xPos<<" Y Position: "<<yPos<<"\n";
        //cout<<"Left Speed: "<<lspeed<<" Right Speed: "<<rspeed<<"\n";
        //sendspeed(lspeed, rspeed);
        imshow("HSV_thresholded image", hsvOutputImage);
        if( waitKey(1) == 27 ) break; // stop capturing by pressing ESC
        }
}
