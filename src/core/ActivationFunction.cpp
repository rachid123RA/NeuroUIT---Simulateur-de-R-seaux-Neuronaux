#include "core/ActivationFunction.h"
#include <cmath>
#include <algorithm>
#include <memory>

namespace NeuroUIT {

double SigmoidFunction::activate(double x) const {
    // Éviter l'overflow
    if (x > 700) return 1.0;
    if (x < -700) return 0.0;
    return 1.0 / (1.0 + std::exp(-x));
}

double SigmoidFunction::derivative(double x) const {
    // La dérivée de sigmoid: sigmoid(x) * (1 - sigmoid(x))
    double s = activate(x);
    return s * (1.0 - s);
}

double TanhFunction::activate(double x) const {
    return std::tanh(x);
}

double TanhFunction::derivative(double x) const {
    double th = std::tanh(x);
    return 1.0 - th * th;
}

double ReLUFunction::activate(double x) const {
    return std::max(0.0, x);
}

double ReLUFunction::derivative(double x) const {
    return x > 0.0 ? 1.0 : 0.0;
}

double LinearFunction::activate(double x) const {
    return x;
}

double LinearFunction::derivative(double x) const {
    (void)x; // Unused
    return 1.0;
}

std::unique_ptr<ActivationFunction> ActivationFunction::create(ActivationType type) {
    switch (type) {
        case ActivationType::Sigmoid:
            return std::make_unique<SigmoidFunction>();
        case ActivationType::Tanh:
            return std::make_unique<TanhFunction>();
        case ActivationType::ReLU:
            return std::make_unique<ReLUFunction>();
        case ActivationType::Linear:
            return std::make_unique<LinearFunction>();
        default:
            return std::make_unique<SigmoidFunction>();
    }
}

} // namespace NeuroUIT


