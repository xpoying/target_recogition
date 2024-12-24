// Author:xpoying
//函数的定义： 识别装甲板，先预处理图像，转为灰度图后二值化，开运算，找出轮廓，描绘最小矩形，根据其匹配程度（角度差，面积差，水平程度）找对应的装甲板

#include "opencv2/opencv.hpp"
#include <vector>
#include <iostream>
#include "Target.h"

// 构造函数  初步筛选装甲板并加入数组
void armorDetector::Select_possible_rectangles(const std::vector<std::vector<cv::Point>> &contours) 
{
    for (int i = 0; i < contours.size(); ++i)
    {
        cv::Point2f vec[4];
        cv::RotatedRect minRects = cv::minAreaRect(cv::Mat(contours[i]));
        minRects.points(vec);
        if (minRects.size.width > minRects.size.height)
        {
            minRects.angle += 90;
            float t = minRects.size.width;
            minRects.size.width = minRects.size.height;
            minRects.size.height = t;
        }
        if ((minRects.size.height / minRects.size.width < plate.max_length_width_ratio) && (plate.min_length_width_ratio < minRects.size.height / minRects.size.width))
        {
            Rects.push_back(minRects);
        }
    }
}
// 两个装甲板面积相差越小得分越高
int armorDetector::Score_based_on_Area(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect)
{
    int cout = 0;
    int Area[2];
    Area[0] = leftRect.size.width * leftRect.size.height;
    Area[1] = rightRect.size.width * rightRect.size.height;

    if (Area[0] == Area[1])
    {
        cout = cout + 10;
    }
    else if (std::min(Area[0], Area[1]) * 1.5 > std::max(Area[0], Area[1]))
    {
        cout = cout + 8;
    }
    else if (std::min(Area[0], Area[1]) * 2 > std::max(Area[0], Area[1]))
    {
        cout = cout + 6;
    }
    else if (std::min(Area[0], Area[1]) * 3 > std::max(Area[0], Area[1]))
    {
        cout = cout + 4;
    }
    else if (std::min(Area[0], Area[1]) * 4 > std::max(Area[0], Area[1]))
    {
        cout = cout + 2;
    }
    else
    {
    }
    return cout;
}
// 两个装甲板角度差越小得分越高
int armorDetector::Score_based_on_Angle(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect)
{
    int cout = 0;
    if (abs(leftRect.angle - rightRect.angle) < 5)
    {
        cout = cout + 10;
    }
    else if (abs(leftRect.angle - rightRect.angle) < 10)
    {
        cout = cout + 6;
    }
    else if (abs(leftRect.angle - rightRect.angle) < 30)
    {
        cout = cout + 4;
    }
    else if (abs(leftRect.angle - rightRect.angle) < 90)
    {
        cout = cout + 2;
    }
    else
    {
    }
    return cout;
}
// 两个装甲板越水平得分越高
int armorDetector::Score_based_on_Proficiency(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect)
{
    int level = 0;
    double half_height = (leftRect.size.height + rightRect.size.height) / 4;
    if (leftRect.center.y == rightRect.center.y)
    {
        level += 10;
    }
    else if (abs(leftRect.center.y - rightRect.center.y) < 0.2 * half_height)
    {
        level += 8;
    }
    else if (abs(leftRect.center.y - rightRect.center.y) < 0.4 * half_height)
    {
        level += 6;
    }
    else if (abs(leftRect.center.y - rightRect.center.y) < 0.8 * half_height)
    {
        level += 4;
    }
    else if (abs(leftRect.center.y - rightRect.center.y) < half_height)
    {
        level += 1;
    }
    else
    {
    }
    return level;
}
// 排除非匹配装甲板
bool armorDetector::isArmor(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect)
{
    if (std::abs(leftRect.center.y - rightRect.center.y) < plate.max_height_difference)
        return true;
    else
        return false;
}

