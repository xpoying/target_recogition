// Author:xpoying
// 主函数，对视频调用函数进行装甲板识别

#include "opencv2/opencv.hpp"
#include "vector"
#include <iostream>
#include "Target.h"

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
    cv::namedWindow("video", cv::WINDOW_FREERATIO);
    while (1)
    {
        cap >> src;
        if (src.empty())
        {
            std::cout << "NO FIND SRC" << std::endl;
            return -1;
        }
        armorDetector find(15,8,1);
        find.exectue(src);
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




