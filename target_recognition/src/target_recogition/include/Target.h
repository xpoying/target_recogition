// Author:xpoying
// 头文件 装甲板结构体，装甲板识别类，标记装甲板的函数声明

#ifndef TARGET_H
#define TARGET_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
class armorDetector
{
public:
    struct Armor
    {

        cv::Point2f right_top_;
        cv::Point2f right_bottom_;
        cv::Point2f left_top_;
        cv::Point2f left_buttom_;
    };

    struct Light
    {
        cv::RotatedRect light;
        double area;
        double angle;
    };

    explicit armorDetector(double max_height_difference, double max_length_width_ratio, double min_length_width_ratio);
    void exectue(const cv::Mat &image);

private:
    //寻找装甲板
    std::vector<std::vector<cv::Point>> ProcessImage(const cv::Mat &image, const float threshold);
    void DrawArmor(const cv::Mat &image,Armor &target_Armor);
    Armor MatchArmor(std::vector<armorDetector::Light> light_Rects);
    std::vector<Light> IsLight(std::vector<std::vector<cv::Point>> contours);

    double max_height_difference_;
    double max_length_width_ratio_;
    double min_length_width_ratio_;
};


int Score_based_on_Area(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect);
int Score_based_on_Angle(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect);
int Score_based_on_Proficiency(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect);
bool isArmor(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect);


#endif
