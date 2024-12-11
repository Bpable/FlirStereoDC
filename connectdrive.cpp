#include "connectdrive.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

std::string ConnectDrive::connectUSBDrive(const std::string& baseDir)
{
    for (const auto& entry : fs::directory_iterator(baseDir))
   {
      if (entry.is_directory())
      {
         for (const auto& sub_entry : fs::directory_iterator(entry.path()))
         {
            if(sub_entry.is_directory())
            {
               return sub_entry.path().string();
            }
         }
      }
   }
   // if nothing is found we return empty
   return "";
}
