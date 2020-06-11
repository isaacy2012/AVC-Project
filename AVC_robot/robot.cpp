#include "robot.hpp"
#include <cmath>
using namespace std;

const int WHITE_THRESH = 254, RED_RATIO = 3, motorSpeed = 40, maxLines = 10, safety = 68, defaultLeft = 12;

int getMotorDifference ( int xError) {
    //https://www.desmos.com/calculator/hdrl9extep
    return (int)(60*sin( (xError/300.0)* M_PI ));
}

int distanceToColor( int angle, int max ) {
    for (int i = 0; i < max; i ++ ) {
        int x = (int)(i *(double)cos( (angle)/180.0 * M_PI)), y = (int)(i * (double)sin( (angle)/180.0 * M_PI)), red = (int) get_pixel(cameraView, cameraView.height-y-3, (cameraView.width/2)+x, 0), green = (int) get_pixel(cameraView, cameraView.height-y-3, (cameraView.width/2)+x, 1), blue = (int) get_pixel(cameraView, cameraView.height-y-3, (cameraView.width/2)+x, 2);
        if ( ((double)(2.0*red/((double)(green + blue))) > (RED_RATIO) )) {
            return i;
        }
    }
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
        int motorDifference = 0, distToFront = distanceToColor(90, 40), distToLeft = distanceToColor(180, (int)(cameraView.width/2.0)), distToRight = distanceToColor( 0,50);
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