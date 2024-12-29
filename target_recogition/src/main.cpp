// Author:xpoying
// 主函数，对视频调用函数进行装甲板识别
#include <algorithm>
#include "Target.h"

bool compare(armorDetector::Light left_light, armorDetector::Light right_light)
{
    return right_light.light.center.x > left_light.light.center.x;
}

int main()
{
    std::string path = "/home/x/visionlib/vision.mp4";
    cv::VideoCapture cap(path);
    if (!cap.isOpened())
    {
        std::cout << "NO FIND" << std::endl;
        return -1;
    }

    cv::Mat src, edges;
    armorDetector find(15, 10, 2.5); // 初始化参数  装甲板高度差   最大长宽比  最小长宽比
    cv::namedWindow("video", cv::WINDOW_FREERATIO);
    while (1)
    {
        cap >> src;
        if (src.empty())
        {
            std::cout << "NO FIND SRC" << std::endl;
            return -1;
        }
        find.ProcessImage(src, edges, 145); // 图像处理  输入图像  输出图像   二值化的阈值
        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(edges, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        std::vector<armorDetector::Light> light_Rects;
        for (int i = 0; i < contours.size(); ++i)
        {
            cv::Point2f vec[4];
            cv::RotatedRect minRects = cv::minAreaRect(cv::Mat(contours[i]));
            minRects.points(vec);
            if (find.IsLight(minRects)) // 判断是否是符合条件的灯条
            {
                armorDetector::Light minRects_ = {minRects, minRects.size.width * minRects.size.height, minRects.angle};
                light_Rects.push_back(minRects_);
              
            }
        }

        std::sort(light_Rects.begin(), light_Rects.end(), compare); // 灯条按x的大小降序排序

        if (light_Rects.size() >= 2)
        {
            for (int i = 0; i+1 < light_Rects.size(); i++)
            {
                armorDetector::Light left_light = light_Rects[i];
                armorDetector::Light right_light = light_Rects[i + 1];

                if (find.MatchArmor(left_light, right_light)) // 如果是装甲板就标记
                {
                    armorDetector::Armor target_armor = {left_light, right_light};
                    find.DrawArmor(src, target_armor);
                    break;
                }
            }
        }
        if (cv::waitKey(1) == 27)
        {
            break;
        }
        cv::imshow("video", src);
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
