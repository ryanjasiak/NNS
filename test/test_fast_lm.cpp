
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "RInsideGlobal.h"

#include "../src/fast_lm.h"

using Catch::Approx;

void show(const Rcpp::List &L)
{
    // this function is cumbersome as we haven't defined << operators
    std::cout << "Showing list content:\n";
    std::cout << "L[0] " << Rcpp::as<int>(L[0]) << std::endl;
    std::cout << "L[1] " << Rcpp::as<double>(L[1]) << std::endl;
    Rcpp::IntegerVector v = Rcpp::as<Rcpp::IntegerVector>(L[2]);
    std::cout << "L[2][0] " << v[0] << std::endl;
    std::cout << "L[2][1] " << v[1] << std::endl;
}

TEST_CASE("fast_lm_1")
{
    Rcpp::List mylist(3);
    mylist[0] = 1;
    mylist[1] = 2.5;
    Rcpp::IntegerVector v(2);
    v[0] = 10;
    v[1] = 11; // with C++0x we could assign directly
    mylist[2] = v;
    show(mylist);

    RINSIDE["myRlist"] = mylist;
    std::string r_code =
        "myRlist[[1]] = 42; myRlist[[2]] = 42.0; myRlist[[3]][2] = 42; myRlist";

    Rcpp::List reslist = RINSIDE.parseEval(r_code);
    show(reslist);
    INFO("LOL1");
    REQUIRE(1 + 1 == 2);
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
