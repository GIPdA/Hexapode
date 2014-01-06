Hexapode
========

Mini-projet hexapode

/!\ Projet non fonctionnel


## Cahier des charges

Nous sommes en charge du développement du firmware d'un robot hexapode, basé sur FreeRTOS. Le robot sera piloté via une liaison série sans fil type ZigBee.

#### Mécanique
 Le robot hexapode dispose de 6 pattes en 2 rangées, chaque patte est articulée en trois points via 3 servomoteurs classiques de modélisme. À cela s'ajoute une tourelle 2 axes sur l'avant équipée d'un télémètre infrarouge et d'une caméra CMUCAM2.
De plus, chaque patte possède un contact TOR au bout du pied.

#### Électronique
La cible est un LPC17xx auquel sont connectés directement tous les éléments du robot : contacts TOR des pieds, servomoteurs, télémètre, caméra, etc.

#### Firmware
Fonctionnalités:

 * Pilotage à partir d'un terminal série standard
   * Utilisation simple via des commandes type "AT" définies dans le manuel utilisateur
 * Affichage des informations de contact des pieds
   * Envoi automatique de l'état sur la la liaison série (commande AT)
 * Affichage des informations de la tête chercheuse
   * À définir

#### Contraintes de développement
 * Utilisation de git
 * Utilisation de Keil/RealView
 * Documentation du code avec une syntaxe compatible doxygen
 * Rédaction d'un manuel utilisateur


## Fonctionnement global

Le système est architecturé autour de tâches gatekeeper gérant le matériel, de tâches de contrôle et un interpréteur. Cet interpréteur est la passerelle entre le robot et le "monde extérieur" et parle "actions" côté robot et "commandes AT" de l'autre. Chaque tâche écoute des actions particulières et en émet vers l'interpréteur. Charge à l'interpréteur de renvoyer les actions vers d'autres tâches ou de convertir ces actions en commande AT et inversement et si nécessaire.
Les tâches gatekeeper n'émettent pas d'actions mais peuvent en écouter (configuration par ex.).

Une action est une structure de type Action qui contient un identifiant d'action destiné à reconnaitre le type de donnée associée. Cet ID permet également de transférer l'action aux tâches qui l'écoutent (via des queues FreeRTOS).

  Action {
    ActionID xID;
    void *pvData;
  }


## Interpréteur

L'interpréteur :
 - converti les commande AT en actions
 - écoute les actions émises par les tâches de contrôle (Queue)
 - renvoi les actions reçues vers d'autres tâches qui les écoutent (vers Queues)
 - converti les actions en commandes AT si nécessaire


* Interface : ATComCom (voir tâches gatekeeper)
* Entrée : FIFO (Queue)
  * Type de donnée : Action
* Sortie : aucune


### Tâches gatekeeper

Les tâches gatekeeper gèrent le matériel. Elle peuvent écouter des actions mais pas en émettre. Chaque tâche met une donnée à disposition des autres tâches de façon sécurisée (mutex, sémaphore ou queue).

#### Tâche: Interface de communication (ATComCom [AT Commands Communication])
Tâche en charge de gérer la liaison série et de transférer les commandes AT.

* Hardware : UART (LPCOpen UART)
* Entrée : FIFO (Queue)
  * Type de donnée : structure xATCommand
* Sortie : FIFO (Queue)
  * Type de donnée : structure xATCommand


#### Tâche: Télémètre infrarouge
Tâche en charge de gérer le télémètre infrarouge. Lit la tension de sortie du Sharp pour la convertir en distance.
La conversion est effectuée à intervalles régulières.

* Hardware : ADC (LPCOpen ADC)
* Entrée : auncune
* Sortie : distance en cm
  * Type de donnée : entier (8 ou 16 bits)
  * Sécurité : Mutex


#### Tâche: Contacts TOR (pieds)
Tâche en charge de gérer l'état des contacts des pieds. (mode pooling ou via interruptions)

* Hardware : GPIO (LPCOpen GPIO)
* Entrée : aucune
* Sortie : champ de bit
  * Type de donnée : entier (32 bits)
  * Sécurité : Mutex


### Tâches de contrôle

#### Tâche: CMUcam
Tâche en charge de gérer la CMUcam.

* Interface : CMUcom (interface de comm avec la CMUcam)
* Entrée : Action CMUcamIn
* Sortie : Action CMUcamOut


#### Tâche: ArmControl
Tâche en charge de gérer les mouvements des pattes de l'hexapode via les servomoteurs (interface de haut niveau).

* Interface : ServoDriver (interface de pilotage des servomoteurs)
* Entrée : Action ArmMovement
* Sortie : Action ArmStatus


#### Tâche: HeadControl
Tâche en charge de gérer les mouvements de la tête supportant la CMUcam et le capteur de distance (interface de haut niveau).

* Interface : ServoDriver (interface de pilotage des servomoteurs)
* Entrée : Action HeadMovement
* Sortie : Action ArmStatus


#### Tâche: Movement
Tâche permettant de contrôler les mouvements de l'hexapode de façon simple (avancer, tourner, etc). Coordonne les mouvements des pattes pour générer des mouvements cohérents.

* Interface : -
* Entrée : Action Movement
* Sortie : Actions MovementStatus, ArmMovement


### Librairies
Les librairies sont créées pour contrôler le hardware. Ce ne sont pas des tâches et ne doivent pas en dépendre.

#### Librairie : contrôle servomoteurs (ServoDriver)
Permet de contrôler les servomoteurs. Le module PWM1 est utilisé pour générer les 20 signaux avec une période de 21ms et une précision à la microseconde. Voir le fichier servodriver.h pour les détails sur son fonctionnement.

#### Librairie : interface TOR
La librairie TOR permet de récupérer l'état des capteurs TOR des pieds de l'hexapode.

#### Librairie : interface Sharp
La librairie IR_Sharp gère le ou les capteurs IR Sharp du robot. Elle permet de récupérer la distance en centimètres.
/!\ Seul le canal 0 est fonctionnel

#### Librairie : interface CMUcam (CMUcom)
Le portage de la librairie Arduino pour la CMUcam4 était prévu, mais devant la masse de travail il a été abandonné. Une méthode pour le portage consiste à effectuer un wrapper de CMUcom4 pour LPCOpen, mais des problèmes se posent en raison de l'architecture de l'interface UART de LPCOpen qui ne s'y prête pas vraiment. Autre solution : porter CMUcam4.c/.h pour utiliser LPCOpen. 2000 lignes de code à analyser.

