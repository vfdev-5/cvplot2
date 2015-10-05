
// Qt
#include <QLabel>
#include <QImage>

// Project
#include "plot.h"

namespace CvPlot
{

//*************************************************************************************************

class FigureQt4 : public Figure
{
public:
    FigureQt4(const std::string & name) :
        Figure(name)
    {}
    virtual ~FigureQt4() {}
    QWidget * GetWidget();
};

//*************************************************************************************************

/*! method to get QWidget
 */
QWidget * FigureQt4::GetWidget()
{
    Initialize();

    cv::Mat output = cv::Mat(figure_size, CV_8UC3, backgroud_color); // cvCreateImage(figure_size, IPL_DEPTH_8U, 3); cvSet(output, backgroud_color, 0);

    DrawAxis(output);

    DrawPlots(output);

    DrawLabels(output, figure_size.width - 100, 10);

    QLabel * label = new QLabel();
    QImage img( output.data, output.cols, output.rows, QImage::Format_RGB888 );
    label->setPixmap(QPixmap::fromImage(img));

    label->setMinimumSize(img.size());
    label->setMaximumSize(img.size());

    return label;
}

//*************************************************************************************************

// manage plot windows
class PlotManagerQt4 : public PlotManager
{
public:
    QWidget * GetPlotWidget(const std::string &figure_name, cv::Mat & dataInFloat, int count, int R, int G, int B);
};

//*************************************************************************************************
/*! get plot Qt widget
 */
QWidget * PlotManagerQt4::GetPlotWidget(const std::string &figure_name, cv::Mat &dataInFloat, int count, int R, int G, int B)
{
    if (count < 1)
        return 0;

    FLOAT *data = reinterpret_cast<FLOAT*>(dataInFloat.data);
    // Data is copied while Series is created:
    Series *s = new Series(count, data);

    if ((R > 0) || (G > 0) || (B > 0))
        s->SetColor(R, G, B, false);

    // search the named window and create one if none was found
    active_figure = FindFigure(figure_name);
    if ( !active_figure )
    {
        FigureQt4 *new_figure = new FigureQt4(figure_name);
        figure_list.push_back(new_figure);
        active_figure = new_figure;
    }

    active_series = active_figure->Add(s);

    FigureQt4 * figure = static_cast<FigureQt4*>(active_figure);
    if (figure)
        return figure->GetWidget();
    else
        return 0;
}

//*************************************************************************************************
//*************************************************************************************************

//  use anonymous namespace to hide global variables.
namespace
{

PlotManagerQt4 pm;

}

//*************************************************************************************************
//*************************************************************************************************

/// Get plot Qt widget. Caller is responsible for the QWidget
QWidget* getPlotWidget(const std::string & figure_name, const cv::Mat & oneDimData, int count, int R, int G, int B)
{
    // test input : if single channel and 1D
    if (oneDimData.channels() > 1)
    {
        WARN("Input data matrix should have one channel");
        return 0;
    }

    if (oneDimData.rows > 1 && oneDimData.cols > 1)
    {
        WARN("Input data matrix should be one dimensional");
        return 0;
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

    return pm.GetPlotWidget(figure_name, dataInFloat, count, R, G, B);
}

//*************************************************************************************************

}
