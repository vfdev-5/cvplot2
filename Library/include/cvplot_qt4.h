#pragma once

// Qt
#include <QWidget>

// Project
#include "cvplot.h"

namespace CvPlot
{

//*************************************************************************************************

CVPLOT_DLL_EXPORTS QWidget * getPlotWidget(const std::string & figure_name, const cv::Mat & oneDimData, int count=0, int R = -1, int G = -1, int B = -1);

//*************************************************************************************************

} // namespace
