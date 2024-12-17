#include "Target.h"
#include <chrono>
// 构造函数  初步筛选装甲板并加入数组
Armor::Armor(const std::vector<std::vector<cv::Point>> &contours)
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
        if ((minRects.size.width * 8 > minRects.size.height) && (minRects.size.width * 1 < minRects.size.height))
        {
            Rects.push_back(minRects);
        }
    }
}
// 两个装甲板面积相差越小得分越高
int Armor::AreaCout(cv::RotatedRect &a, cv::RotatedRect &b)
{
    int cout = 0;
    int area[2];
    area[0] = a.size.width * a.size.height;
    area[1] = b.size.width * b.size.height;
    if (area[0] == area[1])
    {
        cout = cout + 10;
    }
    else if (std::min(area[0], area[1]) * 1.5 > std::max(area[0], area[1]))
    {
        cout = cout + 8;
    }
    else if (std::min(area[0], area[1]) * 2 > std::max(area[0], area[1]))
    {
        cout = cout + 6;
    }
    else if (std::min(area[0], area[1]) * 3 > std::max(area[0], area[1]))
    {
        cout = cout + 4;
    }
    else if (std::min(area[0], area[1]) * 4 > std::max(area[0], area[1]))
    {
        cout = cout + 2;
    }
    else
    {
    }
    return cout;
}
// 两个装甲板角度差越小得分越高
int Armor::AngleCout(cv::RotatedRect &a, cv::RotatedRect &b)
{
    int cout = 0;
    if (abs(a.angle - b.angle) < 5)
    {
        cout = cout + 10;
    }
    else if (abs(a.angle - b.angle) < 10)
    {
        cout = cout + 6;
    }
    else if (abs(a.angle - b.angle) < 30)
    {
        cout = cout + 4;
    }
    else if (abs(a.angle - b.angle) < 90)
    {
        cout = cout + 2;
    }
    else
    {
    }
    return cout;
}
// 两个装甲板越水平得分越高
int Armor::DistanceCout(cv::RotatedRect &leftRect, cv::RotatedRect &rightRect)
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
bool Armor::isArmor(cv::RotatedRect &a, cv::RotatedRect &b)
{
    if (std::abs(a.center.y - b.center.y) < 15)
        return true;
    else
        return false;
}

// 图像的预处理
cv::Mat Preprocessing(cv::Mat &src)
{
    // 初始化
    cv::Mat ken = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat thr, gray, dil, edges;
    double threshold1 = 3;
    double threshold2 = 9;
    // 处理图像  二值化 开运算  轮廓增强
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thr, 160, 255, cv::THRESH_BINARY);
    cv::morphologyEx(thr, dil, cv::MORPH_OPEN, ken, cv::Point(-1, -1));
    cv::Canny(dil, edges, threshold1, threshold2);
    return edges;
}
// 筛选匹配的装甲板并加入vector容器中
std::vector<cv::RotatedRect> FindRects(cv::Mat &edges)
{
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    Armor plate(contours);
    std::vector<std::vector<int>> reliability;
    for (int i = 0; i < plate.Rects.size(); i++)
    {
        for (int j = i + 1; j < plate.Rects.size(); j++)
        {
            int cout = 0, angle = 0, area = 0, distance = 0;
            cv::RotatedRect a = plate.Rects[i];
            cv::RotatedRect b = plate.Rects[j];
            angle = plate.AngleCout(a, b);
            if (angle == 0)
            {
            }
            area = plate.AreaCout(a, b);
            if (area == 0)
            {
                break;
            }
            distance = plate.DistanceCout(a, b);
            if (distance == 0)
            {
                break;
            }
            if (!plate.isArmor(a, b))
            {
                break;
            }
            cout = area + angle + distance;
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
        int max_level = 0;
        int x = 0, y = 0;
        // 选择评分最高的
        for (int i = 0; i < reliability.size(); i++)
        {
            if (reliability[i][2] > max_level)
            {
                max_level = reliability[i][2];
                x = reliability[i][0];
                y = reliability[i][1];
            }
        }
        AmrorPlate.push_back(plate.Rects[x]);
        AmrorPlate.push_back(plate.Rects[y]);
    }
    return AmrorPlate;
}
// 绘画出目标位置
void DrawAmrorPlate(cv::Mat &src, std::vector<cv::RotatedRect> &AmrorPlate)
{

    if (AmrorPlate.size() < 2)
    {
        return;
    }
    cv::RotatedRect left = AmrorPlate[0];
    cv::RotatedRect right = AmrorPlate[1];
    std::vector<cv::Point2f> leftxy, rightxy;
    right.points(rightxy);
    left.points(leftxy);
    cv::Point2f a1 = (leftxy[1] + leftxy[2]) / 2;
    cv::Point2f a2 = (leftxy[0] + leftxy[3]) / 2;
    cv::Point2f b1 = (rightxy[1] + rightxy[2]) / 2;
    cv::Point2f b2 = (rightxy[0] + rightxy[3]) / 2;

    //校正坐标
    if (a1.y < a2.y)
    {
        auto a = a1;
        a1 = a2;
        a2 = a;
    }
    if (b1.y < b2.y)
    {
        auto b = b1;
        b1 = b2;
        b2 = b;
    }
    cv::line(src, a1, b2, cv::Scalar(0, 255, 255), 3);
    cv::line(src, a2, b1, cv::Scalar(0, 255, 255), 3);
    for (int l = 0; l < 4; l++)
    {
        cv::line(src, leftxy[l], leftxy[(l + 1) % 4], cv::Scalar(0, 0, 225), 3, cv::LINE_8, 0);
        cv::line(src, rightxy[l], rightxy[(l + 1) % 4], cv::Scalar(0, 0, 225), 3, cv::LINE_8, 0);
    }
}