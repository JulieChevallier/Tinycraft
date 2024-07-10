# Tinycraft
A little minecraft for my own pleasure

Ce que j'ai fais :
- Génération procédurale algorithme (Bruit de Perlin)
- Utilisation de hashmap pour l'optimisation
- Utilisation de Culling/Frustum Culling pour l'optimisation
- Utilisation d'un gestionnaire de texture pour l'optimisation afin de ne pas générer la texture d'un bloc à chaque création de bloc
- Gestion de "chunk" 16*16 avec génération de map progressive en fonction des déplacements de la caméra
- Différent types de blocs (herbe, terre, pierre, sable, eau, bois, feuillage) avec du polymorphisme
- Génération d'arbres (A corriger)
- Déplacement de la caméra
- Plusieurs Biomes (îles et montagnes)
- Mise en place d'un système de coordonnées visuelle

A faire :
- Multithreading
- Mettre en place une minimap pour mieux voir les effets du Bruit de Perlin
- Biomes générés nativement
- Intéraction joueur : Casser/Poser, Déplacement mode "Survie" avec la gestion des colisions
- Gestion du chargement des chunks, en affichant uniquement les chunks pouvant être à porter et pas tout ceux générés
- Réorganiser tout le code
