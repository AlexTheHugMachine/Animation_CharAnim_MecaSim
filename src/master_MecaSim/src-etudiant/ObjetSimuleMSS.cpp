/*
 * ObjetSimuleMSS.cpp : definition des objets anime.
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

/** \file ObjetSimuleMSS.cpp
 \brief Methodes specifiques aux objets a simuler de type MSS.
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
#include "Viewer.h"
#include "SolveurExpl.h"
#include "SolveurImpl.h"

#include "vec.h"
#include "draw.h"

/**
 * Constructeur de la class ObjetSimuleMSS.
 */
ObjetSimuleMSS::ObjetSimuleMSS(std::string fich_param)
: ObjetSimule(fich_param),
_Size(),
_SytemeMasseRessort(0)
{
    /* Allocation du systeme masse-ressort */
    _SytemeMasseRessort = new MSS();
    
    /** Recuperation des parametres du systeme masse-ressort mis dans le fichier **/
    Param_mss(fich_param);
    
    /* Calcul du facteur d amortissement des ressorts */
    _SytemeMasseRessort->_RessOS.SetFactAmorti();
    
}



/**
 * Initialisation des tableaux et construction du maillage
 a partir des donnees lues dans le fichier.
 */
void ObjetSimuleMSS::initObjetSimule()
{
    /* Variables intermediaires pour la lecture des donnees */
    Vector tmp_coord;
    float tmp_masse;
    Texture tmp_text;
    
    /* Position min de tous les sommets */
    Point Pmin(0.0,0.0,0.0);
    
    /* Position max de tous les sommets */
    Point Pmax(0.0,0.0,0.0);
    
    
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
    
    
    /* Fichier de donnees des textures */
    std::ifstream _FichIn_Texture(_Fich_Texture.c_str());

    if (_use_texture)
    {
        /// Test ouverture du fichier
        if (! _FichIn_Texture)
        {
            std::cout << "Erreur d ouverture du fichier de donnees des textures : "
            << _Fich_Texture << std::endl;
            
            // N utilise pas les textures au final
            _use_texture = 0;
            
        }
    }

    cout << "Utilisation des textures pour ObjetSimule : " << _use_texture << endl;
    
    /* Lecture des textures */
    if (_use_texture)
    {
        
        /// Lecture des coordonnees de texture
        while (!_FichIn_Texture.eof())
        {
            _FichIn_Texture >> tmp_text.u;
            _FichIn_Texture >> tmp_text.v;
            
            /// Remplissage du tableau des textures
            _vectTexture.push_back(tmp_text);
            
        }
        std::cout << "Textures lues ..." << std::endl;
        
    }
    
    
    /** Calculs intermediaires **/
    /* Calcul de Pmin et Pmax */
    for (int i=0; i<_Nb_Sommets; ++i)
    {
        Pmin = Point(std::min(Pmin.x, P[i].x),
                     std::min(Pmin.y, P[i].y),
                     std::min(Pmin.z, P[i].z));
        
        Pmax = Point(std::max(Pmax.x, P[i].x),
                     std::max(Pmax.y, P[i].y),
                     std::max(Pmax.z, P[i].z));
        
    }
    
    /* Taille du tissu dans chacune des directions x, y, z */
    _Size.x = fabs(Pmin.x - Pmax.x);
    _Size.y = fabs(Pmin.y - Pmax.y);
    _Size.z = fabs(Pmin.z - Pmax.z);
    
    
    /*** Initialisation des tableaux pour chacun des sommets ***/
    for (int i=0; i<_Nb_Sommets; ++i)
    {
        /** Vecteur nulle pour initialiser les vitesses,
         accel, forces, des normales des sommets **/
        V.push_back(Vector(0.0,0.0,0.0));
        A.push_back(Vector(0.0,0.0,0.0));
        Force.push_back(Vector(0.0,0.0,0.0));
        _vectNormals.push_back(Vector(0, 0, 0));

        
        /** Ajout du sommet dans le systeme masses-ressorts **/
        /* Construction d une particule */
        Particule *Part = new Particule();
        
        /* Attribution de son identificateur */
        Part->SetId(i);
        
        /* Position de la particule */
        Part->SetPosition(P[i]);
        // TODO : cette position n est pas mise a jour car on se sert du tableau P
        // TODO : mettre un pointeur sur la valeur contenue dans P[i] pour que la position de la particule soit mise ˆ jour
        
        /* Masse de la particule */
        Part->SetMass(M[i]);
                
        /* Ajout effectif de la particule dans le systeme masses-ressorts */
        _SytemeMasseRessort->AddParticule(Part);
        
    }//for
    
    
    
    /** Constructions de toutes les facettes du maillage **/
    
    std::ifstream _FichIn_FaceSet(_Fich_FaceSet.c_str());
    
    // Test ouverture du fichier
    if (! _FichIn_FaceSet)
    {
        std::cout << "Erreur d ouverture du fichier de donnees des faceSet : "
        << _Fich_FaceSet << std::endl;
        
        // Arret du programme
        exit(1);
    }
    
    
    /// Nombre de facets
    int nb_facet = 0;
    
    /* Construction des facettes */
    while (!_FichIn_FaceSet.eof())
    {
        /// Remplissage de allfacets
        FacetTriangle facet;
        
        _FichIn_FaceSet >> facet.fi;
        _FichIn_FaceSet >> facet.fj;
        _FichIn_FaceSet >> facet.fk;
        
        /* Construction de la facette fi, fj, fk */
        // Creation de la facet en mettant les sommets
        // dans l ordre inverse des aiguilles d une montre
        _SytemeMasseRessort->MakeFace(_SytemeMasseRessort->GetParticule(facet.fk),
                                      _SytemeMasseRessort->GetParticule(facet.fj),
                                      _SytemeMasseRessort->GetParticule(facet.fi),
                                      &_SytemeMasseRessort->_RessOS);
        
        
        // Recopie dans le tableau des indices des sommets
        _VIndices.push_back(facet.fk);
        _VIndices.push_back(facet.fj);
        _VIndices.push_back(facet.fi);
        
        nb_facet ++;
        
    }
    
    
    /* Taille du tableau des indices des sommets */
    _NFacets = nb_facet-1;
    _VISize = 3 * _NFacets;
    
    
    /** Fermeture des fichiers de donnees **/
    _FichIn_FaceSet.close();
    _FichIn_Masses.close();
    _FichIn_Points.close();
    _FichIn_Texture.close();

    
    /** Modification des normales **/
    setNormals();
    
    /** Message pour la fin de la creation du maillage **/
    std::cout << "Systeme masse-ressort build ..." << std::endl;
    
    // Allocation des structures de donnees dans le cas utilisation solveur implicite
    // X, Y, Df_Dx, Df_Dx_diag, Df_Dv, Df_Dv_diag
    if (_Integration == "implicite")
        _SolveurImpl->Allocation_Structure(_Nb_Sommets);
    
}


