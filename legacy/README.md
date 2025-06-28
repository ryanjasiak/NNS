<img src="vignettes/images/LegacyNNS_hex_sticker.png" width="150" style="border: none; outline: none; margin: 0; padding: 0; display: block;"/>




[![packageversion](https://img.shields.io/badge/LegacyNNS%20version-11.3-blue.svg?style=flat-square)](https://github.com/OVVO-Financial/LegacyNNS/commits/LegacyNNS-Beta-Version)   [![Licence](https://img.shields.io/badge/licence-GPL--3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0.en.html)

<h2 style="margin: 0; padding: 0; border: none; height: 40px;"></h2>

# LegacyNNS
Nonlinear nonparametric statistics using partial moments. Partial moments are the [elements of variance](https://www.linkedin.com/pulse/elements-variance-fred-viole) and [asymptotically approximate the area of f(x)](https://doi.org/10.2139/ssrn.2186471). These robust statistics provide the basis for nonlinear analysis while retaining linear equivalences.



LegacyNNS offers: 
  - Numerical Integration & Numerical Differentiation
  - Partitional & Hierarchial Clustering
  - Nonlinear Correlation & Dependence
  - Causal Analysis
  - Nonlinear Regression & Classification
  - ANOVA
  - Seasonality & Autoregressive Modeling
  - Normalization 
  - Stochastic Dominance
  - Advanced Monte Carlo Sampling


Companion R-package and datasets to: 
#### Viole, F. and Nawrocki, D. (2013) "*Nonlinear Nonparametric Statistics: Using Partial Moments*" (ISBN: 1490523995)


#### For a quantitative finance implementation of LegacyNNS, see [OVVO Labs](https://www.ovvolabs.com)


## Current Version
Current [![LegacyNNS](https://img.shields.io/badge/LegacyNNS--blue.svg)](https://cran.r-project.org/package=LegacyNNS) CRAN version is  [![CRAN\_Status\_Badge](https://www.r-pkg.org/badges/version/LegacyNNS)](https://www.r-pkg.org/badges/version/LegacyNNS)

## Installation
[![LegacyNNS](https://img.shields.io/badge/LegacyNNS--blue.svg)](https://cran.r-project.org/package=LegacyNNS) requires [![minimal R version](https://img.shields.io/badge/R%3E%3D-3.5.0-6666ff.svg)](https://cran.r-project.org/).  See https://cran.r-project.org/ or [![installr](https://img.shields.io/badge/installr-0.18.0-blue.svg)](https://cran.r-project.org/package=installr) for upgrading to latest R release.

```r
library(remotes); remotes::install_github('OVVO-Financial/LegacyNNS', ref = "LegacyNNS-Beta-Version")
```
or via CRAN
```r
install.packages('LegacyNNS')
```

## Examples
Please see https://github.com/OVVO-Financial/LegacyNNS/blob/LegacyNNS-Beta-Version/examples/index.md for basic partial moments equivalences, hands-on statistics, machine learning and econometrics examples.


## Citation
```
@Manual{,
    title = {LegacyNNS: Nonlinear Nonparametric Statistics},
    author = {Fred Viole},
    year = {2016},
    note = {R package version 11.3},
    url = {https://CRAN.R-project.org/package=LegacyNNS},
  }
```

## Thank you for your interest in LegacyNNS!
![](https://cranlogs.r-pkg.org/badges/LegacyNNS)
![](https://cranlogs.r-pkg.org/badges/grand-total/LegacyNNS)
