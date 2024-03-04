# Projet d'Interface de Mesure pour Calculateur de Voiture

Ce projet vise à développer une interface de mesure entre un calculateur de voiture et l'être humain. L'objectif principal est de gérer jusqu'à 4 signaux d'injection pour une première carte, tandis que pour une deuxième carte, l'interface doit gérer les signaux de PMH (Point Mort Haut), l'écart en degré sur l'avance à l'allumage, le temps de conduction et la valeur d'accélération.

## Configuration du Projet
### Matériel Requis
- Accès à un calculateur de voiture (Clio 2)
- Carte électronique Arduino
- Sonde de programmation JLink
- Port USB B pour le téléversement du code

### Installation des Pilotes
1. Ajoutez la carte via le lien : [https://www.mattairtech.com/software/arduino/package_MattairTech_index.json](https://www.mattairtech.com/software/arduino/package_MattairTech_index.json)
2. Installez la carte via Outils / Type de carte / Gestionnaire de carte
3. Recherchez "MattairTech SAMD|L|C core for Arduino" et installez-le

### Programmation de la Carte
1. Sélectionnez la carte dans Arduino : Outils / Type de carte / "MattairTech MT-D21E(rev B)"
2. Sélectionnez l'oscillateur interne et le port de communication

### Blink (Test de Programmation)
- Assurez-vous que la carte est détectée dans le logiciel Arduino
- Chargez le programme Blink pour tester la programmation

## Fonctionnalités Principales
### Injection
- Contrôle jusqu'à 4 signaux d'injection pour la première carte (à compléter)

### Régime Moteur
- Mesure et affichage du régime moteur en tr/min

### Avance à l'Allumage
- Calcul et contrôle de l'avance à l'allumage en degrés

### Temps de Conduction
- Mesure du temps de conduction du moteur

### Accélération
- Calcul de l'accélération en fonction du régime moteur

## Production et Commande
- Les fichiers Gerber dans la section hardware permettent la production de cartes électroniques. Les entreprises peuvent être contactées en fournissant ces fichiers.
- Les fichiers de la section mécanique facilitent la production de boîtiers pour les cartes électroniques.

## Téléversement du Code
- Les codes nécessaires sont disponibles dans la section software.
- Utilisez une connectique en port USB B pour téléverser le code sur la carte.

## Contribuer
1. Fork le projet
2. Créez une branche pour votre fonctionnalité (`git checkout -b fonctionnalite/nouvelle-fonction`)
3. Committez vos changements (`git commit -m 'Ajout de nouvelle fonctionnalité'`)
4. Push vers la branche (`git push origin fonctionnalite/nouvelle-fonction`)
5. Créez une Pull Request
