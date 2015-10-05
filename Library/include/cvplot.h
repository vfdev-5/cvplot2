/*! A fork of cvplot project : https://code.google.com/p/cvplot/
	with CMake support and optional QWidget integration (Qt 4)


 Matlab style plot functions for OpenCV by Changbo (zoccob@gmail).
 plot and label:

 template<typename T>
 void plot(const string figure_name, const T* p, int count, int step = 1,
		     int R = -1, int G = -1, int B = -1);

 figure_name: required. multiple calls of this function with same figure_name
              plots multiple curves on a single graph.
 p          : required. pointer to data.
 count      : required. number of data.
 step       : optional. step between data of two points, default 1.
 R, G,B     : optional. assign a color to the curve.
              if not assigned, the curve will be assigned a unique color automatically.

 void label(string lbl):

 label the most recently added curve with lbl.
 
*/

#pragma once

// OPENCV
#include <opencv2/core/core.hpp>


#if (defined WIN32 || defined _WIN32 || defined WINCE) && defined CVPLOT_EXPORTS
#  define CVPLOT_DLL_EXPORTS __declspec(dllexport)
#else
#  define CVPLOT_DLL_EXPORTS
#endif


namespace CvPlot
{

//*************************************************************************************************

void CVPLOT_DLL_EXPORTS plot(const std::string & figure_name, const cv::Mat & oneDimData, int count=0, int R = -1, int G = -1, int B = -1);

void CVPLOT_DLL_EXPORTS clear(const std::string & figure_name);

void CVPLOT_DLL_EXPORTS label(const std::string & lbl);

//*************************************************************************************************

} // namespace
