// Matlab style plot functions for OpenCV by Changbo (zoccob@gmail).

// STD
#include <cstring>

// OpenCV
#include <opencv2/highgui/highgui.hpp>

// Project
#include "cvplot.h"
#include "plot.h"

namespace CvPlot
{

//*************************************************************************************************

//  use anonymous namespace to hide global variables.
namespace
{
const cv::Scalar CV_BLACK = CV_RGB(0,0,0);
const cv::Scalar CV_WHITE = CV_RGB(255,255,255);
const cv::Scalar CV_GREY = CV_RGB(150,150,150);

PlotManager pm;

}

//*************************************************************************************************
//****************************** Class Series *****************************************************
//*************************************************************************************************

/// Constructor of class Series
Series::Series(void) :
    data(0)
{
    Clear();
}

//*************************************************************************************************
/// Constructor of class Series
Series::Series(int n, FLOAT *p)
{
    color = CV_BLACK;
    auto_color = true;
    label = "";

    count = n;
    data = new FLOAT[count];
    std::memcpy(data, p, count * sizeof(FLOAT));
}

//*************************************************************************************************
/// Constructor of class Series
Series::Series(const Series& s) :
    count(s.count),
    label(s.label),
    auto_color(s.auto_color),
    color(s.color)
{
    data = new FLOAT[count];
    std::memcpy(data, s.data, count * sizeof(FLOAT));
}

//*************************************************************************************************
/// Destructor of class Series
Series::~Series(void)
{
    Clear();
}

//*************************************************************************************************
/// Method : clear all data and info
void Series::Clear(void)
{
    if (data)
    {
        delete data;
        data=0;
    }

    count = 0;
    color = CV_BLACK;
    auto_color = true;
    label = "";
}

//*************************************************************************************************
/// Method : set data 
void Series::SetData(int n, FLOAT *p)
{
    Clear();
    count = n;
    data = p;
}

//*************************************************************************************************
/// Method : set color 
void Series::SetColor(int R, int G, int B, bool auto_color)
{
    R = R > 0 ? R : 0;
    G = G > 0 ? G : 0;
    B = B > 0 ? B : 0;
    color = CV_RGB(R, G, B);
    SetColor(color, auto_color);
}

//*************************************************************************************************
/// Method : set color
void Series::SetColor(cv::Scalar color, bool auto_color)
{
    this->color = color;
    this->auto_color = auto_color;
}

//*************************************************************************************************
//****************************** Class Figure *****************************************************
//*************************************************************************************************
/// Constructor 
Figure::Figure(const std::string & name)
{
    figure_name = name;

    custom_range_y = false;
    custom_range_x = false;
    backgroud_color = CV_WHITE;
    axis_color = CV_BLACK;
    text_color = CV_BLACK;

    figure_size = cv::Size(600, 200);
    border_size = 30;

}

//*************************************************************************************************
/// Destructor
Figure::~Figure(void)
{
    Clear();
}

//*************************************************************************************************
/// Method to get figure name
std::string Figure::GetFigureName(void) const
{
    return figure_name;
}

//*************************************************************************************************
/// Method to add a serie
Series* Figure::Add(Series *s)
{
    plots.push_back(s);
    return plots.back();
}

//*************************************************************************************************
/// Method to clear the figure
void Figure::Clear()
{
    if (!plots.empty())
    {
        for (std::vector<Series*>::iterator iter = plots.begin();
             iter != plots.end();
             iter++)
        {
            Series * s = *iter;
            if (s)
                delete s;
        }
    }
    plots.clear();
}

//*************************************************************************************************
/// Method to initialize a figure
void Figure::Initialize()
{
    color_index = 0;

    // size of the figure
    if (figure_size.width <= border_size * 2 + 100)
        figure_size.width = border_size * 2 + 100;
    if (figure_size.height <= border_size * 2 + 200)
        figure_size.height = border_size * 2 + 200;

    y_max = FLOAT_MIN;
    y_min = FLOAT_MAX;

    x_max = 0;
    x_min = 0;

    // find maximum/minimum of axes
    for (std::vector<Series*>::iterator iter = plots.begin();
         iter != plots.end();
         iter++)
    {
        FLOAT *p = (*iter)->data;
        for (int i=0; i < (*iter)->count; i++)
        {
            FLOAT v = p[i];
            if (v < y_min)
                y_min = v;
            if (v > y_max)
                y_max = v;
        }

        if (x_max < (*iter)->count)
            x_max = (*iter)->count;
    }

    // calculate zoom scale
    // set to 2 if y range is too small
    FLOAT y_range = y_max - y_min;
    FLOAT eps = 1e-9f;
    if (y_range <= eps)
    {
        y_range = 1;
        y_min = y_max / 2;
        y_max = y_max * 3 / 2;
    }

    x_scale = 1.0f;
    if (x_max - x_min > 1)
        x_scale = (FLOAT)(figure_size.width - border_size * 2) / (x_max - x_min);

    y_scale = (FLOAT)(figure_size.height - border_size * 2) / y_range;
}

/// Method to initialize a figure
cv::Scalar Figure::GetAutoColor() const 
{
    // 	change color for each curve.
    cv::Scalar col;

    switch (color_index)
    {
    case 1:
        col = CV_RGB(60,60,255);	// light-blue
        break;
    case 2:
        col = CV_RGB(60,255,60);	// light-green
        break;
    case 3:
        col = CV_RGB(255,60,40);	// light-red
        break;
    case 4:
        col = CV_RGB(0,210,210);	// blue-green
        break;
    case 5:
        col = CV_RGB(180,210,0);	// red-green
        break;
    case 6:
        col = CV_RGB(210,0,180);	// red-blue
        break;
    case 7:
        col = CV_RGB(0,0,185);		// dark-blue
        break;
    case 8:
        col = CV_RGB(0,185,0);		// dark-green
        break;
    case 9:
        col = CV_RGB(185,0,0);		// dark-red
        break;
    default:
        col =  CV_RGB(200,200,200);	// grey
        //color_index = 0;
    }
    //color_index++;
    return col;
}

//*************************************************************************************************
/// Method to draw axis
void Figure::DrawAxis(cv::Mat &output)
{
    int bs = border_size;
    int h = figure_size.height;
    int w = figure_size.width;

    // size of graph
    int gh = h - bs * 2;
    int gw = w - bs * 2;

    // draw the horizontal and vertical axis
    // let x, y axies cross at zero if possible.
    float y_ref = y_min;
    if ((y_max > 0) && (y_min <= 0))
        y_ref = 0;

    int x_axis_pos = h - bs - cvRound((y_ref - y_min) * y_scale);

    cv::line(output, cv::Point(bs, x_axis_pos), cv::Point(w - bs, x_axis_pos), axis_color);
    cv::line(output, cv::Point(bs, h - bs), cv::Point(bs, h - bs - gh), axis_color);

    // Write the scale of the y axis
    int fontFace = cv::FONT_HERSHEY_PLAIN;
    double fontScale=0.55;
    int thickness=1;
    int linetype=CV_AA;

    //cvInitFont(&font,CV_FONT_HERSHEY_PLAIN,0.55,0.7, 0,1,CV_AA);

    int chw = 6, chh = 10;
    char text[1000];

    // y max
    if ((y_max - y_ref) > 0.05 * (y_max - y_min))
    {
        CVPLOT_snprintf(text, sizeof(text)-1, "%.1f", y_max);
        cv::putText(output, text, cv::Point(bs / 5, bs - chh / 2), fontFace, fontScale, text_color, thickness, linetype);
    }
    // y min
    if ((y_ref - y_min) > 0.05 * (y_max - y_min))
    {
        CVPLOT_snprintf(text, sizeof(text)-1, "%.1f", y_min);
        cv::putText(output, text, cv::Point(bs / 5, h - bs + chh), fontFace, fontScale, text_color, thickness, linetype);
    }

    // x axis
    CVPLOT_snprintf(text, sizeof(text)-1, "%.1f", y_ref);
    cv::putText(output, text, cv::Point(bs / 5, x_axis_pos + chh / 2), fontFace, fontScale, text_color, thickness, linetype);

    // Write the scale of the x axis
    CVPLOT_snprintf(text, sizeof(text)-1, "%.0f", x_max );
    cv::putText(output, text, cv::Point(w - bs - strlen(text) * chw, x_axis_pos + chh), fontFace, fontScale, text_color, thickness, linetype);

    // x min
    CVPLOT_snprintf(text, sizeof(text)-1, "%.0f", x_min );
    cv::putText(output, text, cv::Point(bs, x_axis_pos + chh), fontFace, fontScale, text_color, thickness, linetype);


}

//*************************************************************************************************
/// Method to draw plots
void Figure::DrawPlots(cv::Mat &output)
{
    int bs = border_size;
    int h = figure_size.height;
    int w = figure_size.width;

    // draw the curves
    for (std::vector<Series*>::iterator iter = plots.begin();
         iter != plots.end();
         iter++)
    {
        FLOAT *p = (*iter)->data;

        // automatically change curve color
        if ((*iter)->auto_color == true)
            (*iter)->SetColor(GetAutoColor());

        cv::Point prev_point;
        for (int i=0; i<(*iter)->count; i++)
        {
            int y = cvRound(( p[i] - y_min) * y_scale);
            int x = cvRound((   i  - x_min) * x_scale);
            cv::Point next_point = cv::Point(bs + x, h - (bs + y));
            //cvCircle(output, next_point, 1, (*iter)->color, 1);
            cv::ellipse(output, next_point, cv::Size(1,1), 0.0, 0.0, 360.0, (*iter)->color, 1);

            // draw a line between two points
            if (i >= 1)
                cv::line(output, prev_point, next_point, (*iter)->color, 1, CV_AA);
            prev_point = next_point;
        }
    }

}

//*************************************************************************************************
/// Method to draw labels
void Figure::DrawLabels(cv::Mat &output, int posx, int posy)
{

    //CvFont font;
    //cvInitFont(&font,CV_FONT_HERSHEY_PLAIN,0.55,1.0, 0,1,CV_AA);
    int fontFace = cv::FONT_HERSHEY_PLAIN;
    //double hscale=0.55;
    //double vscale=0.7;
    double fontScale=0.55;
    //double shear=0.0;
    int thickness=1;
    int linetype=CV_AA;

    // character size
    int chw = 6, chh = 8;

    for (std::vector<Series*>::iterator iter = plots.begin();
         iter != plots.end();
         iter++)
    {
        std::string lbl = (*iter)->label;
        // draw label if one is available
        if (lbl.length() > 0)
        {
            cv::line(output, cv::Point(posx, posy - chh / 2), cv::Point(posx + 15, posy - chh / 2),
                     (*iter)->color, 2, CV_AA);

            cv::putText(output, lbl.c_str(), cvPoint(posx + 20, posy), fontFace, fontScale, (*iter)->color, thickness, linetype);

            posy += int(chh * 1.5);
        }
    }

}

//*************************************************************************************************
// whole process of draw a figure.
void Figure::Show()
{
    Initialize();

    cv::Mat output = cv::Mat(figure_size, CV_8UC3, backgroud_color); // cvCreateImage(figure_size, IPL_DEPTH_8U, 3); cvSet(output, backgroud_color, 0);

    DrawAxis(output);

    DrawPlots(output);

    DrawLabels(output, figure_size.width - 100, 10);

    //cvShowImage(figure_name.c_str(), output);
    cv::imshow(figure_name.c_str(), output);
    cv::waitKey(1);
    //cvReleaseImage(&output);

}

//*************************************************************************************************
//****************************** Class Figure *****************************************************
//*************************************************************************************************

// search a named window, return null if not found.
Figure* PlotManager::FindFigure(const std::string & wnd)
{
    for(std::vector<Figure*>::iterator iter = figure_list.begin();
        iter != figure_list.end();
        iter++)
    {
        if ((*iter)->GetFigureName() == wnd)
            return (*iter);
    }
    return 0;
}

//*************************************************************************************************
// plot a new curve, if a figure of the specified figure name already exists,
// the curve will be plot on that figure; if not, a new figure will be created.
void PlotManager::Plot(const std::string & figure_name, const FLOAT *p, int count, int step, int R, int G, int B)
{
    if (count < 1)
        return;

    if (step <= 0)
        step = 1;

    // copy data and create a series format.
    FLOAT *data_copy = new FLOAT[count];

    for (int i = 0; i < count; i++)
        *(data_copy + i) = *(p + step * i);

    Series *s = new Series(count, data_copy);

    if ((R > 0) || (G > 0) || (B > 0))
        s->SetColor(R, G, B, false);

    // search the named window and create one if none was found
    active_figure = FindFigure(figure_name);
    if ( active_figure == 0)
    {
        Figure * new_figure = new Figure(figure_name);
        figure_list.push_back(new_figure);
        active_figure = new_figure;
    }

    active_series = active_figure->Add(s);
    active_figure->Show();

}

//*************************************************************************************************

void PlotManager::Plot(const std::string & figure_name, cv::Mat & dataInFloat, int count, int R, int G, int B)
{
    if (count < 1)
        return;

    FLOAT *data = reinterpret_cast<FLOAT*>(dataInFloat.data);
    // Data is copied while Series is created:
    Series *s = new Series(count, data);

    if ((R > 0) || (G > 0) || (B > 0))
        s->SetColor(R, G, B, false);

    // search the named window and create one if none was found
    active_figure = FindFigure(figure_name);
    if ( !active_figure )
    {
        Figure *new_figure = new Figure(figure_name);
        figure_list.push_back(new_figure);
        active_figure = new_figure;
    }

    active_series = active_figure->Add(s);
    active_figure->Show();

}

// add a label to the most recently added curve
void PlotManager::Label(const std::string & lbl)
{
    if((active_series!=0) && (active_figure != 0))
    {
        active_series->label = lbl;
        active_figure->Show();
    }
}

//*************************************************************************************************
//****************************** Global methods ***************************************************
//*************************************************************************************************

/// plot a new curve, if a figure of the specified figure name already exists,
/// the curve will be plot on that figure; if not, a new figure will be created.

void plot(const std::string & figure_name, const cv::Mat & oneDimData, int count, int R, int G, int B)
{
    // test input : if single channel and 1D
    if (oneDimData.channels() > 1)
    {
        WARN("Input data matrix should have one channel");
        return;
    }

    if (oneDimData.rows > 1 && oneDimData.cols > 1)
    {
        WARN("Input data matrix should be one dimensional");
        return;
    }

    if (count < 1)
    {
        count = oneDimData.rows > 1 ? oneDimData.rows : oneDimData.cols;
    }

    cv::Mat dataInFloat;
    if (oneDimData.type() != OCV_FLOAT_TYPE)
        oneDimData.convertTo(dataInFloat, OCV_FLOAT_TYPE);
    else
        dataInFloat = oneDimData;

    pm.Plot(figure_name, dataInFloat, count, R, G, B);
}

//*************************************************************************************************
// delete all plots on a specified figure
void clear(const std::string & figure_name)
{
    Figure *fig = pm.FindFigure(figure_name);
    if (fig != NULL)
    {
        fig->Clear();
    }

}

//*************************************************************************************************
// add a label to the most recently added curve
// static method
void label(const std::string & lbl)
{
    pm.Label(lbl);
}

//*************************************************************************************************

}
