// test_partial_moments.cpp
// #define CATCH_CONFIG_MAIN
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <vector>
#include <cmath>
#include <limits>

// #include <NNS/partial_moments_omp.hpp>

using Catch::Approx;
// using Catch::REQUIRE;

// TEST_CASE("NNS::partial_moment behaves correctly", "[partial_moment]")
// {
//     // using namespace NNS;

//     std::vector<double> data = {1.0, 2.0, 3.0, 4.0, 5.0};
//     NNS_P::VectorView<double> v{data.data(), data.size()};

//     double degree = 1.0;
//     double target = 3.0;

//     SECTION("Lower tail")
//     {
//         double result = NNS_P::partial_moment<NNS_P::Tail::Lower>(degree, target, v);
//         REQUIRE(result == Approx((2.0 + 1.0 + 0.0 + 0.0 + 0.0) / 5.0));
//     }

//     SECTION("Upper tail")
//     {
//         double result = NNS_P::partial_moment<NNS_P::Tail::Upper>(degree, target, v);
//         REQUIRE(result == Approx((0.0 + 0.0 + 0.0 + 1.0 + 2.0) / 5.0));
//     }
// }

// TEST_CASE("NNS::bi_partial_moment behaves correctly", "[bi_partial_moment]")
// {
//     // using namespace NNS;

//     std::vector<double> x = {1.0, 2.0, 3.0, 4.0};
//     std::vector<double> y = {10.0, 20.0, 30.0, 40.0};

//     NNS_P::VectorView<double> xv{x.data(), x.size()};
//     NNS_P::VectorView<double> yv{y.data(), y.size()};

//     double Tx = 2.0;
//     double Ty = 25.0;
//     double degree = 1.0;

//     SECTION("Lower x, Lower y")
//     {
//         double result = NNS_P::bi_partial_moment<NNS_P::Tail::Lower, NNS_P::Tail::Lower>(degree, xv, yv, Tx, Ty);
//         double expected = ((1.0 * 15.0) + (0.0 * 5.0) + (0.0 * 0.0) + (0.0 * 0.0)) / 4.0;
//         REQUIRE(result == Approx(expected));
//     }

//     SECTION("Upper x, Upper y")
//     {
//         double result = NNS_P::bi_partial_moment<NNS_P::Tail::Upper, NNS_P::Tail::Upper>(degree, xv, yv, Tx, Ty);
//         double expected = ((0.0 * 0.0) + (0.0 * 0.0) + (1.0 * 5.0) + (2.0 * 15.0)) / 4.0;
//         REQUIRE(result == Approx(expected));
//     }

//     SECTION("Degree == 0.0 gives count of co-occurring tail events")
//     {
//         double result = bi_partial_moment<NNS_P::Tail::Lower, NNS_P::Tail::Lower>(0.0, xv, yv, Tx, Ty);
//         REQUIRE(result == Approx(2.0 / 4.0));
//     }
// }

// TEST_CASE("Thin wrapper aliases produce same results", "[NNS_P::co_partial_moment][divergent_partial_moment]")
// {
//     // using namespace NNS;

//     std::vector<double> x = {1.0, 2.0, 3.0, 4.0};
//     std::vector<double> y = {10.0, 20.0, 30.0, 40.0};

//     NNS_P::VectorView<double> xv{x.data(), x.size()};
//     NNS_P::VectorView<double> yv{y.data(), y.size()};

//     double k = 1.0, Tx = 2.0, Ty = 25.0;

//     auto v1 = NNS_P::co_partial_moment<NNS_P::Tail::Upper>(k, xv, yv, Tx, Ty);
//     auto v2 = bi_partial_moment<NNS_P::Tail::Upper, NNS_P::Tail::Upper>(k, xv, yv, Tx, Ty);
//     REQUIRE(v1 == Approx(v2));

//     auto v3 = NNS_P::divergent_partial_moment<NNS_P::Tail::Upper, NNS_P::Tail::Lower>(k, xv, yv, Tx, Ty);
//     auto v4 = NNS_P::bi_partial_moment<NNS_P::Tail::Upper, NNS_P::Tail::Lower>(k, xv, yv, Tx, Ty);
//     REQUIRE(v3 == Approx(v4));
// }
