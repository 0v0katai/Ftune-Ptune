===============================================================================

Ptune3 est un utilitaire de réglage CPG, BSC et DBSC SH7305 pour PRIZM fx-CG50 / Graph 90+E v0.24

  copyright (c) 2014/2015/2016/2017/2018/2019/2020 de sentaro21
  courriel sentaro21@pm.matrix.jp

===============================================================================

C'est à peu près la même chose avec Ptune2.
Mais vous ne pouvez pas utiliser correctement la synchronisation SDRAM.
Cela n'a toujours pas été résolu.

###############################################################################
#Comme pour la version actuelle, seul fx-CG50/Graph 90+E est supporté.        #
#Cela ne fonctionne pas dans CG10/20.                                         #
###############################################################################

====== Avertissement !!! ==========================
Cet outil est conçu pour fonctionner en toute tranquillité, mais de possibles dysfonctionnements peuvent survenir.
Néanmoins, cet outil peut également causer des pertes de données sur votre calculatrice voire la destruction irrémédiable de son système !
Utilisez-le donc à vos risques et périls. En aucun cas, je ne serai tenu responsable d'éventuels dégâts (et Planète-Casio non plus).
===================================================

-------------------------------------------------------------------------------
Comment utiliser
-------------------------------------------------------------------------------
Écran principal
----------------------------------
 FLL:x900          * 14.75MHz
 PLL:x16           *235.93MHz
 IFC:1/2  CPU      *117.96MHz
 SFC:1/4  roR 8    * 58.98MHz
 BFC:1/4  CL  2    * 58.98MHz
 PFC:1/8           * 29.49MHz
zone de message / score de référence
[touche de fonction]
-----------------------------------

roR : Le nombre de ROM attend des cycles dans l'accès lu.   rouge en couleur : gamme dangereuse.
CL  : Le nombre de SDRAM CL
-------------------------------------------------------------------------------

-[EN HAUT]      choisissez en haut (FLL,PLL,IFC,SFC,BFC,PFC)
-[EN BAS]       choisissez en bas  (FLL,PLL,IFC,SFC,BFC,PFC)

-[GAUCHE]       réduction 1 multiplication de pas ou cloison
-[DROIT]        augmentez 1 multiplication de pas ou cloison

-[SHIFT]+[EN HAUT]   choisissez la multiplication FLL (=> l'élection d'organisation)

-[F1]   Chargez le cadre implicite normal            Le CPU  59MHz, PLLx16, le bus  29MHz, le défaut attend
-[F2]   Souvenez-vous [F2] du cadre       programmez le CPU  59MHz, PLLx32, le bus  29MHz, identique à CG10/20
-[F3]   Souvenez-vous [F2] du cadre       programmez le CPU  96MHz, PLLx26, le bus  48MHz, identique à l'overclock CG10/20 (Pover)
-[F4]   Souvenez-vous [F2] du cadre       programmez le CPU 236MHz, PLLx32, le bus  29MHz, uniquement CPU overclocké
-[F5]   Souvenez-vous [F2] du cadre       programmez le CPU 192MHz, PLLx26, le bus  95MHz, PLLx16 -> x26
-[F6]   point de référence simple
        CPUcore comte de boucle simple par 100 millisecondes. (9860G ajoutent - dans "UTIL" comme)
        et MÉMOIRE (ROM, RAM, IO) comte de boucle d'accès par 50 millisecondes
        cabillot plus
        CPUcore et vitesse de PutDsipDD (fps)

-[SHIFT]
    -[F1]       Sauvez le fait de montrer au dossier (la mémoire principale). données de charge automatiquement sur la course suivante.
    -[F2]       Courant de magasin montrant à la touche de fonction [F2]
    -[F3]       Courant de magasin montrant à la touche de fonction [F3]
    -[F4]       Courant de magasin montrant à la touche de fonction [F4]
    -[F5]       Courant de magasin montrant à la touche de fonction [F5]
    -[F6]       Cadre de charge du dossier (mémoire principale)

