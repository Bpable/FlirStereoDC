# Flir & Stereo Data collection

The project combines both the flir and stereo cameras used in data collection. To function at the same time.

## Description

The code is similar to previous versions, with minor alterations and tweaks, along with threading. To accomadte two cameras using different software to work at the same time.

### Dependencies

This requires opencv version 4.0 or above.
The requires the Zed sdk.
The spinnaker sdk & freeImage.

### GPU Usage
This program should be loaded in a build folder of the respected gpu. To use it for data collection do the following.
1. Go to the program "Startup Applications"
2. Click Add
3. Give a brief name and comment
4. In the command section click browse, then navigate and select the executable.
5. Reboot the gpu.

At that point on startup the program should execute and begin data collection immediately.
