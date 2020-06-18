#include "robot.hpp"
#include <cmath>

const int WHITE_THRESH = 254, BLACK_THRESH = 1, RED_RATIO = 3, motorSpeed = 40, maxLines = 15, safety = 68, defaultLeft = 18, blackDist = 30;

struct TripleDist {
    int left, front, right;
};

int getMotorDifference ( int xError) {
    //https://www.desmos.com/calculator/elxqlz5fub
    return (int)(40*sin( (xError/300.0)* M_PI ));
}

bool blackInFront() {
    //count i until a black pixel is found
    for (int i = 0; i < blackDist; i++ ) {
        if ( (int) get_pixel(cameraView, cameraView.height-i-3, (cameraView.width/2), 3) < BLACK_THRESH ) {
            //if a black pixel is found ( at the flag ) within the blackDist, return true
            return true;
        }
    }
    //if no black pixel found ( not at the flag yet ), return false
    return false;
}
int distanceToColor( int angle, int max ) {
    //count i until red pixel is found
    for (int i = 0; i < max; i ++ ) {
        //go in a straight line from origin (+3 in y axis to account for edge of camera)
        int x = (int)(i *(double)cos( (angle)/180.0 * M_PI)), y = (int)(i * (double)sin( (angle)/180.0 * M_PI));
        int red = (int) get_pixel(cameraView, cameraView.height-y-3, (cameraView.width/2)+x, 0);
        int green = (int) get_pixel(cameraView, cameraView.height-y-3, (cameraView.width/2)+x, 1);
        int blue = (int) get_pixel(cameraView, cameraView.height-y-3, (cameraView.width/2)+x, 2);
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
    int redMode = 0, previousDistToFront = -1 ;
    while (true) {
        takePicture();
        //start the motorDifference at 0 (going straight ahead)
        int motorDifference = 0;
        //create the TripleDist of distances on the left, front, and right respectively.
        TripleDist distances;
        distances.left = distanceToColor( 180, (int)(cameraView.width/2.0));
        distances.front = distanceToColor(90,40);
        distances.right = distanceToColor( 0, 50 );
        //if there's something to the left, suggest that the bot moves right
        if (distances.left > -1 ) {
            motorDifference = getMotorDifference(safety - distances.left);
        } else {
            //if there's nothing to the left, bot go left by default slowly (full turn)
            motorDifference = getMotorDifference(-defaultLeft);
        }

        // if impedance from the front
        if (distances.front < previousDistToFront && distances.front > -1 ) {
            motorDifference = motorDifference+(int)(90-distances.front), redMode = 1;
        }

        //finding average x of white pixels
        int avgX = -1, xTotal = 0, counter = 0;
        for (int i = 0; i < maxLines; i++ ) {
            for (int j = 0; j < cameraView.width; j ++ ) {
                if ((int) get_pixel(cameraView, cameraView.height - i - 1 , j, 3) > WHITE_THRESH) {
                    xTotal = xTotal+j, counter = counter+1;
                }
            }
        }
        //if there is a significant number of white pixels, follow the white line
        if (counter > 10) {
            //-1 is so that if it's equal on both sides, it will always go left
            avgX = xTotal/counter, motorDifference = getMotorDifference( avgX - cameraView.width/2 -1);
        } else if (motorDifference == getMotorDifference(-defaultLeft) && distances.left == -1 && distances.right == -1 && redMode == 0 ) {
            //if redmode is off and there's no other stuff to guide the bot, spin around quickly since that's a dead end
            motorDifference = -60;
        }
        //set the previous distance to front for next time around the while loop.
        previousDistToFront = distances.front;
        //if we have reached the chequered flag (i.e black pixel in front)
        if (blackInFront() == true ) {
            //stop the bot
            setMotors(0, 0);
            //stop the while loop
            break;
        }
        //set the motor velocities.
        setMotors(motorSpeed+motorDifference, motorSpeed-motorDifference);
        usleep(500);
    } //while
} // main
