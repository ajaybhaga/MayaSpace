//
// C++ Implementation by Ajay Bhaga
//
// Modified Genetic Algorithm
// Based on design of Samuel Arzt (March 2017)
//

#ifndef EANN_SIMPLE_MATH_HELPER_H
#define EANN_SIMPLE_MATH_HELPER_H

#include <string>
#include <math.h>       /* exp */

class MathHelper {
public:

    MathHelper();
    ~MathHelper();

    // The standard sigmoid function.
    static double sigmoidFunction(double xValue);

    // The standard TanH function.
    static double tanHFunction(double xValue);

    // The SoftSign function as proposed by Xavier Glorot and Yoshua Bengio (2010):
    // "Understanding the difficulty of training deep feed-forward neural networks."
    static double softSignFunction(double xValue);
};

#endif //EANN_SIMPLE_MATH_HELPER_H
