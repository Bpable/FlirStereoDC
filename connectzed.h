#pragma once

#include <string>
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>
#include <filesystem>

class ConnectZed {
public:
    // Constructor
    explicit ConnectZed(const std::string& base_directory);

    // Initializes the ZED camera
    bool initializeCamera();

    // Captures images and saves them to the USB drive
    void captureImages();

private:
    // Finds the next available folder index
    int findNextAvailableFolder(const std::string& base_directory);

    // Gets the current date formatted as a string
    std::string getCurrentDateFormatted();

    // Creates a new subfolder in the directory
    std::string createNewSubFolder(const std::string& base_directory, int& folder_count);

    // Base directory for saving images
    std::string base_directory;

    // ZED camera object
    sl::Camera zed;

    // Folder and image count tracking
    int image_count;
    int folder_count;
};
