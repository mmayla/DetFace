#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

static Mat toGrayscale(InputArray _src)
{
	Mat src = _src.getMat();
	// only allow one channel
	if (src.channels() != 1)
	{
		CV_Error(CV_StsBadArg, "Only Matrices with one channel are supported");
	}
	// create and return normalized image
	Mat dst;
	cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
	return dst;
}

static void read_csv(const string& filename, vector<Mat>& images,
		vector<int>& labels, char separator = ';')
{
	std::ifstream file(filename.c_str(), ifstream::in);
	if (!file)
	{
		string error_message =
				"No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	while (getline(file, line))
	{
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if (!path.empty() && !classlabel.empty())
		{
			images.push_back(imread(path, 0));
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
}

int _main(int argc, const char *argv[])
{
	// holds images and labels
	vector<Mat> images;
	vector<int> labels;
	// images for first person
	images.push_back(imread("images/persons/person0/0.jpg", CV_LOAD_IMAGE_GRAYSCALE)); labels.push_back(0);
	images.push_back(imread("images/persons/person0/1.jpg", CV_LOAD_IMAGE_GRAYSCALE)); labels.push_back(0);
	images.push_back(imread("images/persons/person0/2.jpg", CV_LOAD_IMAGE_GRAYSCALE)); labels.push_back(0);
	// images for second person
	images.push_back(imread("images/persons/person1/0.jpg", CV_LOAD_IMAGE_GRAYSCALE)); labels.push_back(1);
	images.push_back(imread("images/persons/person1/1.jpg", CV_LOAD_IMAGE_GRAYSCALE)); labels.push_back(1);
	images.push_back(imread("images/persons/person1/2.jpg", CV_LOAD_IMAGE_GRAYSCALE)); labels.push_back(1);
	
	// images for second person
	images.push_back(imread("images/persons/person2/0.jpg", CV_LOAD_IMAGE_GRAYSCALE)); labels.push_back(2);
	images.push_back(imread("images/persons/person2/1.jpg", CV_LOAD_IMAGE_GRAYSCALE)); labels.push_back(2);
	images.push_back(imread("images/persons/person2/2.jpg", CV_LOAD_IMAGE_GRAYSCALE)); labels.push_back(2);
	
	// Create a new Fisherfaces model and retain all available Fisherfaces,
	// this is the most common usage of this specific FaceRecognizer:
	//
	Ptr<FaceRecognizer> model =  createFisherFaceRecognizer();
	//Ptr<FaceRecognizer> model = createEigenFaceRecognizer();
	//Ptr<FaceRecognizer> model = createLBPHFaceRecognizer(1, 8, 8, 8, DBL_MAX);
	
	// This is the common interface to train all of the available cv::FaceRecognizer
	model->train(images, labels);
	
	// Read in a sample image:
	Mat img = imread("images/persons/person0/3.jpg", CV_LOAD_IMAGE_GRAYSCALE);
	// And get a prediction from the cv::FaceRecognizer:
	int predicted = model->predict(img);
	
	cout<<"The image for person "<<predicted<<endl;
	
	return 0;
}
