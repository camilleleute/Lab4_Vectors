/*****************************************************
* Filename: main.c
******************************************************/
#include "main.h"

//! main for image to greyscale
int main(int argc, char* argv[]){
    // make sure that there are enough arguments provided
    if (argc != 2) {
        std::cerr << "Error: not enough arguments" << std::endl;
        return 1;
    }

    // open the file
    std::string filename = argv[1];
    VideoCapture frame(filename);

    if(!frame.isOpened())
    {
        std::cout << "Could not open the video: " << filename << std::endl;
        return 1;
    }
    
    Mat src_frame;

    //while there are still more frames to read
    while(frame.read(src_frame) == true){
        //greyscale that hoe
        Mat gray_img = to442_grayscale(src_frame);
        //make the frame that the children threads are going to write to.
        output_frame.create(gray_img.rows, gray_img.cols, CV_8UC1);
        //calculate the threadbounds
        array<ThreadBounds, 4> bounds = find_chunk(gray_img.rows);
        //print_thread_bounds(bounds, gray_img.rows);

        //now make the children and assign them the task
        thread t0(thread_task, ref(gray_img), ref(bounds), 0);
        thread t1(thread_task, ref(gray_img), ref(bounds), 1);
        thread t2(thread_task, ref(gray_img), ref(bounds), 2);
        thread t3(thread_task, ref(gray_img), ref(bounds), 3);

        //wait for all the threads to complete
        t0.join();
        t1.join();
        t2.join();
        t3.join();
        //cout << "threads joined" << endl;

        //stop it if u press the escape key
        imshow("sobby filty", output_frame);
        if(waitKey(1) == 27) {
            cout << "stopped the video" << endl;
            break;
        }
    }

    frame.release();
    destroyAllWindows();
    return 0;
}