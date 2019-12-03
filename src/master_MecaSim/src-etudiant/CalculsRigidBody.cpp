/*
 * CalculsRigidBody.cpp :
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

/** \file CalculsMSS.cpp
 Programme calculant pour chaque particule i d un MSS son etat au pas de temps suivant
 (methode d 'Euler semi-implicite) : principales fonctions de calculs.
 \brief Fonctions de calculs de la methode semi-implicite sur un systeme masses-ressorts.
 */

#include <stdio.h>
#include <math.h>
#include <vector>
#include <iostream>

#include "vec.h"
#include "ObjetSimule.h"
#include "ObjetSimuleRigidBody.h"
#include "Viewer.h"

using namespace std;




/*
 * Calcul de la masse de l objet rigide
 */
void ObjetSimuleRigidBody::CalculMasse()
{
    
    
}


/*
 * Calcul du tenseur d inertie de l objet rigide - - partie constante Ibody
 * et remplissage tableau roi (position particules dans repere objet)
 */
void ObjetSimuleRigidBody::CalculIBody()
{
    
}


/*
 * Calcul de l etat de l objet rigide.
 */
void ObjetSimuleRigidBody::CalculStateX()
{
    
    
}



/*
 * Calcul de la derivee de l etat : d/dt X(t).
 */
void ObjetSimuleRigidBody::CalculDeriveeStateX(Vector gravite)
{
    
}


/**
 * Schema integration pour obbtenir X(t+dt) en fct de X(t) et d/dt X(t)
 */
void ObjetSimuleRigidBody::Solve(float visco)
{
  
    
}//void



/**
 * Gestion des collisions avec le sol.
 */
void ObjetSimuleRigidBody::Collision()
{

   
    
}// void

