#include <iostream>
#include "opencv2/opencv.hpp"
#include "Target.h"
#include <vector>


#include<algorithm>

cv::Mat Target::Preprocessing(cv::Mat &src)
{
    // 初始化
    cv::Mat ken = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::Mat ken2 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat thr, gray, mor, dil, edges;
    double threshold1 = 3;
    double threshold2 = 9;

    // 处理图像  二值化 开运算  膨胀   轮廓增强
    cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    cv::threshold(gray, thr, 150, 255, cv::THRESH_BINARY);
    cv::morphologyEx(thr, mor, cv::MORPH_OPEN, ken, cv::Point(-1, -1));
    cv::dilate(mor, dil, ken2, cv::Point(-1, -1));
    cv::dilate(dil, dil, ken2, cv::Point(-1, -1));
    cv::Canny(dil, edges, threshold1, threshold2);
    return edges;
}

void Target::findrects(cv::Mat &src, cv::Mat &dst)
{
    // 初始化
    std::vector<std::vector<cv::Point>> contour;
    std::vector<cv::RotatedRect> Rects;
    cv::Point2f vertex[4];
    cv::RotatedRect a, b;
    int area[2];
    std::vector<std::vector<int>> reliability;

    // 初步筛选矩形
    cv::findContours(src, contour, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); // 获取所有轮廓的坐标
    for (int i = 0; i < contour.size(); ++i)
    {
        cv::RotatedRect minRect = cv::minAreaRect(cv::Mat(contour[i])); // 用坐标得到最小矩形
        minRect.points(vertex);                                         // 获取矩形四角坐标

        // 将矩形统一
        if (minRect.size.width > minRect.size.height)
        {
            minRect.angle += 90;
            float t = minRect.size.width;
            minRect.size.width = minRect.size.height;
            minRect.size.height = t;
        }
        // 初步筛选符合装甲板定义的矩形
        if (minRect.size.height > minRect.size.width * 1.5)
        {
            Rects.push_back(minRect);
        }
        // 绘制所有矩形的轮廓
        for (int l = 0; l < 4; l++)
        {
            cv::line(dst, vertex[l], vertex[(l + 1) % 4], cv::Scalar(0, 0, 225), 3, cv::LINE_8, 0);
        }
    }

    // 筛选矩形
    for (int i = 0; i < Rects.size(); i++)
    {
        for (int j = i + 1; j < Rects.size(); j++)
        {
            int cout = 0;
            a = Rects[i];
            b = Rects[j];
            cv::Point2f axy[4];
            cv::Point2f bxy[4];
            a.points(axy);
            b.points(bxy);
            area[0] = a.size.width * a.size.height;
            area[1] = b.size.width * b.size.height;
            double updown = std::abs((axy[1].y + axy[2].y) / 2 - (bxy[1].y + bxy[2].y) / 2);

            // 根据两矩形的平行情况评分  角度差越小分越高
            if (a.angle == b.angle)
            {
                cout = cout + 10;
            }
            else if (abs(a.angle - b.angle) < 5)
            {
                cout = cout + 8;
            }
            else if (abs(a.angle - b.angle) < 10)
            {
                cout = cout + 6;
            }
            else if (abs(a.angle - b.angle) < 30)
            {
                cout = cout + 4;
            }
            else if (abs(a.angle - b.angle) < 60)
            {
                cout = cout + 2;
            }
            else if (abs(a.angle - b.angle) < 90)
            {
                cout = cout + 1;
            }
            else
            {
                break;
            }

            // 根据两个矩形的面积差评分  面积相差越小分越高
            if (area[0] == area[1])
            {
                cout = cout + 10;
            }
            else if (std::min(area[0], area[1]) * 1.5 > std::max(area[0], area[1]))
            {
                cout = cout + 6;
            }
            else if (std::min(area[0], area[1]) * 2 > std::max(area[0], area[1]))
            {
                cout = cout + 4;
            }
            else if (std::min(area[0], area[1]) * 3 > std::max(area[0], area[1]))
            {
                cout = cout + 2;
            }
            else if (std::min(area[0], area[1]) * 4 > std::max(area[0], area[1]))
            {
                cout = cout + 1;
            }
            else
            {
                break;
            }

            // 根据两个矩形的中心位置评分
            if (std::abs(a.center.y - b.center.y) < 1)
            {
                cout = cout + 14;
            }
            else if (std::abs(a.center.y - b.center.y) < 3)
            {
                cout = cout + 12;
            }
            else if (std::abs(a.center.y - b.center.y) < 5)
            {
                cout = cout + 8;
            }
            else if (std::abs(a.center.y - b.center.y) < 7)
            {
                cout = cout + 4;
            }
            else if (std::abs(a.center.y - b.center.y) < 11)
            {
                cout = cout + 2;
            }
            else if (std::abs(a.center.y - b.center.y) < 15)
            {
                cout = cout + 1;
            }
            else
            {
                break;
            }

     

            // 筛选最近的矩形配对
            if (std::abs(a.center.x - b.center.x) < 120)
            {
                cout = cout + 20;
            }
            else if (std::abs(a.center.x - b.center.x) < 150)
            {
                cout = cout + 12;
            }
            else if (std::abs(a.center.x - b.center.x) < 180)
            {
                cout = cout + 6;
            }
            else if (std::abs(a.center.x - b.center.x) < 250)
            {
                cout = cout + 4;
            }
            else if (std::abs(a.center.x - b.center.x) < 1000)
            {
                cout = cout + 2;
            }
            else if (std::abs(a.center.x - b.center.x) < 2000)
            {
                cout = cout + 1;
            }
            else
            {
                break;
            }

            // 记录评分
            std::vector<int> temp = {i, j, cout};
            reliability.push_back(temp);
        }
    }

    if (reliability.empty())
    {
    }
    else
    {
        int max_level = 0;
        int x = 0, y = 0;
        cv::Point2f arect[4];
        cv::Point2f brect[4];
        Rects[x].points(arect);
        Rects[y].points(brect);
        a = Rects[x];
        b = Rects[y];
        double ang = std::abs(a.angle - b.angle);
        cv::Point a1 = (arect[1] + arect[2]) / 2;
        cv::Point a2 = (arect[0] + arect[3]) / 2;
        cv::Point b1 = (brect[1] + brect[2]) / 2;
        cv::Point b2 = (brect[0] + brect[3]) / 2;
        double center_x = (a.center.x + b.center.x) / 2;
        double center_y = (a.center.y + a.center.y) / 2;


        //选择评分最高的
        for (int i = 0; i < reliability.size(); i++)
        {
            if (reliability[i][2] > max_level)
            {
                max_level = reliability[i][2];
                x = reliability[i][0];
                y = reliability[i][1];
            }
        }
        //最终排查
        if (ang < 30 && std::abs(a1.y - a2.y) > 1 && std::abs(a.center.y - b.center.y) < 15)
        {
            cv::line(dst, a1, b2, cv::Scalar(0, 255, 0), 5);
            cv::line(dst, a2, b1, cv::Scalar(0, 255, 0), 5);
            std::cout << "x=" << center_x << " y=" << center_y << std::endl;
        }
    }
    cv::imshow("video", dst);
}
