#ifndef TARGET_H
#define TARGET_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
class Armor
{
public:
    std::vector<cv::RotatedRect> Rects;
    Armor(const std::vector<std::vector<cv::Point>> &contours);
    int AreaCout(cv::RotatedRect &a, cv::RotatedRect &b);
    int AngleCout(cv::RotatedRect &a, cv::RotatedRect &b);
    int DistanceCout(cv::RotatedRect &a, cv::RotatedRect &b);
    bool isArmor(cv::RotatedRect &a, cv::RotatedRect &b);
};
cv::Mat Preprocessing(cv::Mat &src);
std::vector<cv::RotatedRect> FindRects(cv::Mat &edges);
void DrawAmrorPlate(cv::Mat &src, std::vector<cv::RotatedRect> &AmrorPlate);
#endif