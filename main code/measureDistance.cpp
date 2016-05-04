#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <ctype.h>

#define OBJ_HEIGHT 760 //physical height of reference object in mm
#define CAM_HEIGHT 2   //physical height of imaging sensor of camera in mm

#define MIN_AREA (10*10)    //smallest area of blob before it becomes noise
//#define MAX_AREA (210*210)  //biggest area of blob before robot collides with it

using namespace cv;
using namespace std;

int h_low=20;         //hue is the color. For yellow, =20
int h_high=30;       //for yellow, =30
int s_low=220;      //saturation is the amount of a color
int s_high=256;
int v_low=160;      //value is the shininess of the color
int v_high=255;

int morphSize=3;

struct blob {
int xPos;
int yPos;
int maxArea;
//float lturn;
//float rturn;
bool blobDetect;
};

void trackbarInit();
void blobCenter(Mat&);

int main( int argc, char** argv )
{
    VideoCapture cap;
    blob blobPosition;
    Mat bgrImage, hsvImage, hsvOutImage, temp;


    cap.open(1); //0:default web cam   1:external web cam
    trackbarInit();

    int fin=1;
    while(fin>=1) {
        cap>>bgrImage;
        imshow("bgr", bgrImage);
        //Canny(bgrImage, temp, 50, 200, 3);
        //cvtColor(bgrImage, hsvImage, COLOR_BGR2HSV);
        //imshow("canny", temp);
        //temp=getGaussianKernel(5, 1.4);
        GaussianBlur(bgrImage, hsvImage, Size(5,5), 1.4, 0);
        imshow("blur", hsvImage);
        //inRange(hsvImage,Scalar(h_low,s_low,v_low), Scalar(h_high,s_high,v_high), hsvOutImage);

        //hsvOutImage.copyTo(temp);


       /* imshow("After Thresh", hsvOutImage);
        Mat dilateElement=getStructuringElement(MORPH_RECT, Size(morphSize,morphSize));
        dilate(hsvOutImage,hsvOutImage,dilateElement);
        dilate(hsvOutImage,hsvOutImage,dilateElement);
        imshow("After Morph", hsvOutImage);

        blobCenter(hsvOutImage);*/
        //cv::circle(hsvOutImage,cv::Point(blobPosition.xPos,blobPosition.yPos),10,cv::Scalar(0,0,0));
        //imshow("Final", hsvOutImage);

        if( waitKey(1) == 27 ) break;
    }
}
void trackbarInit()
{
    namedWindow( "Blob Detection", 1 );
    createTrackbar("Hue High",   "Blob Detection", &h_high, 256);
    createTrackbar("Hue Low",    "Blob Detection", &h_low,  256);
    createTrackbar("Sat High",   "Blob Detection", &s_high, 256);
    createTrackbar("Sat Low",    "Blob Detection", &s_low,  256);
    createTrackbar("Value High", "Blob Detection", &v_high, 256);
    createTrackbar("Value Low",  "Blob Detection", &v_low,  256);
    createTrackbar("Morph Size",  "Blob Detection", &morphSize,  9);
}

//blob blobCenter(Mat &temp)   {//, int width) {
void blobCenter(Mat &temp) {
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    blob blobPos;
    //float xDiff, halfWidth;
    //halfWidth=width/2;
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
        Rect boundRect = boundingRect(contours[areaIndex]);
        cout<<"Height: "<<boundRect.height<<endl;
        rectangle(temp, boundRect, Scalar(255), 1, 8, 0);
        //drawContours( temp, contours, areaIndex, Scalar(255), CV_FILLED, 8, hierarchy );

        //cout << "Max Area is: "<< blobPos.maxArea << "\n";
        if (blobPos.maxArea>MIN_AREA) {     //make sure largest blob is big enough to be significant and not noise
            blobPos.xPos=moment[areaIndex].m10/area[areaIndex];
            blobPos.yPos=moment[areaIndex].m01/area[areaIndex];
            blobPos.blobDetect=true;
        } else {
            blobPos.xPos=-8;
            blobPos.yPos=-8;
            blobPos.blobDetect=false;
        }
        cv::circle(temp,cv::Point(blobPos.xPos,blobPos.yPos),10,cv::Scalar(0,0,0));
        /*if (blobPos.maxArea<MAX_AREA) {
                if (blobPos.xPos>=0){
                    xDiff=blobPos.xPos-halfWidth;
                    blobPos.lturn=1-(xDiff/halfWidth);
                    blobPos.rturn=1+(xDiff/halfWidth);
                } else {
                    blobPos.lturn=0;
                    blobPos.rturn=0;
                }
        }*/
    } else {
        blobPos.xPos=-8;
        blobPos.yPos=-8;
        //blobPos.lturn=0;
        //blobPos.rturn=0;
        blobPos.maxArea=0;
        blobPos.blobDetect=false;
        //cout<<"No blobs detected\n";
    }
    //return(temp);
}
