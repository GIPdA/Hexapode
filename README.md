Hexapode
========

Mini-projet hexapode


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


## Découpage du projet

Le développement est découpé en plusieurs tâches, chacune étant indépendante mais peut en nécessiter d'autres pour fonctionner (contraintes de précédence).

##### Tâche: Interface de communication
Tâche en charge de gérer la liaison série et de transférer les commandes AT.

* Hardware : UART
 * Entrée : FIFO (Queue)
   * Type de donnée : structure xATCommand
 * Sortie : FIFO (Queue)
   * Type de donnée : structure xATCommand


##### Tâche: Télémètre infrarouge
Tâche en charge de gérer le télémètre infrarouge. Lit la tension de sortie du Sharp pour la convertir en distance.

* Hardware : ADC
* Entrée : auncune
* Sortie : distance en cm
  * Type de donnée : entier (8 ou 16 bits)

##### Tâche: Contacts TOR (pieds)
Tâche en charge de gérer l'état des contacts des pieds. Envoi des commandes AT à l'interface de communication lors de changement des états (pas d'interruptions).

* Hardware : GPIO
* Entrée : auncune
* Sortie : distance en cm
  * Type de donnée : entier (8 ou 16 bits)


