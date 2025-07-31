
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "rinside.h"

#include <NNS/fast_lm.h>

using Catch::Approx;

/////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::vector<double>> numericMatrixToVector(const Rcpp::NumericMatrix& mat) {
    std::vector<std::vector<double>> result(mat.nrow(), std::vector<double>(mat.ncol()));
    for (int i = 0; i < mat.nrow(); ++i) {
        for (int j = 0; j < mat.ncol(); ++j) {
            result[i][j] = mat(i, j);
        }
    }
    return result;
}
/////////////////////////////////////////////////////////////////////////////////////////////

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

TEST_CASE("NNS::fast_lm (2) output matches that of the legacy NNS R code", "[mean]")
{
    // Get the R inside singleton
    auto &R = NNS_test::rinside::r();

    // Create sample data for NNS-C++
    std::vector<double> x = {15,22,37,42,55};
    std::vector<double> y = {1.5, 2.5, 3.0, 4.0, 5.0};
    // Create sample data for Legacy NNS
    Rcpp::NumericVector r_x(x.begin(), x.end());
    Rcpp::NumericVector r_y(y.begin(), y.end());
    R["x"] = r_x; // Assign x to R -- necessary for R testing
    R["y"] = r_y; // Assign y to R -- necessary for R testing

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

TEST_CASE("NNS::cholesky_decomposition output matches that of the legacy NNS R code", "[mean]")
{
    // Get the R inside singleton
    auto &R = NNS_test::rinside::r();

    // Create sample data for NNS-C++
    using Scalar = double;
    using Matrix = std::vector<std::vector<Scalar>>;
    Matrix x = {
        {4, 2, 1},
        {2, 5, 3},
        {1, 3, 6}
    };
    // Create sample data for Legacy NNS
    std::vector<double> flat_x;
    for (const auto& row : x) {
        flat_x.insert(flat_x.end(), row.begin(), row.end());
    }
    // Construct NumericMatrix for R (3x3) in column-major layout
    Rcpp::NumericMatrix r_x(3, 3);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            r_x(j, i) = x[i][j];  // ✅ Transpose: fix C++ (row-major) -> R (col-major)

    R["x"] = r_x; // Assign x to R -- ready for direct use in R

    // Compute cholesky_decomposition for NNS-C++
    vector<vector<double>> cpp_res = NNS::cholesky_decomposition(x);
    // Compute cholesky_decomposition for Legacy NNS
    Rcpp::NumericMatrix r_res = R.parseEval("cholesky_decomposition(x);");

    auto converted = numericMatrixToVector(r_res);
    //
    // Compare NNS-C++ results with Legacy NNS results

    REQUIRE(static_cast<std::size_t>(converted.size()) == cpp_res.size());
    REQUIRE(static_cast<std::size_t>(converted[0].size()) == cpp_res[0].size());

    // Compare two matrices element-wise
    for (std::size_t i = 0; i < cpp_res.size(); ++i) {
        for (std::size_t j = 0; j < cpp_res[i].size(); ++j) {
            INFO("Comparing matrix element at (" << i << ", " << j << ")");
            REQUIRE(Approx(converted[i][j]).margin(1e-10) == cpp_res[i][j]);
        }
    }
}

TEST_CASE("Version 2: NNS::cholesky_decomposition output matches that of the legacy NNS R code", "[mean]")
{
    // Get the R inside singleton
    auto &R = NNS_test::rinside::r();

    // Create sample data for NNS-C++
    using Scalar = double;
    using Matrix = std::vector<std::vector<Scalar>>;
    Matrix x = {
        {4, 1, 1, 0},
        {1, 5, 2, 1},
        {1, 2, 6, 3},
        {0, 1, 3, 7}
    };
    // Create sample data for Legacy NNS
    std::vector<double> flat_x;
    for (const auto& row : x) {
        flat_x.insert(flat_x.end(), row.begin(), row.end());
    }
    // Construct NumericMatrix for R (4x4) in column-major layout
    Rcpp::NumericMatrix r_x(4, 4);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            r_x(j, i) = x[i][j];  // ✅ Transpose: fix C++ (row-major) -> R (col-major)

    R["x"] = r_x; // Assign x to R -- ready for direct use in R

    // Compute cholesky_decomposition for NNS-C++
    vector<vector<double>> cpp_res = NNS::cholesky_decomposition(x);
    // Compute cholesky_decomposition for Legacy NNS
    Rcpp::NumericMatrix r_res = R.parseEval("cholesky_decomposition(x);");

    auto converted = numericMatrixToVector(r_res);
    //
    // Compare NNS-C++ results with Legacy NNS results

    REQUIRE(static_cast<std::size_t>(converted.size()) == cpp_res.size());
    REQUIRE(static_cast<std::size_t>(converted[0].size()) == cpp_res[0].size());

    // Compare two matrices element-wise
    for (std::size_t i = 0; i < cpp_res.size(); ++i) {
        for (std::size_t j = 0; j < cpp_res[i].size(); ++j) {
            INFO("Comparing matrix element at (" << i << ", " << j << ")");
            REQUIRE(Approx(converted[i][j]).margin(1e-10) == cpp_res[i][j]);
        }
    }
}

TEST_CASE("NNS::forward_substitution output matches that of the legacy NNS R code", "[mean]")
{
    // Get the R inside singleton
    auto &R = NNS_test::rinside::r();

    // Create sample data for NNS-C++
    using Scalar = double;
    using Matrix = std::vector<std::vector<Scalar>>;
    Matrix x = {
        {4, 0, 0},
        {2, 4, 0},
        {1, 1, 3}
    };
    
    // Construct NumericMatrix for R (3x3) in column-major layout
    Rcpp::NumericMatrix r_x(3, 3);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            r_x(i, j) = x[i][j];  // ✅ Transpose: fix C++ (row-major) -> R (col-major)

    R["x"] = r_x; // Assign x to R -- ready for direct use in R

    vector<double> b = {3, 5, 0};
    R["b"] = b; // Assign b to R -- ready for direct use in R

    // Compute cholesky_decomposition for NNS-C++
    std::vector<double> cpp_res = NNS::forward_substitution(x,b);
    // Compute cholesky_decomposition for Legacy NNS
    std::vector<double> r_res = R.parseEval("forward_substitution(x,b);");

    //
    // Compare NNS-C++ results with Legacy NNS results

    REQUIRE(static_cast<std::size_t>(r_res.size()) == cpp_res.size());

    // Compare two vectors element-wise
    for (std::size_t i = 0; i < cpp_res.size(); ++i) {
        INFO("Comparing vector element at index " << i);
        REQUIRE(Approx(r_res[i]).margin(1e-10) == cpp_res[i]);
    }
}

TEST_CASE("Version 2: NNS::forward_substitution output matches that of the legacy NNS R code", "[mean]")
{
    // Get the R inside singleton
    auto &R = NNS_test::rinside::r();

    // Create sample data for NNS-C++
    using Scalar = double;
    using Matrix = std::vector<std::vector<Scalar>>;
    Matrix x = {
        {4, 0, 0, 0},
        {2, 4, 0, 0},
        {1, 1, 3, 0},
        {2, 3, 1, 5}
    };
    // Create sample data for Legacy NNS
    
    // Construct NumericMatrix for R (4x4) in column-major layout
    Rcpp::NumericMatrix r_x(4, 4);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            r_x(i, j) = x[i][j];  // ✅ Transpose: fix C++ (row-major) -> R (col-major)

    R["x"] = r_x; // Assign x to R -- ready for direct use in R

    vector<double> b = {1,5,0,2};
    R["b"] = b; // Assign b to R -- ready for direct use in R

    // Compute cholesky_decomposition for NNS-C++
    std::vector<double> cpp_res = NNS::forward_substitution(x,b);
    // Compute cholesky_decomposition for Legacy NNS
    std::vector<double> r_res = R.parseEval("forward_substitution(x,b);");

    //
    // Compare NNS-C++ results with Legacy NNS results

    REQUIRE(static_cast<std::size_t>(r_res.size()) == cpp_res.size());

    // Compare two vectors element-wise
    for (std::size_t i = 0; i < cpp_res.size(); ++i) {
        INFO("Comparing vector element at index " << i);
        REQUIRE(Approx(r_res[i]).margin(1e-10) == cpp_res[i]);
    }
}


TEST_CASE("NNS::back_substitution output matches that of the legacy NNS R code", "[mean]")
{
    // Get the R inside singleton
    auto &R = NNS_test::rinside::r();

    // Create sample data for NNS-C++
    using Scalar = double;
    using Matrix = std::vector<std::vector<Scalar>>;
    Matrix x = {
        {4, 0, 0},
        {2, 4, 0},
        {1, 1, 3}
    };
    // Create sample data for Legacy NNS
    
    // Construct NumericMatrix for R (3x3) in column-major layout
    Rcpp::NumericMatrix r_x(3, 3);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            r_x(i, j) = x[i][j];  // ✅ Transpose: fix C++ (row-major) -> R (col-major)

    R["x"] = r_x; // Assign x to R -- ready for direct use in R

    vector<double> z = {3, 5, 1};
    R["z"] = z; // Assign b to R -- ready for direct use in R

    // Compute cholesky_decomposition for NNS-C++
    std::vector<double> cpp_res = NNS::back_substitution_L_transpose(x,z);
    // Compute cholesky_decomposition for Legacy NNS
    std::vector<double> r_res = R.parseEval("back_substitution(x,z);");

    //
    // Compare NNS-C++ results with Legacy NNS results

    REQUIRE(static_cast<std::size_t>(r_res.size()) == cpp_res.size());

    // Compare two vectors element-wise
    for (std::size_t i = 0; i < cpp_res.size(); ++i) {
        INFO("Comparing vector element at index " << i);
        REQUIRE(Approx(r_res[i]).margin(1e-10) == cpp_res[i]);
    }
}

TEST_CASE("Version 2: NNS::back_substitution output matches that of the legacy NNS R code", "[mean]")
{
    // Get the R inside singleton
    auto &R = NNS_test::rinside::r();

    // Create sample data for NNS-C++
    using Scalar = double;
    using Matrix = std::vector<std::vector<Scalar>>;
    Matrix x = {
        {4, 0, 0, 0},
        {2, 4, 0, 0},
        {1, 1, 3, 0},
        {2, 3, 1, 5}
    };
    // Create sample data for Legacy NNS

    // Construct NumericMatrix for R (4x4) in column-major layout
    Rcpp::NumericMatrix r_x(4, 4);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 4; ++j)
            r_x(i, j) = x[i][j];  // ✅ Transpose: fix C++ (row-major) -> R (col-major)

    R["x"] = r_x; // Assign x to R -- ready for direct use in R

    vector<double> z = {1,5,4,2};
    R["z"] = z; // Assign b to R -- ready for direct use in R

    // Compute cholesky_decomposition for NNS-C++
    std::vector<double> cpp_res = NNS::back_substitution_L_transpose(x,z);
    // Compute cholesky_decomposition for Legacy NNS
    std::vector<double> r_res = R.parseEval("back_substitution(x,z);");

    //
    // Compare NNS-C++ results with Legacy NNS results

    REQUIRE(static_cast<std::size_t>(r_res.size()) == cpp_res.size());

    // Compare two vectors element-wise
    for (std::size_t i = 0; i < cpp_res.size(); ++i) {
        INFO("Comparing vector element at index " << i);
        REQUIRE(Approx(r_res[i]).margin(1e-10) == cpp_res[i]);
    }
}

