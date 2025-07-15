#include <cmath>

#include <Eigen/Dense>

namespace NNS
{

    enum class Tail
    {
        Lower,
        Upper
    };

    template <Tail T>
    inline double partial_moment(double degree,
                                 double target,
                                 const Eigen::VectorXd &v)
    {
        Eigen::ArrayXd sv;
        auto tV = Eigen::ArrayXd::Constant(v.size(), target);
        if constexpr (T == Tail::Lower)
            sv = tV - v.array();
        else // Upper
            sv = v.array() - tV;
        return sv.cwiseMax(0.0).pow(degree).mean();
    }

    template <Tail Tx, Tail Ty>
    inline double bi_partial_moment(double degree,
                                    const Eigen::VectorXd &x,
                                    const Eigen::VectorXd &y,
                                    double Tx_target,
                                    double Ty_target)
    {
        const std::size_t n = std::min<std::size_t>(x.size(), y.size());
        if (n == 0)
            return 0.0;

        // ----- signed deviations -------------------------------------------------
        auto TxV = Eigen::ArrayXd::Constant(n, Tx_target);
        auto TyV = Eigen::ArrayXd::Constant(n, Ty_target);

        Eigen::ArrayXd sx = x.head(n).array() - TxV;
        if (Tx == Tail::Lower)
            sx = -sx;

        Eigen::ArrayXd sy = y.head(n).array() - TyV;
        if (Ty == Tail::Lower)
            sy = -sy;

        sx = sx.cwiseMax(0.0);
        sy = sy.cwiseMax(0.0);

        if (degree == 0.0)
            return ((sx > 0.0).cast<double>() * (sy > 0.0).cast<double>()).mean();

        return (sx.pow(degree) * sy.pow(degree)).mean();
    }

    //----------------------------------------------
    // Thin wrappers (original API)
    //----------------------------------------------
    template <Tail T>
    inline double co_partial_moment(double k, const Eigen::VectorXd &x, const Eigen::VectorXd &y,
                                    double Tx, double Ty)
    {
        return bi_partial_moment<T, T>(k, x, y, Tx, Ty);
    }

    template <Tail xT, Tail yT>
    inline double divergent_partial_moment(double k, const Eigen::VectorXd &x, const Eigen::VectorXd &y,
                                           double Tx, double Ty)
    {
        return bi_partial_moment<xT, yT>(k, x, y, Tx, Ty);
    }

}