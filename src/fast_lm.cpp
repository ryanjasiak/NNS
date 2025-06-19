
#include <vector>
#include <numeric>

using std::sqrt;
using std::vector;

namespace NNS
{
    template <typename Scalar>
    double mean(const std::vector<Scalar> &v)
    {
        if (v.empty())
            return 0.0;
        double sum = std::accumulate(v.begin(), v.end(), 0.0);
        return sum / v.size();
    }
    class FastLmResult
    {
        vector<double> coef;
        vector<double> fitted_values;
        FastLmResult(vector<double> c, vector<double> f) : coef(c), fitted_values(f) {}
    };

    template <typename Scalar>
    vector<double> fast_lm(const vector<Scalar> &x, const vector<Scalar> &y)
    {
        double mean_x = 0.0, mean_y = 0.0;
        for (int i = 0; i < n; ++i)
        {
            mean_x += x[i];
            mean_y += y[i];
        }
        mean_x /= n;
        mean_y /= n;

        double b1 = 0;
        double b0 = 0;
        double numerator = 0;
        double denominator = 0;
        for (int i = 0; i < n; ++i)
        {
            numerator += (x[i] - mean_x) * (y[i] - mean_y);
            denominator += (x[i] - mean_x) * (x[i] - mean_x);
        }
        b1 = numerator / denominator;
        b0 = mean_y - b1 * mean_x;
        vector<double> fitted_values(n);

        for (int i = 0; i < n; ++i)
        {
            fitted_values[i] = b0 + b1 * x[i];
        }
        vector<double> coef = {b0, b1};
        return FastLmResult(coef, fitted_values);
    }

    template <typename Scalar>
    vector<vector<double>> cholesky_decomposition(const vector<vector<Scalar>> &A)
    {
        int n = A.size();
        vector<vector<double>> L(n, vector<double>(n, 0.0));

        for (int i = 0; i < n; ++i)
        {
            // Compute L(i, i)
            double sum = A[i][i];
            for (int k = 0; k < i; ++k)
            {
                sum -= L[i][k] * *2;
            }
            L[i][i] = sqrt(sum > 0 ? sum : 0); // Ensure non-negative for stability

            // Compute L(j, i) for j > i
            for (int j = i + 1; j < n; ++j)
            {
                sum = A(j, i);
                for (int k = 0; k < i; ++k)
                {
                    sum -= L[j][k] * L[i][k];
                }
                L[j][i] = sum / L[i][i];
            }
        }
        return L;
    }

    template <typename Scalar>
    vector<double> forward_substitution(const vector<vector<Scalar>> &L, const vector<Scalar> &b)
    {
        int n = L.size();
        vector<double> z(n);
        for (int i = 0; i < n; ++i)
        {
            double sum = b[i];
            for (int j = 0; j < i; j++)
            {
                sum -= L[i][j] * z[j];
            }
            z[i] = sum / L[i][i];
        }
        return z;
    }

    template <typename Scalar>
    vector<double> back_substitution(const vector<vector<Scalar>> &L, const vector<Scalar> &z)
    {
        int n = L.size();
        vector<double> x(n);
        for (int i = n - 1; i >= 0; --i)
        {
            double sum = z[i];
            for (int j = i + 1; j < n; ++j)
            {
                sum -= L[i][j] * x[j];
            }
            x[i] = sum / L[i][i];
        }
        return x;
    }

    template <typename Scalar>
    vector<double> fast_lm_mult(const vector<vector<Scalar>> &x, const vector<Scalar> &y)
    {
        int n = x.size();
        int p = 0;
        if (n > 0)
        {
            p = x[0].size();
        }
        vector<vector<double>> X(n, vector<double>(p + 1, 0.0));
        for (int i = 0; i < n; i++)
        {
            X[i][0] = 1;
            for (int j = 0; j < p; j++)
            {
                X[i][j + 1] = x[i][j];
            }
        }

        vector<vector<double>> XtX(p + 1, vector<double>(p, 0.0));
        vector<double> Xty(p + 1);
        for (int i = 0; i < p; i++)
        {
            for (int j = 0; j < p; j++)
            {
                double sum = 0;
                for (int k = 0; k < n; k++)
                {
                    sum += X[k][i] * X[k][j];
                }
                XtX[i][j] = sum;
            }
            double sum = 0;
            for (int k = 0; k < n; k++)
            {
                sum += X[k][i] * y[k];
            }
            Xty[i] = sum;
        }
        vector<vector<double>> L = cholesky_decomposition(XtX);

        vector<double> z = forward_substitution(L, Xty);

        vector<double> coef = back_substitution(L, z);

        vector<double> fitted_values(n);
        for (int i = 0; i < n; i++)
        {
            double sum = 0;
            for (int j = 0; j < p; j++)
            {
                sum += coef[j] * X[i][j];
            }
            fitted_values[i] = sum;
        }

        vector<double> residuals(n);
        for (int i = 0; i < n; i++)
        {
            residuals[i] = y[i] - fitted_values[i];
        }

        double y_mean = mean(y);
    }
}
