
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "rinside.h"

#include <NNS/partial_moments.h>

TEST_CASE("NNS::partial_moment - 1")
{
    // Get the R inside singleton
    RInside &R = NNS_test::rinside::r();

    //-------------------------------------------------
    // partial_moment
    //-------------------------------------------------

    // Create sample data
    std::vector<double> v = {3.0, 4.5, 5.2, 7.8, 2.1};
    Eigen::VectorXd e_v = Eigen::Map<Eigen::VectorXd>(v.data(), v.size());
    Rcpp::NumericVector r_v(v.begin(), v.end());
    R["v"] = r_v;

    double degree = 2.0; // e.g. second-order moment
    double target = 5.0; // threshold/benchmark
    R["d"] = degree;
    R["t"] = target;

    double e_res = NNS::partial_moment<NNS::Tail::Lower>(degree, target, e_v);
    double r_res = R.parseEval("LPM(d, t, v)");
    Rcpp::Rcout << "e_res: " << e_res << "\n";
    Rcpp::Rcout << "r_res: " << r_res << "\n";
    REQUIRE(e_res == Catch::Approx(r_res).margin(1e-12));
}

TEST_CASE("NNS::co_partial_moment - 1")
{
    //
    //-------------------------------------------------
    // co_partial_moment
    //-------------------------------------------------
    Eigen::VectorXd x(5);
    x << 0.04, 0.06, 0.02, -0.01, 0.03;
    Eigen::VectorXd y(5);
    y << 0.01, 0.05, 0.00, -0.02, 0.04;

    double k = 2.0;
    double Tx = 0.0;
    double Ty = 0.0;

    double cpm_up = NNS::co_partial_moment<NNS::Tail::Upper>(k, x, y, Tx, Ty);
    Rcpp::Rcout << "Upper-tail CPM  = " << cpm_up << '\n';

    double cpm_lo = NNS::co_partial_moment<NNS::Tail::Lower>(k, x, y, Tx, Ty);
    Rcpp::Rcout << "Lower-tail CPM  = " << cpm_lo << '\n';
}

TEST_CASE("NNS::divergent_partial_moment - 1")
{
    //
    //-------------------------------------------------
    // divergent_partial_moment
    //-------------------------------------------------
    Eigen::VectorXd x(4);
    x << -0.05, 0.02, -0.01, 0.03;
    Eigen::VectorXd y(4);
    y << 0.04, -0.02, 0.01, 0.05;

    double k = 1.0;
    double Tx = 0.0;
    double Ty = 0.0;

    // Lower tail for x, upper tail for y
    double dpm = NNS::divergent_partial_moment<
        NNS::Tail::Lower,
        NNS::Tail::Upper>(k, x, y, Tx, Ty);

    Rcpp::Rcout << "Divergent PM (x lower, y upper) = " << dpm << '\n';
}