-[OPTN]         La liste de table de valeur limite de fréquence de bus dans chacun d'attend.
    -[F4]       initialisez le cadre implicite
    -[F5]       La ROM & la RAM / la RAM ÉCRIT & a LU la table chenge.
                La fréquence de limite de la RAM n'augmente pas d'une certaine tache,
                mais devient là la limite de l'horloge SHW & l'horloge de bus.
    -[F6]       La vérification d'auto pour la vitesse de mémoire dans chacun d'attend.

-[VARS]
    -[F1]       inscrivez-vous affichent FRQCR, CCR
    -[F2]       inscrivez-vous affichent le cabillot BCR/WCR

    -[F3]       modifier CS0BCR CS2BCR
                choisissez la clé de curseur
        -[F1]   +
        -[F2]   -
        -[F4]   initialisez le défaut

    -[F4]       modifier CS0WCR CS2WCR
                choisissez la clé de curseur
        -[F1]   +
        -[F2]   -
        -[F4]   initialisez le défaut

    -[F5]       modifier CS5ABCR CS5BBCR
                choisissez la clé de curseur
        -[F1]   +
        -[F2]   -
        -[F4]   initialisez le défaut

    -[F6]       modifier CS5AWCR CS5BWCR
                choisissez la clé de curseur
        -[F1]   +
        -[F2]   -
        -[F4]   initialisez le défaut

-[PRGM]
    -[F1]       modifier CS3BCR  CS4BCR
    -[F2]       modifier CS3WCR  CS4WCR
    -[F3]       modifier CS5ABCR CS5BBCR
    -[F4]       modifier CS5AWCR CS4BWCR
    -[F5]       modifier CS6ABCR CS6BBCR
    -[F6]       modifier CS6AWCR CS6BWCR

-[EXIT]         sortie

-[AC]           revenez à l'écran principal

-[EXE]          si le point de référence l'a choisi est réalisé encore une fois.

-[*]            la ROM d'augmentation attend
-[/]            la ROM de réduction attend
                Quand il peut être baissé, la vérification de mémoire commence
                si l'erreur, ne peut pas la réduction.

-[X^2]
-[^]            activer / désactiver la fonction d'étalement du spectre.

-[SETUP]                                 défaut
        ROM Attendez la marge    0-15%    5%      ( moins de 1 % est dangereux.) la remise attend la table.
        RAM Attendez la marge    0-15%    5%      ( moins de 1 % est dangereux.) la remise attendent la table.
        PLL fréquence MAX              800MHz
        CPU fréquence MAX              275MHz
        Shw fréquence MAX              150MHz
        Bus fréquence MAX              100MHz
        IO  fréquence MAX               50MHz
        Démarrage mem vérifier   Sur/De   Sur
        F1   Sur/De vérifier     Sur/De   De
        Wait Réduction d'auto    Sur/De   Sur
        RAM WW  Auto inc/dec     Sur/De   Sur
        ROM IWW Réduction d'auto Sur/De   Sur
        PFC Augmentation d'auto  Sur/De   Sur
        FLL étalage no-shift     Sur/De   De
        BATT étalage de volt     Sur/De   De
	Fréquence réelle         Sur/De   Sur


-------------------------------------------------------------------------------
Vérification de la mémoire ROM
-------------------------------------------------------------------------------
C'est un point important de cet outil pour mesurer la vitesse de la mémoire.
Au début, recherchez le bloc 64K le plus lent.
et vérifiez la limite supérieure à chaque attente.
Vérifiez-le en effectuant une lecture de la même adresse deux fois si ce n'est pas différent.
C'est une erreur de lecture si différente.

-------------------------------------------------------------------------------
Vérification de la mémoire SDRAM
-------------------------------------------------------------------------------
Cette version est testée par le délai d'attente actuel de la mémoire.
Une erreur peut survenir après le test de la mémoire SDRAM.
Veuillez réinitialiser après vérification de la mémoire SDRAM même si aucune erreur ne vous est apparue.

