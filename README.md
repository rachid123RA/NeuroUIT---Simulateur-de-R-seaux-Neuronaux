# NeuroUIT - Simulateur de Réseaux Neuronaux

**NeuroUIT** (Neuro + UI + Tool) est un simulateur de réseaux de neurones artificiels avec interface graphique, développé en C++ avec Qt.

## Auteur
Rachid ait aissa - M1 IAOC  
Université Ibn Tofail Kénitra

## Description

NeuroUIT permet de :
- Créer et configurer des réseaux de neurones (couches, neurones, fonctions d'activation)
- Charger des datasets au format CSV
- Entraîner des réseaux avec rétropropagation
- Visualiser le réseau et l'évolution de l'erreur
- Analyser les performances avec métriques détaillées
- Sauvegarder/charger des réseaux
- Exporter les résultats

## Prérequis

- **Qt 6.10.1** ou supérieur (Core, Widgets, Charts)
- **MinGW 13.1.0** ou compilateur C++17 compatible
- **Windows 10/11** (testé sur Windows)

## Compilation

### Méthode recommandée : Qt Creator

1. Ouvrez `NeuroUIT.pro` dans Qt Creator
2. Sélectionnez le kit "Desktop Qt 6.10.1 MinGW 64-bit"
3. Appuyez sur `Ctrl+B` pour compiler
4. Appuyez sur `Ctrl+R` pour exécuter

L'exécutable sera généré dans le dossier `bin/`.

## Structure du projet

```
NeuroUIT/
├── include/          # Headers
│   ├── core/        # Neuron, Layer, Network, ActivationFunction
│   ├── dataset/     # DatasetManager
│   ├── training/    # Trainer
│   ├── persistence/ # Sauvegarde/chargement
│   ├── ui/          # Interface graphique
│   └── controller/  # Contrôleur MVC
├── src/             # Implémentations
├── data/            # Datasets d'exemple
├── ui ux/           # Maquette HTML/CSS/JS originale
└── NeuroUIT.pro     # Fichier projet Qt
```

## Format des données

### Dataset CSV
Format : `input1,input2,...,output1,output2,...`
- Première ligne optionnelle : en-têtes
- Colonnes numériques uniquement
- Pas de valeurs manquantes

Exemple :
```csv
x1,x2,y
0,0,0
0,1,1
1,0,1
1,1,0
```

### Fichier réseau (.nui)
Format propriétaire JSON contenant :
- Architecture (couches, neurones)
- Fonctions d'activation
- Poids des connexions

## Utilisation

1. **Créer un réseau** : Menu → Nouveau réseau
2. **Configurer l'architecture** : Couches, neurones, fonctions d'activation
3. **Charger un dataset** : Menu → Charger dataset
4. **Configurer l'entraînement** : Taux d'apprentissage, epochs, etc.
5. **Lancer l'apprentissage** : Bouton "Lancer"
6. **Tester le réseau** : Bouton "Tester"
7. **Visualiser les résultats** : Onglets Métriques, Confusion, Prédictions, etc.

## Fonctionnalités

### Core
- ✅ Création/config réseau multicouche
- ✅ Fonctions d'activation (Sigmoid, Tanh, ReLU, Linear)
- ✅ Chargement CSV
- ✅ Entraînement (backpropagation avec momentum)
- ✅ Visualisation réseau
- ✅ Graphique d'évolution de l'erreur
- ✅ Sauvegarde/chargement (.nui)
- ✅ Export résultats

### Visualisation et Analyse
- ✅ Métriques de performance (Accuracy, Precision, Recall, F1-Score)
- ✅ Métriques de régression (MSE, MAE, R²)
- ✅ Matrice de confusion
- ✅ Matrice de corrélation
- ✅ Graphique prédictions vs réelles
- ✅ Statistiques détaillées du dataset

## Datasets d'exemple

Le dossier `data/` contient plusieurs datasets pour tester :
- `XOR_dataset.csv` - Problème XOR (classification binaire)
- `AND_dataset.csv` - Problème AND (classification binaire)
- `OR_dataset.csv` - Problème OR (classification binaire)
- `linear_regression.csv` - Régression linéaire
- `sine_wave.csv` - Fonction sinusoïdale
- `classification_simple.csv` - Classification avec 3 entrées

## Licence

Projet académique - Université Ibn Tofail Kénitra Rachid ait aissa - M1 IAOC
