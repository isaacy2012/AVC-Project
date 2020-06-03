#include "robot.hpp"
#include <cmath>
using std::cout;
using std::endl;
//Pixel
struct Pixel {
    unsigned char red, green, blue;
};

const double WHITE_THRESH = 250;
<<<<<<< Updated upstream
const int motorSpeed = 25, maxLines = 10;

int getMotorDifference ( int xError) {
    //https://www.desmos.com/calculator/hdrl9extep
    return 50*sin( (xError/150.0)* M_PI );
=======
const double RED_RATIO = 1.5;
const int motorSpeed = 50, maxLines = 70;

int getMotorDifference ( int xError) {
    //https://www.desmos.com/calculator/hdrl9extep
    //return 50*sin( (xError/300.0)* M_PI );
    return 30*sin( (xError/300.0)* M_PI );
}

int averageLocOfArray ( int nums[], int size ) {
    int total = 0;
    for (int i = 0; i < size; i ++) {
        if (nums[i] == 1) {
            total = total + i;
        }
    }
    return (int)((double)total/(double)size);
>>>>>>> Stashed changes
}

int main() {
    //sf::TcpSocket socket;
    cameraView.width = 150; // 100?
    cameraView.height = 100;
    cameraView.data = new char[cameraView.width * cameraView.height * 3];
    cameraView.n_bytes = cameraView.width * cameraView.height * 3;
    if (initClientRobot() != 0) {
        cout << " Error initializing robot" << endl;
    }
    while (1) {
        takePicture();
        //make a new array of the rows, set to the maximum number of rows we want
        int *averageXArrayWhite = new int[maxLines], *averageXArrayRed = new int[maxLines];
        int fullRowN = 0;
        for (int row = 0; row < maxLines ; row++) {
            int *rowWhites = new int[cameraView.width]; //table for storing the white pixels in each row
            int *rowReds = new int[cameraView.width]; //table for storing the red pixels in each row
            //set the whole row to no whites.
            for (int rowWhite = 0; rowWhite < cameraView.width; rowWhite++) {
                rowWhites[rowWhite] = 0, rowReds[rowWhite] = 0;
            }
            //in each column (for each pixel * )
            for (int column = 0; column < cameraView.width; column++) {
                //mark another pixel
                bool isWhite = false;
                bool isRed = false;
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
                if (isRed == true) {//&& (column < cameraView.width-30)) {
                    rowReds[column] = 1;
                }
            }
            //get average of rowWhites
            int counterWhite = 0, totalRowWhite = 0, counterRed = 0, totalRowRed = 0;
            for (int i = 0; i < cameraView.width; i++) {
                if (rowWhites[i] == 1) {
                    counterWhite = counterWhite + i, totalRowWhite = totalRowWhite + 1;
                }
                if (rowReds[i] == 1) {
                    counterRed = counterRed + i, totalRowRed = totalRowRed + 1;
                }
                if ((i > cameraView.width/2) && (row>maxLines-10) && (rowReds[i] == 1)) {
                    fullRowN =  fullRowN + 1;
                }
            }
            if (counterWhite > 2) {
                averageXArrayWhite[row] = (counterWhite / totalRowWhite);
            } else {
                averageXArrayWhite[row] = -1;
            }
            if (counterRed > 2) {
                averageXArrayRed[row] = (counterRed / totalRowRed);
            } else {
                averageXArrayRed[row] = -1;
            }
        }
        int *angles = new int[90];
        int angleCounter = 0;
        for (int i = 45; i < 135; i++ ) {
            //get the colors from get_pixel;
            int radius = 30;
            int x = radius*cos( i/180.0 * M_PI);
            int y = radius*sin( i/180.0 * M_PI);
            int red = (int) get_pixel(cameraView, cameraView.height-y, (cameraView.width/2)+x, 0);
            int green = (int) get_pixel(cameraView, cameraView.height-y, (cameraView.width/2)+x, 1);
            int blue = (int) get_pixel(cameraView, cameraView.height-y, (cameraView.width/2)+x, 2);
            //cout << "red: " << red << "green: " << green << "blue: " << blue << endl;
            if ((double)red/(double)(green+blue)/2 > RED_RATIO) {
                angleCounter = angleCounter +1;
                angles[i] = 1;
            }
        }
        int aError;
        if (angleCounter == 0) {
            aError = -91;
        } else {
            aError = averageLocOfArray(angles, 90)-90;
        }
        //add up all of the -'s and get the average x value
        /*
        int counterWhite = 0;
        int totalWhite = 0;
        for (int i = 0; i < maxLines; i++) {
            if (averageXArrayWhite[i] != -1) {
                counterWhite = counterWhite + averageXArrayWhite[i];
                totalWhite = totalWhite+1;
            }
        }
        int counterRed = 0;
        int totalRed = 0;
        for (int i = 0; i < maxLines; i++) {
            if (averageXArrayRed[i] != -1) {
                counterRed = counterRed + averageXArrayRed[i];
                totalRed = totalRed+1;
            }
        }


        //int averageX = (counter / cameraView.height);
        int averageXWhite = (counterWhite / totalWhite);
        int averageXRed = (counterRed / totalRed);
         */
        //get the motorDifference from the function
        int motorDifference;
        cout << aError << endl;
        if (aError == -91) {
            motorDifference = 15;
            0;
        } else {
            motorDifference = getMotorDifference(aError);
        }
        /*
        if (totalWhite > 3) {
            motorDifference = getMotorDifference(averageXWhite - cameraView.width / 2);
        } else if (fullRowN  > (cameraView.width/2)-20 ) {
            cout << "FULLROW" << endl;
            motorDifference = 15;
        } else if  (totalRed > 3) {
            motorDifference = getMotorDifference( averageXRed - 30 );
        } else {
            motorDifference = -2;
        }
         */
        //set the motor speeds to the base speed +- the motorDifference
        double vLeft = motorSpeed+motorDifference, vRight = motorSpeed-motorDifference;
        setMotors(vLeft, vRight);
        usleep(500);
    } //while
} // main