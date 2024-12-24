// Author:xpoying
//头文件 装甲板结构体，装甲板识别类，标记装甲板的函数声明



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
    void Select_possible_rectangles(const std::vector<std::vector<cv::Point>> &contours);


    int Score_based_on_Area(cv::RotatedRect &left_point, cv::RotatedRect &right_point);
    int Score_based_on_Angle(cv::RotatedRect &left_point, cv::RotatedRect &right_point);
    int Score_based_on_Proficiency(cv::RotatedRect &left_point, cv::RotatedRect &right_point);
    bool isArmor(cv::RotatedRect &left_point, cv::RotatedRect &right_point);
    private:

};

cv::Mat Image_preprocessing(const cv::Mat &kSrc);
std::vector<cv::RotatedRect> FindRects(const cv::Mat &kEdges);
void Mark_AmrorPlate(const cv::Mat &kSrc,const  std::vector<cv::RotatedRect> &kAmrorPlate);
#endif
