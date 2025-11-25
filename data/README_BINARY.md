# Datasets de Classification Binaire

Ces datasets contiennent des valeurs 0 et 1 pour la classification binaire.

## Datasets disponibles

### 1. `binary_classification.csv` (Recommandé)
- **Entrées** : 4 (x1, x2, x3, x4)
- **Sortie** : 1 (y: 0 ou 1)
- **Échantillons** : 30 (15 de classe 0, 15 de classe 1)
- **Format** : Avec en-têtes

**Utilisation** :
- Créer un réseau : **4 entrées → 5 neurones cachés → 1 sortie** (activation Sigmoid)
- Charger avec : 4 entrées, 1 sortie, avec en-têtes

---

### 2. `binary_count_dataset.csv`
- **Entrées** : 3 (feature1, feature2, feature3)
- **Sortie** : 1 (class: 0 ou 1)
- **Échantillons** : 26 (13 de classe 0, 13 de classe 1)
- **Format** : Avec en-têtes

**Utilisation** :
- Créer un réseau : **3 entrées → 4 neurones cachés → 1 sortie** (activation Sigmoid)
- Charger avec : 3 entrées, 1 sortie, avec en-têtes

---

### 3. `binary_large_dataset.csv`
- **Entrées** : 5 (x1, x2, x3, x4, x5)
- **Sortie** : 1 (output: 0 ou 1)
- **Échantillons** : 60 (20 de classe 0, 40 de classe 1)
- **Format** : Avec en-têtes

**Utilisation** :
- Créer un réseau : **5 entrées → 6 neurones cachés → 1 sortie** (activation Sigmoid)
- Charger avec : 5 entrées, 1 sortie, avec en-têtes

---

### 4. `binary_two_outputs.csv` (Pour 2 sorties)
- **Entrées** : 3 (x1, x2, x3)
- **Sorties** : 2 (class0, class1) - Format one-hot encoding
- **Échantillons** : 26 (13 de classe 0, 13 de classe 1)
- **Format** : Avec en-têtes

**Utilisation** :
- Créer un réseau : **3 entrées → 4 neurones cachés → 2 sorties** (activation Sigmoid)
- Charger avec : 3 entrées, 2 sorties, avec en-têtes
- **Note** : class0=1,class1=0 signifie classe 0 | class0=0,class1=1 signifie classe 1

---

## Comment compter les 0 et les 1

Après avoir testé le réseau :

1. Allez dans l'onglet **"Resultats"**
2. Dans la section **"Resultats de Classification"**, vous verrez un tableau avec :
   - **Valeur Reelle** : 0 ou 1
   - **Prediction** : 0 ou 1
   - **Statut** : Correct ou Incorrect

3. Comptez manuellement ou regardez les métriques :
   - **Accuracy** : Pourcentage de prédictions correctes
   - La **matrice de confusion** (onglet "Confusion") montre :
     - Nombre de 0 prédits comme 0
     - Nombre de 0 prédits comme 1
     - Nombre de 1 prédits comme 0
     - Nombre de 1 prédits comme 1

---

## Exemple d'utilisation

1. **Créer un réseau** : 4 → 5 → 1 (Sigmoid)
2. **Charger** : `binary_classification.csv` (4 entrées, 1 sortie, avec en-têtes)
3. **Entraîner** : 1000-2000 époques, learning rate 0.1
4. **Tester** : Cliquez sur "Tester le réseau"
5. **Voir les résultats** : Onglet "Resultats" pour voir le décompte

---

## Distribution des classes

- `binary_classification.csv` : 15 classe 0, 15 classe 1 (50/50)
- `binary_count_dataset.csv` : 13 classe 0, 13 classe 1 (50/50)
- `binary_large_dataset.csv` : 20 classe 0, 40 classe 1 (33/67)
- `binary_two_outputs.csv` : 13 classe 0, 13 classe 1 (50/50)

