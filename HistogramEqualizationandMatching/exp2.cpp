#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <conio.h>
#include <math.h>
#include <cmath>

#define LEVEL 256
#define INTENSITY_MAX 255
#define INTENSITY_MIN 0

using namespace std;
using namespace cv;


// This function takes image object, histogram and size
// Function reads the image and creates a histogram.
void imageToHistogram(Mat image,float histogram[], int size){

      for(int i = 0; i < LEVEL ; i++)
		  histogram[i] = 0;

      for(int y = 0 ; y < image.rows ; y ++)
          for(int x = 0 ; x < image.cols ; x++)
              histogram[(int)image.at<uchar>(y,x)]++;

      for(int i = 0; i < LEVEL ; i++)
            histogram[i] = histogram[i]/size;

}

// This fucnction is used to calculate the transfer function of from a given histogram.
// The transfer function created is just the cumulative frequency distribution
void calTranFunc(float histogram[], float tranFunc[]){

      tranFunc[0] = histogram[0];
      for(int i = 1 ; i < LEVEL; i++)
            tranFunc[i] = histogram[i] + tranFunc[i - 1];

}

// THis function is used to map the histogram to the intensity values that will be displayed on the image
void intensityMapping(float tranFunc[], float histogram[]){
      float tranFuncMin = INTENSITY_MAX+1;
	  tranFuncMin = tranFunc[0];

	  for(int i = 0; i < LEVEL ; i++)
          histogram[i] = (((tranFunc[i] - tranFuncMin)/(1 - tranFuncMin))*(LEVEL - 1) + 0.5);

}

//Function to match histogram of the input image to the target image
void histogramMatching(float inputTranFunc[], float targetTranFunc[], float histogram[] , float targetHistogram[]){

      for(int i = 0; i < LEVEL ; i++){
            int j = 0;
            do {
                  histogram[i] = j;
                  j++;
            }while(inputTranFunc[i] > targetTranFunc[j]);
      }
      return;
}

//Function to display histogram of an image and to write the historam in the outout file
void showHistogram(Mat& image, string fileName){

    int bins = 256;    // number of bins
	Mat histogram;     // for storing the histogram
	Mat canvas;		   // for displaying the histogram
    int hmax = 0;      // peak value for each histogram

    histogram = Mat::zeros(1, bins, CV_32SC1);

	for (int i = 0; i < image.rows; i++)
		for (int j = 0; j < image.cols; j++){
			uchar val = image.at<uchar>(i,j);
			histogram.at<int>(val) += 1;
		}

	for (int j = 0; j < bins-1; j++)
		hmax = histogram.at<int>(j) > hmax ? histogram.at<int>(j) : hmax;

	canvas = Mat::ones(125, bins, CV_8UC3);

	for (int j = 0, rows = canvas.rows; j < bins-1; j++)
		line(canvas, Point(j, rows), Point(j, rows - (histogram.at<int>(j) * rows/hmax)), Scalar(255, 255, 255) , 1, 8, 0);

	imshow(fileName, canvas);
	
}

Mat readImage(string &fileName, string type){
      cout << endl << "Please Select " <<type<<" Image." << endl;
      cout << "Example <lena_gray.bmp> <Baboon_dull.jpg>" << endl;
      cin >> fileName;

      cout<< "\nFile Selected: " << fileName << endl;
      Mat inputImage = imread(fileName, -1);

      return inputImage;
}

int main(){

      int option;
      string name;
      cout << "ASSIGNMENT 2: HISTOGRAM EQUALISATION AND MATCHING" << endl;
	  char exit_key;
	  do{
      cout << "Select the following options" << endl;
      cout << "   1. Histogram Equalisation" << endl;
      cout << "   2. Histogram Matching" << endl;
	  cout << " Please select one (1 or 2):";
      cin  >> option;
      if(option == 1){
            cout << "\nHistogram Equalization"<< endl;
            string fileName;

		Mat inputImage = readImage(fileName, "Input");
		if (inputImage.empty()){
                  cerr << "Error: Loading image" << endl;
                  _getch();
                  return -1;
            }

            if(inputImage.channels() == 1){
                  cout << "Grayscale Image" << endl;
                  int size = inputImage.rows * inputImage.cols;

                  float histogram[LEVEL];
                  imageToHistogram(inputImage,histogram , size);

                  float tranFunc[LEVEL];
                  calTranFunc(histogram, tranFunc);

                  float outHistogram[LEVEL];
                  intensityMapping(tranFunc, outHistogram);

                  Mat outputImage = inputImage.clone();
                  for (int y = 0; y < inputImage.rows; y++)
                        for (int x = 0; x < inputImage.cols; x++)
                              outputImage.at<uchar>(y, x) = saturate_cast<uchar>(saturate_cast<int>(outHistogram[inputImage.at<uchar>(y, x)]));

                  namedWindow("Original Image");
                  imshow("Original Image", inputImage);
                  showHistogram(inputImage, " Original Histogram");

                  namedWindow("Histogram Equilized Image");
                  imshow("Histogram Equilized Image", outputImage);
				  //imwrite("EQ_img.bmp",outputImage);
                  showHistogram(outputImage, " Equalized Histogram");

                  waitKey();
            }

      }else if(option == 2){
            cout << "\nHistogram Matching"<< endl;

            string inputFileName, targetFileName;

            Mat inputImage = readImage(inputFileName, "Input");
		if (inputImage.empty()){
                  cerr << "Error: Loading image" << endl;
                  _getch();
                  return -1;
            }

            Mat targetImage = readImage(targetFileName, "Target");
		if (targetImage.empty()){
                  cerr << "Error: Loading image" << endl;
                  _getch();
                  return -1;
            }


            cout << "inputFileName: " << inputFileName <<  endl;
            cout << "targetFileName: " << targetFileName << endl;

            if(inputImage.channels() == 1 && targetImage.channels() == 1 ){
                  int inputSize = inputImage.rows * inputImage.cols;

                  float inputHistogram[LEVEL];
                  imageToHistogram(inputImage, inputHistogram , inputSize);

                  float inputTranFunc[LEVEL];
                  calTranFunc(inputHistogram, inputTranFunc);

                  int targetSize = targetImage.rows * targetImage.cols;
                  float targetHistogram[LEVEL];
                  imageToHistogram(targetImage,targetHistogram , targetSize);

                  float targetTranFunc[LEVEL];
                  calTranFunc(targetHistogram, targetTranFunc);

                  float outHistogram[LEVEL];
                  histogramMatching(inputTranFunc, targetTranFunc, outHistogram, targetHistogram);

                  Mat outputImage = inputImage.clone();

                  for (int y = 0; y < inputImage.rows; y++)
                        for (int x = 0; x < inputImage.cols; x++)
                              outputImage.at<uchar>(y, x) = (int)(outHistogram[inputImage.at<uchar>(y, x)]);

                  namedWindow("Original Image");
                  imshow("Original Image", inputImage);
                  showHistogram(inputImage, " Original Histogram");

                  namedWindow("Target Image");
                  imshow("Target Image", targetImage);
                  showHistogram(targetImage, " Target Histogram");

                  namedWindow("Histogram Matched Image");
                  imshow("Histogram Matched Image", outputImage);
                  showHistogram(outputImage, " Matched Histogram");

                  waitKey();

            }

      }else{
            cout << "\nPlease choose the correct option" << endl;
            return -1;
      }
	   
      waitKey();

	  cout << "\nWould you like to exit? (y/n):";
	  cin >> exit_key;
	  cout << endl << endl;
	  }while(exit_key == 'n');
      return 0;
}