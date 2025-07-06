
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "rinside.h"

#include "fast_lm.h"

using Catch::Approx;

void print_list(Rcpp::List r_res)
{
    Rcpp::CharacterVector names = r_res.names();

    for (int i = 0; i < r_res.size(); ++i)
    {
        Rcpp::Rcout << names[i] << ": ";

        SEXP elem = r_res[i];

        if (Rf_isNumeric(elem))
        {
            Rcpp::NumericVector vec(elem);
            Rcpp::Rcout << vec << std::endl;
        }
        else if (Rf_isString(elem))
        {
            Rcpp::CharacterVector vec(elem);
            Rcpp::Rcout << vec << std::endl;
        }
        else if (Rf_isLogical(elem))
        {
            Rcpp::LogicalVector vec(elem);
            Rcpp::Rcout << vec << std::endl;
        }
        else if (Rf_isList(elem))
        {
            Rcpp::Rcout << "[list]" << std::endl;
            // Optionally recurse here
        }
        else
        {
            Rcpp::Rcout << "[unprintable type]" << std::endl;
        }
    }
}

void printFastLmResult(const NNS::FastLmResult &res)
{
    Rcpp::Rcout << "coef: ";
    for (double val : res.coef)
    {
        Rcpp::Rcout << val << " ";
    }
    Rcpp::Rcout << std::endl;

    Rcpp::Rcout << "fitted.values: ";
    for (double val : res.fitted_values)
    {
        Rcpp::Rcout << val << " ";
    }
    Rcpp::Rcout << std::endl;
}

TEST_CASE("fast_lm_2")
{
    INFO("LOL3");
    REQUIRE(1 + 1 == 2);
}

TEST_CASE("NNS::mean computes correct mean for different inputs", "[mean]")
{
    using NNS::mean;

    SECTION("Empty vector returns 0.0")
    {
        std::vector<double> v;
        REQUIRE(mean(v) == 0.0);
    }

    SECTION("Single element vector returns that element")
    {
        std::vector<int> v{42};
        REQUIRE(mean(v) == 42.0);
    }

    SECTION("Multiple elements with positive integers")
    {
        std::vector<int> v{1, 2, 3, 4, 5};
        REQUIRE(mean(v) == Approx(3.0));
    }

    SECTION("Multiple elements with mixed sign integers")
    {
        std::vector<int> v{-2, -1, 0, 1, 2};
        REQUIRE(mean(v) == Approx(0.0));
    }

    SECTION("Floating-point numbers")
    {
        std::vector<float> v{1.5f, 2.5f, 3.0f};
        REQUIRE(mean(v) == Approx(2.333333).epsilon(0.0001));
    }

    SECTION("Large numbers")
    {
        std::vector<long long> v{1000000000LL, 2000000000LL, 3000000000LL};
        REQUIRE(mean(v) == Approx(2000000000.0));
    }

    SECTION("All zeros")
    {
        std::vector<int> v(10, 0);
        REQUIRE(mean(v) == 0.0);
    }
}

TEST_CASE("NNS::fast_lm output matches that of the legacy NNS R code", "[mean]")
{
    // Get the R inside singleton
    auto &R = NNS_test::rinside::r();

    // Create sample data for NNS-C++
    std::vector<double> x = {0, 1, 2, 3, 4};
    std::vector<double> y = {1, 3, 5, 7, 9};
    // Create sample data for Legacy NNS
    Rcpp::NumericVector r_x(x.begin(), x.end());
    Rcpp::NumericVector r_y(y.begin(), y.end());
    R["x"] = r_x;
    R["y"] = r_y;

    // Compute fast_lm for NNS-C++
    NNS::FastLmResult cpp_res = NNS::fast_lm(x, y);
    // Compute fast_lm for Legacy NNS
    Rcpp::List r_res = R.parseEval("fast_lm(x, y);");
    Rcpp::List r_fv = r_res["fitted.values"];
    Rcpp::NumericVector r_coef = r_res["coef"];

    //
    // Compare NNS-C++ results with Legacy NNS results
    REQUIRE(static_cast<std::size_t>(r_coef.size()) == cpp_res.coef.size());
    REQUIRE(static_cast<std::size_t>(r_fv.size()) == cpp_res.fitted_values.size());

    for (std::size_t i = 0; i < cpp_res.coef.size(); ++i)
    {
        INFO("Comparing coef at index " << i);
        REQUIRE(Approx(r_coef[i]).margin(1e-10) == cpp_res.coef[i]);
    }

    for (std::size_t i = 0; i < cpp_res.fitted_values.size(); ++i)
    {
        INFO("Comparing fitted value at index " << i);
        REQUIRE(Approx(r_fv[i]).margin(1e-10) == cpp_res.fitted_values[i]);
    }
}