TEST_CASE("NNS::fast_lm_mult output matches that of the legacy NNS R code", "[mean]")
{
    // Get the R inside singleton
    auto &R = NNS_test::rinside::r();

    // Create sample data for NNS-C++
    using Scalar = double;
    using Matrix = std::vector<std::vector<Scalar>>;
    Matrix x = {
        {1, 1},
        {2, 4},
        {3, 9},
        {4, 16}
    };
    // Create sample data for Legacy NNS

    // Construct NumericMatrix for R (4x2) in column-major layout
    Rcpp::NumericMatrix r_x(4, 2);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 2; ++j)
            r_x(i, j) = x[i][j];  // ✅ Transpose: fix C++ (row-major) -> R (col-major)

    R["x"] = r_x; // Assign x to R -- ready for direct use in R

    vector<double> y = {7, 10, 13, 16};
    R["y"] = y; // Assign b to R -- ready for direct use in R

    LMResult cpp_res = NNS::fast_lm_mult(x, y);

    Rcpp::List r_res = R.parseEval("fast_lm_mult(x,y)");

    std::vector<double> r_coef = Rcpp::as<std::vector<double>>(r_res["coefficients"]);
    std::vector<double> r_fit  = Rcpp::as<std::vector<double>>(r_res["fitted.values"]);
    std::vector<double> r_resid = Rcpp::as<std::vector<double>>(r_res["residuals"]);
    double r_r2 = Rcpp::as<double>(r_res["r.squared"]);

    // Print coefficients
    std::cout << "C++ Coefficients: ";
    for (auto v : cpp_res.coefficients) std::cout << v << " ";
    std::cout << "\nR Coefficients:   ";
    for (auto v : r_coef) std::cout << v << " ";
    std::cout << "\n";

    // Print fitted values
    std::cout << "C++ Fitted Values: ";
    for (auto v : cpp_res.fitted_values) std::cout << v << " ";
    std::cout << "\nR Fitted Values:   ";
    for (auto v : r_fit) std::cout << v << " ";
    std::cout << "\n";

    // Print residuals
    std::cout << "C++ Residuals: ";
    for (auto v : cpp_res.residuals) std::cout << v << " ";
    std::cout << "\nR Residuals:   ";
    for (auto v : r_resid) std::cout << v << " ";
    std::cout << "\n";

    // Print R^2
    std::cout << "C++ R^2: " << cpp_res.r_squared << "\n";
    std::cout << "R R^2:   " << r_r2 << "\n";


    // Coefficients
    REQUIRE(r_coef.size() == cpp_res.coefficients.size());
    for (std::size_t i = 0; i < r_coef.size(); ++i) {
        REQUIRE(Approx(r_coef[i]).margin(1e-10) == cpp_res.coefficients[i]);
    }

    // Fitted values
    REQUIRE(r_fit.size() == cpp_res.fitted_values.size());
    for (std::size_t i = 0; i < r_fit.size(); ++i) {
        REQUIRE(Approx(r_fit[i]).margin(1e-10) == cpp_res.fitted_values[i]);
    }

    // Residuals
    REQUIRE(r_resid.size() == cpp_res.residuals.size());
    for (std::size_t i = 0; i < r_resid.size(); ++i) {
        REQUIRE(Approx(r_resid[i]).margin(1e-10) == cpp_res.residuals[i]);
    }

    // R^2
    REQUIRE(Approx(r_r2).margin(1e-10) == cpp_res.r_squared);

}

