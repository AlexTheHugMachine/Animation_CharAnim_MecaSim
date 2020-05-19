/*
 * ObjetSimuleParticule.cpp : definition des objets anime.
 * Copyright (C) 2016 Florence Zara, LIRIS
 *               florence.zara@liris.univ-lyon1.fr
 *               http://liris.cnrs.fr/florence.zara/
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** \file ObjetSimuleParticule.cpp
 \brief Methodes specifiques aux objets a simuler de type ensemble de particules.
 */

/** Librairies **/
#include <stdio.h>
#include <vector>
#include <string.h>
#include <math.h>
#include <iostream>
#include <fstream>


// Fichiers de master_meca_sim
#include "MSS.h"
#include "Noeuds.h"
#include "ObjetSimule.h"
#include "ObjetSimuleParticule.h"
#include "Viewer.h"

#include "vec.h"
#include "draw.h"
#include "Viewer.h"





/**
 * Constructeur de la class ObjetSimuleParticule.
 */
ObjetSimuleParticule::ObjetSimuleParticule(std::string fich_param)
: ObjetSimule(fich_param)
{

    /** Recuperation des parametres du systeme de particules mis dans le fichier **/
    Param_particule(fich_param);
    
}


/**
 * Initialisation des tableaux a partir des donnees lues dans le fichier.
 */
void ObjetSimuleParticule::initObjetSimule()
{
    /* Variables intermediaires pour la lecture des donnees */
    Vector tmp_coord;
    float tmp_masse;
    
    /** Lecture des positions des sommets **/
    
    /// Fichier de donnees des points
    std::ifstream _FichIn_Points(_Fich_Points.c_str());
    
    /// Test ouverture du fichier
    if (! _FichIn_Points)
    {
        std::cout << "Erreur d ouverture du fichier de donnees des points : "
        << _Fich_Points << std::endl;
        
        // Arret du programme
        exit(1);
        
    }
    
    /// Lecture du nombre de sommets
    _FichIn_Points >> _Nb_Sommets;
    std::cout << "Nombre de sommets " << _Nb_Sommets <<std::endl;
    
    /// Lecture des positions
    while (!_FichIn_Points.eof())
    {
        _FichIn_Points >> tmp_coord.x;
        _FichIn_Points >> tmp_coord.y;
        _FichIn_Points >> tmp_coord.z;
        
        /// remplissage de P
        P.push_back(tmp_coord);
        
    }
    std::cout << "Positions lues ..." << std::endl;
    
    
    /** Lecture de toutes les masses **/
    
    std::ifstream _FichIn_Masses(_Fich_Masses.c_str());
    
    /// Test ouverture du fichier
    if (! _FichIn_Masses)
    {
        std::cout << "Erreur d ouverture du fichier de donnees des masses : "
        << _Fich_Masses << std::endl;
        
        // Arret du programme
        exit(1);
        
    }
    
    /// Lecture des masses
    while (!_FichIn_Masses.eof())
    {
        _FichIn_Masses >> tmp_masse;
        
        /// Remplissage de M
        M.push_back(tmp_masse);
        
    }
    std::cout << "Masses lues ..." << std::endl;
    
    
    /*** Initialisation des tableaux pour chacun des sommets ***/
    for (int i=0; i<_Nb_Sommets; ++i)
    {
        /** Vecteur nulle pour initialiser les vitesses,
         accel, forces des sommets **/
        V.push_back(Vector(0.0,0.0,0.0));
        A.push_back(Vector(0.0,0.0,0.0));
        Force.push_back(Vector(0.0,0.0,0.0));
        
    }//for
    
    
    /** Fermeture des fichiers de donnees **/
    _FichIn_Masses.close();
    _FichIn_Points.close();
    
    /** Message pour la fin de la creation du maillage **/
    std::cout << "Systeme particule build ..." << std::endl;
    
}


/**
 * Creation du maillage (pour affichage) de l objet simule.
 */
void ObjetSimuleParticule::initMeshObjet()
{
    // Rien car on utilise une sphere + translation par rapport aux positions P[i] des particules
    // Pas de Mesh a creer
}


/**
 * Mise a jour du Mesh (pour affichage) de l objet en fonction des nouvelles positions calculees.
 */
void ObjetSimuleParticule::updateVertex()
{
    // Rien car on utilise une sphere + translation par rapport aux positions P[i] des particules
    // Pas de Mesh a mettre a jour
}


/**
 * Simulation de l objet.
 */
void ObjetSimuleParticule::Simulation(Vector gravite, float viscosite, int Tps)
{
    /* Calcul des accelerations (avec ajout de la gravite aux forces) */
    //std::cout << "Accel.... " << std::endl;
    _SolveurExpl->CalculAccel_ForceGravite(gravite, _Nb_Sommets, A, Force, M);
    
    /* Calcul des vitesses et positions au temps t */
    //std::cout << "Vit.... " << std::endl;
    _SolveurExpl->Solve(viscosite, _Nb_Sommets, Tps, A, V, P);
    
    /* ! Gestion des collisions avec sol et bords de la fenetre */
    // Reponse : rebond
    // Penser au Translate de l objet dans la scene pour trouver plan coherent
    Collision();
    
    // Affichage des positions
    // AffichagePos(Tps);
    
}
