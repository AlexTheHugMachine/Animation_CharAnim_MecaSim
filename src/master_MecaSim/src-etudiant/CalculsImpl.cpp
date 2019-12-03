/*
 * Calculs.cpp : Application schemas implicite sur les objets.
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


/** \file SolveurImpl.cpp
 * Solveur implicite : schem d'integration d Euler implicite
 * permettant le calcul des nouvelles vitesses et positions.
 */

#include <stdio.h>
#include <math.h>
#include <vector>


#include "vec.h"
#include "ObjetSimule.h"
#include "Viewer.h"
#include "SolveurImpl.h"





/**
 * Calcul de l acceleration des particules
 * avec ajout de la gravite aux forces des particules
 * et ajout de la force due au vent sur une des particules du maillage
 * et reinitialisation des forces.
 */
void SolveurImpl::CalculAccel_ForceGravite(Vector g,
                                           int nb_som,
                                           std::vector<Vector> &A,
                                           std::vector<Vector> &Force,
                                           std::vector<float> &M)
{
    
    
    
    
    
}//void


/*! Calcul des vitesses et positions */
void SolveurImpl::Solve(float visco,
                        int nb_som,
                        int Tps,
                        std::vector<Vector> &Force,
                        std::vector<Vector> &A,
                        std::vector<Vector> &V,
                        std::vector<Vector> &P,
                        std::vector<float> &M,
                        Vector gravite,
                        MSS * _SystemeMasseRessort)
{
    
    /* Initialisation des structures X, Y, PP, W, W1, W2, Df_Dx, Df_Dx_diag, Df_Dv, Df_Dv_diag */
    Init(nb_som, _SystemeMasseRessort);
    
    /* Remplissage matrices df/dx et df/dv */
    Remplissage_df_dx_dv(nb_som, _SystemeMasseRessort, P);
    
    /* Remplissage du vecteur Y */
    Remplissage_Y(nb_som, V, Force, M, gravite, _SystemeMasseRessort);
    
    /* Resolution de HX = Y */
    Resolution(nb_som, M, _SystemeMasseRessort);
    
    /* Calcul des vitesses */
    CalculVitesse(visco, nb_som, Tps, V, X);
    
    /* Calcul de la position */
    CalculPosition(nb_som, V, P);
    
    /* Re-initialisation des vecteurs : Df_Dx_diag, Df_Dv_diag, Y, Force */
    Initialisation(nb_som, Force);
    
}


/*
 * Remplissage du vecteur Y du systeme H X = Y.
 * Y = dt f(t) + dt^2 df/dx v(t)
 * Remplissage du vecteur R = Y - H X.
 */
void SolveurImpl::Remplissage_Y(int nb_som,
                                std::vector<Vector> &V,
                                std::vector<Vector> &Force,
                                std::vector<float> &M,
                                Vector g,
                                MSS * _SystemeMasseRessort)
{
    /// Matrice des contributions des forces df/dx
    // chaque element de la matrice de taille 3x3
    
    // Calcul du produit df/dx * V
    
    // [0 1 2]      [0 1 2]     [V.x]
    // [3 4 5] -->  [  3 4]   * [V.y]
    // [6 7 8]      [    5]     [V.z]
    
    //////
    // Calcul des elements diagonaux de df/dx v(t)
    //////
    
    
    
    
    ////////
    // Calcul des elements non diagonaux de df/dx v(t)
    ///////
    
    
    
    ////
    // Remplissage du vecteur Y
    ///
    
    
    
}


/*
 * Calcul du produit matrice * vecteur :
 *  W = H * PP = (M - dt df/dv - dt^2 df/dx) * PP
 */
void SolveurImpl::CalculProdMatVect(int nb_som,
                                    std::vector<float> &M,
                                    MSS *_SystemeMasseRessort)
{    
    ////////
    // Calcul des elements diagonaux
    ///////
    
    
    ////////
    // Calcul des elements non diagonaux
    ///////
    
}


/*
 * Calcul de la direction - algo du Gradient Conjugue.
 * Si _beta != 0, PP = Y + (_alpha/ _beta) * PP, sinon PP = Y.
 */
void SolveurImpl::CalculDirection(int nb_som)
{
    
    
}


/**
 * Calcul de la solution X = X + (_alpha / _beta) * PP
 * et calcul du residu Y = Y - (_alpha / _beta) * W.
 */
void SolveurImpl::CalculSolution_Residu(int nb_som)
{
    
}


/*
 * Calcul de _beta = PP^T W.
 */
void SolveurImpl::CalculBeta(int nb_som)
{
    
    
}


/*
 * Calcul de _alpha = Y^T Y.
 */
void SolveurImpl::CalculNorme(int nb_som)
{
    
}


/*
 * Fonction resolvant HX = Y par la methode du gradient conjugue.
 *
 * H = M - dt df/dv - dt^2 df/dx
 *
 * X = delta vitesse = v(t+dt) - v(t)
 *
 * Y = dt f(t) + dt^2 df/dx v(t)
 */
void SolveurImpl::Resolution(int nb_som,
                             std::vector<float> &M,
                             MSS * _SystemeMasseRessort)
{
    // Rq : Le vecteur Y sera utilise comme vecteur de residu
    
    // Precision de la resolution
    float eps2 = 0.1;// * 0.1;
  
    // Facteur d erreur
    _beta = 0;
    
    do
    {
        // _alpha = Y^T Y
        CalculNorme(nb_som);
        
        // Calcul de la nouvelle direction
        // Si _beta != 0, PP = Y + (_alpha/ _beta) * PP
        // Sinon PP = Y
        CalculDirection(nb_som);
        
        // W = H PP = (M - dt df/dv - dt^2 df/dx) * PP
        CalculProdMatVect(nb_som, M, _SystemeMasseRessort);
        
        // Calcul de _beta = PP^T W
        CalculBeta(nb_som);
        
        // Calcul du residu Y = Y - (_alpha / _beta) * W
        // et calcul de la nouvelle solution X = X + (_alpha / _beta) * PP
        CalculSolution_Residu(nb_som);
        
        // Nouveau facteur d erreur
        _beta = _alpha;
        
        std::cout << "beta " << _beta << std::endl;
        
    } while(_beta < eps2);

}


/*
 * Calcul des vitesses :
 * v(t+dt) = v(t) + delta v.
 * delta v = solution du systeme H X = Y
 * donc delta v = X.
 */
void SolveurImpl::CalculVitesse(float visco,
                                int nb_som,
                                int Tps,
                                std::vector<Vector> &V,
                                std::vector<Vector> &X)
{
    
    
    
}


/*
 * Calcul des positions :
 * x(t+dt) = x(t) + dt v(t+h).
 */
void SolveurImpl::CalculPosition(int nb_som,
                                 std::vector<Vector> &V,
                                 std::vector<Vector> &P)
{
   
    
}
