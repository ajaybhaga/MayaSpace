//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#include "math_helper.h"

MathHelper::MathHelper() {
}

MathHelper::~MathHelper() {
}

// The standard sigmoid function.
double MathHelper::sigmoidFunction(double xValue) {

    if (xValue > 10) return 1.0;
    else if (xValue < -10) return 0.0;
    else return 1.0 / (1.0 + exp(-xValue));

}

// The standard TanH function.
double MathHelper::tanHFunction(double xValue) {

    if (xValue > 10) return 1.0;
    else if (xValue < -10) return -1.0;
    else return tanh(xValue);

}

// The SoftSign function as proposed by Xavier Glorot and Yoshua Bengio (2010):
// "Understanding the difficulty of training deep feed-forward neural networks."
double MathHelper::softSignFunction(double xValue) {

    return xValue / (1 + abs(xValue));

}