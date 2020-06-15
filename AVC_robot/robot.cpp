
#include "robot.hpp"
#include <cmath>

//Pixel
struct Pixel {
    unsigned char red, green, blue;
};

const double WHITE_THRESH = 250;
const int motorSpeed = 20, maxLines = 10;

int getMotorDifference ( int xError) {
    //https://www.desmos.com/calculator/hdrl9extep
    return 50*sin( (xError/300.0)* M_PI );
}

int main() {
    //sf::TcpSocket socket;
    cameraView.width = 150; // 100?
    cameraView.height = 100;
    cameraView.data = new char[cameraView.width * cameraView.height * 3];
    cameraView.n_bytes = cameraView.width * cameraView.height * 3;
    if (initClientRobot() != 0) {
        std::cout << " Error initializing robot" << std::endl;
    }
    while (1) {
        takePicture();
        //make a new array of the rows, set to the maximum number of rows we want
        int *averageXArray = new int[maxLines];
        for (int row = 0; row < maxLines ; row++) {
            int *rowWhites = new int[cameraView.width]; //table for storing the white pixels in each row
            //set the whole row to no whites.
            for (int rowWhite = 0; rowWhite < cameraView.width; rowWhite++) {
                rowWhites[rowWhite] = 0;
            }
            //in each column (for each pixel * )
            for (int column = 0; column < cameraView.width; column++) {
                //mark another pixel
                bool isWhite = false;
                //get the luminance from get_pixel, getting the maxLines rows closest to the robot.
                int luminance = (int) get_pixel(cameraView, cameraView.height-row, column, 3);
                //if the pixel is white
                if (luminance > WHITE_THRESH) {
                    //mark that pixel as white
                    isWhite = true;
                }
                if (isWhite == true) {
                    rowWhites[column] = 1;
                }
            }
            //get average of rowWhites
            int counter = 0, totalRow = 0;
            for (int i = 0; i < cameraView.width; i++) {
                if (rowWhites[i] == 1) {
                    counter = counter + i, totalRow = totalRow + 1;
                }
            }
            if (counter > 2) {
                averageXArray[row] = (counter / totalRow);
            } else {
                averageXArray[row] = -1;
            }
        }
        //add up all of the -'s and get the average x value
        int counter = 0;
        for (int i = 0; i < maxLines; i++) {
            if (averageXArray[i] != -1) {
                counter = counter + averageXArray[i];
            }
        }
        //int averageX = (counter / cameraView.height);
        int averageX = (counter / maxLines);
        //get the motorDifference from the function
        int motorDifference = getMotorDifference( averageX - cameraView.width/2 );
        //set the motor speeds to the base speed +- the motorDifference
        double vLeft = motorSpeed+motorDifference, vRight = motorSpeed-motorDifference;
        setMotors(vLeft, vRight);
        usleep(500);
    } //while
} // main
