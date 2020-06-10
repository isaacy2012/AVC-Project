#include "robot.hpp"
#include <cmath>
using namespace std;

const int WHITE_THRESH = 254, RED_RATIO = 3, motorSpeed = 40, maxLines = 10, safety = 68, defaultLeft = 12;

int getMotorDifference ( int xError) {
    //https://www.desmos.com/calculator/hdrl9extep
    return (int)(60*sin( (xError/300.0)* M_PI ));
}


int distanceToColor( int angle, int color /*0 white 1 red*/, int max ) {
    // gets the nearest distance to either white or red pixel for a particular angle, and a max search distance.
    // for documentation, go to:
    // https://github.com/isaacy2012/AVC-Project/blob/master/documentationImages/line%20saving%20function%20.png
    for (int i = 0; i < max; i ++ ) {
        int x = (int)(i *(double)cos( (angle)/180.0 * M_PI)), y = (int)(i * (double)sin( (angle)/180.0 * M_PI)), red = (int) get_pixel(cameraView, cameraView.height-y-3, (cameraView.width/2)+x, 0), green = (int) get_pixel(cameraView, cameraView.height-y-3, (cameraView.width/2)+x, 1), blue = (int) get_pixel(cameraView, cameraView.height-y-3, (cameraView.width/2)+x, 2);
        if ( ((double)(red + green + blue + color*(red - green - blue)))/((double)(3+color*(green + blue - 3))) > (WHITE_THRESH-(WHITE_THRESH-RED_RATIO)*color) ) {
            //if pixel found of that colour, return the distance to that colour.
            return i;
        }
    }
    //if no pixel found of that colour, return -1
    return -1;
}


int main() {

    cameraView.width = 150, cameraView.height = 100, cameraView.data = new char[cameraView.width * cameraView.height * 3], cameraView.n_bytes = cameraView.width * cameraView.height * 3;
    if (initClientRobot() != 0) {
        cout << " Error initializing robot" << endl;
    }
    vector<int> distToFrontValues;
    while (true) {
        takePicture();
        int motorDifference = 0, distToFront = distanceToColor(90, 1, 40), distToLeft = distanceToColor(180, 1, (int)(cameraView.width/2.0)), distToRight = distanceToColor( 0, 1, 50), distToWhite = distanceToColor(90, 0, 10);
        distToFrontValues.push_back(distToFront);
        if (distToLeft > -1 ) {
            motorDifference = getMotorDifference(safety - distToLeft);
        } else {
            motorDifference = getMotorDifference(-defaultLeft);
        }
        if (distToFrontValues[distToFrontValues.size()-2] > distToFront && distToFront > -1) {
            //impedance from the front
            motorDifference = motorDifference+(int)(90-distToFront);
        } else {
            //nothing impeding from the front
        }
        int avgX = -1, xTotal = 0, counter = 0;
        for (int i = 0; i < maxLines; i++ ) {
            for (int j = 0; j < cameraView.width; j ++ ) {
                if ((int) get_pixel(cameraView, cameraView.height-i, j, 3) > WHITE_THRESH) {
                    xTotal = xTotal+j, counter = counter+1;//yTotal = yTotal+j, counter = counter+1;
                }
            }
        }
        if (counter > 10) {
            avgX = xTotal/counter; //, avgY = yTotal/counter
            motorDifference = getMotorDifference( avgX - cameraView.width/2);
        } else if (motorDifference == 0 && distToLeft == -1 && distToRight == -1 ) {
            motorDifference = getMotorDifference(-cameraView.width);
        }
        int vLeft = motorSpeed+motorDifference, vRight = motorSpeed-motorDifference;
        setMotors(vLeft, vRight);
        usleep(500);
    } //while
} // main