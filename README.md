## Source code of the paper "Pedestrian Detection System for Vehicle Images"
Modified from https://github.com/opencv/opencv_contrib  
Use ROIs (Region of Interest) and Fast Feature Pyramids to improve speed of detection.  
Need OpenCV3 library.  
  
## How to use this program
```
ROIsDPM(.exe) videoPath modelPath resultPathToSave skyline coefficient [logPath [overlapThreshold]]
```
videoPath - Path of input video.  
modelPath - Path of model ex. ../data/inriaperson.xml[1]  
resultPathToSave - Detection result path. Format refer to ../include/rectData.h  
skyline - Hieght of skyline in the input video.  
coefficient - Relationship coefficient between hieght and distance.  
  
## About "Pedestrian Detection System for Vehicle Images"
In our work, the ROIs (Region of Interest) are firstly predicted based on the camera status of video to reduce unnecessary feature calculation and target search.
Then, the Fast Feature Pyramids algorithm is employed to calculate features to further reduce the time spent in the feature calculation phase.
Finally, Cascade DPM (Deformable Part Models) method is utilized to detect pedestrians.
The speed of our proposed system can uplift the speed to 2.54 times faster than Cascade DPM with slightly lowering precision and recall rate.
  
## Links

[1] From https://dl.dropboxusercontent.com/s/gh7reh931y1wqmf/voc-release5.tgz?dl=0