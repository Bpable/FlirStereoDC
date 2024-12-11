#include <iostream>
#include <string>
#include <thread>
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>
#include "connectdrive.h"
#include "connectflir.h"
#include "connectzed.h"

void runZedCamera(const std::string& stereoPath)
{
    ConnectZed zedCamera(stereoPath);
    zedCamera.captureImages();
}

void runFlirCamera(const std::string& usbPath, const std::string& signature)
{
    ConnectFlir connectFlirCamera(usbPath, signature);

    if (connectFlirCamera.findCamera()) {
        std::cout << "FLIR Camera connected. Starting image capture..." << std::endl;
        connectFlirCamera.captureImages();
    } else {
        std::cerr << "FLIR Camera not found. Exiting." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Default parameters
    std::string defaultDriveName = "Drive2";
    std::string driveName = defaultDriveName;
    std::string signature = "Data_Collection";

    // Parse command-line arguments
    if (argc > 1) driveName = argv[1];
    if (argc > 2) signature = argv[2];

    // Connect to USB drive
    ConnectDrive connectDrive;
    std::string base_directory = "/media/";
    std::string usbPath = connectDrive.connectUSBDrive(base_directory);

    if (usbPath.empty()) {
        std::cerr << "Specified USB drive (" << driveName << ") not found. Attempting to scan all drives..." << std::endl;
        usbPath = connectDrive.connectUSBDrive(defaultDriveName); // Scan all available drives
    }

    if (!usbPath.empty()) {
        std::cout << "USB Drive found at: " << usbPath << std::endl;
    } else {
        std::cerr << "No USB drives detected. Exiting." << std::endl;
        return -1;
    }
 
    std::string stereoPath = usbPath += "/Stereo_Data_Collection/";
    std::filesystem::create_directories(stereoPath);

    // Create each thread for the cameras.
    std::thread zedThread(runZedCamera, stereoPath);
    std::thread flirThread(runFlirCamera, usbPath, signature);

    // Should wait for both threads to finish.
    zedThread.join();
    flirThread.join();

    return 0;
}

