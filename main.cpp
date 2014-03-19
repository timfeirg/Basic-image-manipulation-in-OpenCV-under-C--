#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>

using namespace cv;
using namespace std;

int help(void)
{
    cout<<"Usage: "<<endl
    <<"./exp_1 [mode] [image directory]"<<endl;
    return 0;
}

int main(int argc, char *argv[])
{
    
    Mat source_image = imread( argv[argc - 1], 0 );
    Mat target_image;
    // function declaration
    Mat halftoning(Mat source_image);
    Mat reduceIntensity(Mat source_image, int grayLevel);
    Mat arith(Mat img1, Mat img2);
    
    if (strcmp(argv[1], "--halftone") == 0) {
        target_image = halftoning(source_image);
    }
    else if (strcmp(argv[1], "--reduce") == 0) {
        int grayLevel;
        cout<<"Please specify the gray level you want"<<endl;
        cin>>grayLevel;
        target_image = reduceIntensity(source_image, grayLevel);
    }
    else if (strcmp(argv[1], "--arithmetic") == 0) {
        Mat img1 = imread(argv[2],0);
        target_image = arith(img1, source_image);
    }
    else return help();
    
    imshow("target image", target_image);
    waitKey();
	return 0;
}

Mat halftoning(Mat source_image)
{
    Mat target_image(source_image.rows * 3, source_image.cols * 3, CV_8UC1);
    // arrays used to generate halftone pattern
    uchar halftone_source[10][3][3] =
    {   //0
        {
            {0,0,0},
            {0,0,0},
            {0,0,0}
        },
        //1
        {
            {0,255,0},
            {0,0,0},
            {0,0,0}
        },
        //2
        {
            {0,255,0},
            {0,0,0},
            {0,0,255}
        },
        //3
        {
            {255,255,0},
            {0,0,0},
            {0,0,255}
        },
        //4
        {
            {255,255,0},
            {0,0,0},
            {255,0,255}
        },
        //5
        {
            {255,255,255},
            {0,0,0},
            {255,0,255}
        },
        //6
        {
            {255,255,255},
            {0,0,255},
            {255,0,255}
        },
        //7
        {
            {255,255,255},
            {0,0,255},
            {255,255,255}
        },
        //8
        {
            {255,255,255},
            {255,0,255},
            {255,255,255}
        },
        //9
        {
            {255,255,255},
            {255,255,255},
            {255,255,255}
        },
        
    };
    
    
    // generating halftone dots pattern
    Mat halftone_matrix[10];
    for (int i = 0; i<=9; i++) {
        halftone_matrix[i] = Mat(3, 3, CV_8UC1, halftone_source[i]);
    }
    
    // speed up the process by moving function call outside the loop
    uint source_rows = source_image.rows,
    source_cols = source_image.cols;
    // creating halftoning image
    for ( uint i = 0; i < source_rows - 1; i++) {
        for ( uint j = 0; j < source_cols - 1; j++) {
            
            int pixel_intensity = source_image.at<uchar>(i, j) / 28.3;
            halftone_matrix[pixel_intensity].copyTo(target_image(Rect_<uint>(j*3, i*3, 3, 3)));
            
        }
    }
    return target_image;
}

Mat reduceIntensity(Mat source_image, int grayLevel)
{
    uchar divider = ceil( 255 / (double)grayLevel );
    
    // calculate and redistribute indensity for each pixel
    for ( uint i = 0; i < source_image.rows; i++ ) {
        for ( uint j = 0; j < source_image.cols; j++) {
            
            source_image.at<uchar>(i, j) = (source_image.at<uchar>(i, j) / divider) * (255 / (grayLevel - 1 ));
            
        }
    }
    return source_image;
}

Mat arith(Mat img1, Mat img2) {
    
    // convert to 16 bit to prevent overflow
    img1.convertTo(img1, CV_16UC1);
    img2.convertTo(img2, CV_16UC1);
    int user_choice;
    
    // resize the smaller image to fit the larger one
    if ( img1.rows * img1.cols >= img2.rows * img2.cols ) {
        resize(img2, img2, img1.size());
    }
    else {
        resize(img1, img1, img2.size());
    }
    
    cout<<"What do you want: "<<endl
    <<"1. add two images"<<endl
    <<"2, multiply"<<endl
    <<"3. divide"<<endl
    <<"4. minus"<<endl;
    
    cin>>user_choice;
    
    switch (user_choice) {
        case 1:
            add(img1, img2, img1);
            img1 *= 257;
            break;
        case 2:
            multiply(img1, img2, img1);
            break;
        case 3:
            divide(img1, img2, img1);
            img1 *= 256;
            break;
        case 4:
            subtract(img1, img2, img1);
            img1 *= 257;
            break;
        default:
            cout<<"bad input";
            break;
    }
    return img1;
}