#include "connectflir.h"
#include <ctime>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;

ConnectFlir::ConnectFlir(const std::string& usbPath, const std::string& signature)
    : usbPath(usbPath), signature(signature), imageCount(0), folderCount(0)
{
    createDateFolder();
}

ConnectFlir::~ConnectFlir()
{
    if (camera)
    {
        camera->DeInit();
        camera = nullptr;
    }
    cameraList.Clear();
    system->ReleaseInstance();
}

bool ConnectFlir::findCamera()
{
    system = Spinnaker::System::GetInstance();
    cameraList = system->GetCameras();

    if (cameraList.GetSize() == 0)
    {
        std::cerr << "No cameras detected!" << std::endl;
        return false;
    }

    camera = cameraList.GetByIndex(0);
    camera->Init();
    std::cout << "Camera initialized successfully!" << std::endl;
    return true;
}

void ConnectFlir::captureImages()
{
    try
    {
        camera->BeginAcquisition();
        std::cout << "Starting camera acquisition..." << std::endl;

        // Create OpenCV window for displaying the stream
        const std::string windowName = "FLIR Live Stream";
        cv::namedWindow(windowName, cv::WINDOW_NORMAL);

        curSubFolderPath = createSubFolder(dateFolderPath, folderCount);

        while (true)
        {
            std::ostringstream imageFileNameStream;
            imageFileNameStream << curSubFolderPath << "/Flir_"
                                                    << std::setfill('0') << std::setw(3) << folderCount << "_"
                                                    << std::setfill('0') << std::setw(4) << imageCount << ".jpg";

            std::string imageFileName = imageFileNameStream.str();
            
            try
            {
                Spinnaker::ImagePtr spinImage = camera->GetNextImage();
                if (spinImage->IsIncomplete())
                {
                    std::cerr << "Image incomplete, status: " << spinImage->GetImageStatus() << std::endl;
                    continue;
                }

                // Converting to opencv mat type
                cv::Mat cvImage = spinnakerToOpenCV(spinImage);

                cv::imshow(windowName, cvImage);
                if (cv::waitKey(1) == 27) {
                std::cout << "Exiiting live stream..." << std::endl;
                break;
                }
                
                // saves the frame
                if (!cv::imwrite(imageFileName, cvImage))
                {
                    std::cerr << "Failed to save image to: " << imageFileName << std::endl;
                }
                else
                {
                    std::cout << "Saved image: " << imageFileName << std::endl;
                }

                spinImage->Release();
            }
            catch (const Spinnaker::Exception& e)
            {
                std::cerr << "Error capturing image: " << e.what() << std::endl;
                continue;
            }

            imageCount++;
            if (imageCount >= 1000)
            {
                folderCount++;
                curSubFolderPath = createSubFolder(dateFolderPath, folderCount);
                imageCount = 0;
            }
        }

        camera->EndAcquisition();
        cv::destroyWindow(windowName);
    }
    catch (const Spinnaker::Exception& e)
    {
        std::cerr << "Error starting acquisition: " << e.what() << std::endl;
    }
}

void ConnectFlir::createDateFolder()
{
    std::time_t t = std::time(nullptr);
    std::tm* localTime = std::localtime(&t);

    std::ostringstream dateStream;
    dateStream << std::put_time(localTime, "%d_%m_%y");
    std::string dateString = dateStream.str();

    dateFolderPath = usbPath + "/" + dateString;
    if (!fs::exists(dateFolderPath))
    {
        fs::create_directories(dateFolderPath);
        std::cout << "Created directory: " << dateFolderPath << std::endl;
    }
    else
    {
        std::cout << "Directory already exists: " << dateFolderPath << std::endl;
    }

    // Determine folderCount
    folderCount = findNextAvailableFlirFolder(dateFolderPath);
    std::cout << "Next available Flir folder count: " << folderCount << std::endl;
}

int ConnectFlir::findNextAvailableFlirFolder(const std::string& basePath)
{
    int folderIndex = 0;
    while (true)
    {
        std::ostringstream folderStream;
        folderStream << basePath << "/FLIR_" << std::setw(3) << std::setfill('0') << folderIndex;

        if (!fs::exists(folderStream.str()))
        {
            // Found the next available folder
            break;
        }
        folderIndex++;
    }
    return folderIndex;
}

std::string ConnectFlir::createSubFolder(const std::string& basePath, int folderCount)
{
    std::ostringstream folderStream;
    folderStream << basePath << "/FLIR_" << std::setw(3) << std::setfill('0') << folderCount;

    std::string newFolderPath = folderStream.str();

    if (!fs::exists(newFolderPath))
    {
        if (fs::create_directories(newFolderPath))
        {
            std::cout << "Created new FLIR subfolder: " << newFolderPath << std::endl;
        }
        else
        {
            std::cerr << "Failed to create FLIR subfolder: " << newFolderPath << std::endl;
        }
    }
    else
    {
        std::cout << "FLIR subfolder already exists: " << newFolderPath << std::endl;
    }

    return newFolderPath;
}

cv::Mat ConnectFlir::spinnakerToOpenCV(Spinnaker::ImagePtr spinImage)
{
    processor.SetColorProcessing(Spinnaker::SPINNAKER_COLOR_PROCESSING_ALGORITHM_HQ_LINEAR);
    Spinnaker::ImagePtr convertedImage = processor.Convert(spinImage, Spinnaker::PixelFormat_BGR8);

    int width = convertedImage->GetWidth();
    int height = convertedImage->GetHeight();
    unsigned char* imageData = static_cast<unsigned char*>(convertedImage->GetData());

    return cv::Mat(height, width, CV_8UC3, imageData).clone();
}

