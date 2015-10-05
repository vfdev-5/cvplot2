# CvPlot 2 project
A fork of [cvplot](https://code.google.com/p/cvplot/) project to plot cv::Mat 1D data

- CMake support
- input with cv::Mat
- Optional output with QWidget (Qt4)

## Usage :
### Simple example
    ```
    cv::Mat data; // a [1xDATA_LENGTH] matrix
    CvPlot::plot("Data", data);
    CvPlot::plot("Half of the data in blue", data, DATA_LENGTH/2, 0, 0, 255);
    cv::waitKey(0);
    ```

### Example with Qt4 :
    ```
    cv::Mat data; // a [1xDATA_LENGTH] matrix
    QWidget * plot = CvPlot::getPlotWidget("Data", data, DATA_LENGTH/2, 0, 0, 255);
    if (plot1)
    {
        plot->show();
    }

### See [demo]() example for more details
