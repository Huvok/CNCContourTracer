#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sstream>
#include <fstream>
//#include <pthread.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//#define NUM_THREADS 2

using namespace cv;
using namespace std;

int leds[] = {0,0,0};

int iLowH  = 40;
int iHighH = 75;
int iLowS  = 44;
int iHighS = 255;
int iLowV  = 104;
int iHighV = 255;

int thresh = 100;
int max_thresh = 255;
RNG rng(12345);
vector<vector<Point> > contours;

//void *pinitCam();
//void *pInitButtons();

int scanButton(int button);
int buttons();
void capImage();
void HSVcolorFilter(Mat originalImage);
void thresh_callback(Mat originalImage);
Mat traceContours(Mat traceImage);
void sendPoint(Point p, int fd);
void sendChar(char c, int fd);
int initSerialTransmission();

int main(int argc, char **argv){
  
  //for(;;){
  //  int button = buttons();
  //  cout << button << endl;
  //  if(button == 3){
      //Get Image 
      capImage();
      //Serial Communication
      initSerialTransmission();
  //  }
  //  else if(button == 5){
  //    cout << "Stop Button Pressed" << endl;
  //  }  
    
  //}
	
  return 0;
}

void capImage(){
  //char key = 'c';

  VideoCapture cap(0);

  Mat img = imread("greenRombo.jpg",1 );
  Mat myFigures(img.size(), CV_8UC3, Scalar(0,0,0)); 

  //printf("Height %d , Width %d" , img.cols, img.rows);
  HSVcolorFilter(img);
  thresh_callback(img);
  myFigures = traceContours(myFigures);
  
}


void HSVcolorFilter(Mat originalImage) {
  Mat orig = originalImage.clone();
  Mat src_gray;
  Mat imgHSV ;
  Mat imgThresholded;
  Mat BGRHSVimg;

  cvtColor(orig, imgHSV, COLOR_BGR2HSV);       // Convert the captured frame from BGR to HSV.
  inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), 
  Scalar(iHighH, iHighS, iHighV), imgThresholded); // Threshold the image.
    
  // Morphological opening (remove small objects from the foreground).
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );

  // Morphological closing (fill small holes in the foreground).
  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );

  Mat other_filter;
  bitwise_and(imgHSV,imgHSV,other_filter, imgThresholded = imgThresholded);
  //cvtColor(other_filter, BGRHSVimg, CV_HSV2BGR);

  src_gray = imgThresholded.clone();
  blur(src_gray, src_gray, Size(3,3));
  //imshow("HSV Filter", src_gray);
}


/** @function thresh_callback */
void thresh_callback(Mat originalImage){
  ofstream myFile;
  myFile.open("Coordenadas.txt");
  Mat canny_output;
  vector<Vec4i> hierarchy;
  Mat myFigures(originalImage.size(), CV_8UC3, Scalar(0,0,0)); 

  /// Detect edges using canny
  Canny( originalImage, canny_output, thresh, thresh*2, 3 );
  /// Find contours
  findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

  /// Draw contours
  Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
  //cout << contours.size() << endl;
  for( int i = 0; i< contours.size(); i++ ){
      Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
      drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
      //myFile << "&" << i << "&" <<  endl;
      myFile << contours[i] << endl;	     
  }

  //namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
  //imshow( "Contours", drawing );
  myFile.close();
}

Mat traceContours(Mat traceImage){
  for( int i = 0; i< contours.size(); i++ ){
      for( int j = 1; j < contours[i].size(); j++){
         line(traceImage,contours[i][j-1],contours[i][j], Scalar(0,255,0), 2, 8, 0);
      }
  }
  return traceImage;
}


void sendPoint(Point p, int fd){
 //Convert the point to String
 stringstream sx, sy;
 sx << p.x;
 sy << p.y;
 string x, y;
 x = sx.str();
 y = sy.str();

 //Get length of the String to transfer by chars
 int xl = x.length();
 int yl = y.length();
  cout << "Char sended = ";
 for(int i = 0; i < xl; i++){
   sendChar(x[i] , fd );
   cout << x[i] << " ";
 }
   //X , Y Division
  sendChar(',',fd);
  cout << ", ";
 for(int i = 0; i < yl; i++){
   sendChar(y[i], fd);
   cout << y[i] << " " ;
 }
   cout << endl;
}

void sendChar(char c, int fd){
  serialPutchar(fd, c);
  fflush(stdout);
}

int initSerialTransmission(){
  Point p;
  int fd ;
  //int count;
  unsigned int nextTime ;

  if ((fd = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }

  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }

  nextTime = millis () + 300 ;
  
  int vecSize = contours.size();
  int totPoints = 0;
  //int tPSize = 0;
  string sTP = "";

  for(int i = 0; i < vecSize; i ++){
    totPoints += contours[i].size();
  }
  cout << "Puntos totales = " << totPoints << endl;

  sendChar('&',fd);
  cout << "& Sended " << endl;
  sTP = to_string(totPoints);
  for(int i = 0; i < sTP.length(); i++){
    sendChar(sTP[i], fd);
  }
  sendChar(',',fd);

  for(int i = 0; i < vecSize; i++){
    for(int j = 0; j < contours[i].size(); j+=2){
      sendPoint(contours[i][j],fd);
      cout << contours[i][j].x << "  "  <<  contours[i][j].y <<endl; 
      delay(1);
      sendChar(',',fd);
      delay(1);
    }
  }
  /*for(int i = 0; i < 10; i++){
  	sendPoint(contours[contours.size()-1][contours[contours.size()-1].size()], fd); 
 }*/

  for(int i = 0 ; i < 5 ; i++){
	sendChar('&',fd);
	delay(1);
 }
  
}

int buttons(){
  int photoB = 3;
  //int transB = 7;
  int stopB = 5;
  int pressed = -1;

  pinMode(photoB , INPUT);
  //pinMode(transB , INPUT);
  pinMode(stopB  , INPUT);
  pullUpDnControl(photoB, PUD_UP);
  //pullUpDnControl(transB, PUD_UP);
  pullUpDnControl(stopB , PUD_UP);

  printf("On Buttons Thread");
  
  //Capture Pressed
  pressed = scanButton(3);
  delay(1);
  if(pressed)
    return 3;

  //Stop button Pressed
  scanButton(5);
  delay(1);
  if(pressed)
    return 5;
}

int scanButton(int button){
  if(digitalRead(button) == HIGH)
    return 1;

  leds[button] ^= 1; // Invert State
  digitalWrite( 4 + button, leds[button]);

  while(digitalRead(button) == LOW)
    delay(10);
}
