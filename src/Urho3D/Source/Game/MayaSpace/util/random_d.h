//
//This snippet is straight from Stroustrup's The C++ Programming Language (4th Edition), §40.7; it requires C++11
//

#ifndef EANN_SIMPLE_RANDOM_D_H
#define EANN_SIMPLE_RANDOM_D_H


#include <functional>
#include <random>

class random_d
{
public:
    random_d(double low, double high)
            :r(std::bind(std::uniform_real_distribution<>(low,high),std::default_random_engine())){}

    double operator()(){ return r(); }

private:
    std::function<double()> r;
};


#endif //EANN_SIMPLE_RANDOM_D_H