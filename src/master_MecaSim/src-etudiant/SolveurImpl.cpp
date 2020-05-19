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



/*
 * Constructeur de SolveurImpl.
 */
SolveurImpl::SolveurImpl()
{
    // Initialisation des variables pour precision resolution du GC
    _alpha = 0.0;
    _beta = 0.0;
        
}


/* 
 * Allocation des structures de donnees 
 * X, Y, PP, W, W1, W2, Df_Dx, Df_Dx_diag, Df_Dv, Df_Dv_diag.
 */
void SolveurImpl::Allocation_Structure(int nb_som)
{
    X.resize(nb_som);
    Y.resize(nb_som);
    PP.resize(nb_som);
    R.resize(nb_som);
    
    W.resize(nb_som);
    W1.resize(nb_som);
    W2.resize(nb_som);
    
    Df_Dx_diag.resize(nb_som);
    Df_Dv_diag.resize(nb_som);
    
    Df_Dx.resize(nb_som);
    Df_Dv.resize(nb_som);
    
}


/*
 * Initialisation des structures de donnees
 * X, Y, PP, W, W1, W2, Df_Dx, Df_Dx_diag, Df_Dv, Df_Dv_diag.
 */
void SolveurImpl::Init(int nb_som,
                       MSS * _SystemeMasseRessort)
{
    
    // Pour initialiser chaque element df/dx ou df/dv
    // Matrice 3 x 3 symetrique representee par 5 valeurs
    std::vector<float> init_element(6);
    for (int j=0; j<6; j++)
    {
        init_element[j] = 0.0;
    }
    
    
    // Allocation et initialisation des structures de donnees
    for(int i=0; i < nb_som; i++)
    {
        // Vecteurs pour la resolution H X = Y
        X[i] = Vector(0.0, 0.0, 0.0);
        Y[i] = Vector(0.0, 0.0, 0.0);
        PP[i] = Vector(0.0, 0.0, 0.0);
        R[i] = Vector(0.0, 0.0, 0.0);
        
        W[i] = Vector(0.0, 0.0, 0.0);
        W1[i] = Vector(0.0, 0.0, 0.0);
        W2[i] = Vector(0.0, 0.0, 0.0);
        
        // Matrices des contributions des forces
        // Elements diagonaux
        Df_Dx_diag[i] = init_element;
        Df_Dv_diag[i] = init_element;
        
        /// RessortList relies a la particule NumPart_i
        std::vector<Ressort *>& RessortList_i = _SystemeMasseRessort->GetParticule(i)->GetRessortList();
        
        /// Nombre de ressorts relies a la particule NumPart_i
        int NbRessortList_i = RessortList_i.size();
        
        // Pour initialiser les elements non diagonaux
        std::vector<std::vector<float>> init(NbRessortList_i);

        for (int j=0; j< NbRessortList_i; j++)
        {
            init[j] = init_element;

        }
        
        // elements non diagonaux
        Df_Dx[i].resize(NbRessortList_i);
        Df_Dv[i].resize(NbRessortList_i);
        
        Df_Dx[i] = init;
        Df_Dv[i] = init;
        
    }//for_i

}


/*
 * Remplissage matrices df/dx et df/dv dans le cas d un MSS
 * Utilisation formulation Volino ou Baraff.
 */
void SolveurImpl::Remplissage_df_dx_dv(int nb_som,
                                       MSS * _SystemeMasseRessort,
                                       std::vector<Vector> &P)
{
   
    
}


/*
 * Re-initialisation des structures de donnees : Df_Dx_diag, Df_Dv_diag, Y, Force.
 */
void SolveurImpl::Initialisation(int nb_som,
                                 std::vector<Vector> &Force)
{
    // Pour initialiser
    Vector init = Vector(0.0, 0.0, 0.0);
    
    // Allocation et initialisation des structures de donnees
    for(int i = 0; i < nb_som; i++)
    {
        // Vecteurs pour la resolution H X = Y
        Y[i] = init;
        
        // Vecteur des forces
        Force[i] = init;
        
        // Matrices des contributions des forces
        for (int j=0; j<6; j++)
        {
            Df_Dx_diag[i][j] = 0.0;
            Df_Dv_diag[i][j] = 0.0;
            
        }
        
    }//for_i
    
}
