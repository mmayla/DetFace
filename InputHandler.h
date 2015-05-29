/*
 * InputHandler.h
 *
 *  Created on: Jan 23, 2014
 *      Author: mohamedmayla
 */

#ifndef INPUTHANDLER_H_
#define INPUTHANDLER_H_

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace cv;
using namespace std;

struct Person
{
	int id;
	string info;
	int nimages;

	Person(int id, string info, int n)
	{
		this->id = id;
		this->info = info;
		this->nimages = n;
	}
};

class InputHandler
{
private:
	string dir;
	vector<Person*> personlist;
	vector<Mat>* images;
	vector<int>* labels;

public:
	
	InputHandler(vector<Mat>* ims,vector<int>* lbl)
	{
		dir = "images/persons";
		images = ims;
		labels = lbl;
	}

	void fillPlist()
	{
		string indir = dir + "/info.txt";

		ifstream infile(indir.c_str());

		int n, id, count;
		string ininfo;
		infile >> n;
		for (int i = 0; i < n; i++)
		{
			infile >> id >> count;
			getline(infile, ininfo);
			getline(infile, ininfo);
			
			personlist.push_back(new Person(id, ininfo, count));
		}

		infile.close();
	}

	void fillcv()
	{	
		for (int i = 0; i < personlist.size(); i++)
		{
			for (int k = 0; k < personlist[i]->nimages; k++)
			{
				string sid, imn;
				stringstream ss1, ss2;
				ss1 << personlist[i]->id;
				ss1 >> sid;
				ss2 << k;
				ss2 >> imn;

				images->push_back(
						imread(
								"images/persons/person" + sid + "/" + imn
										+ ".jpg", CV_LOAD_IMAGE_GRAYSCALE));
				labels->push_back(personlist[i]->id);
			}
		}
	}

	string getinfo(int id)
	{
		int index=-1;
		for (int i = 0; i < personlist.size(); i++)
		{
			if (personlist[i]->id == id)
			{
				index = i;
				break;
			}
		}

		if(index==-1)
			return "-1";
		else return personlist[index]->info;
	}
};

#endif /* INPUTHANDLER_H_ */
