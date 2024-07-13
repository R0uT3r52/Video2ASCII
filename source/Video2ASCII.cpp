#include <iostream>
#include <chrono>
#include <fstream>
#include <thread>
#include <windows.h>
#include <filesystem>
#include <opencv2/opencv.hpp>
using namespace std;

int frame_dur = -1;

string ascii_alphabet = "";

string videos_folder = "videos";

bool cycle = true;

//bool config_exist(string path) {
//	ifstream f(path);
//	return f.good();
//}
//
//void read_config(string path) {
//
//	if (config_exist(path)) {
//		fstream config(path);
//		string tmp;
//
//		while (getline(config, tmp)) {
//			if (tmp[0] == '#') continue;
//
//			if (tmp.find("alphabet") != string::npos && tmp[0] != '#' && tmp[0] != ' ') {
//				tmp.erase(tmp.begin(), tmp.begin() + tmp.find("= "));
//				ascii_alphabet = tmp;
//			}
//			if (tmp.find("frame_duration") != string::npos && tmp[0] != '#') {
//				tmp.erase(tmp.begin(), tmp.begin() + tmp.find("= "));
//				if (tmp.length() != 0 && tmp[0] != ' ') frame_dur = stoi(tmp);
//			}
//			if (tmp.find("cycle") != string::npos && tmp[0] != '#') {
//				tmp.erase(tmp.begin(), tmp.begin() + tmp.find("= "));
//				if (tmp.find("true") != string::npos) cycle = true;
//			}
//		}
//		config.close();
//	}
//
//	else {
//		cout << "No config found. Creating one..." << endl;
//
//		string create_conf = R"(
//# - comment
//# Please if you set custom alphabet dont set <space> as first symbol in sequence (Right way: @%#*+=-:. ). In this example space is last symbol
//# Specify alphabet in black -> white (Example: @MBHENR#KWXDFPQASUZbdehx*8Gm&04LOVYkpq5Tagns69owz$CIu23Jcfry%1v7l+it[] {}?j|()=~!-/<>\"^_';,:`. )
//# Frame duration is an int (milliseconds)
//# cycle - true/false (cycles video until program closed)
//
//# Some basic alphabets
//# Best in most cases: @%#*+=-:. 
//# The longest one: @MBHENR#KWXDFPQASUZbdehx*8Gm&04LOVYkpq5Tagns69owz$CIu23Jcfry%1v7l+it[] {}?j|()=~!-/<>\"^_';,:`. 
//# Just another one: !@#%&.+-=
//# And another: $@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\ 
//
//alphabet = 
//
//frame_duration = 5
//
//cycle = 
//
//		)";
//		fstream config(path, ios_base::out);
//
//		config << create_conf << endl;
//		config.close();
//	}
//
//	
//}


string pix2ASCII(int pixel_intensity) {
	string ASCII_alp = "@%#*+=-:. ";
	//const string ASCII_alp = "!@#%&.+-=";
	if (ascii_alphabet.length() > 2) {
		ASCII_alp = ascii_alphabet;
	}
	//string ASCII_alp = "   \" .: -= +*# % @\"";
	//string ASCII_alp = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\ ";
	//string ASCII_alp = "@MBHENR#KWXDFPQASUZbdehx*8Gm&04LOVYkpq5Tagns69owz$CIu23Jcfry%1v7l+it[] {}?j|()=~!-/<>\"^_';,:`. ";
	reverse(ASCII_alp.begin(), ASCII_alp.end());
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
	int width = 675;
	int height = 225;
	cv::Mat frame, gray_frame, resized;
	double fps = cap.get(cv::CAP_PROP_FPS);
	if(frame_dur == -1) frame_dur = 1000 / fps;
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
	//read_config("config.txt");
	int choose_video;
	vector<string> paths;
	for (const auto& entry : filesystem::directory_iterator(videos_folder)) {
		paths.push_back(entry.path().string());
	}

	for (int i = 0; i < paths.size(); i++) {
		cout << i << ". " << paths[i] << endl;
	}

	cout << "Choose video file: ";
	cin >> choose_video;
	

	string video_path = paths[choose_video]; //vid02_02.mp4 vid02.mp4
	vector<string> converted = Convert(video_path);
	if (cycle) {
		while (true) {
			for (string s : converted) {
				cout << s;
				this_thread::sleep_for(chrono::milliseconds(frame_dur));
				clearscreen();
			}
		}
	}
	else {
		for (string s : converted) {
			cout << s;
			this_thread::sleep_for(chrono::milliseconds(frame_dur));
			clearscreen();
		}
	}
	system("pause");
	return 0;
}
