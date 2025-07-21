
#pragma once

#include <cmath>
#include <vector>
#include <algorithm>
#include <cstddef>
#include <iostream>
#ifdef _OPENMP
#include <omp.h>
#endif

namespace NNS_P
{
    enum class Tail
    {
        Lower,
        Upper
    };

    template <typename T>
    struct VectorView
    {
        const T *data;
        std::size_t size;

        inline T operator[](std::size_t i) const { return data[i]; }
    };

    template <Tail T, typename TFloat>
    inline TFloat partial_moment(TFloat degree,
                                 TFloat target,
                                 VectorView<TFloat> v)
    {
        TFloat sum = 0.0;

#pragma omp parallel for reduction(+ : sum)
        for (std::size_t i = 0; i < v.size; ++i)
        {
            TFloat delta = (T == Tail::Lower) ? target - v[i] : v[i] - target;
            delta = std::max(delta, TFloat(0));
            sum += std::pow(delta, degree);
        }

        return (v.size > 0) ? sum / static_cast<TFloat>(v.size) : 0.0;
    }

    template <Tail Tx, Tail Ty, typename TFloat>
    inline TFloat bi_partial_moment(TFloat degree,
                                    VectorView<TFloat> x,
                                    VectorView<TFloat> y,
                                    TFloat Tx_target,
                                    TFloat Ty_target)
    {
        const std::size_t n = std::min(x.size, y.size);
        if (n == 0)
            return 0.0;

        TFloat acc = 0.0;

#pragma omp parallel for reduction(+ : acc)
        for (std::size_t i = 0; i < n; ++i)
        {
            TFloat dx = (Tx == Tail::Lower) ? Tx_target - x[i] : x[i] - Tx_target;
            TFloat dy = (Ty == Tail::Lower) ? Ty_target - y[i] : y[i] - Ty_target;

            dx = std::max(dx, TFloat(0));
            dy = std::max(dy, TFloat(0));

            if (degree == TFloat(0))
                acc += (dx > 0 && dy > 0) ? TFloat(1.0) : TFloat(0.0);
            else
                acc += std::pow(dx, degree) * std::pow(dy, degree);
        }

        return acc / static_cast<TFloat>(n);
    }

    // Thin wrappers (backward-compatible)
    template <Tail T, typename TFloat>
    inline TFloat co_partial_moment(TFloat k, VectorView<TFloat> x, VectorView<TFloat> y,
                                    TFloat Tx, TFloat Ty)
    {
        return bi_partial_moment<T, T>(k, x, y, Tx, Ty);
    }

    template <Tail xT, Tail yT, typename TFloat>
    inline TFloat divergent_partial_moment(TFloat k, VectorView<TFloat> x, VectorView<TFloat> y,
                                           TFloat Tx, TFloat Ty)
    {
        return bi_partial_moment<xT, yT>(k, x, y, Tx, Ty);
    }

    // Adapter for Eigen if needed externally (non-core)
#ifdef EIGEN_WORLD_VERSION
#include <Eigen/Dense>
    inline VectorView<double> make_view(const Eigen::VectorXd &v)
    {
        return {v.data(), static_cast<std::size_t>(v.size())};
    }
#endif

} // namespace NNS