TEST_CASE("Version 2: NNS::fast_lm_mult output matches that of the legacy NNS R code", "[mean]")
{
    // Get the R inside singleton
    auto &R = NNS_test::rinside::r();

    // Create sample data for NNS-C++
    using Scalar = double;
    using Matrix = std::vector<std::vector<Scalar>>;
    Matrix x = {
        {1, 1, 6},
        {2, 4, 7},
        {3, 9, 2},
        {4, 16, 5}
    };
    // Create sample data for Legacy NNS

    // Construct NumericMatrix for R (4x3) in column-major layout
    Rcpp::NumericMatrix r_x(4, 3);
    for (int i = 0; i < 4; ++i)
        for (int j = 0; j < 3; ++j)
            r_x(i, j) = x[i][j];  // ✅ Transpose: fix C++ (row-major) -> R (col-major)

    R["x"] = r_x; // Assign x to R -- ready for direct use in R

    vector<double> y = {5, 12, 3, 6};
    R["y"] = y; // Assign b to R -- ready for direct use in R

    LMResult cpp_res = NNS::fast_lm_mult(x, y);

    Rcpp::List r_res = R.parseEval("fast_lm_mult(x,y)");

    std::vector<double> r_coef = Rcpp::as<std::vector<double>>(r_res["coefficients"]);
    std::vector<double> r_fit  = Rcpp::as<std::vector<double>>(r_res["fitted.values"]);
    std::vector<double> r_resid = Rcpp::as<std::vector<double>>(r_res["residuals"]);
    double r_r2 = Rcpp::as<double>(r_res["r.squared"]);

    // Print coefficients
    std::cout << "C++ Coefficients: ";
    for (auto v : cpp_res.coefficients) std::cout << v << " ";
    std::cout << "\nR Coefficients:   ";
    for (auto v : r_coef) std::cout << v << " ";
    std::cout << "\n";

    // Print fitted values
    std::cout << "C++ Fitted Values: ";
    for (auto v : cpp_res.fitted_values) std::cout << v << " ";
    std::cout << "\nR Fitted Values:   ";
    for (auto v : r_fit) std::cout << v << " ";
    std::cout << "\n";

    // Print residuals
    std::cout << "C++ Residuals: ";
    for (auto v : cpp_res.residuals) std::cout << v << " ";
    std::cout << "\nR Residuals:   ";
    for (auto v : r_resid) std::cout << v << " ";
    std::cout << "\n";

    // Print R^2
    std::cout << "C++ R^2: " << cpp_res.r_squared << "\n";
    std::cout << "R R^2:   " << r_r2 << "\n";


    // Coefficients
    REQUIRE(r_coef.size() == cpp_res.coefficients.size());
    for (std::size_t i = 0; i < r_coef.size(); ++i) {
        REQUIRE(Approx(r_coef[i]).margin(1e-10) == cpp_res.coefficients[i]);
    }

    // Fitted values
    REQUIRE(r_fit.size() == cpp_res.fitted_values.size());
    for (std::size_t i = 0; i < r_fit.size(); ++i) {
        REQUIRE(Approx(r_fit[i]).margin(1e-10) == cpp_res.fitted_values[i]);
    }

    // Residuals
    REQUIRE(r_resid.size() == cpp_res.residuals.size());
    for (std::size_t i = 0; i < r_resid.size(); ++i) {
        REQUIRE(Approx(r_resid[i]).margin(1e-10) == cpp_res.residuals[i]);
    }

    // R^2
    REQUIRE(Approx(r_r2).margin(1e-10) == cpp_res.r_squared);

}
