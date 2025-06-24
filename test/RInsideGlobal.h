#ifndef RCPP_SAFE_INCLUDE_H
#define RCPP_SAFE_INCLUDE_H

// Suppress unused parameter warnings for third-party headers
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <RInside.h>

#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__) || defined(__GNUG__)
    #pragma GCC diagnostic pop
#endif

RInside RINSIDE;

#endif // RCPP_SAFE_INCLUDE_H
