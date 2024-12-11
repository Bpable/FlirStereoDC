#pragma once

#include <string>
#include <vector>
#include <filesystem>

class ConnectDrive
{
public:
    ConnectDrive() = default;

    // Function to search for a USB drive by name
    std::string connectUSBDrive(const std::string& baseDir);

private:
    std::string driveName;
    std::string basePath = "/media/";
};

