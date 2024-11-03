#include <windows.h>
#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <fstream>
#include <random>

// Generowanie losowego UID
std::string GenerateUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(100000, 999999);  // Zakres UID
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
        std::cout << "Failed to write to Arduino!" << std::endl;
    }
}

int ReadPowerLevel() {
    int power = 5;
    std::ifstream powerFile("power.uwu");
    if (powerFile.is_open()) {
        powerFile >> power;
        powerFile.close();
    }
    return power;
}

void SetPowerLevel(int power) {
    std::ofstream powerFile("power.uwu");
    if (powerFile.is_open()) {
        powerFile << power;
        powerFile.close();
        std::cout << "Power level set to: " << power << std::endl;
    }
}

bool isGameActive = false;

void RecoilThread(HANDLE hSerial) {
    for (;;) {
        std::wstring currentWindowTitle = GetActiveWindowTitle();
        if (currentWindowTitle == L"Apex Legends") {
            if (!isGameActive) {
                //std::cout << "Recoil control activated! uwu~" << std::endl;
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
                //std::cout << "Exited Apex Legends." << std::endl;
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

void PowerInputThread() {
    int power;
    bool firstPrompt = true;
    while (true) {
        if (firstPrompt) {
            std::cout << "Enter power level (0-6): ";
            firstPrompt = false;
        }
        std::cin >> power;
        if (power >= 0 && power <= 6) {
            SetPowerLevel(power);
        }
        else {
            std::cout << "Invalid power level. Please enter a value between 0 and 6." << std::endl;
        }
    }
}

int main() {
    CreatePowerFile();

    SetConsoleTitleA("fsb.cc - UwU Jitter");
    SetPriorityClass(GetCurrentProcess(), 0x00000080);

    // Generowanie losowego UID przy starcie programu
    std::string uid = GenerateUID();

    std::cout << "\033[1;35m"
        << "   ___         __                             \n"
        << " /'___\\       /\\ \\                            \n"
        << "/\\ \\__/   ____\\ \\ \\____        ___     ___   \n"
        << "\\ \\ ,__\\ /',__\\\\ \\ '__`\\      /'___\\  /'___\\ \n"
        << " \\ \\ \\_//\\__, `\\\\ \\ \\L\\ \\ __ /\\ \\__/ /\\ \\__/ \n"
        << "  \\ \\_\\ \\/\\____/ \\ \\_,__//\\_\\\\ \\____\\\\ \\____\\\n"
        << "   \\/_/  \\/___/   \\/___/ \\/_/ \\/____/ \\/____/ \n"
        << "\033[0m";
    std::cout << "" << std::endl;
    std::cout << "https://foreversadboys.ovh/" << std::endl;
    std::cout << "" << std::endl;
    std::cout << uid << " nya~" << std::endl;

    std::wstring serialPort = GetSerialPort();
    if (!serialPort.empty()) {
        HANDLE hSerial = CreateFileW(serialPort.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hSerial != INVALID_HANDLE_VALUE) {
            std::wcout << L"Connected to Arduino on port: " << serialPort << L" uwu~" << std::endl;

            DWORD threadId;
            HANDLE threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RecoilThread, hSerial, 0, &threadId);
            if (threadHandle == NULL) {
                std::cout << "Failed to create thread!" << std::endl;
                CloseHandle(hSerial);
                return 1;
            }

            HANDLE inputThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PowerInputThread, NULL, 0, NULL);
            if (inputThreadHandle == NULL) {
                std::cout << "Failed to create input thread!" << std::endl;
                CloseHandle(hSerial);
                return 1;
            }

            for (;;) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            CloseHandle(hSerial);
            CloseHandle(threadHandle);
            CloseHandle(inputThreadHandle);
        }
        else {
            std::wcout << L"Failed to open serial port!" << std::endl;
        }
    }
    else {
        std::wcout << L"No Arduino found on available ports. " << std::endl;
    }

    return 0;
}
