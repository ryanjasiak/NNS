library(testthat)
library(LegacyNNS)
Sys.setenv("OMP_THREAD_LIMIT" = 2)
test_check("LegacyNNS")
