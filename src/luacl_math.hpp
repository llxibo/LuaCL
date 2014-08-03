#ifndef __LUACL_MATH_HPP
#define __LUACL_MATH_HPP

#include <cmath>
#include <vector>
#include <errno.h>

namespace util {
#define WOULD_BE_MODIFIED
#define ARG_IN
#define ARG_OUT

template <typename T>
T luacl_prefix_sum(ARG_IN WOULD_BE_MODIFIED std::vector<T>& data) {
    for (size_t k = 1; k < data.size(); k += k) {
        for (size_t i = 0; i < data.size(); i += k) {
            auto j = i + k;
            if (j < data.size()) {
                data[i] += data[j];
                i = j;
            } else {
                break;
            }
        }
    }
    return data[0];
}

template <typename T>
void luacl_mean_and_stddev(ARG_IN const T* _data, ARG_IN size_t n, ARG_OUT T& mean, ARG_OUT T& stddev) {
    /* Make a copy of _data. */
    std::vector<T> data(_data, _data + n);

    /* Prefix-sum to minimize rounding errors for FP types. */
    mean = luacl_prefix_sum(data) / static_cast<T>(n);

    /* data --> sqr(data - mean) */
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = _data[i] - mean;
        data[i] *= data[i];
    }

    /* sqr(data - mean) --> stddev */
    stddev = sqrt(luacl_prefix_sum(data) / static_cast<T>(n));
}

/* Phi(x) - the cumulative distribution function for standard normal distribution( mean = 0, stddev = 1 ). */
inline double luacl_stdnorm_cdf(double x) {
    return 0.5 * (1 + erf(x * (1 / 1.41421356237309504880)));
}

/* Phi^-1(clvl) - the inverse function of `lualc_stdnorm_cdf`. http://home.online.no/~pjacklam/notes/invnorm/ */
inline double luacl_stdnorm_cdf_inv(double p) {
    /* Coefficients in rational approximations. */
    static const double a[] = {
        -3.969683028665376e+01,
        2.209460984245205e+02,
        -2.759285104469687e+02,
        1.383577518672690e+02,
        -3.066479806614716e+01,
        2.506628277459239e+00
    };

    static const double b[] = {
        -5.447609879822406e+01,
        1.615858368580409e+02,
        -1.556989798598866e+02,
        6.680131188771972e+01,
        -1.328068155288572e+01
    };

    static const double c[] = {
        -7.784894002430293e-03,
        -3.223964580411365e-01,
        -2.400758277161838e+00,
        -2.549732539343734e+00,
        4.374664141464968e+00,
        2.938163982698783e+00
    };

    static const double d[] = {
        7.784695709041462e-03,
        3.224671290700398e-01,
        2.445134137142996e+00,
        3.754408661907416e+00
    };
    double q, r;

    if (p < 0 || p > 1) {
        return 0.0;
    } else if (p == 0) {
        return -HUGE_VAL /* minus "infinity" */;
    } else if (p == 1) {
        return HUGE_VAL /* "infinity" */;
    } else if (p < 0.02425) {
        /* Rational approximation for lower region */
        q = sqrt(-2 * log(p));
        return (((((c[0] * q + c[1])*q + c[2])*q + c[3])*q + c[4])*q + c[5]) /
               ((((d[0] * q + d[1])*q + d[2])*q + d[3])*q + 1);
    } else if (p > 0.97575) {
        /* Rational approximation for upper region */
        q = sqrt(-2 * log(1 - p));
        return -(((((c[0] * q + c[1])*q + c[2])*q + c[3])*q + c[4])*q + c[5]) /
               ((((d[0] * q + d[1])*q + d[2])*q + d[3])*q + 1);
    } else {
        /* Rational approximation for central region */
        q = p - 0.5;
        r = q*q;
        return (((((a[0] * r + a[1])*r + a[2])*r + a[3])*r + a[4])*r + a[5])*q /
               (((((b[0] * r + b[1])*r + b[2])*r + b[3])*r + b[4])*r + 1);
    }
}
    
}   /* namespace util */

#endif /* __LUACL_MATH_HPP */
