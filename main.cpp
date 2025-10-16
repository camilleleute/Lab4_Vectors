/*****************************************************
* Filename: main.c
******************************************************/
#include "main.h"

// --- IMAGE TESTING MAIN --- //
int main(int argc, char* argv[]){
    // make sure that there are enough arguments provided
    if (argc != 2) {
        std::cerr << "Error: not enough arguments" << std::endl;
        return 1;
    }

    // open the file
    std::string filename = argv[1];
    // returns a matrix with the image loaded into it in BGR-8bit format
    Mat image = imread(filename, IMREAD_COLOR);

    // check if file couldn't be read
    if(image.empty())
    {
        std::cout << "Could not read the image: " << filename << std::endl;
        return 1;
    }
    
    Mat gray_img = to442_grayscale(image);
    Mat sobel_img = to442_sobel(gray_img);

    // Write result
    imwrite("output_gray.jpg", gray_img);
    imwrite("output_sobel.jpg", sobel_img);

     
    // title of the window and what image to show
    imshow("Display Window", sobel_img);
    waitKey(0);
     
    return 0;

}




// --- VIDEO TESTING MAIN --- //
// int main(int argc, char* argv[]){

//     // make sure that there are enough arguments provided
//     if (argc != 2) {
//         std::cerr << "Error: Not enough arguments" << std::endl;
//         return 1;
//     }

//     // Open file
//     std::string videoPath = argv[1];
//     cv::VideoCapture cap(videoPath);

//     // Check if the video file was opened successfully
//     if (!cap.isOpened()) {
//         std::cerr << "Error: Could not open or not a valid video file: " << std::endl;
//         return 1;
//     }


//     return 0;

// }
