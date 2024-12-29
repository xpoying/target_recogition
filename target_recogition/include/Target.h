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
    struct Light
    {
        cv::RotatedRect light;
        double area;
        double angle;
    };

    struct Armor
    {
        armorDetector::Light left_light;
        armorDetector::Light right_light;
    };

    explicit armorDetector(double max_height_difference, double max_length_width_ratio, double min_length_width_ratio);
    void ProcessImage(const cv::Mat &image, cv::Mat &edges, const float threshold);
    void DrawArmor(const cv::Mat &image, Armor &target_Armor);
    bool IsLight(cv::RotatedRect &minRects);
    bool MatchArmor(const armorDetector::Light &right_light, const armorDetector::Light &left_light);

private:
    double max_height_difference_;
    double max_length_width_ratio_;
    double min_length_width_ratio_;
};
#endif