-------------------------------------------------------------------------------
Correspondance d’espace de la fréquence ultérieure ver.0.20
-------------------------------------------------------------------------------
La fréquence de fonctionnement réelle correspond à la fréquence calculée à partir de la PLL interne dans CG10 / 20,
mais il semble y avoir environ 1,6% de basse fréquence dans CG50/Graph90+E.
Par conséquent, l'affichage de la fréquence de fonctionnement réelle à partir du ver.0.20 a été ajouté.
La fréquence réelle de fonctionnement est inconnue exactement,
La fréquence de correction temporaire multipliée par 900/914 par la fréquence PLL interne.
La fréquence de correction <-> la fréquence PLL interne peut changer selon les réglages.

faire référence à
http://www.casiopeia.net/forum/viewtopic.php?f=25&t=7327

(ver.0.21-)
Dans CG50/Graph90+E, la fonction d’étalement du spectre du générateur d’horloge est activée par défaut,
Il a été confirmé que la fréquence de fonctionnement réelle est inférieure à la fréquence calculée à partir de la PLL interne.
Après 0,21, la notation sur l'écran changera si la fonction d'étalement du spectre est activée.

-------------------------------------------------------------------------------
Remerciements
-------------------------------------------------------------------------------
Ce programme était basé sur Pover (par Ashbad).
développé par PrizmSDK 0.3 (Ptune3).

documentation du projet fxReverse,
Calculatrices fx basées sur SuperH,
Cemetech WikiPrizm,
a beaucoup servi de référence.

http://www.casiopeia.net/forum/viewtopic.php?f=11&t=1756&p=20678#p14588
http://casiopeia.net/forum/viewtopic.php?f=2&t=1783&start=50
https://tiplanet.org/forum/viewtopic.php?p=215399#p215399
Très merci pour les informations de SimonLothar et TeamFX et Critor.

Je serais heureux si cet outil vous est utile.

-------------------------------------------------------------------------------
LICENCE
-------------------------------------------------------------------------------
Ce logiciel est un logiciel gratuit.
La licence suit la GPLv2.

-------------------------------------------------------------------------------
v0.24 2020.2.20 Affichage "X" changé en symbole de multiplication.

v0.23 2020.1.3  Amélioration des icônes de la clé de fonction. (conçu par Colon)

v0.22 2019.8.29 Modification de la valeur de la tension de la batterie en hausse de 9% dans CG50/Graph90+E.

v0.21 2019.2.20 Ajout de l'activation/désactivation de la fonction d'étalement du spectre.
                On/Off est activé avec la touche [X^2] ou [^].

v0.20 2018.8.19 Modifier le programme de test ROM / SDRAM.
      2018.8.18 Ajouté effectuer une correction de fréquence précise (peut être sélectionné par configuration)
                (La correction temporaire fréq = fréq interne * 900/914)

v0.10 2017.10.1 Modifier le programme de test SDRAM en fonction des paramètres actuels.
                Changer d'icône. (Type CG50)

v0.05 2017.7.20 Modifier le programme de test SDRAM,
                Changer le réglage par défaut.
                Correction des bugs non sauvegardés (CS3BCR, CS3WCR)

v0.04 2017.4.19 Modifier le programme de test de la ROM,
                Correction du bug de test de la ROM à nouveau. (À prévoir ....)
                Retour à la structure du registre CS2WCR. (SH7730 -> SH7724)

v0.03 2017.4.18 Bug de test de la ROM corrigé (impossible de le faire ..)

v0.02 2017.4.17 Modifier la structure de registre CS2WCR, CS3WCR. (SH7724 -> SH7730)
                Pour pouvoir modifier le minutage de la SDRAM. (Il n'y a pas la certitude)
                Pour limiter PLL x33.

v0.01 2017.4.15 première version