// 图像的预处理
cv::Mat Image_preprocessing(const cv::Mat &kSrc)
{ // 初始化
    cv::Mat thr, gray, dil, edges;
    cv::Mat ken = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    double threshold1 = 3;
    double threshold2 = 9;

    // 处理图像  二值化 开运算  轮廓增强
    cv::cvtColor(kSrc, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thr, 160, 255, cv::THRESH_BINARY);
    cv::morphologyEx(thr, dil, cv::MORPH_OPEN, ken, cv::Point(-1, -1));
    cv::Canny(dil, edges, threshold1, threshold2);
    return edges;
}
// 筛选匹配的装甲板并加入vector容器中
std::vector<cv::RotatedRect> FindRects(const cv::Mat &kEdges)
{
    std::vector<std::vector<cv::Point>> contours;
    armorDetector plate;
    std::vector<std::vector<int>> reliability;

    cv::findContours(kEdges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    plate.Select_possible_rectangles(contours);

    for (int i = 0; i < plate.Rects.size(); i++)
    {
        for (int j = i + 1; j < plate.Rects.size(); j++)
        {
            int cout = 0, angle = 0, Area = 0, Proficiency = 0;
            cv::RotatedRect leftRect = plate.Rects[i];
            cv::RotatedRect rightRect = plate.Rects[j];
            angle = plate.Score_based_on_Angle(leftRect, rightRect);
            Area = plate.Score_based_on_Area(leftRect, rightRect);
            if (Area == 0)
            {
                break;
            }
            Proficiency = plate.Score_based_on_Proficiency(leftRect, rightRect);
            if (Proficiency == 0)
            {
                break;
            }
            if (!plate.isArmor(leftRect, rightRect))
            {
                break;
            }
            cout = Area + angle + Proficiency;
            std::vector<int> temp = {i, j, cout};
            reliability.push_back(temp);
        }
    }
    std::vector<cv::RotatedRect> AmrorPlate;
    if (reliability.empty())
    {
    }
    else
    {
        int max_score = 0;
        int x = 0, y = 0;
        // 选择评分最高的
        for (int i = 0; i < reliability.size(); i++)
        {
            if (reliability[i][2] > max_score)
            {
                max_score = reliability[i][2];
                x = reliability[i][0];
                y = reliability[i][1];
            }
        }
        AmrorPlate.push_back(plate.Rects[x]);
        AmrorPlate.push_back(plate.Rects[y]);
    }
    return AmrorPlate;
}
// 标记出目标位置
void Mark_AmrorPlate(const cv::Mat &kSrc,const std::vector<cv::RotatedRect> &kAmrorPlate)
{

    if (kAmrorPlate.size() < 2)
    {
        return;
    }
    cv::RotatedRect left = kAmrorPlate[0];
    cv::RotatedRect right = kAmrorPlate[1];

    std::vector<cv::Point2f> leftxy, rightxy;
    right.points(rightxy);
    left.points(leftxy);

    cv::Point2f a1 = (leftxy[1] + leftxy[2]) / 2;
    cv::Point2f a2 = (leftxy[0] + leftxy[3]) / 2;
    cv::Point2f b1 = (rightxy[1] + rightxy[2]) / 2;
    cv::Point2f b2 = (rightxy[0] + rightxy[3]) / 2;

    // 校正坐标
    if (a1.y < a2.y)
    {
        auto leftRect = a1;
        a1 = a2;
        a2 = leftRect;
    }
    if (b1.y < b2.y)
    {
        auto rightRect = b1;
        b1 = b2;
        b2 = rightRect;
    }
    cv::line(kSrc, a1, b2, cv::Scalar(0, 255, 255), 3);
    cv::line(kSrc, a2, b1, cv::Scalar(0, 255, 255), 3);
    for (int l = 0; l < 4; l++)
    {
        cv::line(kSrc, leftxy[l], leftxy[(l + 1) % 4], cv::Scalar(0, 0, 225), 3, cv::LINE_8, 0);
        cv::line(kSrc, rightxy[l], rightxy[(l + 1) % 4], cv::Scalar(0, 0, 225), 3, cv::LINE_8, 0);
    }
}