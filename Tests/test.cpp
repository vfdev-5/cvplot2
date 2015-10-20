
// Opencv
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/highgui/highgui.hpp>

// Project
#include "cvplot.h"


int main(int argc, char** argv)
{

    // TEST 1
    {
        // Simulate image
        cv::Mat src(100, 100, CV_8U, cv::Scalar::all(0));
        cv::ellipse(src, cv::Point(50, 50), cv::Size(30, 20), 0, 0, 360, cv::Scalar::all(200), CV_FILLED);

        int y=50;
        int x=50;
        cv::Mat cut1 = src(cv::Rect(0, y, src.cols, 1));
        cv::Mat cut2 = src(cv::Rect(x, 0, 1, src.rows));
        CvPlot::plot("Cut 1", cut1);
        CvPlot::plot("Cut 2", cut2);

        cv::line(src, cv::Point(0, y), cv::Point(src.cols, y), cv::Scalar(255));
        cv::line(src, cv::Point(x, 0), cv::Point(x, src.rows), cv::Scalar(255));

        cv::imshow("Source", src);
        cv::waitKey(0);
    }


    // TEST 2
    {
        // Simulate image
        cv::Mat src(100, 200, CV_8U, cv::Scalar::all(10));
        //        cv::randu(src, 0, 100);
        cv::ellipse(src, cv::Point(50, 50), cv::Size(40, 20), 0, 0, 360, cv::Scalar::all(200), CV_FILLED);

        for (int x=10; x<src.cols; x+=25)
        {
            cv::Mat cut1 = src(cv::Rect(x, 0, 1, src.rows));
            CvPlot::plot("Cut", cut1);
            cv::line(src, cv::Point(x, 0), cv::Point(x, src.rows), cv::Scalar(255));
        }

        cv::imshow("Source", src);
        cv::waitKey(0);
    }



    return 0;

}
