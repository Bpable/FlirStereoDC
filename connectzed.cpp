#include "connectzed.h"

#include <stdio.h>
#include <string.h>
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace fs = std::filesystem;

ConnectZed::ConnectZed(const std::string& base_directory) : base_directory(base_directory), image_count(0), folder_count(0) {}



int ConnectZed::findNextAvailableFolder(const std::string& base_directory) {
    int max_index = -1;
    for (const auto& entry : fs::directory_iterator(base_directory)) {
        if (entry.is_directory()) {
            std::string folder_name = entry.path().filename().string();
            if (folder_name.rfind("ZED_", 0) == 0) { // Looks for folders starting with "ZED_"
                size_t underscore_pos = folder_name.find('_', 4); // Find the next underscore after "ZED_"
                if (underscore_pos != std::string::npos) {
                    int index = std::stoi(folder_name.substr(4, underscore_pos - 4)); // Extract the number after "ZED_"
                    max_index = std::max(max_index, index);
                }
            }
        }
    }
    return max_index + 1; // Start at the next folder
}

std::string ConnectZed::getCurrentDateFormatted() {
    auto now = std::chrono::system_clock::now();
    std::time_t current_time = std::chrono::system_clock::to_time_t(now);
    std::tm *local_time = std::localtime(&current_time);

    // Check if the year is earlier than 2024 and default it to 2024 if so
    int year = local_time->tm_year + 1900; // tm_year is years since 1900
    if (year < 2024) {
        year = 2024;
    }

    std::ostringstream date_stream;
    date_stream << std::setw(2) << std::setfill('0') << local_time->tm_mon + 1 << "_"
                << std::setw(2) << std::setfill('0') << local_time->tm_mday << "_"
                << std::setw(2) << std::setfill('0') << (year % 100);
    return date_stream.str();
}

/*
std::string ConnectZed::findAvailableDrive(const std::string& base_path) {
    for (const auto& entry : fs::directory_iterator(base_path)) {
        if (entry.is_directory()) {
            for (const auto& sub_entry : fs::directory_iterator(entry.path())) {
                if (sub_entry.is_directory()) {
                    return sub_entry.path().string();
                }
            }
        }
    }
    // if nothing is found, return empty
    return "";
}
*/
// Initializes the ZED camera
bool ConnectZed::initializeCamera() {
    sl::InitParameters init_parameters;
    init_parameters.sdk_verbose = true;
    init_parameters.camera_resolution = sl::RESOLUTION::AUTO;
    init_parameters.depth_mode = sl::DEPTH_MODE::NONE;
    init_parameters.async_grab_camera_recovery = true;
    init_parameters.enable_image_validity_check = true;

    auto returned_state = zed.open(init_parameters);
    if (returned_state != sl::ERROR_CODE::SUCCESS) {
        std::cout << "Failed to open ZED camera" << std::endl;
        return false;
    }

    folder_count = findNextAvailableFolder(base_directory);
    return true;
}

std::string ConnectZed::createNewSubFolder(const std::string& base_directory, int& folder_count) {
    std::string folder_name;
    do {
        std::ostringstream folder_name_stream;
        folder_name_stream << base_directory << "/ZED_" << std::setw(3) << std::setfill('0') << folder_count << "_"
                           << getCurrentDateFormatted();
        folder_name = folder_name_stream.str();
        folder_count++;
    } while (fs::exists(folder_name)); // Ensure the folder doesn't already exist

    fs::create_directories(folder_name); // Create the folder after verifying it's unique
    return folder_name;
}

void ConnectZed::captureImages() {
    if (!initializeCamera()) { return; }

    sl::Mat zed_image;
    std::string folder_name = createNewSubFolder(base_directory, folder_count);

    char key = ' ';
    while (key != 'q') {
        auto returned_state = zed.grab();
        if (returned_state == sl::ERROR_CODE::SUCCESS) {
            zed.retrieveImage(zed_image, sl::VIEW::LEFT);
            cv::Mat cvImage = cv::Mat((int)zed_image.getHeight(), (int)zed_image.getWidth(), CV_8UC4, zed_image.getPtr<sl::uchar1>(sl::MEM::CPU));

            if (image_count >= 999) {
                folder_name = createNewSubFolder(base_directory, folder_count);
                image_count = 0;
            }

            std::ostringstream filename_stream;
            filename_stream << folder_name << "/ZED_"
                            << std::setw(3) << std::setfill('0') << (folder_count - 1) << "_"
                            << std::setw(4) << std::setfill('0') << image_count << "_"
                            << getCurrentDateFormatted() << ".jpg";
            std::string filename = filename_stream.str();

            cv::imwrite(filename, cvImage);
            image_count++;
            cv::imshow("Camera View", cvImage);
        }
        key = cv::waitKey(10);
    }

    zed.close();
}

