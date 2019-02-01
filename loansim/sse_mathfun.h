#ifndef SSE_MATHFUN_HPP
#define SSE_MATHFUN_HPP

#ifdef USE_APPR
inline double exp_ps(double y) {
     double d;
     //*(reinterpret_cast<int*>(&d) + 0) = 0;
     //*(reinterpret_cast<int*>(&d) + 1) = (int)(1512775 * y + 1072632447);
     *((int*)(&d) + 0) = 0;
     *((int*)(&d) + 1) = (int)(1512775 * y + 1072632447);
     return d;
}
#else
#include <cmath>
inline double exp_ps(double y){
    return exp(y);
}
#endif

#include <cstdio>
#include <ctime>
class timer{
    time_t t;
    public:
        timer():t(time(NULL)){}
        ~timer(){printf("\nThis program has run for: %d seconds.\n",  static_cast<int>(time(NULL)-t));}
};

int naive_atoi(const char *);
double naive_atof(const char *);

#ifndef USE_FASTRAND
static unsigned long x=123456789, y=362436069, z=521288629, period = 79228162514264337593543950336;

inline double fast_rand() {          //period 2^96-1
    unsigned long t;
    x ^= x << 16;
    x ^= x >> 5;
    x ^= x << 1;

    t = x;
    x = y;
    y = z;
    z = t ^ x ^ y;

    return 1.*z/79228162514264337593543950336L;
}
#else
#include <cstdlib>

inline double fast_rand(){
    return 1.*rand()/RAND_MAX;
}
#endif

#endif
