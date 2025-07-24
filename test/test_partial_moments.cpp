
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "rinside.h"

#include <NNS/algorithms/partial_moments.hpp>

TEST_CASE("NNS::partial_moment NATTY - 1")
{
    // Get the R inside singleton
    RInside &R = NNS_test::rinside::r();

    //-------------------------------------------------
    // partial_moment
    //-------------------------------------------------

    // Create sample data
    std::vector<double> v = {3.0, 4.5, 5.2, 7.8, 2.1};
    Rcpp::NumericVector r_v(v.begin(), v.end());
    R["v"] = r_v;

    double degree = 2.0; // e.g. second-order moment
    double target = 5.0; // threshold/benchmark
    R["d"] = degree;
    R["t"] = target;

    double e_res = NNS::partial_moment_natty<NNS::Tail::Lower>(degree, target, v);
    double r_res = R.parseEval("LPM(d, t, v)");
    Rcpp::Rcout << "e_res: " << e_res << "\n";
    Rcpp::Rcout << "r_res: " << r_res << "\n";
    REQUIRE(e_res == Catch::Approx(r_res).margin(1e-12));
}

TEST_CASE("NNS::co_partial_moment - 1")
{
    // R instance --------------------------------------------------------------
    RInside &R = NNS_test::rinside::r();

    std::vector<double> x = {0.04, 0.06, 0.02, -0.01, 0.03};
    std::vector<double> y = {0.01, 0.05, 0.00, -0.02, 0.04};

    double k = 2.0;  // moment degree
    double Tx = 1.0; // targets
    double Ty = 0.0;

    // shove C++ data into R ----------------------------------------------------
    R["x"] = Rcpp::NumericVector(x.data(), x.data() + x.size());
    R["y"] = Rcpp::NumericVector(y.data(), y.data() + y.size());
    R["k"] = k;
    R["Tx"] = Tx;
    R["Ty"] = Ty;

    // ---- upper–tail CPM ------------------------------------------------------
    double cpm_up_cpp = NNS::Co_UPM(k, x, y, Tx, Ty);
    double cpm_up_r = R.parseEval("Co.UPM(k, x, y, Tx, Ty)");
    REQUIRE(cpm_up_cpp == Catch::Approx(cpm_up_r).margin(1e-12));
    Rcpp::Rcout << "Lower CPM C++ = " << cpm_up_cpp << "\n";
    Rcpp::Rcout << "Lower CPM R   = " << cpm_up_r << "\n";

    double cpm_lo_cpp = NNS::Co_LPM(k, x, y, Tx, Ty);
    double cpm_lo_r = R.parseEval("Co.LPM(k, x, y, Tx, Ty)");
    REQUIRE(cpm_lo_cpp == Catch::Approx(cpm_lo_r).margin(1e-12));
    Rcpp::Rcout << "Lower CPM C++ = " << cpm_lo_cpp << "\n";
    Rcpp::Rcout << "Lower CPM R   = " << cpm_lo_r << "\n";
}

TEST_CASE("NNS::divergent_partial_moment - 1")
{
    // R instance --------------------------------------------------------------
    RInside &R = NNS_test::rinside::r();

    // sample data -------------------------------------------------------------
    std::vector<double> x = {10.04, 0.06, 0.02, -0.01, 0.03};
    std::vector<double> y = {10.01, 0.05, 0.00, -100.02, 0.04};

    double kx = 2.0; // moment degree
    double ky = 2.0; // moment degree
    double Tx = 1.0; // targets
    double Ty = 0.0;

    // shove C++ data into R ----------------------------------------------------
    R["x"] = Rcpp::NumericVector(x.data(), x.data() + x.size());
    R["y"] = Rcpp::NumericVector(y.data(), y.data() + y.size());
    R["kx"] = kx;
    R["ky"] = ky;
    R["Tx"] = Tx;
    R["Ty"] = Ty;

    // ---- upper–tail CPM ------------------------------------------------------
    double cpm_up_cpp = NNS::D_UPM(kx, ky, x, y, Tx, Ty);
    double cpm_up_r = R.parseEval("D.UPM(kx, ky, x, y, Tx, Ty)");
    REQUIRE(cpm_up_cpp == Catch::Approx(cpm_up_r).margin(1e-12));
    Rcpp::Rcout << "Lower CPM C++ = " << cpm_up_cpp << "\n";
    Rcpp::Rcout << "Lower CPM R   = " << cpm_up_r << "\n";

    double cpm_lo_cpp = NNS::D_LPM(kx, ky, x, y, Tx, Ty);
    double cpm_lo_r = R.parseEval("D.LPM(kx, ky, x, y, Tx, Ty)");
    REQUIRE(cpm_lo_cpp == Catch::Approx(cpm_lo_r).margin(1e-12));
    Rcpp::Rcout << "Lower CPM C++ = " << cpm_lo_cpp << "\n";
    Rcpp::Rcout << "Lower CPM R   = " << cpm_lo_r << "\n";
}