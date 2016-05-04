#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "nasaroboteq.cpp"

#include <iostream>
#include <ctype.h>
#define MAX_SPEED (200)
#define MIN_AREA (10*10)
#define MAX_AREA (210*210)

using namespace cv;
using namespace std;

int h_low=5;         //hue is the color. For orange, =0
int h_high=16;       //for orange, =20
int s_low=160;      //saturation is the amount of a color
int s_high=256;
int v_low=130;      //value is the shininess of the color
int v_high=255;

int h_low2=100;         //hue is the color. For blue, =100
int h_high2=120;       //for orange, =120
int s_low2=160;      //saturation is the amount of a color
int s_high2=256;
int v_low2=70;      //value is the shininess of the color
int v_high2=210;


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
float lturn;
float rturn;
bool blobDetect;
};

/*struct decision {
static int lspeed;
static int rspeed;
bool lookBlob1;
bool lookBlob2;
};*/
void trackbarInit()     //initialize trackbars
{
    namedWindow( "Blob 1 Detection", 1 );
    createTrackbar("Hue High",   "Blob 1 Detection", &h_high, 256);
    createTrackbar("Hue Low",    "Blob 1 Detection", &h_low,  256);
    createTrackbar("Sat High",   "Blob 1 Detection", &s_high, 256);
    createTrackbar("Sat Low",    "Blob 1 Detection", &s_low,  256);
    createTrackbar("Value High", "Blob 1 Detection", &v_high, 256);
    createTrackbar("Value Low",  "Blob 1 Detection", &v_low,  256);

    namedWindow("Blob 2 Detection", 1);
    createTrackbar("Hue High",   "Blob 2 Detection", &h_high2, 256);
    createTrackbar("Hue Low",    "Blob 2 Detection", &h_low2,  256);
    createTrackbar("Sat High",   "Blob 2 Detection", &s_high2, 256);
    createTrackbar("Sat Low",    "Blob 2 Detection", &s_low2,  256);
    createTrackbar("Value High", "Blob 2 Detection", &v_high2, 256);
    createTrackbar("Value Low",  "Blob 2 Detection", &v_low2,  256);
}

void morphOps(Mat &thresh)      //function to apply morphological operations to image to remove noise and make existing target easier to detect
{
    Mat erodeElement=getStructuringElement(MORPH_RECT, Size(3,3));
    Mat dilateElement=getStructuringElement(MORPH_RECT, Size(6,6));

    erode(thresh,thresh,erodeElement);
    erode(thresh,thresh,erodeElement);

    dilate(thresh,thresh,dilateElement);
    dilate(thresh,thresh,dilateElement);
}

void bw2color (Mat colorImage, Mat bwImage) {       //attempt to make areas black on colored image
    int cols = bwImage.cols;
    int rows =bwImage.rows;
    Mat outImage;
    int i,j;
    //cout<<"bw rows: "<<bwImage.rows<<"\tbw cols: "<<bwImage.cols<<"\n";
    //cout<<"color rows: "<<colorImage.rows<<"\tcolor cols: "<<colorImage.cols<<"\n";
    cvtColor(colorImage, outImage, COLOR_BGR2GRAY);
    for (i=0;i<rows;i++){
        for (j=0;j<cols;j++) {
            if(bwImage.at<uchar>(i,j)==0) {
                outImage.at<uchar>(i,j)=0;
            }
        }
    }
    imshow("Output Image", outImage);
}

blob blobCenter(Mat temp, int width) {
    vector< vector<Point> > contours;
    vector<Vec4i> hierarchy;
    blob blobPos;
    float xDiff, halfWidth;
    halfWidth=width/2;
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
        if (blobPos.maxArea<MAX_AREA) {
                if (blobPos.xPos>=0){
                    xDiff=blobPos.xPos-halfWidth;
                    blobPos.lturn=1-(xDiff/halfWidth);
                    blobPos.rturn=1+(xDiff/halfWidth);
                } else {
                    blobPos.lturn=0;
                    blobPos.rturn=0;
                }
        }
    } else {
        blobPos.xPos=-8;
        blobPos.yPos=-8;
        blobPos.lturn=0;
        blobPos.rturn=0;
        blobPos.maxArea=0;
        blobPos.blobDetect=false;
        //cout<<"No blobs detected\n";
    }
    return(blobPos);
}

