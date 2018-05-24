#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

#include "RectData.h"
#include "dpm.hpp"

using namespace cv;
using namespace cv::dpm;
using namespace std;


int main(int argc, char* argv[])
{
	time_t t_ini, t_fin;

	std::string vidName, modelName, rectDataName, logName;
	float overlapThreshold = 0.5f;
	double a = 0, skylineY = 0;
	int numThreads = -1;
	Mat frame;

	if (argc > 6)
	{
		vidName = argv[1];
		modelName = argv[2];
		rectDataName = argv[3];
		a = atof(argv[4]);
		skylineY = atof(argv[5]);
		if (argc > 7) logName = argv[6];
		if (argc > 8) overlapThreshold = (float)atof(argv[7]);
		if (overlapThreshold < 0 || overlapThreshold > 1)
		{
			std::cout << "overlapThreshold must be in interval (0,1)." << std::endl;
			exit(-1);
		}

		if (argc > 9) numThreads = atoi(argv[9]);
	}
	else
	{
		//vidName = "..\\data\\video\\set03\\test.avi";
		vidName = "..\\data\\video\\set01\\720p\\V002.avi";
		modelName = "data\\inriaperson.xml";
		rectDataName = "..\\data\\result\\SUtest.dat";
		logName = "..\\data\\result\\SUtest.log";
		a = 1.43243243243; // 3-0
		skylineY = 433.358490566; // 3-0
		//a = 1.06172839506;
		//skylineY = 352.593023256;
	}

	VideoCapture vid(vidName);
	if (!vid.isOpened()) {
		std::cout << "Read video error." << std::endl;
		exit(-1);
	}
	cout << "Loaded video:" << vidName << endl;

	cv::Ptr<DPMDetector> detector = DPMDetector::create(vector<string>(1, modelName));
	cout << "Loaded models:" << modelName << endl;

	RectData * rectdata = new RectData(vid.get(CV_CAP_PROP_FRAME_COUNT), Size(vid.get(CV_CAP_PROP_FRAME_WIDTH), vid.get(CV_CAP_PROP_FRAME_HEIGHT)));
	unsigned int frameCount = 0;
	detector->setParameter(vid.get(CV_CAP_PROP_FRAME_WIDTH), vid.get(CV_CAP_PROP_FRAME_HEIGHT), a, skylineY);

	ofstream logout;
	if (!logName.empty())
		logout.open(logName); // frameCount, numOfRect, detection time
	logout << "\"Frame Count\",\"The Number Of rect\",\"detection time\"" << endl;
	while (vid.read(frame))
	{
		vector<DPMDetector::ObjectDetection> detections;

		double t = (double)getTickCount();
		//Mat tmp(frame, Rect(0, skipZone, frame.cols, frame.rows - skipZone));
		detector->detect(frame, detections);
		//detector->detect(tmp, detections);
		t = ((double)getTickCount() - t) / getTickFrequency(); //elapsed time

		vector<CvRect*> v;
		if (detections.size() != 0)
		{
			for each (const DPMDetector::ObjectDetection& od in detections)
			{
				v.push_back(new CvRect(od.rect));
			}
			rectdata->addRects(frameCount, v);
		}

		if (logout)
			logout << frameCount << "," << detections.size() << "," << t << endl;

		++frameCount;
	}
	writeFile(rectDataName, rectdata);

	vid.release();
	logout.close();
	delete rectdata;
}