// Matlab style plot functions for OpenCV by Changbo (zoccob@gmail).

// STD
#include <cstring>

// OpenCV
#include <opencv2/imgproc/imgproc.hpp>
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

void printMat(const cv::Mat & m, const std::string & name, int userWidth=0, int userHeight=0)
{
    std::cout << "Print matrix : " << name << std::endl;
    std::cout << "Size : " << m.cols << ", " << m.rows << std::endl;
    std::cout << "Nb of Bands : " << m.channels() << std::endl;

    if (m.channels() > 1)
        return;

    cv::Mat d;
    m.convertTo(d, CV_32F);

    if (userHeight==0) userHeight = m.rows;
    if (userWidth==0) userWidth = m.cols;

    for (int i=0;i<userHeight;i++)
    {
        std::cout << "Row " << i << " : ";
        for (int j=0;j<userWidth;j++)
        {
            std::cout << d.at<float>(i,j) << " ";
        }
        std::cout << std::endl;
    }
}


//*************************************************************************************************
//****************************** Class Series *****************************************************
//*************************************************************************************************

/// Constructor of class Series
Series::Series(void) :
    color(CV_BLACK),
    auto_color(true)
{
}

//*************************************************************************************************
/// Constructor of class Series
/// data is a [1 x Width] matrix
/// count is between 1 and Width
Series::Series(const cv::Mat &data, int count) :
    color(CV_BLACK),
    auto_color(true)
{
    // copy data
    if (data.cols == 1)
    {
        data.rowRange(0,count).convertTo(this->data, CV_32F);
        this->data = this->data.t();
    }
    else
    {
        data.colRange(0,count).convertTo(this->data, CV_32F);
    }
}

//*************************************************************************************************
/// Constructor of class Series
Series::Series(const Series& s) :
    label(s.label),
    auto_color(s.auto_color),
    color(s.color)
{
    s.data.copyTo(this->data);
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
// Method to add a serie
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

    y_max = 0.0;
    y_min = 0.0;

    x_max = 0.0;
    x_min = 0.0;

    // find maximum/minimum of axes
    for (std::vector<Series*>::iterator iter = plots.begin();
         iter != plots.end();
         iter++)
    {
        double ymin, ymax;
        cv::minMaxLoc((*iter)->data, &ymin, &ymax);

        if (ymin < y_min) y_min = ymin;
        if (ymax > y_max) y_max = ymax;

        if (x_max < (*iter)->data.cols)
            x_max = (*iter)->data.cols;
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
cv::Scalar Figure::GetAutoColor()
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
    }
    color_index++;
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
        cv::Mat data = (*iter)->data;

        // automatically change curve color
        if ((*iter)->auto_color == true)
            (*iter)->SetColor(GetAutoColor());

        cv::Point prev_point;
        for (int i=0; i<data.cols; i++)
        {
            int y = cvRound(( data.at<FLOAT>(0,i) - y_min) * y_scale);
            int x = cvRound((   i  - x_min) * x_scale);
            cv::Point next_point = cv::Point(bs + x, h - (bs + y));
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

    cv::Mat output = cv::Mat(figure_size, CV_8UC3, backgroud_color);

    DrawAxis(output);

    DrawPlots(output);

    DrawLabels(output, figure_size.width - 100, 10);

    cv::imshow(figure_name.c_str(), output);
    cv::waitKey(1);

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

void PlotManager::Plot(const std::string & figure_name, const cv::Mat & data, int count, int R, int G, int B)
{
    if (count < 1)
    {
        WARN("PlotManager::Plot : count is less than one");
        return;
    }

    // Data is copied while Series is created:
    Series *s = new Series(data, count);

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

    pm.Plot(figure_name, oneDimData, count, R, G, B);
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
