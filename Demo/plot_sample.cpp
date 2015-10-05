// Matlab style plot functions for OpenCV by Changbo (zoccob@gmail).

// STD
#include <iostream>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#ifdef HAS_QT4
#include <QApplication>
#endif

// Project
#include "cvplot.h"

const int DATA_LENGTH=512;

int main(int argc, char* argv[])
{
    // create data : a pulse
    float dataBuffer[DATA_LENGTH];
    int pulseLenght=20;
    int pulseStart=0;
    for (int i=pulseStart; i<pulseStart+pulseLenght;i++)
        dataBuffer[i]=1.0;
    for (int i=0; i<pulseStart;i++)
        dataBuffer[i]=0.0;
    for (int i=pulseStart+pulseLenght; i<DATA_LENGTH;i++)
        dataBuffer[i]=0.0;


    cv::Mat data(1,DATA_LENGTH, CV_32F, dataBuffer);

    std::cout << "DATA dims : " << data.rows << ", " << data.cols << ", " << data.channels() << std::endl;

    // compute FFT :
    int optSize = cv::getOptimalDFTSize(DATA_LENGTH);
    cv::Mat paddedData;
    cv::copyMakeBorder(data, paddedData, 0, 0, 0, optSize - data.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0) );

    std::cout << "padded DATA dims : " << paddedData.rows << ", " << paddedData.cols << ", " << paddedData.channels() << std::endl;

    cv::Mat fftCData, fftCDataPacked, fftAbsData;
    cv::dft(paddedData, fftCData, cv::DFT_ROWS | cv::DFT_COMPLEX_OUTPUT);
    cv::dft(paddedData, fftCDataPacked);

    std::cout << "fftCData type : " << fftCData.type() << " CV_32FC2 : " << CV_32FC2 << std::endl;
    std::cout << "fftCData rows, cols, nbChannels : " << fftCData.rows << ", " << fftCData.cols << ", " << fftCData.channels() << std::endl;

    std::cout << "fftCDataPacked type : " << fftCDataPacked.type() << " CV_32FC2 : " << CV_32FC2 << std::endl;
    std::cout << "fftCDataPacked rows, cols, nbChannels : " << fftCDataPacked.rows << ", " << fftCDataPacked.cols << ", " << fftCDataPacked.channels() << std::endl;

    //
    std::cout << "FFT Complex data : " << std::endl;
    float * ptr = reinterpret_cast<float*>(fftCData.data);
    for (int i=0; i<DATA_LENGTH; i++)
    {
        double v = ptr[0];
        std::cout << "(" << i << ") :" << v << " | ";
        ptr++;
    }
    std::cout << std::endl;


    //
    std::cout << "FFT Complex data packed : " << std::endl;
    ptr = reinterpret_cast<float*>(fftCDataPacked.data);
    for (int i=0; i<fftCDataPacked.cols; i++)
    {
        double v = ptr[0];
        std::cout << "(" << i << ") :" << v << " | ";
        ptr++;
    }
    std::cout << std::endl;

    // compute magnitude :
    std::vector<cv::Mat> iChannels(2);
    cv::split(fftCData, &iChannels[0]);
    cv::magnitude(iChannels[0], iChannels[1], fftAbsData);


    //
    std::cout << "FFT abs data : " << std::endl;
    ptr = reinterpret_cast<float*>(fftAbsData.data);
    for (int i=0; i<DATA_LENGTH; i++)
    {
        double v = ptr[0];
        std::cout << v << " | ";
        ptr++;
    }
    std::cout << std::endl;


 #ifndef HAS_QT4

    CvPlot::plot("Input data : pulse", data);
//    CvPlot::label("Input Data");


    CvPlot::plot("FFT Abs data : pulse", fftAbsData, DATA_LENGTH/2, 255);

//    cv::imshow("fft", fftAbsData);

    // compute inverse FFT :
    cv::Mat nData;
    cv::dft(fftCData, nData, cv::DFT_ROWS | cv::DFT_REAL_OUTPUT | cv::DFT_INVERSE | cv::DFT_SCALE);

    CvPlot::plot("inverse of FFT data : pulse", nData, 0, 0, 0, 255);
    CvPlot::plot("Input data : pulse", fftAbsData*0.05, DATA_LENGTH/2, 0, 0, 255);

    cv::waitKey(0);

#endif

#ifdef HAS_QT4
    std::cout << "QApplication test : " << std::endl;
    QApplication app(argc, argv);

    QWidget * plot1 = CvPlot::getPlotWidget("Input data : pulse", fftAbsData*0.05, DATA_LENGTH/2, 0, 0, 255);
    if (!plot1)
    {
        std::cerr << "Failed to get plot widget" << std::endl;
        return 1;
    }
    plot1->show();
    app.exec();
#endif

    return 0;
}

