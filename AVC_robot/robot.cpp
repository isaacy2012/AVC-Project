#include "robot.hpp"
#include <cmath>

int WHITE_THRESH = 254, RED_RATIO = 3, motorSpeed = 40, maxLines = 10, safety = 68, defaultLeft = 12, redMode = 0, previousDistToFront = -1, avgX = -1, xTotal = 0, counter = 0 ;

int getMotorDifference ( int xError) {
    //https://www.desmos.com/calculator/sze5nvrrbr
    return (int)(60*sin( (xError/300.0)* M_PI ));
}

int distanceToColor( int angle, int max ) {
    //count i until colour is found
    for (int i = 0; i < max; i ++ ) {
        //go in a straight line from origin (+3 in y axis to account for edge of camera)
        int x = (int)(i *(double)cos( (angle)/180.0 * M_PI)), y = (int)(i * (double)sin( (angle)/180.0 * M_PI)), red = (int) get_pixel(cameraView, cameraView.height-y-3, (cameraView.width/2)+x, 0), green = (int) get_pixel(cameraView, cameraView.height-y-3, (cameraView.width/2)+x, 1), blue = (int) get_pixel(cameraView, cameraView.height-y-3, (cameraView.width/2)+x, 2);
        if ( ((double)(2.0*red/((double)(green + blue))) > (RED_RATIO) )) {
            //once it's found a red pixel, return how far we had to go along that line in terms of i
            return i;
        }
    }
    //if no red pixels found, return -1
    return -1;
}

int main() {

    //camera initialisation
    cameraView.width = 150, cameraView.height = 100, cameraView.data = new char[cameraView.width * cameraView.height * 3], cameraView.n_bytes = cameraView.width * cameraView.height * 3;
    if (initClientRobot() != 0) {
        std::cout << " Error initializing robot" << std::endl;
    }
    while (true) {
        int pictureState = takePicture(), motorDifference = 0, distToFront = distanceToColor(90, 40), distToLeft = distanceToColor(180, (int)(cameraView.width/2.0)), distToRight = distanceToColor( 0,50);
        if (distToLeft > -1 ) {
            //if there's something to the left, suggest that the bot moves right
            motorDifference = getMotorDifference(safety - distToLeft);
        } else {
            //if there's nothing to the left, bot go left by default slowly (full turn)
            motorDifference = getMotorDifference(-defaultLeft);
        }
        if (distToFront < previousDistToFront && distToFront > -1 ) {
            //impedance from the front
            motorDifference = motorDifference+(int)(90-distToFront), redMode = 1;
        } else {
            //nothing impeding from the front
        }
        //finding average x of white pixels
        for (int i = 0; i < maxLines; i++ ) {
            for (int j = 0; j < cameraView.width; j ++ ) {
                if ((int) get_pixel(cameraView, cameraView.height-i, j, 3) > WHITE_THRESH) {
                    xTotal = xTotal+j, counter = counter+1;//yTotal = yTotal+j, counter = counter+1
                }
            }
        }
        //cout << "Left: " << distToLeft << " Right: " << distToRight << " Motor: " << motorDifference << endl
        if (counter > 10) {
            //if there is a significant number of white pixels, follow the white line
            //-1 is so that if it's equal on both sides, it will always go left
            avgX = xTotal/counter, motorDifference = getMotorDifference( avgX - cameraView.width/2 -1);
        } else if (motorDifference == getMotorDifference(-defaultLeft) && distToLeft == -1 && distToRight == -1 && redMode == 0 ) {
            //if redmode is off and there's no other stuff to guide the bot, spin around quickly since that's a dead end
            motorDifference = getMotorDifference(-cameraView.width);
        }
        //set the motors to the velocities, and reset variables, and usleep
        setMotors(motorSpeed+motorDifference, motorSpeed-motorDifference), previousDistToFront = distToFront, avgX = -1, xTotal = 0, counter = 0, usleep(10000);
    } //while
} // main