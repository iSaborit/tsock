# tsock
Groupe : Iker Saborit López - Tabatha Drumez

Commande utilisée pour générer l'exécutable :
```
make
```
TP 1 :

On a fini la `.v1`.
Il faut formater correctement le message en ajoutant le numéro.
Aussi il faut mieux structurer le code, car toute la logique de creation/utilisation de sockets est dans `void main(int argc, char **argv)`.
---
TP2 :

On a fini la `.v2/3`. 

Nous avons structuré le projet de manière cohérente, en créant des fonctions aux responsabilités bien séparées ainsi qu’un petit wrapper des fonctions de l’API des sockets, afin de centraliser la gestion des erreurs (dans un style POSIX).

L’application est divisée en cli.c/h, chargé de paramétrer le fonctionnement de l’application via l’entrée utilisateur, app.c/h, chargé de gérer la logique, et net.c/h, qui regroupe les wrappers de l’API pour simplifier le flux de travail.

Nous avons finalisé les wrappers TCP, mais nous n’avons pas eu le temps d’écrire la logique dans app.c, ce qui ne devrait pas prendre beaucoup de temps.

Avec cela, le travail sera terminé.