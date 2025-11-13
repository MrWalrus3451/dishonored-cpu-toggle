#include <iostream>
#include <Windows.h>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

void printState(int maxCores, int minCores, bool limiting) {
    system("cls");
    std::cout << ("Max cores: " + std::to_string(maxCores));
    std::cout << ("\nMin Cores: " + std::to_string(minCores));
    if (limiting) {
        std::cout << "\n\nLimiting Dishonored!";
    }
    else {
        std::cout << "\n\nNot limiting Dishonored!";
    }
}

void runAffinity(int coreMask) {
    ofstream file;
    file.open("test.ps1");
    string powershell;
    powershell = "$Process = Get-Process Dishonored; $Process.ProcessorAffinity=" + std::to_string(coreMask);
    file << powershell << endl;
    file.close();
    system("powershell -ExecutionPolicy Bypass -F test.ps1");
    remove("test.ps1");
}

int main()
{
    bool limitedCores = false;
    bool pressDown = false;

    ifstream settingFile;
    string line;

    int maxCores;
    int minCores;

    int maxCoresMask;
    int minCoresMask;

    unsigned int key;

    settingFile.open("settings.ini");
    if (settingFile.is_open())
    {
        while (getline(settingFile, line))
        {
            if (line.find("maxcorecount=") != std::string::npos) {
                std::string maxCoresStr = line.substr(line.find("=") + 1, line.capacity() - line.find("="));
                maxCores = std::stoi(maxCoresStr);
                maxCoresMask = pow(2, maxCores) - 1;
            } else if (line.find("limitedcorecount=") != std::string::npos) {
                std::string limitedCoresStr = line.substr(line.find("=") + 1, line.capacity() - line.find("="));
                minCores = std::stoi(limitedCoresStr);
                minCoresMask = pow(2, minCores) - 1;
            } else if (line.find("togglekey=") != std::string::npos) {
                std::string toggleKeyStr = line.substr(line.find("=") + 1, line.capacity() - line.find("="));
                std::stringstream ss;
                ss << std::hex << toggleKeyStr;
                ss >> key;
            }
        }
        printState(maxCores, minCores, limitedCores);
        settingFile.close();
    }

    while (true) {
        if (GetKeyState(key) & 0x8000 && !pressDown) {
            pressDown = true;
            if (!limitedCores) {
                limitedCores = true;
                runAffinity(minCoresMask);
            }
            else {
                limitedCores = false;
                runAffinity(maxCoresMask);
            }
            printState(maxCores, minCores, limitedCores);
        }
        else if (!(GetKeyState(key) & 0x8000)) {
            pressDown = false;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}