/**
 * Calcul de la normale a une face (a, b, c).
 */
void ObjetSimuleMSS::NormaleFace(Vector &normale, int a, int b, int c)
{
    // Coordonnees des vecteurs AB et AC
    Vector AB, AC;
    
    /* Coordonnee du vecteur AB */
    AB = (P[b] - P[a]);

    /* Coordonnee du vecteur AC */
    AC = (P[c] - P[a]);
  
    /* Coordonnees de la normale */
    normale = cross(AB, AC);
    
}


/**
 * Modification du tableau des normales (lissage des normales).
 */
void ObjetSimuleMSS::setNormals()
{
    // Normale d une face
    Vector normale;
    
    // Norme d une normale
    double Norme;
    
    // Indices des sommets d une face
    int a, b, c;
    
    /* Parcours des normales des sommets du maillage */
    for(unsigned int i=0; i<_Nb_Sommets; ++i)
    {
        // Initialisation des normales
        _vectNormals[i] = Vector(0, 0, 0);

    }
    
    /* Parcours des faces du maillage */
    for(unsigned int i=0; i<_NFacets; ++i)
    {
        // Sommets a, b, c de la face
        a = _VIndices[3 * i];
        b = _VIndices[(3 * i) + 1];
        c = _VIndices[(3 * i) + 2];
        
        // Calcul de la normale de la face
        NormaleFace(normale, a, b, c);
        
        // Modification des normales des sommets de la face
        // Normale du sommet a
        _vectNormals[a] = _vectNormals[a] + normale;
        
        // Normale du sommet b
        _vectNormals[b] = _vectNormals[b] + normale;
        
        // Normale du sommet c
        _vectNormals[c] = _vectNormals[c] + normale;
        
    }//for_i
    
    /* Parcours des normales des sommets */
    float norme;
    
    for(unsigned int i=0; i<_Nb_Sommets; i++)
    {
        norme = length(_vectNormals[i]);
        
        if (norme !=0)
            _vectNormals[i] = _vectNormals[i] * 1.0/norme;
        
    }
}


/**
 * \brief Creation du maillage (pour l affichage) de l objet simule de type MSS.
 * Methode invoquee par le graphe de scene.
 */
void ObjetSimuleMSS::initMeshObjet()
{
    //std::cout << "------ ObjetSimule::init_Mesh_Object() ----------- " << std::endl;
    
   
    
    
    std::cout << "Maillage du MSS pour affichage build ..." << std::endl;
    
}


/*
 * Mise a jour du Mesh (pour affichage) de l objet
 * en fonction des nouvelles positions calculees.
 */
void ObjetSimuleMSS::updateVertex()
{
    //std::cout << "ObjetSimuleMSS::updateVertex() ..." << std::endl;
    
      
}


/**
 * Simulation de l objet.
 */
void ObjetSimuleMSS::Simulation(Vector gravite, float viscosite, int Tps)
{
    /* Calcul des forces dues aux ressorts */
    //std::cout << "Force.... " << std::endl;
    CalculForceSpring();
    
    /* Calcul des accelerations (avec ajout de la gravite aux forces) */
    //std::cout << "Accel.... " << std::endl;
    if (_Integration == "explicite")
    _SolveurExpl->CalculAccel_ForceGravite(gravite, _Nb_Sommets, A, Force, M);
    else if (_Integration == "implicite")
        _SolveurImpl->CalculAccel_ForceGravite(gravite, _Nb_Sommets, A, Force, M);
    
    /* Calcul des vitesses et positions au temps t */
    //std::cout << "Vit.... " << std::endl;
    if (_Integration == "explicite")
        _SolveurExpl->Solve(viscosite, _Nb_Sommets, Tps, A, V, P);
    else if (_Integration == "implicite")
        _SolveurImpl->Solve(viscosite, _Nb_Sommets, Tps, Force, A, V, P, M, gravite, _SytemeMasseRessort);
    
    /* ! Gestion des collisions  */
    // Reponse : reste a la position du sol - arret des vitesses
    // Penser au Translate de l objet dans la scene pour trouver plan coherent
    //CollisionPlan();
    
    // Affichage des positions
   //  AffichagePos(Tps);
    
    /** Modification des normales **/
    setNormals();
    
}
