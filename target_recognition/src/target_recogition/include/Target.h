#ifndef TARGET_H
#define TARGET_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>

struct Armor
{
    double max_height_difference = 15;
    double max_length_width_ratio = 6;
    double min_length_width_ratio = 1;
};

class armorDetector
{
public:
    Armor plate;
    std::vector<cv::RotatedRect> Rects;
    armorDetector(const std::vector<std::vector<cv::Point>> &contours);
    int AreaCout(cv::RotatedRect &left_point, cv::RotatedRect &right_point);
    int AngleCout(cv::RotatedRect &left_point, cv::RotatedRect &right_point);
    int DistanceCout(cv::RotatedRect &left_point, cv::RotatedRect &right_point);
    bool isArmor(cv::RotatedRect &left_point, cv::RotatedRect &right_point);
};

cv::Mat Preprocessing(cv::Mat &src);
std::vector<cv::RotatedRect> FindRects(cv::Mat &edges);
void DrawAmrorPlate(cv::Mat &src, std::vector<cv::RotatedRect> &AmrorPlate);
#endif
