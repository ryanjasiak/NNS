#pragma once

#include <cmath>
#include <ranges>
#include <type_traits>
#include <concepts>

#include "../core/common.hpp"

namespace NNS
{
    enum class Tail
    {
        Lower,
        Upper
    };
    template <Tail T, Real N>
    inline double pm_single_(double degree, double target, N n)
    {
        double val;
        if constexpr (T == Tail::Upper)
            val = n - target;
        else
            val = target - n;
        return (val < 0.0) ? 0.0 : std::pow(val, degree);
    }
    template <Tail T, RealRange R>
    inline double partial_moment_natty(double degree, double target, const R &v)
    {
        double n = static_cast<double>(std::ranges::size(v));
        if (n == 0.0)
            return 0.0;
        double acc = 0.0;
        for (const auto &i : v)
            acc += pm_single_<T>(degree, target, i);
        return acc / n;
    }
    template <Tail Tx, Tail Ty, RealRange R>
    inline double bi_partial_moment(double degree_x, double degree_y, const R &x, const R &y, double x_targ, double y_targ)
    {
        double n = static_cast<double>(std::max(std::ranges::size(x), std::ranges::size(y)));
        if (n == 0.0)
            return 0.0;
        double acc = 0;
        for (const auto [ix, iy] : std::views::zip(x, y))
        {
            acc += pm_single_<Tx>(degree_x, x_targ, ix) * pm_single_<Ty>(degree_y, y_targ, iy);
        }
        return acc / n;
    }

    //
    ////////////////////////////////////////////////////
    // Partial Moments

    template <RealRange R>
    inline double LPM(double d, const R &x, double t)
    {
        return partial_moment_natty<Tail::Lower>(d, t, x);
    }
    template <RealRange R>
    inline double UPM(double d, const R &x, double t)
    {
        return partial_moment_natty<Tail::Upper>(d, t, x);
    }

    //
    ////////////////////////////////////////////////////
    // Co-Partial Moments

    template <RealRange R>
    inline double Co_LPM(double d, const R &x, const R &y, double t_x, double t_y)
    {
        return bi_partial_moment<Tail::Lower, Tail::Lower>(d, d, x, y, t_x, t_y);
    }
    template <RealRange R>
    inline double Co_UPM(double d, const R &x, const R &y, double t_x, double t_y)
    {
        return bi_partial_moment<Tail::Upper, Tail::Upper>(d, d, x, y, t_x, t_y);
    }

    //
    ////////////////////////////////////////////////////
    // Divergent-Partial Moments

    template <RealRange R>
    inline double D_LPM(double d_l, double d_u, const R &x, const R &y, double t_x, double t_y)
    {
        return bi_partial_moment<Tail::Upper, Tail::Lower>(d_u, d_l, x, y, t_x, t_y);
    }
    template <RealRange R>
    inline double D_UPM(double d_l, double d_u, const R &x, const R &y, double t_x, double t_y)
    {
        return bi_partial_moment<Tail::Lower, Tail::Upper>(d_l, d_u, x, y, t_x, t_y);
    }
}