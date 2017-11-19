/* Multitouch Finger Detection Framework v4.0
* for VS2013 & OpenCV 2.4.10
*
* Bjoern Froemmer, January 2010 - March 2015
*/
//#include "/opencv2/opencv.hpp"
#include "../../../MT_lib/opencv/build/include/opencv2/opencv.hpp"
#include <time.h>
#include <math.h>
#include "Finger.h"
#include <cfloat>
#include <unordered_map>

using namespace cv;


int main(void)
{
	//VideoCapture cap(0); // use the first camera found on the system
	cv::VideoCapture cap("../../../MT_lib/mt_camera_raw.AVI");

	if (!cap.isOpened())
	{
		std::cout << "ERROR: Could not open camera / video stream.\n";
		return -1;
	}

	// get the frame size of the videocapture object
	double videoWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	double videoHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

	cv::Mat frame, original, original2, blurredImage, grey, firstFrame;

	map<int, Finger>::iterator it;

	static unsigned int id = 0;
	vector<Finger> fingers;
	vector<int> wantedFingers;
	vector<int> inQueue; // contains index of the finger in vector fingers
						 //map<int, Finger> fingers;
						 //map<int, int> wantedFingers; //first is new Point, second is old Point

	vector<Finger> newFingers;

	int currentFrame = 0; // frame counter
	clock_t ms_start, ms_end, ms_time; // time

	char buffer[10]; // buffer for int to ascii conversion -> itoa(...)

	for (;;)
	{
		ms_start = clock(); // time start

		cap >> frame; // get a new frame from the videostream

		if (frame.data == NULL) // terminate the program if there are no more frames
		{
			std::cout << "TERMINATION: Camerastream stopped or last frame of video reached.\n";
			break;
		}

		original = frame.clone(); // copy frame to original
		original2 = frame.clone(); // copy frame to original2

		cvtColor(original, grey, CV_BGR2GRAY); // convert frame to greyscale image (copies the image in the process!)

											   //--------------------------
											   // your code goes here
											   //--------------------------


		if (currentFrame == 0) {
			firstFrame = original.clone();

		}


		// background subtraction
		cv::absdiff(firstFrame, original, original);
		//cv::absdiff(firstFrame, original2, original2);

		// simple highpass filter
		blur(original, blurredImage, cv::Size(15, 15));
		absdiff(blurredImage, original, original);
		blur(original, original, cv::Size(10, 10));

		threshold(original, original, 7, 255, THRESH_BINARY);
		cvtColor(original, grey, CV_BGR2GRAY); // convert frame to greyscale image (copies the image in the process!)

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;

		findContours(grey, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
		// iterate through all the top-level contours -> "hierarchy" may not be empty!)
		if (hierarchy.size() > 0)
		{
			for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
			{
				// check contour size (number of points) and area ("blob" size)
				if (contourArea(Mat(contours.at(idx))) > 5 && contours.at(idx).size() > 4) // add maxSize as condition??
				{
					ellipse(original, fitEllipse(Mat(contours.at(idx))),
						Scalar(0, 0, 255), 1, 8); // fit & draw ellipse to contour at index
					drawContours(original, contours, idx, Scalar(255, 0, 0), 1, 8,
						hierarchy); // draw contour at index

									//own code
					RotatedRect rotRect = fitEllipse(contours.at(idx));
					Point2f center = rotRect.center; //get center of rotatedRectangle
					float xCoord = center.x;
					float yCoord = center.y;
					original.at<Vec3b>(Point(xCoord, yCoord)) = Vec3b(0, 1, 0);//point in center of finger
					circle(original, Point(xCoord, yCoord), 30, Scalar(0, 0, 255), 1, 8, 0); // RADRIUS 20, maybe set it even higher
					newFingers.push_back(Finger(id++, Point2f(xCoord, yCoord)));


				}
			}

			if (fingers.empty()) {
				for (int i = 0; i < newFingers.size(); i++) {
					fingers.push_back(newFingers.at(i));
				}

			}
			else {
				//after for loop
				float smallestDistance = FLT_MAX;
				int nearestNeighborIndex = -1;
				for (int i = 0; i < fingers.size(); i++) {
					for (int j = 0; j < newFingers.size(); j++) {
						float currentDistance;
						currentDistance = sqrt(pow(fingers.at(i).getXCoord() - newFingers.at(j).getXCoord(), 2) + pow(fingers.at(i).getYCoord() - newFingers.at(j).getYCoord(), 2));
						if (currentDistance < smallestDistance && currentDistance <= 30) { // variabel machen
							smallestDistance = currentDistance;
							nearestNeighborIndex = j;
						}
					}
					int whatsInsideI = i;
					///// HIER EIN FEHLER
					fingers.at(i).setWanted(newFingers.at(nearestNeighborIndex)); // wants this as following, type: Finger
					fingers.at(i).wantedDist = smallestDistance; // this is the distance, for comparing purposes, type float
					fingers.at(i).idWanted = newFingers.at(nearestNeighborIndex).getId();

					vector<int>::iterator it = find(wantedFingers.begin(), wantedFingers.end(), newFingers.at(nearestNeighborIndex).getId()); // does someone already want this finger as his follower?


					if (it != wantedFingers.end()) { // if yes, need to check who is closer
						int index = it - wantedFingers.begin();
						int indexEnemyFinger = inQueue.at(index);// this is the finger also wanting the new finger - if i delete this



						if (fingers.at(indexEnemyFinger).wantedDist < smallestDistance) { // if other one is closer, delete current one
							fingers.erase(fingers.begin() + i); //this finger found no follower, so its not considered anymore
							cout << "Finger deleted" << endl;
						}
						else { // delete other one
							fingers.erase(fingers.begin() + indexEnemyFinger);
							inQueue.at(index) = i; // other finger who was entered into the queue is not considered anymore
						}
					}


					else { // no collision
						wantedFingers.push_back(newFingers.at(nearestNeighborIndex).getId());  // if no collision
						inQueue.push_back(i);
					}
				}


				// give old Fingers the wanted ones


				if (currentFrame == 49) {
					int debug = 1;
				}
				for (int i = 0; i < fingers.size(); i++) { // give every finger what they want

					if (currentFrame >= 48) {
						cout << "Current Frame: " << currentFrame << endl;
						cout << "Wert von i:" << i << " und größe des vek: " << fingers.size() << endl;
						cout << "wo er anfaengt rumzuheulen: " << fingers.at(i).getCoords().x << " und y: " << fingers.at(i).getCoords().y << endl;
					}
					//fingers.at(i).setCoords(fingers.at(i).getWanted().getCoords()); // getWanted funktioniert net
					//fingers.at(i).setXCoord(fingers.at(i).getWanted().getXCoord());
					fingers.at(i).setXCoord(1.2);
					//fingers.at(i).setYCoord(fingers.at(i).getWanted().getYCoord());
					//fingers.at(i).setWanted(NULL);
					//fingers.at(i).wantedDist(NULL);
					for (int j = 0; j < newFingers.size(); j++) {
						if (newFingers.at(j).getId() == fingers.at(i).idWanted) {
							newFingers.erase(newFingers.begin() + j);
							break;
						}
					}
					fingers.at(i).idWanted = NULL;
				}
				//add new fingers
				while (newFingers.size() > 0) {
					fingers.push_back(newFingers.front());
					newFingers.erase(newFingers.begin());
				}


			}


			newFingers.clear(); // delete new ones to be able to get new data


		}

		if (cv::waitKey(0) == 27) // wait for user input
		{
			std::cout << "TERMINATION: User pressed ESC\n";
			break;
		}


		currentFrame++;

		// time end
		ms_end = clock();
		ms_time = ms_end - ms_start;

		putText(original, "frame #" + (std::string)_itoa(currentFrame, buffer, 10), cvPoint(0, 15), cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 255, 255), 1, 8); // write framecounter to the image (useful for debugging)
		putText(original, "time per frame: " + (std::string)_itoa(ms_time, buffer, 10) + "ms", cvPoint(0, 30), cv::FONT_HERSHEY_PLAIN, 1, CV_RGB(255, 255, 255), 1, 8); // write calculation time per frame to the image

		imshow("window", original); // render the frame to a window	
		imshow("window2", original2); // render the frame to a window	
	}

	std::cout << "SUCCESS: Program terminated like expected.\n";
	return 1;
}