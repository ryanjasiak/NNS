

# Phase 2 Agenda: Core Kernel, MVP, "Valuable 75"
- ❌: not done
- ✅: done - We have a working-implementation in C++


This file will help track the progress of translating each file/function

## C++


| ❌ [`src/fast_lm.cpp`](https://github.com/OVVO-Financial/NNS/blob/NNS-Beta-Version/src/fast_lm.cpp) -> `/include/NNS/fast_lm.h` | Status | New Function | Comments| 
|------------------------------------------------|--------|-|-|
| `List fast_lm(NumericVector x, NumericVector y)`  | ✅ | 
| `List fast_lm_mult`   | ✅  |


|❌ [`src/partial_moments_rcpp.cpp`](https://github.com/OVVO-Financial/NNS/blob/NNS-Beta-Version/src/partial_moments_rcpp.cpp)                                 | Status |
|------------------------------------------------|--------|
| `NumericVector LPM_RCPP`    | ❌  |
| `NumericVector UPM_RCPP`   | ❌  |
| `NumericVector LPM_ratio_RCPP`   | ❌  |
| `NumericVector UPM_ratio_RCPP`   | ❌  |
| `NumericVector CoLPM_RCPP`   | ❌  |
| `NumericVector CoUPM_RCPP`   | ❌  |
| `NumericVector DLPM_RCPP`   | ❌  |
| `NumericVector DUPM_RCPP`   | ❌  |
| `List PMMatrix_RCPP`   | ❌  |
| `List NNS_bin`   | ✅  |

|❌ [`src/partial_moments.cpp`](https://github.com/OVVO-Financial/NNS/blob/NNS-Beta-Version/src/partial_moments.cpp)   | Status |
|------------------------------------------------|--------|
| `double repeatMultiplication` |❌|
| `double fastPow` |❌|
| `bool isInteger` |❌|
| `double LPM_C` |✅|
| `double UPM_C` |✅|
| `NumericVector LPM_CPv` |❌|
| `NumericVector UPM_CPv` |❌|
| `NumericVector LPM_ratio_CPv` |❌|
| `NumericVector UPM_ratio_CPv` |❌|
| `double CoUPM_C` |✅|
| `double CoLPM_C` |✅|
| `double DLPM_C` |✅|
| `double DUPM_C` |✅|


|❌ [`src/RcppExports.cpp`](https://github.com/OVVO-Financial/NNS/blob/NNS-Beta-Version/src/RcppExports.cpp)                                 | Status |
|------------------------------------------------|--------|
| `List fast_lm` |✅|
| `List fast_lm_mult` |❌|
| `NumericVector LPM_RCPP` |❌|
| `NumericVector UPM_RCPP` |❌|
| `NumericVector LPM_ratio_RCPP` |❌|
| `NumericVector UPM_ratio_RCPP` |❌|
| `NumericVector CoLPM_RCPP` |❌|
| `NumericVector CoUPM_RCPP` |❌|
| `NumericVector DLPM_RCPP` |❌|
| `NumericVector DUPM_RCPP` |❌|
| `List PMMatrix_RCPP` |❌|
| `List NNS_bin` |❌|


|❌ [`R/Central_tendencies.R`](https://github.com/OVVO-Financial/NNS/blob/NNS-Beta-Version/R/Central_tendencies.R) | Status |
|------------------------------------------------|--------|
| `mode` |✅|
| `gravity` |✅|
| `rescale` |✅|

|❌ [`R/Regression.R`](https://github.com/OVVO-Financial/NNS/blob/NNS-Beta-Version/R/Regression.R) | Status |
|------------------------------------------------|--------|
|`univariate_regression`|❌|
|`multivariate_regression`|❌|
|`synthetic_regression`|❌|
|`classification`|❌|
|`predict_only`|❌|
|`plot_regression`|❌|
|`confidence_prediction_intervals`|❌|
|`estimate_derivatives`|❌|


|❌ [`R/LPM_UPM_VaR.R`](https://github.com/OVVO-Financial/NNS/blob/NNS-Beta-Version/R/LPM_UPM_VaR.R) | Status |
|------------------------------------------------|--------|
|`UPM.VaR`|❌|
|`LPM.VaR`|❌|

1. FSD.R
2. SSD.R
3. TSD.R