/*decision navTree(blob blobPosition, blob blobPosition2, Mat finalImage, bool findBlob1, bool findBlob2) {
    decision navDecision;
    if (findBlob1) {                            //if first sample has not been retrieved
        if (blobPosition.maxArea<MAX_AREA) {    //if first sample is not close enough to be retrieved
            navDecision.lookBlob1=true;
            if (blobPosition2.blobDetect && findBlob2) {     //if the second sample is also on screen and we are looking for it
                if (blobPosition2.maxArea<MAX_AREA) {        //if the second sample is not close enough to be retrieved
                    navDecision.lookBlob2=true;
                    if (blobPosition.maxArea>=blobPosition2.maxArea) {  //Navigate towards largest/closest sample
                        navDecision.lspeed=blobPosition.lturn*MAX_SPEED;
                        navDecision.rspeed=blobPosition.rturn*MAX_SPEED;
                        cv::circle(finalImage,cv::Point(blobPosition.xPos,blobPosition.yPos),10,cv::Scalar(0,0,0));
                    } else {
                        navDecision.lspeed=blobPosition2.lturn*MAX_SPEED;
                        navDecision.rspeed=blobPosition2.rturn*MAX_SPEED;
                        cv::circle(finalImage,cv::Point(blobPosition2.xPos,blobPosition2.yPos),10,cv::Scalar(0,0,0));
                    }
                } else {
                    cout<<"Blob 2 was found\n";
                    navDecision.lookBlob2=false;
                }
            } else {                            //if the second sample isn't on screen, then navigate towards first sample
                if (findBlob2) {
                    navDecision.lookBlob2=true;
                } else {
                    navDecision.lookBlob2=false;
                }
                navDecision.lspeed=blobPosition.lturn*MAX_SPEED;
                navDecision.rspeed=blobPosition.rturn*MAX_SPEED;
                cv::circle(finalImage,cv::Point(blobPosition.xPos,blobPosition.yPos),10,cv::Scalar(0,0,0));
            }
        } else {                                //if first sample is close enough to be retrieved
            cout<<"Blob 1 was found\n";         //then stop looking for first sample
            navDecision.lookBlob1=false;
        }
    } else if (findBlob2) {                     //if we found the first sample. but are still looking for the second sample
        if (blobPosition2.maxArea<MAX_AREA) {
            if (blobPosition.blobDetect&&findBlob1) {
                if (blobPosition2.maxArea>blobPosition2.maxArea) {
                    lspeed=blobPosition2.lturn*MAX_SPEED;
                    rspeed=blobPosition2.rturn*MAX_SPEED;
                    cv::circle(finalImage,cv::Point(blobPosition2.xPos,blobPosition2.yPos),10,cv::Scalar(0,0,255));
                } else {
                    lspeed=blobPosition.lturn*MAX_SPEED;
                    rspeed=blobPosition.rturn*MAX_SPEED;
                    cv::circle(finalImage,cv::Point(blobPosition.xPos,blobPosition.yPos),10,cv::Scalar(0,0,0));
                }
            } else {
                lspeed=blobPosition2.lturn*MAX_SPEED;
                rspeed=blobPosition2.rturn*MAX_SPEED;
                cv::circle(finalImage,cv::Point(blobPosition2.xPos,blobPosition2.yPos),10,cv::Scalar(0,0,255));
            }
        } else {
            cout<<"Blob 2 was found\n";
            findBlob2=false;
        }
    }
}*/
int main( int argc, char** argv )
{
    initRoboteq();//Refer to nasaroboteq.c
    VideoCapture cap;
    blob blobPosition, blobPosition2;
    Mat frame, temp, temp2;
    Mat bgrImage, hsvImage, hsvOutputImage, hsvOutputImage2, finalImage;
    static int lspeed, rspeed;
    int x,y;
    int mArea;
    int width;
    bool useMorphOps=true;
    bool findBlob1 = true;
    bool findBlob2 = true;

    cap.open(1); //0:default web cam   1:external web cam
    trackbarInit();

    int fin=1;
    while(fin>=1)
    {
        cap>>frame;
        frame.copyTo(bgrImage);
        width=frame.cols;
        cvtColor(bgrImage, hsvImage, COLOR_BGR2HSV);
        imshow("Before Thresh", bgrImage);
        inRange(hsvImage,Scalar(h_low,s_low,v_low), Scalar(h_high,s_high,v_high), hsvOutputImage);
        inRange(hsvImage,Scalar(h_low2,s_low2,v_low2), Scalar(h_high2,s_high2,v_high2), hsvOutputImage2);
        imshow("Before Morph", hsvOutputImage);
        if(useMorphOps) {
                morphOps(hsvOutputImage);
                morphOps(hsvOutputImage2);
        }
        finalImage=hsvOutputImage2|hsvOutputImage;
        hsvOutputImage.copyTo(temp);
        hsvOutputImage2.copyTo(temp2);
        blobPosition=blobCenter(temp, width);
        blobPosition2=blobCenter(temp2, width);



        if (findBlob1||findBlob2) {                     //if all samples have not been retrieved
            if (findBlob1) {                            //if first sample has not been retrieved
                if (blobPosition.maxArea<MAX_AREA) {    //if first sample is not close enough to be retrieved
                    if (blobPosition2.blobDetect && findBlob2) {     //if the second sample is also on screen and we are looking for it
                        if (blobPosition2.maxArea<MAX_AREA) {        //check to see if second sample is close enough to be retrieved
                            if (blobPosition.maxArea>=blobPosition2.maxArea) {  //Navigate towards largest/closest sample
                                lspeed=blobPosition.lturn*MAX_SPEED;
                                rspeed=blobPosition.rturn*MAX_SPEED;
                                cv::circle(finalImage,cv::Point(blobPosition.xPos,blobPosition.yPos),10,cv::Scalar(0,0,255));
                            } else {
                                lspeed=blobPosition2.lturn*MAX_SPEED;
                                rspeed=blobPosition2.rturn*MAX_SPEED;
                                cv::circle(finalImage,cv::Point(blobPosition2.xPos,blobPosition2.yPos),10,cv::Scalar(0,0,255));
                            }
                        } else {
                            cout<<"Blob 2 was found\n";
                            findBlob2=false;
                        }
                    } else {                            //if the second sample isn't on screen or we aren't looking for it, then navigate towards first sample
                        lspeed=blobPosition.lturn*MAX_SPEED;
                        rspeed=blobPosition.rturn*MAX_SPEED;
                        cv::circle(finalImage,cv::Point(blobPosition.xPos,blobPosition.yPos),10,cv::Scalar(0,0,255));
                    }
                } else {                                //if first sample is close enough to be retrieved
                    cout<<"Blob 1 was found\n";         //then stop looking for first sample
                    findBlob1=false;
                }
            } else if (findBlob2) {                         //if the first sample was retrieved but the second one hasn't
                if (blobPosition2.maxArea<MAX_AREA) {
                    lspeed=blobPosition2.lturn*MAX_SPEED;
                    rspeed=blobPosition2.rturn*MAX_SPEED;
                    cv::circle(finalImage,cv::Point(blobPosition2.xPos,blobPosition2.yPos),10,cv::Scalar(0,0,255));
                } else {
                    cout<<"Blob 2 was found\n";
                    findBlob2=false;
                }
            }
        } else {                                        //If all the samples have been retrieved
            lspeed=rspeed=0;                            //Then end the program (in future, make robot go home)
            fin=0;
            cout<<"Robot has reached home base\n";
        }



        cout<<"Left Speed: "<<lspeed<<" Right Speed: "<<rspeed<<"\n";
        //sendspeed(lspeed, rspeed);
        //imshow("Orange", hsvOutputImage);
        //imshow("Blue", hsvOutputImage2);
        imshow("Combined Image", finalImage);
        //bw2color(bgrImage, finalImage);
        if( waitKey(1) == 27 ) break; // stop capturing by pressing ESC
        }
}
