#include "ODESolver.h"
#include <cmath>
#include <algorithm>

// Dormand-Prince 8(7) a matrix coefficients
const double ODESolver::a[STAGES][STAGES] = {
    {0.0},
    {1.0 / 18.0},
    {1.0 / 48.0, 1.0 / 16.0},
    {1.0 / 32.0, 0.0, 3.0 / 32.0},
    {5.0 / 16.0, 0.0, -75.0 / 64.0, 75.0 / 64.0},
    {3.0 / 80.0, 0.0, 0.0, 3.0 / 16.0, 3.0 / 20.0},
    {29443841.0 / 614563906.0, 0.0, 0.0, 77736538.0 / 692538347.0, -28693883.0 / 1125000000.0, 23124283.0 / 1800000000.0},
    {16016141.0 / 946692911.0, 0.0, 0.0, 61564180.0 / 158732637.0, 22789713.0 / 633445777.0, 545815736.0 / 2771057229.0, -180193667.0 / 1043307555.0},
    {39632708.0 / 573591083.0, 0.0, 0.0, -433636366.0 / 683701615.0, -421739975.0 / 2616292301.0, 100302831.0 / 723423059.0, 790204164.0 / 839813087.0, 800635310.0 / 3783071287.0},
    {246121993.0 / 1340847787.0, 0.0, 0.0, -37695042795.0 / 15268766246.0, -309121744.0 / 1061227803.0, -12992083.0 / 490766935.0, 6005943493.0 / 2108947869.0, 393006217.0 / 1396673457.0, 123872331.0 / 1001029789.0},
    {-1028468189.0 / 846180014.0, 0.0, 0.0, 8478235783.0 / 508512852.0, 1311729495.0 / 1432422823.0, -10304129995.0 / 1701304382.0, -48777925059.0 / 3047939560.0, 15336726248.0 / 1032824649.0, -45442868181.0 / 3398467696.0, 3065993473.0 / 597172653.0},
    {185892177.0 / 718116043.0, 0.0, 0.0, -3185094517.0 / 667107341.0, -477755414.0 / 1098053517.0, -703635378.0 / 230739211.0, 5731566787.0 / 1027545527.0, 5232866602.0 / 850066563.0, -4093664535.0 / 808688257.0, 3962137247.0 / 1805957418.0, 65686358.0 / 487910083.0},
    {403863854.0 / 491063109.0, 0.0, 0.0, -5068492393.0 / 434740067.0, -411421997.0 / 543043805.0, 652783627.0 / 914296604.0, 11173962825.0 / 925320556.0, -13158990841.0 / 6184727034.0, 3936647629.0 / 1978049680.0, -160528059.0 / 685178525.0, 248638103.0 / 1413531060.0, 0.0}
};

ODESolver::ODESolver()
{
}

void ODESolver::initializeWorkspace(size_t stateSize)
{
    if (k.size() != STAGES || (k.size() > 0 && k[0].size() != stateSize)) {
        k.resize(STAGES);
        for (int i = 0; i < STAGES; ++i) {
            k[i].resize(stateSize);
        }
        tempState.resize(stateSize);
    }
}

double ODESolver::step(double t, std::vector<double>& state,
    DerivativeFunction derivFunc,
    double dt, double tolerance)
{
    initializeWorkspace(state.size());

    const size_t n = state.size();
    std::vector<double> state8(n);
    std::vector<double> state7(n);

    // Compute all 13 k values
    for (int stage = 0; stage < STAGES; ++stage) {
        // Compute temp state for this stage
        for (size_t i = 0; i < n; ++i) {
            tempState[i] = state[i];
            for (int j = 0; j < stage; ++j) {
                tempState[i] += dt * a[stage][j] * k[j][i];
            }
        }

        // Evaluate derivative
        double t_stage = t + c[stage] * dt;
        std::vector<double> deriv = derivFunc(t_stage, tempState);

        // Store k value
        for (size_t i = 0; i < n; ++i) {
            k[stage][i] = deriv[i];
        }
    }

    // Compute 8th order solution
    for (size_t i = 0; i < n; ++i) {
        state8[i] = state[i];
        for (int stage = 0; stage < STAGES; ++stage) {
            state8[i] += dt * b8[stage] * k[stage][i];
        }
    }

    // Compute 7th order solution for error estimation
    for (size_t i = 0; i < n; ++i) {
        state7[i] = state[i];
        for (int stage = 0; stage < STAGES; ++stage) {
            state7[i] += dt * b7[stage] * k[stage][i];
        }
    }

    // Estimate error
    double error = computeError(state8, state7);

    // Accept step if error is small enough
    if (error < tolerance || dt < 1e-10) {
        state = state8;
        return dt;
    }
    else {
        // Reject step and try smaller dt
        double safety = 0.9;
        double newDt = safety * dt * std::pow(tolerance / error, 1.0 / 8.0);
        newDt = std::max(newDt, dt * 0.2);  // Don't shrink too much
        return step(t, state, derivFunc, newDt, tolerance);
    }
}

void ODESolver::stepFixed(double t, std::vector<double>& state,
    DerivativeFunction derivFunc, double dt)
{
    initializeWorkspace(state.size());

    const size_t n = state.size();

    // Compute all 13 k values
    for (int stage = 0; stage < STAGES; ++stage) {
        for (size_t i = 0; i < n; ++i) {
            tempState[i] = state[i];
            for (int j = 0; j < stage; ++j) {
                tempState[i] += dt * a[stage][j] * k[j][i];
            }
        }

        double t_stage = t + c[stage] * dt;
        std::vector<double> deriv = derivFunc(t_stage, tempState);

        for (size_t i = 0; i < n; ++i) {
            k[stage][i] = deriv[i];
        }
    }

    // Apply 8th order solution
    for (size_t i = 0; i < n; ++i) {
        for (int stage = 0; stage < STAGES; ++stage) {
            state[i] += dt * b8[stage] * k[stage][i];
        }
    }
}

double ODESolver::computeError(const std::vector<double>& state8,
    const std::vector<double>& state7)
{
    double error = 0.0;
    for (size_t i = 0; i < state8.size(); ++i) {
        double diff = std::abs(state8[i] - state7[i]);
        error = std::max(error, diff);
    }
    return error;
}