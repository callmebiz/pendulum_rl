#pragma once

#include <functional>
#include <vector>

/**
 * ODESolver - Dormand-Prince 8(7) Runge-Kutta solver
 *
 * High-accuracy adaptive ODE solver used in the chaos video.
 * 8th order solution with 7th order error estimation.
 */
class ODESolver
{
public:
    // State derivative function type: f(t, state) -> derivative
    using DerivativeFunction = std::function<std::vector<double>(double, const std::vector<double>&)>;

    ODESolver();

    /**
     * Take one adaptive step using Dormand-Prince 8(7)
     *
     * @param t Current time
     * @param state Current state vector
     * @param derivFunc Function that computes derivatives
     * @param dt Suggested time step (will be adjusted)
     * @param tolerance Error tolerance for adaptive stepping
     * @return Actual time step taken
     */
    double step(double t, std::vector<double>& state,
        DerivativeFunction derivFunc,
        double dt, double tolerance = 1e-8);

    /**
     * Fixed step (no adaptation) - useful for consistent frame timing
     */
    void stepFixed(double t, std::vector<double>& state,
        DerivativeFunction derivFunc, double dt);

private:
    // Dormand-Prince 8(7) Butcher tableau coefficients
    static constexpr int STAGES = 13;

    // Time coefficients (c values)
    static constexpr double c[STAGES] = {
        0.0, 1.0 / 18.0, 1.0 / 12.0, 1.0 / 8.0, 5.0 / 16.0, 3.0 / 8.0, 59.0 / 400.0,
        93.0 / 200.0, 5490023248.0 / 9719169821.0, 13.0 / 20.0, 1201146811.0 / 1299019798.0,
        1.0, 1.0
    };

    // Integration weights for 8th order solution (b values)
    static constexpr double b8[STAGES] = {
        14005451.0 / 335480064.0, 0.0, 0.0, 0.0, 0.0,
        -59238493.0 / 1068277825.0, 181606767.0 / 758867731.0,
        561292985.0 / 797845732.0, -1041891430.0 / 1371343529.0,
        760417239.0 / 1151165299.0, 118820643.0 / 751138087.0,
        -528747749.0 / 2220607170.0, 1.0 / 4.0
    };

    // Integration weights for 7th order solution (b* values) for error estimation
    static constexpr double b7[STAGES] = {
        13451932.0 / 455176623.0, 0.0, 0.0, 0.0, 0.0,
        -808719846.0 / 976000145.0, 1757004468.0 / 5645159321.0,
        656045339.0 / 265891186.0, -3867574721.0 / 1518517206.0,
        465885868.0 / 322736535.0, 53011238.0 / 667516719.0, 2.0 / 45.0, 0.0
    };

    // a matrix coefficients (how k values combine)
    static const double a[STAGES][STAGES];

    // Workspace for k values
    std::vector<std::vector<double>> k;
    std::vector<double> tempState;

    void initializeWorkspace(size_t stateSize);
    double computeError(const std::vector<double>& state8,
        const std::vector<double>& state7);
};