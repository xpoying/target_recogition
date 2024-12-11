#ifndef TARGET_H
#define TARGET_H

#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
class Target
{
public:
    cv::Mat Preprocessing(cv::Mat &src);
    void findrects(cv::Mat &src, cv::Mat &dst);
    void dealrects();
    void drawrect(cv::Mat &dst);

private:
 std::vector<cv::RotatedRect> Rects;
 std::vector<std::vector<int>> reliability;
 cv::RotatedRect left, b;
};

#endif