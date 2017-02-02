/** \mainpage Pacman


\section intro Introduction

Un début de Pacman pour le cours LIFAP4
(Auteur : Alexandre.Meyer@univ-lyon1.fr)

Code écrit en C++
Se compile avec g++

remarque : documentation très très minimal, tout comme le code ...
<br> <br> <br>


\section compil Pour compiler
Tester sous Linux (Ubuntu) et Windows. Editer le fichier Makefile pour choisir votre plateforme!

Dépendances :
 - SDL2 : http://www.libsdl.org/

$ make ou ouvrir le projet avec CodeBlocks puis F9
<br> <br> <br>

\section exec Pour executer
$ ./bin/pacman_txt <br>
ou
$ ./bin/pacman_sdl <br>
<br> <br> <br>

\section doc Pour générer la documentation de code
 Dépendance : doxygen http://www.stack.nl/~dimitri/doxygen/
 $ cd doc <br>
 $ doxygen doxyfile <br>
 Puis ouvrir doc/html/index.html avec firefox

 ATTENTION : seul le module Terrain est documenté.

*/
