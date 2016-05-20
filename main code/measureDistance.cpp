#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <ctype.h>

#define OBJ_HEIGHT 760 //physical height of reference object in mm

#define MIN_AREA (10*10)    //smallest area of blob before it becomes noise

#define MIN_PIX 10

using namespace cv;
using namespace std;

int h_low=90;         //hue is the color. For yellow, =20; red=0; blue=100
int h_high=110;       //for yellow, =30; red=20; blue=110
int s_low=220;      //saturation is the amount of a color
int s_high=256;
int v_low=160;      //value is the shininess of the color
int v_high=255;

//int morphSize=3;

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
int getHeight(Mat);

int main( int argc, char** argv )
{
    VideoCapture cap;
    blob blobPosition;
    //vector<int> blobStarts;
    int height=0;
    int count_f=0;
    Mat bgrImage, hsvImage, hsvOutImage, temp;
    string fileName ="outIm";
    cap.open(1); //0:default web cam   1:external web cam

    trackbarInit();
    //cap>>bgrImage;
    //blobStarts.resize(bgrImage.cols, vector<int>(3, 0));
    int fin=1;
    while(fin>=1) {
        cap>>bgrImage;
        imshow("bgr", bgrImage);
        //blobStarts.resize(bgrImage.rows, 0);
        //Canny(bgrImage, temp, 50, 200, 3);
        cvtColor(bgrImage, hsvImage, COLOR_BGR2HSV);
        //imshow("canny", temp);
        //temp=getGaussianKernel(5, 1.4);
        //GaussianBlur(bgrImage, hsvImage, Size(5,5), 1.4, 0);
       // imshow("blur", hsvImage);
        inRange(hsvImage,Scalar(h_low,s_low,v_low), Scalar(h_high,s_high,v_high), hsvOutImage);

        //hsvOutImage.copyTo(temp);


        imshow("After Thresh", hsvOutImage);
        height=getHeight(hsvOutImage);
        cout<<"HEIGHT: "<<height<<endl;
       /* for(i=0;i<hsvOutImage.rows;i++){        //count num of white pixels in each row
            numWPix=0;
            for(j=0;j<hsvOutImage.cols;j++){
                if(hsvOutImage[i][j]==1){
                    numWPix++;
                }
            }
                blobStarts[i]=numWPix;
        }
        /*Mat dilateElement=getStructuringElement(MORPH_RECT, Size(morphSize,morphSize));
        dilate(hsvOutImage,hsvOutImage,dilateElement);
        dilate(hsvOutImage,hsvOutImage,dilateElement);
        imshow("After Morph", hsvOutImage);*/

        //blobCenter(hsvOutImage);
        //cv::circle(hsvOutImage,cv::Point(blobPosition.xPos,blobPosition.yPos),10,cv::Scalar(0,0,0));
        //imshow("Final", hsvOutImage);
        if( waitKey(1) == 27 ) break;
        if( waitKey(1)== 's' ){
            stringstream file;
            file<<"bCap"<<count_f<<".jpg";
            count_f++;
            imwrite(file.str(), hsvOutImage);
        }

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
    //createTrackbar("Morph Size",  "Blob Detection", &morphSize,  9);
}

int getHeight(Mat image){
    int i,j;
    int numPix=0;
    int pixCount[image.rows];
    int height=0;
    int prevHeight=0;
    bool pixFlag=false;
    for (i=0; i<image.rows; i++){
        numPix=0;
        for(j=0; j<image.cols;j++){
                //cout<<image.at<uchar>(i,j);
            if(image.at<uchar>(i,j)!=0){        //keep it as .at<uchar>, as .at<int> will count non-white pixels for the last 3 rows
                    //cout<<"hi"<<endl;
                numPix++;            //count number of white pixels in each row
            }
        }
        //cout<<"I: "<<i<<"\tPIX: ";
        //cout<<numPix<<endl;
        pixCount[i]=numPix;
        if(numPix>=MIN_PIX) pixFlag=true;
        //if(i%20==0) cout<<endl;
    }
    if(pixFlag==true){                  //if at least one significant blob detected
        for(i=0;i<image.rows;i++){
            if(pixCount[i]>=MIN_PIX){   //if rising edge encountered
                height=1;
                while(pixCount[i+1]>=MIN_PIX){  //if neighbor also part of blob
                    height++;                   //increment height and index
                    i++;
                }
                if(height>prevHeight) prevHeight=height;    //save biggest height
                //cout<<"HEIGHT: "<<height<<endl;
            }
        }
        return prevHeight;
    } else {
        return 0;
    }
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
        //Rect boundRect = boundingRect(contours[areaIndex]);
        //cout<<"Height: "<<boundRect.height<<endl;
        //rectangle(temp, boundRect, Scalar(255), 1, 8, 0);
        drawContours( temp, contours, areaIndex, Scalar(255), CV_FILLED, 8, hierarchy );

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
