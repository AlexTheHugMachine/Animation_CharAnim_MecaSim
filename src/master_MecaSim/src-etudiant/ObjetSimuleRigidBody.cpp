/*
 * ObjetSimuleRigidBody.cpp : definition des objets anime.
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

/** \file ObjetSimuleRigidBody.cpp
 \brief Methodes specifiques aux objets a simuler de type objet rigide.
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
#include "ObjetSimuleRigidBody.h"
#include "Viewer.h"
#include "Matrix.h"

#include "vec.h"
#include "draw.h"
#include "Viewer.h"



/**
 * Constructeur de la class ObjetSimuleRigidBody.
 */
ObjetSimuleRigidBody::ObjetSimuleRigidBody(std::string fich_param)
: ObjetSimuleMSS(fich_param)
{
    /** Recuperation des parametres de l objet rigide mis dans le fichier **/
    Param_rigid(fich_param);
    
    /// Pour l etat X(t) - specification de :
    // - position : x(t) - position du barycentre : _Position
    // - rotation : R(t) : _Rotation
    // - quantite de mouvement : P(t) : _QuantiteMouvement
    // - moment cinetique : L(t) : _MomentCinetique
    
   // cout << _Position << endl;
   // cout << _Rotation << endl;
   //cout << _QuantiteMouvement << endl;
   //cout << _MomentCinetique << endl;
    
}



/**
 * Initialisation des tableaux et construction du maillage
 a partir des donnees lues dans le fichier.
 */
void ObjetSimuleRigidBody::initObjetSimule()
{
    /** Initialisation des structures de donnees heritees du ObjetSimuleMSS **/
    ObjetSimuleMSS::initObjetSimule();
    
    /** Initialisation des membres specifiques a l objet rigide **/
    
    // Calcul de la masse - cst
    CalculMasse();
    
    // Calcul de la partie constante du tenseur d inertie - IBody
    CalculIBody();
    
    /** Message pour la fin de la creation du maillage **/
    std::cout << "Systeme rigid body build ..." << std::endl;
    
}


/**
 * \brief Creation du maillage (pour l affichage) de l objet simule de type RigidBody.
 * Methode invoquee par le graphe de scene.
 */
void ObjetSimuleRigidBody::initMeshObjet()
{
    //std::cout << "------ ObjetSimuleRigidBody::init_Mesh_Object() ----------- " << std::endl;
    
    
    std::cout << "Maillage du RigidBody pour affichage build ..." << std::endl;
    
}



/*
 * Mise a jour du Mesh (pour affichage) de l objet
 */
void ObjetSimuleRigidBody::updateVertex()
{
    //std::cout << "ObjetSimuleRigidBody::updateVertex() ..." << std::endl;
    
    // Cas ou on utilise le tableau des positions P pour faire la mise a jour du Mesh
    // Sinon rien dans cette fonction, et translation du Mesh initial en fonction de _Position
    
}


/**
 * Simulation de l objet.
 */
void ObjetSimuleRigidBody::Simulation(Vector gravite, float viscosite, int Tps)
{
    // Calcul pour l etat X(t) : Inverse tenseur inertie : I^-1(t) et vitesse angulaire
    CalculStateX();
    
    // Calcul pour d/dt X(t)
    // v(t) = = P(t) / mass
    // R'(t)
    // P'(t) = F(t)
    // L'(t) = moment total de la force (torque)
    CalculDeriveeStateX(gravite);

    // Schema d integration pour obtenir X(t+dt) en fonction de d/dt X(t)
    // x(t+dt) - position
    // R(t+dt) - rotation
    // L(t+dt) - moment cinetique
    // P(t+dt) - quantite mouvement
    Solve(viscosite);
 
    /* ! Gestion des collisions */
    // Reponse : reste a la position du sol par exemple - arret des vitesses
    // Penser au Translate de l objet dans la scene pour trouver plan coherent
    //  Collision();
    
    // Affichage des positions
    //cout << " Position du centre de masse de l objet rigide :" << _Position << endl;
    
}
