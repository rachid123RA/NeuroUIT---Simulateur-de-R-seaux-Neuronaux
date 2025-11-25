#ifndef ACTIVATIONFUNCTION_H
#define ACTIVATIONFUNCTION_H

#include <functional>
#include <string>
#include <vector>
#include <memory>

namespace NeuroUIT {

enum class ActivationType {
    Sigmoid,
    Tanh,
    ReLU,
    Linear
};

/**
 * @brief Interface pour les fonctions d'activation
 * Pattern Strategy pour permettre différentes fonctions d'activation
 */
class ActivationFunction {
public:
    virtual ~ActivationFunction() = default;
    
    /**
     * @brief Calcule la fonction d'activation
     * @param x Valeur d'entrée
     * @return Valeur activée
     */
    virtual double activate(double x) const = 0;
    
    /**
     * @brief Calcule la dérivée de la fonction d'activation
     * @param x Valeur d'entrée ou valeur activée (selon l'implémentation)
     * @return Dérivée
     */
    virtual double derivative(double x) const = 0;
    
    /**
     * @brief Nom de la fonction
     */
    virtual std::string getName() const = 0;
    
    /**
     * @brief Crée une fonction d'activation à partir du type
     */
    static std::unique_ptr<ActivationFunction> create(ActivationType type);
};

/**
 * @brief Fonction sigmoïde: f(x) = 1 / (1 + exp(-x))
 */
class SigmoidFunction : public ActivationFunction {
public:
    double activate(double x) const override;
    double derivative(double x) const override;
    std::string getName() const override { return "Sigmoid"; }
};

/**
 * @brief Fonction Tanh: f(x) = tanh(x)
 */
class TanhFunction : public ActivationFunction {
public:
    double activate(double x) const override;
    double derivative(double x) const override;
    std::string getName() const override { return "Tanh"; }
};

/**
 * @brief Fonction ReLU: f(x) = max(0, x)
 */
class ReLUFunction : public ActivationFunction {
public:
    double activate(double x) const override;
    double derivative(double x) const override;
    std::string getName() const override { return "ReLU"; }
};

/**
 * @brief Fonction linéaire: f(x) = x
 */
class LinearFunction : public ActivationFunction {
public:
    double activate(double x) const override;
    double derivative(double x) const override;
    std::string getName() const override { return "Linear"; }
};

} // namespace NeuroUIT

#endif // ACTIVATIONFUNCTION_H


