#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <chrono>
#include <fstream>
#include <thread>
#include <windows.h>
#include <cwchar>
#include <filesystem>
#include <opencv2/opencv.hpp>
using namespace std;

int frame_dur = -1, width = 540, height = 180; // set default values

string ascii_alphabet = "";

string videos_folder = "videos";

bool cycle = true;

string pix2ASCII(int pixel_intensity) {
	string ASCII_alp = "@%#*+=-:. ";
	if (ascii_alphabet.length() > 2) {
		ASCII_alp = ascii_alphabet;
	}
	// OTHER ASCII ALPHABETS, WHICH YOU CAN USE IF YOU WANT
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

void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

void SetConsoleFontSize(int width, int height) {

	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	std::wcscpy(cfi.FaceName, L"Consolas"); // Choose your font
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	cfi.cbSize = sizeof(CONSOLE_FONT_INFOEX);
	if (!GetCurrentConsoleFontEx(hConsole, FALSE, &cfi)) {
		std::cerr << "Failed to get console font info" << std::endl;
		return;
	}
	cfi.dwFontSize.X = width;
	cfi.dwFontSize.Y = height;
	if (!SetCurrentConsoleFontEx(hConsole, FALSE, &cfi)) {
		std::cerr << "Failed to set console font info" << std::endl;
		return;
	}

}

void Set_Console_Size(int width, int height, int x, int y) {
	
	SetConsoleFontSize(2, 2);

	HWND console = GetConsoleWindow();
	
	SetWindowPos(console, 0, 1, 1, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	MoveWindow(console, x, y, width, height, TRUE);
}

vector<string> Convert(string video_path) {
	vector<string> result;

	cv::VideoCapture cap(video_path);
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
	int choose_video, change_scale;
	int desktop_hor = 0, desktop_ver = 0;
	GetDesktopResolution(desktop_hor, desktop_ver);
	vector<string> paths;
	vector<string> converted;
	string video_path;
	try {
		
		for (const auto& entry : filesystem::directory_iterator(videos_folder)) {
			paths.push_back(entry.path().string());
		}

		for (int i = 0; i < paths.size(); i++) {
			cout << i << ". " << paths[i] << endl;
		}
	}
	catch (exception e){
		cout << "Can\'t parse \"videos\" directory.\nCheck that program and \"videos\" folder are in the same directory." << endl;
		system("pause");
		return 0;
	}
	

	cout << "Choose video file: ";
	cin >> choose_video;
	cout << "Change width/height default values (width: 540; height: 180)?\n[1 - Y / 2 - N]:" << endl;
	cin >> change_scale;
	switch (change_scale)
	{
	case 1:
		cout << "Width: ";
		cin >> width;
		cout << "Height: ";
		cin >> height;
		Set_Console_Size(desktop_hor - 5, desktop_ver - 5, 0, 0);
		video_path = paths[choose_video];
		converted = Convert(video_path);
		system("cls");
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
		break;
	case 2:
		Set_Console_Size(desktop_hor - 5, desktop_ver - 5, 0, 0);
		video_path = paths[choose_video];
		converted = Convert(video_path);
		system("cls");
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
		break;
	default:
		break;
	}
	system("pause");
	return 0;
}
