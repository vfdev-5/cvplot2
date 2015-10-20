#pragma once

// STD
#include <iostream>
#include <vector>
#include <cstdio>

// OPENCV
#include <opencv2/core/core.hpp>

namespace CvPlot
{

#if (defined WIN32 || defined _WIN32 || defined WINCE)
#   ifndef CVPLOT_snprintf
#       define CVPLOT_snprintf sprintf_s
#   endif
#else
#   ifndef CVPLOT_snprintf
#       define CVPLOT_snprintf snprintf
#   endif
#endif

#ifdef CVPLOT_DOUBLE_PRECISION
typedef double FLOAT;
#define FLOAT_MIN DBL_MIN
#define FLOAT_MAX DBL_MAX
#define OCV_FLOAT_TYPE CV_64F
#else
typedef float FLOAT;
#define FLOAT_MIN FLT_MIN
#define FLOAT_MAX FLT_MAX
#define OCV_FLOAT_TYPE CV_32F
#endif

#define INFO(text) std::cout << text << std::endl;
#define WARN(text) std::cerr << text << std::endl;

//*************************************************************************************************

// A curve.
struct Series
{
    // data
    cv::Mat data;
    // name of the curve
    std::string label;

    // allow automatic curve color
    bool auto_color;
    cv::Scalar color;

    Series(void);
    Series(const cv::Mat & data, int count);
    Series(const Series& s);

    void SetColor(cv::Scalar color, bool auto_color = true);
    void SetColor(int R, int G, int B, bool auto_color = true);
};

// a figure comprises of several curves
class Figure
{
public:
    Figure(const std::string & name);
    ~Figure();

    std::string GetFigureName() const ;
    Series* Add(Series *s);

    void Clear();
    void DrawLabels(cv::Mat & output, int posx, int posy);

    // show plot window
    void Show();

protected:
    Figure();

    void DrawAxis(cv::Mat & output);
    void DrawPlots(cv::Mat & output);

    // call before plot
    void Initialize();
    cv::Scalar GetAutoColor();
    // window name
    std::string figure_name;
    cv::Size figure_size;

    // margin size
    int    border_size;

    cv::Scalar backgroud_color;
    cv::Scalar axis_color;
    cv::Scalar text_color;

    // several curves
    std::vector<Series*> plots;

    // manual or automatic range
    bool custom_range_y;
    FLOAT y_max;
    FLOAT y_min;

    FLOAT y_scale;

    bool custom_range_x;
    FLOAT x_max;
    FLOAT x_min;

    FLOAT x_scale;

    // automatically change color for each curve
    int color_index;


};

// manage plot windows
class PlotManager
{
public:

    Figure* FindFigure(const std::string &wnd);

    void Plot(const std::string &figure_name, const cv::Mat & data, int count,
              int R, int G, int B);

    void Label(const std::string &lbl);
protected:
    std::vector<Figure*> figure_list;
    Series *active_series;
    Figure *active_figure;
};

//*************************************************************************************************

}
