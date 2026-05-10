# Documentation des Tests — GuardDrive

## Vue d'ensemble

Le projet GuardDrive intègre deux niveaux de tests :

| Type | Framework | Fichiers | Cas de test |
|------|-----------|----------|-------------|
| Tests unitaires | Vitest + jsdom | 1 | 29 |
| Tests E2E | Playwright | 3 | 24 |
| **Total** | | **4** | **53** |

---

## Configuration

### Vitest (tests unitaires)

Configuré dans [vite.config.ts](vite.config.ts) :
- Environnement : `jsdom` (simulation du DOM navigateur)
- Globals activés (pas besoin d'importer `describe`, `it`, `expect`)
- Fichier de setup : [src/\_\_tests\_\_/setup.ts](src/__tests__/setup.ts) — charge `@testing-library/jest-dom` pour les matchers DOM

### Playwright (tests E2E)

Configuré dans [playwright.config.ts](playwright.config.ts) :
- URL de base : `http://localhost:8000`
- Répertoire de tests : `./e2e`
- Timeout par test : 30 secondes
- Screenshots automatiques en cas d'échec
- Navigateur : Desktop Chrome
- Serveur de dev démarré automatiquement avant les tests (`npm run start`)

---

## Tests Unitaires

### Fichier : [src/\_\_tests\_\_/helpers.test.ts](src/__tests__/helpers.test.ts)

Ces tests couvrent les fonctions utilitaires pures du projet : calcul de couleurs d'état, formatage de dates, et labellisation des alertes.

---

#### `tireWearColor` — Couleur selon l'usure des pneus

| Scénario | Valeur | Couleur attendue |
|----------|--------|-----------------|
| Usure critique | 0%, 29% | Rouge |
| Usure modérée | 30%, 59% | Orange |
| Usure acceptable | 60%, 100% | Vert |

**Logique :** En dessous de 30 % d'usure restante, le pneu est dangereux. Entre 30 et 59 %, c'est une alerte. À partir de 60 %, l'état est bon.

---

#### `pressureColor` — Couleur selon la pression des pneus

| Scénario | Valeur | Couleur attendue |
|----------|--------|-----------------|
| Pression dangereusement basse | 1,5 bar / 1,79 bar | Rouge |
| Pression dangereusement haute | 3,3 bar / 4,0 bar | Rouge |
| Pression légèrement basse | 1,8 bar / 1,99 bar | Orange |
| Pression légèrement haute | 2,9 bar / 3,2 bar | Orange |
| Pression normale | 2,0 bar / 2,5 bar / 2,8 bar | Vert |

**Logique :** La plage normale est 2,0 – 2,8 bar. En dehors de 1,8 – 3,2 bar, c'est dangereux (rouge).

---

#### `fuelColor` — Couleur selon le niveau de carburant

| Scénario | Valeur | Couleur attendue |
|----------|--------|-----------------|
| Réservoir quasi vide | 0%, 19% | Rouge |
| Réservoir bas | 20%, 39% | Orange |
| Réservoir suffisant | 40%, 100% | Vert |

---

#### `batteryColor` — Couleur selon le niveau de batterie

| Scénario | Valeur | Couleur attendue |
|----------|--------|-----------------|
| Batterie critique | 0%, 19% | Rouge |
| Batterie faible | 20%, 49% | Orange |
| Batterie correcte | 50%, 100% | Vert |

---

#### `fmtDate` — Formatage de date en français

| Scénario | Entrée | Sortie attendue |
|----------|--------|-----------------|
| Chaîne vide | `""` | `"—"` |
| Date ISO valide | `"2025-01-15"` | Date formatée en locale FR (ex. `15 janvier 2025`) |
| Date invalide | `"not-a-date"` | La chaîne brute telle quelle |

---

#### `daysUntil` — Nombre de jours jusqu'à une date (horloge simulée au 01/06/2025)

| Scénario | Entrée | Résultat attendu |
|----------|--------|-----------------|
| Chaîne vide | `""` | `null` |
| Date future | `"2025-06-10"` | `9` |
| Aujourd'hui | `"2025-06-01"` | `0` |
| Date passée | `"2025-05-25"` | `-7` |

**Note technique :** utilise `vi.useFakeTimers` pour figer l'horloge et rendre les tests déterministes.

---

#### `weatherEmoji` — Emoji météo selon le code WMO

| Scénario | Code(s) | Emoji attendu |
|----------|---------|--------------|
| Ciel dégagé | 0 | ☀️ |
| Partiellement nuageux | 1, 2 | 🌤️ |
| Nuageux | 3 | ☁️ |
| Brouillard | 10, 48 | 🌫️ |
| Pluie | 61 | 🌧️ |
| Neige | 71 | ❄️ |
| Averses | 80 | 🌦️ |
| Orage | 95 | ⛈️ |
| Code hors plage | 100 | 🌡️ |

---

#### `labelAlertType` — Label textuel des types d'alerte

| Scénario | Type | Label attendu |
|----------|------|--------------|
| Alerte warning | `"warning"` | `"Avertissement"` |
| Alerte intrusion | `"intrusion"` | `"Intrusion"` |
| Alerte info | `"info"` | `"Info"` |
| Type inconnu | `"xyz"` | `"Info"` |

---

## Tests E2E (End-to-End)

Les tests E2E simulent un vrai utilisateur dans un vrai navigateur Chrome. Ils vérifient que les pages s'affichent correctement, que la navigation fonctionne, et que les interactions utilisateur (clic, saisie, toggle...) produisent les bons effets visuels.

---

### Fichier : [e2e/auth.spec.ts](e2e/auth.spec.ts) — Page d'authentification

---

#### Scénario 1 : Redirection de la racine vers `/login`

**Étapes :**
1. L'utilisateur accède à `/`

**Résultat attendu :** Redirection automatique vers `/login`.

---

#### Scénario 2 : Affichage du formulaire de connexion

**Étapes :**
1. Accéder à `/login`

**Résultat attendu :** Les champs `#email`, `#password` et le bouton de soumission sont visibles.

---

#### Scénario 3 : Bouton de connexion désactivé si champs vides

**Étapes :**
1. Accéder à `/login` sans rien saisir

**Résultat attendu :** Le bouton submit est désactivé (`disabled`).

---

#### Scénario 4 : Erreur sur format d'email invalide

**Étapes :**
1. Saisir `not-an-email` dans le champ email
2. Tenter de soumettre

**Résultat attendu :** Un message d'erreur contenant le mot `"email"` apparaît dans `.auth-form__error`.

---

#### Scénario 5 : Erreur sur identifiants incorrects

**Étapes :**
1. Saisir `wrong@example.com` et `wrongpassword`
2. Soumettre le formulaire

**Résultat attendu :** Message `"Identifiants invalides"` affiché.

---

#### Scénario 6 : Toggle de visibilité du mot de passe

**Étapes :**
1. Cliquer sur `.auth-form__toggle`

**Résultat attendu :** Le champ password passe du type `password` au type `text` (et vice-versa).

---

#### Scénario 7 : Lien vers la page d'inscription

**Étapes :**
1. Vérifier que le lien vers `/register` est visible
2. Cliquer dessus

**Résultat attendu :** Navigation vers `/register`.

---

### Fichier : [e2e/dashboard.spec.ts](e2e/dashboard.spec.ts) — Tableau de bord

**Setup commun :** Avant chaque test, connexion avec les credentials de test et attente de la navigation vers `/dashboard`.

---

#### Scénario 1 : Sélecteur de véhicule visible

**Résultat attendu :** L'élément `.db-dropdown__trigger` est visible.

---

#### Scénario 2 : Bouton verrouillage visible

**Résultat attendu :** L'élément `.db-lock__badge` est visible.

---

#### Scénario 3 : Toggle verrouillage/déverrouillage

**Étapes :**
1. Cliquer sur `.db-lock__badge`
2. Attendre 1,5 seconde

**Résultat attendu :** Le texte du badge change entre `"Verrouillé"` et `"Déverrouillé"`.

---

#### Scénario 4 : Jauges carburant et batterie

**Résultat attendu :** Exactement 2 éléments `.db-gauge-card` sont présents.

---

#### Scénario 5 : Section climatisation visible

**Résultat attendu :** L'élément `.db-climate` est visible.

---

#### Scénario 6 : Boutons de température de la climatisation

**Étapes :**
1. Lire la valeur initiale de température
2. Cliquer sur le bouton `+` (`.db-climate__step`)

**Résultat attendu :** La valeur de température affichée change.

---

#### Scénario 7 : Section alertes avec bouton "Voir tout"

**Résultat attendu :** Les éléments `.db-alerts` et `.db-alerts__more` sont visibles.

---

#### Scénario 8 : Navigation vers `/alertes` via le bouton "Voir tout"

**Étapes :**
1. Cliquer sur `.db-alerts__more`

**Résultat attendu :** Navigation vers `/alertes`.

---

#### Scénario 9 : Dropdown de sélection de véhicule

**Étapes :**
1. Cliquer sur `.db-dropdown__trigger`

**Résultat attendu :** `.db-dropdown__menu` est visible et contient au moins 1 véhicule.

---

#### Scénario 10 : Fermeture du dropdown au clic extérieur

**Étapes :**
1. Ouvrir le dropdown
2. Cliquer sur `.db-page` (zone extérieure)

**Résultat attendu :** `.db-dropdown__menu` se ferme (état `hidden`).

---

### Fichier : [e2e/navigation.spec.ts](e2e/navigation.spec.ts) — Navigation et pages

---

#### Suite "Navigation" — Accès aux routes principales

| Scénario | Route cible | Vérification |
|----------|-------------|--------------|
| Accès alertes | `/alertes` | URL correcte, pas de 404 |
| Accès localisation | `/localisation` | URL correcte, pas de 404 |
| Accès état véhicule | `/etat-vehicule` | URL correcte, pas de 404 |
| Accès paramètres | `/settings` | URL correcte, pas de 404 |
| Route inconnue | `/unknown-page-xyz` | Redirigé vers `/login` ou `/dashboard` |

---

#### Suite "Alerts page" — Page des alertes (après connexion)

**Scénario 1 : Affichage de la liste ou état vide**

**Étapes :**
1. Se connecter et naviguer vers `/alertes`
2. Attendre la disparition des skeletons (jusqu'à 10 secondes)

**Résultat attendu :** Soit `.al-list` (avec des alertes), soit `.al-empty` (aucune alerte) est visible.

**Scénario 2 : Boutons de filtres visibles**

**Résultat attendu :** L'élément `.al-filters` est visible.

---

#### Suite "Etat Vehicule page" — Page état du véhicule (après connexion)

**Scénario 1 : Section identité du véhicule**

**Résultat attendu :** La première `.ev-card` est visible.

**Scénario 2 : Carte des pneus**

**Résultat attendu :** `.ev-tire-map` est visible et contient exactement **4** éléments `.ev-tire-card` (un par roue).

**Scénario 3 : Section documents**

**Résultat attendu :** L'élément `.ev-docs` est visible.

---

## Lancer les tests

```bash
# Tests unitaires
npm run test

# Tests unitaires avec interface graphique
npm run test:ui

# Tests E2E
npm run test:e2e

# Tests E2E avec interface Playwright
npm run test:e2e -- --ui
```

---

## Arborescence des tests

```
GuardDrive/
├── src/
│   └── __tests__/
│       ├── setup.ts           # Configuration jest-dom
│       └── helpers.test.ts    # 29 tests unitaires
└── e2e/
    ├── auth.spec.ts           # 7 tests E2E — authentification
    ├── dashboard.spec.ts      # 10 tests E2E — tableau de bord
    └── navigation.spec.ts     # 7 tests E2E — navigation & pages
```
