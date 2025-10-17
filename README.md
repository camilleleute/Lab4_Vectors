# Lab3_GrayscaleAndSobelFilter
Appling a grayscale and the Sobel filter frame by frame to a video file

## Overview
It’s time to start writing some application code on the Raspberry Pi! In this lab, you will be writing a C program that applies grayscale and the Sobel Filter frame by frame to a video file! What video file? That’s your choice, just grab a video file that’s compatible with OpenCV! You'll be compiling and running this code on your Raspberry Pis, so you may wish to write the code on your pi too! Fortunately you'll find all of your favorite Linux code editors (or, at least, VIM and Emacs) are available through the apt repo on your Pi (sudo apt install -y emacs vim).

## Assignment
Your task for this lab is to build and compile a program that pulls in frames from a video file, applies grayscale and Sobel to the frame and displays it. You will be processing one frame at a time until the video is done.

1. Create a to442_grayscale() function that uses the algorithm discussed in class to turn an OpenCV RGB Mat into a grayscale Mat. You may not use OpenCV’s built-in functions to do this conversion.
2. Create a to442_sobel() function that takes a grayscale Mat and returns a Mat with the Sobel filter applied using the algorithm and approximations discussed in class. Again, do not use any built-in Sobel functions from OpenCV, you must write the code to traverse the Mat manually.
3. Create a unified program that takes in a video file and processes it on a per frame basis. Each frame should have grayscale applied, followed by Sobel, and finally displayed to the user on screen.

## Deliverables
Demo step three to the professor (include a youtube or other video sharing link of your program running in your submissions). Submit all of your source code to Canvas. Please include both your name and your partner’s name in the document.

OVERALL PLAN:
- FIRST: implement greyscale and sobel filter calculations
    - test that on still images and verify it works
- SECOND: tile the image
    - test splitting up one image into 4 sections within one thread
    - then putting it back together correctly
    - dont split off into multiple threads yet just make sure it can be split and reattatched
    - handle the halo pixel row reading here
- THIRD: actually split it apart and deal with race conditions
    - the main thread converts the frame to greyscale
    - main thread makes a final global output matrix
    - then keep that global and split to 4 threads
    - each thread reads its respective section from main global frame
        - make sure to account for the borders here
        - thread 0 gets section + 1
        - thread 1 gets section - 1 + 1
        - thread 2 gets section - 1 + 1
        - thread 3 gets section -1
    - GRAB THE ENTIRE ROWS FOR FASTER MEM ACCESS 
    - each thread does the sobel function and writes its section to the output frame
    - set a barrier in the main thread to wait for all the children to be done
    - once done, move onto the next frame.
    - join the sections properly, taking into account the borders
- FOURTH: actually display the video
    - if its really slow maybe need to implement double buffering
        - while writing to one frame, display the other then swap
    - take the video and somehow process it frame by frame (theres probably something for this in openCV)

SOBEL FILTER:
- to start, the first step was to take in a greyscale matrix in openCV
- then take that image, go through the matrix and multiply it by something to simulate calculating the filter
- then put it back together into a final matrix and output that side by side to the original
- i also want to print out the original matrix dimensions and final to make sure the size was maintained in the transformation
- once all of these parts are working, then I can go back and change the math and make sure that the sobel calculation is the one that is applied
- OpenCV is row-major, so iterate the rows then the collumns
- implement a BORDER POLICY: if the pixel is on the edge, set it to 0 automatically
    - if it doesnt have 8 full neghbors to process, then just don't
- so first iterate through the matrix (print it out) 
- for each cell, first just test gathering the right 8 neighbors
- if there is not enough neighbors then find a way to decipher that?
    - how can i tell if a pixel is on the edge? 
        - because i know the size of the matrix? and so i know the edges? or can i check for out of bounds? technically its in memory so i can just grab some data that is not part of the matrix... so idk this part yet
COORDINATE BASED CHECK:
 if (row == 0 || row == rows - 1 || col == 0 || col == cols - 1) {
    // on the border
} 
- once i check that the matrix is on the edge, I can zero it out (get rid of the border)
- include that check as i iterate through the matrix
- then i multiply that neighboring matrix with the Gx matrix, then the Gy matrix, store those result matrices 
- write function to sum those matrices
- then calcualte the weight with absolute value and clamp it to 255
- sub functions i need: 
- printing out matrix
- multiplying matrices together 
- scale and clamp
- summing matrix together 
- finding neighbor and creating neighboring matrix