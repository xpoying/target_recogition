// Author:xpoying
// 函数的定义： 识别装甲板，先预处理图像，转为灰度图后二值化，开运算，找出轮廓，描绘最小矩形，根据其匹配程度（角度差，面积差，水平程度）找对应的装甲板

#include "opencv2/opencv.hpp"
#include <vector>
#include <iostream>
#include "Target.h"
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
armorDetector::armorDetector(double max_height_difference, double max_length_width_ratio, double min_length_width_ratio) : max_height_difference_(max_height_difference),
                                                                                                                           max_length_width_ratio_(max_length_width_ratio),
                                                                                                                           min_length_width_ratio_(min_length_width_ratio) {};
std::vector<std::vector<cv::Point>> armorDetector::ProcessImage(const cv::Mat &image, const float threshold)
{
    std::vector<std::vector<cv::Point>> contours;
    cv::Mat thr, gray, dil, edges, binary_image;
    cv::Mat ken = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    double threshold1 = 3;
    double threshold2 = 9;

    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thr, threshold, 255, cv::THRESH_BINARY);
    cv::morphologyEx(thr, dil, cv::MORPH_OPEN, ken, cv::Point(-1, -1));
    cv::Canny(dil, binary_image, threshold1, threshold2);
    cv::findContours(binary_image, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    return contours;
}

std::vector<armorDetector::Light> armorDetector::IsLight(std::vector<std::vector<cv::Point>> contours)
{
    std::vector<Light> light_Rects;
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
        if ((minRects.size.height / minRects.size.width < max_length_width_ratio_) && (min_length_width_ratio_ < minRects.size.height / minRects.size.width))
        {
            Light minRects_ = {minRects, minRects.size.width * minRects.size.height, minRects.angle};
            light_Rects.push_back(minRects_);
        }
    }
    return light_Rects;
}

armorDetector::Armor armorDetector::MatchArmor(std::vector<armorDetector::Light> light_Rects)
{
    Armor target_Armor;
    std::vector<std::vector<int>> reliability;
    for (int i = 0; i < light_Rects.size(); i++)
    {
        for (int j = i + 1; j < light_Rects.size(); j++)
        {
            int cout = 0, angle = 0, Area = 0, Proficiency = 0;
            cv::RotatedRect leftRect = light_Rects[i].light;
            cv::RotatedRect rightRect = light_Rects[j].light;
            angle = Score_based_on_Angle(leftRect, rightRect);
            Area = Score_based_on_Area(leftRect, rightRect);
            if (Area == 0)
            {
                break;
            }
            Proficiency = Score_based_on_Proficiency(leftRect, rightRect);
            if (Proficiency == 0)
            {
                break;
            }
            if (!isArmor(leftRect, rightRect))
            {
                break;
            }
            cout = Area + angle + Proficiency;
            std::vector<int> temp = {i, j, cout};
            reliability.push_back(temp);
        }
    }
    std::vector<cv::RotatedRect> AmrorPlate;
    int x = 0, y = 0;
    if (reliability.empty())
    {
    }
    else
    {
        int max_score = 0;
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
    }

    cv::RotatedRect left = light_Rects[x].light;
    cv::RotatedRect right = light_Rects[y].light;

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
    target_Armor.left_buttom_ = a2;
    target_Armor.left_top_ = a1;
    target_Armor.right_bottom_ = b2;
    target_Armor.right_top_ = b1;
    return target_Armor;
}

void armorDetector::DrawArmor(const cv::Mat &image, Armor &target_Armor)
{
    cv::line(image, target_Armor.left_top_, target_Armor.right_bottom_, cv::Scalar(0, 255, 0), 3);
    cv::line(image, target_Armor.left_buttom_, target_Armor.right_top_, cv::Scalar(0, 255, 0), 3);

    cv::line(image, target_Armor.left_top_, target_Armor.right_top_, cv::Scalar(0, 255, 0), 3);
    cv::line(image, target_Armor.left_buttom_, target_Armor.right_bottom_, cv::Scalar(0, 255, 0), 3);

    cv::line(image, target_Armor.left_top_, target_Armor.left_buttom_, cv::Scalar(0, 255, 0), 3);
    cv::line(image, target_Armor.right_bottom_, target_Armor.right_top_, cv::Scalar(0, 255, 0), 3);
}

void armorDetector::exectue(const cv::Mat &image)
{
    armorDetector exectue_(15, 8, 1);
    Armor target = MatchArmor(IsLight(ProcessImage(image, 150)));
    DrawArmor(image, target);
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 两个装甲板面积相差越小得分越高
int Score_based_on_Area(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect)
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
int Score_based_on_Angle(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect)
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
int Score_based_on_Proficiency(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect)
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
bool isArmor(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect)
{
    if (std::abs(leftRect.center.y - rightRect.center.y) < 15)
        return true;
    else
        return false;
}
