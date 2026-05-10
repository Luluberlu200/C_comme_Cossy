# GuardDrive — Comment ça fonctionne ?

---

## C'est quoi ?

Application web **mobile-first** qui permet à un conducteur de **surveiller son véhicule à distance** :
localisation GPS, état (batterie, carburant, pneus), alertes intelligentes, verrouillage.

---

## Architecture globale

```
┌──────────────────────────────────────────────────────────────┐
│                        UTILISATEUR                           │
│                     (navigateur / mobile)                    │
└──────────────────────┬───────────────────────────────────────┘
                       │  HTTP / REST API
                       │
         ┌─────────────▼──────────────┐
         │         FRONTEND           │
         │                            │
         │  React 19  +  TypeScript   │
         │  React Router v7  (SPA)    │
         │  Vite (build tool)         │
         │                            │
         │  Pages :                   │
         │  • Login / Register        │
         │  • Dashboard               │
         │  • Localisation (Leaflet)  │
         │  • État du véhicule        │
         │  • Alertes                 │
         │  • Paramètres              │
         └─────────────┬──────────────┘
                       │  fetch( /api/... )
                       │
         ┌─────────────▼──────────────┐
         │          BACKEND           │
         │                            │
         │  Node.js  +  Express 5     │
         │                            │
         │  Routes :                  │
         │  POST /api/auth/login      │
         │  GET  /api/vehicle         │
         │  GET  /api/alerts          │
         │                            │
         │  JWT  →  routes protégées  │
         └─────────────┬──────────────┘
                       │  Mongoose (ODM)
                       │
         ┌─────────────▼──────────────┐
         │       BASE DE DONNÉES      │
         │                            │
         │      MongoDB Atlas         │
         │         (cloud)            │
         │                            │
         │  Collections :             │
         │  • users                   │
         │  • vehicles                │
         │  • alerts                  │
         └────────────────────────────┘
```

---

## Technologies

### Frontend
| Technologie | Rôle |
|---|---|
| **React 19** | Composants UI réactifs |
| **TypeScript** | Typage statique |
| **Vite 7** | Build tool & serveur de dev |
| **React Router v7** | Navigation sans rechargement (SPA) |
| **Leaflet** | Carte GPS interactive |
| **CSS custom** | Thème dark/light, animations |

### Backend
| Technologie | Rôle |
|---|---|
| **Node.js** | Exécution JavaScript côté serveur |
| **Express 5** | Routing HTTP, middlewares |
| **Mongoose 9** | Interface avec MongoDB |
| **JWT** | Authentification par token (7 jours) |

### Infrastructure
| Technologie | Rôle |
|---|---|
| **MongoDB Atlas** | Base de données NoSQL cloud |
| **Vercel** | Hébergement frontend + API serverless |
| **GitHub** | Versioning (branches, PRs) |

### Tests
| Technologie | Rôle |
|---|---|
| **Vitest** | Tests unitaires |
| **Playwright** | Tests End-to-End |

---

## Flux d'une requête (exemple : login)

```
1.  Utilisateur saisit email + mot de passe
        │
2.  React envoie  POST /api/auth/login
        │
3.  Express reçoit la requête
    → vérifie les champs
    → cherche l'utilisateur dans MongoDB
        │
4.  JWT génère un token signé (7 jours)
        │
5.  Token renvoyé au frontend
    → stocké dans le contexte React (AuthContext)
        │
6.  Toutes les requêtes suivantes incluent :
    Authorization: Bearer <token>
        │
7.  Middleware backend vérifie le token
    → accès autorisé ou 401
```

---

## Authentification JWT

```
Frontend                        Backend
   │                               │
   │── POST /login ──────────────► │
   │   { email, password }         │── vérifie en DB
   │                               │── génère token
   │◄── { token, user } ──────────-│
   │                               │
   │── GET /api/vehicle ─────────► │
   │   Authorization: Bearer token │── décode token
   │                               │── renvoie données
   │◄── [ { vehicle data } ] ──── -│
```

---

## Ce qu'on a développé

- **11 branches** de fonctionnalités fusionnées via Pull Requests
- **10 Pull Requests** mergées sur `main`
- **58 commits** au total
- Tests unitaires (Vitest) + tests E2E (Playwright)
- Déploiement continu sur Vercel

---

## URL de production

**https://guarddrive.vercel.app**
