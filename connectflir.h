#pragma once

#include <string>
#include <opencv2/opencv.hpp>
#include <Spinnaker.h>
#include <SpinGenApi/SpinnakerGenApi.h>
#include <filesystem>
#include <iostream>

class ConnectFlir
{
public:
    ConnectFlir(const std::string& usbPath, const std::string& signature);
    ~ConnectFlir();

    bool findCamera();
    void captureImages();

private:
    void createDateFolder();
    std::string createSubFolder(const std::string& basePath, int folderNum);
    cv::Mat spinnakerToOpenCV(Spinnaker::ImagePtr spinImage);
    int findNextAvailableFlirFolder(const std::string& basePath);

    std::string usbPath;
    std::string dateFolderPath;
    std::string signature;
    std::string curSubFolderPath;

    Spinnaker::SystemPtr system;
    Spinnaker::CameraList cameraList;
    Spinnaker::CameraPtr camera;
    Spinnaker::ImageProcessor processor;

    int imageCount;
    int folderCount;
};
