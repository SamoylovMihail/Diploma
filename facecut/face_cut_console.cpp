#include "opencv2/opencv.hpp"
#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <dirent.h>

using namespace std;
using namespace cv;

void detectAndCut(Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip, char* imageName);

string cascadeName = "/usr/share/opencv/haarcascades/haarcascade_frontalface_alt.xml";
string nestedCascadeName = "/usr/share/opencv/haarcascades/haarcascade_eye_tree_eyeglasses.xml";

int main( int argc, const char** argv )
{
    CvCapture* capture = 0;
    Mat frame, frameCopy, image;
    string inputName;
    bool tryflip = false;

    CascadeClassifier cascade, nestedCascade;
    double scale = 1;

	if( !cascade.load( cascadeName ) )
    {
        cerr << "ERROR: Could not load classifier cascade" << endl;
        return -1;
    }

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (argv[1])) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			string path = argv[1];
			image = imread(path + ent->d_name, 1);
			if(image.empty()) {
            	continue;
			}
			cout << path + ent->d_name << endl;
			detectAndCut(image, cascade, nestedCascade, scale, tryflip, ent->d_name);
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		return 1;
	}

    return 0;
}

void detectAndCut(Mat& img, CascadeClassifier& cascade,
                    CascadeClassifier& nestedCascade,
                    double scale, bool tryflip, char *imageName)
{
    int i = 0;
    double t = 0;
    vector<Rect> faces, faces2;
    const static Scalar colors[] =  { CV_RGB(0,0,255),
        CV_RGB(0,128,255),
        CV_RGB(0,255,255),
        CV_RGB(0,255,0),
        CV_RGB(255,128,0),
        CV_RGB(255,255,0),
        CV_RGB(255,0,0),
        CV_RGB(255,0,255)} ;
    Mat gray, smallImg( cvRound (img.rows/scale), cvRound(img.cols/scale), CV_8UC1 );

    cvtColor( img, gray, COLOR_BGR2GRAY );
    resize( gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR );
    equalizeHist( smallImg, smallImg );

    cascade.detectMultiScale( smallImg, faces,
        1.1, 2, 0
        //|CASCADE_FIND_BIGGEST_OBJECT
        //|CASCADE_DO_ROUGH_SEARCH
        |CASCADE_SCALE_IMAGE
        ,
        Size(30, 30) );
    if( tryflip )
    {
        flip(smallImg, smallImg, 1);
        cascade.detectMultiScale( smallImg, faces2,
                                 1.1, 2, 0
                                 //|CASCADE_FIND_BIGGEST_OBJECT
                                 //|CASCADE_DO_ROUGH_SEARCH
                                 |CASCADE_SCALE_IMAGE
                                 ,
                                 Size(30, 30) );
        for( vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++ )
        {
            faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
        }
    }
    for( vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++ )
    {
        Mat smallImgROI;
        vector<Rect> nestedObjects;
		smallImgROI = smallImg(*r);
		string directoryName = "faces/";
		string face = "face";
		string separator = "_";
		string ext = ".png";
		char numstr[8];
		sprintf(numstr, "%d", i);
		string filename = directoryName + face + separator + imageName + separator + numstr + ext;
		imwrite(filename, smallImgROI);
        if( nestedCascade.empty() )
            continue;
        nestedCascade.detectMultiScale( smallImgROI, nestedObjects,
            1.1, 2, 0
            //|CASCADE_FIND_BIGGEST_OBJECT
            //|CASCADE_DO_ROUGH_SEARCH
            //|CASCADE_DO_CANNY_PRUNING
            |CASCADE_SCALE_IMAGE
            ,
            Size(30, 30) );
    }
}

