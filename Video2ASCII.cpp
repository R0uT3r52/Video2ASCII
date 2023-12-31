#include <iostream>
#include <chrono>
#include <thread>
#include <windows.h>
#include <opencv2/opencv.hpp>
using namespace std;

int frame_dur;


string pix2ASCII(int pixel_intensity) {
	const string ASCII_alp = "!@#%&.+-=";
	string s = string(1, ASCII_alp[pixel_intensity * ASCII_alp.length() / 256]); //getting symbol index and making string
	return s;
}
void ShowConsoleCursor(bool showFlag)
{
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_CURSOR_INFO     cursorInfo;

	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}

vector<string> Convert(string video_path) {
	vector<string> result;

	cv::VideoCapture cap(video_path);
	//getchar();
	int width = 150;
	int height = 50;
	cv::Mat frame, gray_frame, resized;
	double fps = cap.get(cv::CAP_PROP_FPS);
	frame_dur = 1000 / fps;
	cout << "Starting conversion" << endl;
	while (true) {
		cap >> frame;
		if (frame.empty()) {
			break;
		}
		cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
		cv::resize(gray_frame, resized, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);

		string ascii_frame;
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				ascii_frame += pix2ASCII(resized.at<unsigned char>(i, j));
			}
			ascii_frame += "\n";
		}
		//cout << ascii_frame;
		result.push_back(ascii_frame);
		this_thread::sleep_for(chrono::milliseconds(10));
	}
	return result;
}

void clearscreen()
{
	HANDLE hOut;
	COORD Position;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	Position.X = 0;
	Position.Y = 0;
	SetConsoleCursorPosition(hOut, Position);
}

int main()
{
	string video_path = "C:\\C++\\Video2ASCII\\videos\\vid02_02.mp4"; //vid02_02.mp4 vid02.mp4
	vector<string> converted = Convert(video_path);
	for (string s : converted) {
		cout << s;
		this_thread::sleep_for(chrono::milliseconds(frame_dur));
		clearscreen();
	}

	return 0;
}
