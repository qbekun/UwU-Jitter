#include <windows.h>
#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <fstream>
#include <random>
#include <conio.h>
#include <shlobj.h> 

std::string GenerateUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);
    return "UID:" + std::to_string(dis(gen));
}

std::wstring GetActiveWindowTitle() {
    wchar_t wnd_title[256];
    HWND hwnd = GetForegroundWindow();
    GetWindowTextW(hwnd, wnd_title, sizeof(wnd_title) / sizeof(wnd_title[0]));
    return wnd_title;
}

std::wstring GetSerialPort() {
    std::vector<std::wstring> serialPorts = { L"COM1", L"COM2", L"COM3", L"COM4", L"COM5", L"COM6", L"COM7", L"COM8", L"COM9" };

    for (const auto& port : serialPorts) {
        HANDLE hSerial = CreateFileW(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hSerial != INVALID_HANDLE_VALUE) {
            CloseHandle(hSerial);
            return port;
        }
    }
    return L"";
}

void SendCommandToArduino(HANDLE hSerial, const std::string& command) {
    DWORD bytesWritten;
    if (!WriteFile(hSerial, command.c_str(), command.size(), &bytesWritten, NULL)) {
        std::cout << "OwO Faiwud to wwite to Awduino! 😿" << std::endl;
    }
}

int ReadPowerLevel() {
    int power = 5;
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        std::string filePath = std::string(appDataPath) + "\\fsb.cc\\apex\\power.uwu";
        std::ifstream powerFile(filePath);
        if (powerFile.is_open()) {
            powerFile >> power;
            powerFile.close();
        }
    }
    return power;
}

void SetPowerLevel(int power) {
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        std::string filePath = std::string(appDataPath) + "\\fsb.cc\\apex\\power.uwu";
        std::ofstream powerFile(filePath);
        if (powerFile.is_open()) {
            powerFile << power;
            powerFile.close();
            std::cout << "Jittew vawue set to: " << power << " OwO" << std::endl;
        }
    }
}

bool isGameActive = false;

void RecoilThread(HANDLE hSerial) {
    for (;;) {
        std::wstring currentWindowTitle = GetActiveWindowTitle();
        if (currentWindowTitle == L"Apex Legends") {
            if (!isGameActive) {
                isGameActive = true;
            }

            while (GetAsyncKeyState(VK_LBUTTON) & 0x8000 && GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
                int powerLevel = ReadPowerLevel();
                std::string command = "M" + std::to_string(-powerLevel) + "," + std::to_string(powerLevel) + "\n";
                SendCommandToArduino(hSerial, command);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));

                command = "M" + std::to_string(powerLevel) + "," + std::to_string(-powerLevel) + "\n";
                SendCommandToArduino(hSerial, command);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
        else {
            if (isGameActive) {
                isGameActive = false;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void CreatePowerFile() {
    std::ofstream powerFile("power.uwu");
    if (powerFile.is_open()) {
        powerFile << 5;
        powerFile.close();
    }
}

int LoadConfig() {
    int power = 5;
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        std::string configPath = std::string(appDataPath) + "\\fsb.cc\\apex\\power.uwu";
        std::ifstream configFile(configPath);
        if (configFile.is_open()) {
            configFile >> power;
            configFile.close();
        }
    }
    return power;
}

void SaveConfig(int power) {
    char appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, appDataPath))) {
        std::string configPath = std::string(appDataPath) + "\\fsb.cc\\apex\\power.uwu";
        std::ofstream configFile(configPath);
        if (configFile.is_open()) {
            configFile << power << std::endl;
            configFile.close();
            std::cout << "Cwonfig saved! UwU" << std::endl;
        }
    }
}

void DisplayMenu(int& power) {
    int choice = 0;
    bool quit = false;

    while (!quit) {
        system("cls");

        std::string uid = GenerateUID();
        std::wstring serialPort = GetSerialPort();
        std::cout << "\033[1;35m"
            << "   ___         __                             \n"
            << " /'___\\       /\\ \\                            \n"
            << "/\\ \\__/   ____\\ \\ \\____        ___     ___   \n"
            << "\\ \\ ,__\\ /',__\\\\ \\ '__\\      /'___\\  /'___\\ \n"
            << " \\ \\ \\_//\\__, \\\\ \\ \\L\\ \\ __ /\\ \\__/ /\\ \\__/ \n"
            << "  \\ \\_\\ \\/\\____/ \\ \\_,__//\\_\\\\ \\____\\\\ \\____\\\n"
            << "   \\/_/  \\/___/   \\/___/ \\/_/ \\/____/ \\/____/ \n"
            << "\033[0m";
        std::cout << "" << std::endl;
        std::cout << "https://foreversadboys.ovh/" << std::endl;
        std::cout << "" << std::endl;
        std::cout << uid << " nya~" << std::endl;
        std::cout << "" << std::endl;

        std::cout << (choice == 0 ? "> " : "  ") << "Jittew vawue: " << power << std::endl;
        std::cout << (choice == 1 ? "> " : "  ") << "Save Cwonfig" << std::endl;
        std::cout << (choice == 2 ? "> " : "  ") << "Load Cwonfig" << std::endl;

        int ch = _getch();
        if (ch == 224) {
            ch = _getch();
            if (ch == 72) {
                if (choice > 0) choice--;
            }
            else if (ch == 80) {
                if (choice < 2) choice++;
            }
            else if (ch == 75) {
                if (choice == 0 && power > 0) {
                    power--;
                    SetPowerLevel(power);
                }
            }
            else if (ch == 77) {
                if (choice == 0 && power < 10) {
                    power++;
                    SetPowerLevel(power);
                }
            }
        }
        else if (ch == 13) {
            switch (choice) {
            case 0:
                SetPowerLevel(power);
                break;
            case 1:
                SaveConfig(power);
                break;
            case 2:
                power = LoadConfig();
                break;
            }
        }
    }
}

int main() {
    SetConsoleTitleA("fsb.cc - UwU Jitter");
    SetPriorityClass(GetCurrentProcess(), 0x00000080);
    int power = LoadConfig();
    HANDLE hSerial = CreateFileW(GetSerialPort().c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        std::cout << "OwO Faiwud to connect to sewiaw pownt! 😿" << std::endl;
        return 1;
    }

    std::thread recoilThread(RecoilThread, hSerial);

    DisplayMenu(power);

    recoilThread.join();
}
