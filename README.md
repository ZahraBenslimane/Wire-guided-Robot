# Wire-guided-Robot
Design and construction of a wire guided delivery robot

A wire guired robot delivery system, that travels in a loop along a defined path with an electric wire. This project found its inspiration in the robots used at industrial warehouses. The robot receives delivery instructions through a control base system to deliver or pick up packages from different workstations.

This is the final undergraduate year projet, of the 2021 electronics and electrical engineering class.

I was the project learder of 5 teamates and my role was : 

* Design the circuits of various sensors and components.
* I was resposible for programming the 3 microcontrollers [robots, workstation and command base]


1. Communication avec les robots via des signaux injectés dans le fil de trajectoire : 
    * Modulation du courant par un code inspiré du code morse, le système de base est capable d'avertir les robots et de leur confier des missions (se garer à     droite/gauche pour un chargement/ déchargement– changement de vitesses).
    * Mise à jour automatiquement des commandes [gérer par des postes de supervision]


2. Codes DTMF émis par les postes de travails:
    * Emission d’un code DTMF différent pour chaque poste de travail via un haut-parleur permettant au robot qui lui passe devant d'identifier son numéro.
    * Utilisation du DAC (Digital to Analog Converter) pour la génération du DTMF.


3. Motorisation des roues du robot mobile :
    * Implémentation d’un système d'arrêt d'urgence.
    * Commande PWM des moteurs de façon automatique en se basant sur le système d'analyse de position par rapport à la trajectoire [Capteurs inductifs embarqué sur le robot mobile].

4. Communication Robot - Poste de travail : INFRA ROUGE
    * Emission d’une trame IR par le Microcontrôleur basé sur le robot (injectée dans une LED IR) représentant des codes sur 20 bits (numéro robot, vitesse..).
