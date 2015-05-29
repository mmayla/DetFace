#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "InputHandler.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;


// holds images and labels
vector<Mat> images;
vector<int> labels;
InputHandler* IH;

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

void testinput()
{
	// images for first person
		images.push_back(
				imread("images/persons/person0/0.jpg", CV_LOAD_IMAGE_GRAYSCALE));
		labels.push_back(0);
		images.push_back(
				imread("images/persons/person0/1.jpg", CV_LOAD_IMAGE_GRAYSCALE));
		labels.push_back(0);
		images.push_back(
				imread("images/persons/person0/2.jpg", CV_LOAD_IMAGE_GRAYSCALE));
		labels.push_back(0);
		// images for second person
		images.push_back(
				imread("images/persons/person1/0.jpg", CV_LOAD_IMAGE_GRAYSCALE));
		labels.push_back(1);
		images.push_back(
				imread("images/persons/person1/1.jpg", CV_LOAD_IMAGE_GRAYSCALE));
		labels.push_back(1);
		images.push_back(
				imread("images/persons/person1/2.jpg", CV_LOAD_IMAGE_GRAYSCALE));
		labels.push_back(1);

		// images for second person
		images.push_back(
				imread("images/persons/person2/0.jpg", CV_LOAD_IMAGE_GRAYSCALE));
		labels.push_back(2);
		images.push_back(
				imread("images/persons/person2/1.jpg", CV_LOAD_IMAGE_GRAYSCALE));
		labels.push_back(2);
		images.push_back(
				imread("images/persons/person2/2.jpg", CV_LOAD_IMAGE_GRAYSCALE));
		labels.push_back(2);
}

int main(int argc, const char *argv[])
{
	//get input
	IH = new InputHandler(&images,&labels);
	IH->fillPlist();
	IH->fillcv();
	//testinput();
	
	//get the size of images
	int im_width = images[0].cols;
	int im_height = images[0].rows;
	
	// Create a new Fisherfaces model and retain all available Fisherfaces,
	Ptr<FaceRecognizer> model = createFisherFaceRecognizer();
	//Ptr<FaceRecognizer> model = createwEigenFaceRecognizer();
	//Ptr<FaceRecognizer> model = createLBPHFaceRecognizer(1, 8, 8, 8, DBL_MAX);

	// This is the common interface to train all of the available cv::FaceRecognizer
	model->train(images, labels);

	string fn_haar = "haarcascade_frontalface_alt.xml";
	CascadeClassifier haar_cascade;
	haar_cascade.load(fn_haar);
	// Get a handle to the Video device:
	VideoCapture cap(0);
	// Check if we can use this device at all:
	if (!cap.isOpened())
	{
		cerr << "Capture Device ID cannot be opened." << endl;
		return -1;
	}
	// Holds the current frame from the Video device:
	Mat frame;
	for (;;)
	{
		cap >> frame;
		// Clone the current frame:
		Mat original = frame.clone();
		// Convert the current frame to grayscale:
		Mat gray;
		cvtColor(original, gray, CV_BGR2GRAY);
		// Find the faces in the frame:
		vector<Rect_<int> > faces;
		haar_cascade.detectMultiScale(gray, faces);
		// At this point you have the position of the faces in
		// faces. Now we'll get the faces, make a prediction and
		// annotate it in the video. Cool or what?
		for (int i = 0; i < faces.size(); i++)
		{
			// Process face by face:
			Rect face_i = faces[i];
			// Crop the face from the image. So simple with OpenCV C++:
			Mat face = gray(face_i);
			// Resizing the face is necessary for Eigenfaces and Fisherfaces. You can easily
			// verify this, by reading through the face recognition tutorial coming with OpenCV.
			// Resizing IS NOT NEEDED for Local Binary Patterns Histograms, so preparing the
			// input data really depends on the algorithm used.
			//
			// I strongly encourage you to play around with the algorithms. See which work best
			// in your scenario, LBPH should always be a contender for robust face recognition.
			//
			// Since I am showing the Fisherfaces algorithm here, I also show how to resize the
			// face you have just found:
			Mat face_resized;
			cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0,
					INTER_CUBIC);
			// Now perform the prediction, see how easy that is:
			int prediction = model->predict(face_resized);
			// And finally write all we've found out to the original image!
			// First of all draw a green rectangle around the detected face:
			rectangle(original, face_i, CV_RGB(0, 255,0), 1);
			
			// Create the text we will annotate the box with:
			string box_text = IH->getinfo(prediction);
			if(box_text=="-1") box_text = "UNKOWN";
			cout<<prediction<<endl;
			
			//string box_text = format("id = %d",prediction);
			// Calculate the position for annotated text (make sure we don't
			// put illegal values in there):
			int pos_x = std::max(face_i.tl().x - 10, 0);
			int pos_y = std::max(face_i.tl().y - 10, 0);
			// And now put it into the image:
			putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN,
					1.0, CV_RGB(0,255,0), 2.0);
		}

		// Show the result:
		imshow("face_recognizer", original);
		// And display it:
		char key = (char) waitKey(20);
		// Exit this loop on escape:
		if (key == 27)
			break;
	}
	return 0;
}
