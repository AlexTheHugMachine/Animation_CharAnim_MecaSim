/*
 * Param.cpp : lecture des fichiers de parametres.
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

/** \file Param.cpp
   \brief Lecture des parametres de l execution.
*/

#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string.h> 


/** Fichiers de l application **/
#include "Properties.h"
#include "Scene.h"
#include "ObjetSimule.h"
#include "ObjetSimuleMSS.h"
#include "ObjetSimuleParticule.h"
#include "ObjetSimuleRigidBody.h"
#include "ObjetSimuleSPH.h"
#include "Matrix.h"


/**
 * Macro qui definit la variable var en lisant le fichier de parametres avec la cle str.
 */
#define GET_PARAM(str, var)			\
  do {						\
  std::string s = Prop[str];			\
  std::istringstream iss(s);			\
  iss >> var;					\
  } while (0);


	
/**
* Lecture des parametres de l execution relatifs a la scene.
 */
void Scene::Param(std::string Fichier_Param)
{
	/** Donnees du fichier contenant les parametres de l execution **/
	/* Proprietes du fichier */
	Properties Prop;
	
	/* Chargement du fichier */
	Prop.load(Fichier_Param);
		
	/* Viscosite du milieu */
	GET_PARAM("viscosite", _visco);
	
	/* Coordonnee de la gravite */
	float vargx, vargy, vargz;
	
	/* Coordonnee en x de la gravite */
	GET_PARAM("gx", vargx); 
	_g.x = vargx;
	
	/* Coordonnee en y de la gravite */
	GET_PARAM("gy", vargy);
	_g.y = vargy;
	
	/* Coordonnee en z de la gravite */
	GET_PARAM("gz", vargz);
	_g.z = vargz;
	
	/* Nombre d iterations de la boucle de simulation */
	GET_PARAM("nbiter", _nb_iter);
    
    /* Type d objet simule */
    std::string typeObjet;
    
    for (int i=1; i<= _NbObj ; i++){
        typeObjet = "objet";
        typeObjet += '0' + i;

        GET_PARAM(typeObjet, _type_objet[i-1]);
    }
	
}


/**
* Lecture des parametres de l execution relatfs au maillage.
 */
void ObjetSimule::Param_mesh(std::string Fichier_Param)
{
	/** Donnees du fichier contenant les parametres de l execution **/
	/* Proprietes du fichier */
	Properties Prop;
	
	/* Chargement du fichier */
	Prop.load(Fichier_Param);
	
	/* Fichier contenant les positions des particules */
	GET_PARAM("points", _Fich_Points);
	
	/* Fichier contenant les masses du maillage */
	GET_PARAM("masses", _Fich_Masses);
    
	// Interaction avec l utilisateur ou non
	GET_PARAM("interaction", _Interaction);

    
}//void


/**
 * Lecture des parametres de l execution relatfs au fluide de type SPH.
 */
void ObjetSimuleSPH::Param_sph(std::string Fichier_Param)
{
    /** Donnees du fichier contenant les parametres de l execution **/
    /* Proprietes du fichier */
    Properties Prop;
    
    /* Chargement du fichier */
    Prop.load(Fichier_Param);
    
    /// Choix du solveur
    _SolveurExpl = new SolveurExpl();
    
    std::cout << "Utilisation du schema d integration d'Euler semi-implicite"
    << std::endl;
    
    /* Intervalle de temps */
    GET_PARAM("dt", _SolveurExpl->_delta_t);
    
    /* Densite de reference */
    GET_PARAM("rho0", rho0);
    
    /* Module de Bulk */
    GET_PARAM("bulk", bulk);
    
    /* Taille des particules */
    GET_PARAM("h", h);
    
}


/**
 * Lecture des parametres de l execution relatfs au systeme de particules.
 */
void ObjetSimuleParticule::Param_particule(std::string Fichier_Param)
{
    /** Donnees du fichier contenant les parametres de l execution **/
    /* Proprietes du fichier */
    Properties Prop;
    
    /* Chargement du fichier */
    Prop.load(Fichier_Param);
    
    /// Choix du solveur
    _SolveurExpl = new SolveurExpl();
    
    std::cout << "Utilisation du schema d integration d'Euler semi-implicite"
    << std::endl;
    
    /* Intervalle de temps */
    GET_PARAM("dt", _SolveurExpl->_delta_t);
    
}


/**
 * Lecture des parametres de l execution relatfs au systeme masses-ressorts.
 */
void ObjetSimuleMSS::Param_mss(std::string Fichier_Param)
{
    /** Donnees du fichier contenant les parametres de l execution **/
    /* Proprietes du fichier */
    Properties Prop;
    
    /* Chargement du fichier */
    Prop.load(Fichier_Param);
    
    /* Fichier contenant les facettes du maillage */
    GET_PARAM("faceset", _Fich_FaceSet);
    
    /* Fichier contenant les masses du maillage */
    GET_PARAM("textures", _Fich_Texture);

    // Utilisation des textures ou non
    GET_PARAM("usetexture", _use_texture);
    
    /* Raideur */
    GET_PARAM("k", _SystemeMasseRessort->_RessOS._Raideur);
    
    /* Amortissement */
    GET_PARAM("nu", _SystemeMasseRessort->_RessOS._Nu);
    
    // Type d integration
    GET_PARAM("integration", _Integration);
    
    /// Choix du solveur
    if (_Integration == "explicite")
    {
        _SolveurExpl = new SolveurExpl();
        
        std::cout << "Utilisation du schema d integration d'Euler semi-implicite"
        << std::endl;
        
        /* Intervalle de temps */
        GET_PARAM("dt", _SolveurExpl->_delta_t);
        
    }
    else if (_Integration == "implicite")
    {
        _SolveurImpl = new SolveurImpl();
        
        std::cout << "Utilisation du schema d integration d'Euler implicite"
        << std::endl;
        
        /* Intervalle de temps */
        GET_PARAM("dt", _SolveurImpl->_delta_t);
        
        /* Nb iteration */
        GET_PARAM("nbitervitimpl", _SolveurImpl->_Nb_Iter_VitImpl);
            
    }
    
}//void



/**
 * Lecture des parametres de l execution relatfs a un objet rigide.
 */
void ObjetSimuleRigidBody::Param_rigid(std::string Fichier_Param)
{
    /** Donnees du fichier contenant les parametres de l execution **/
    /* Proprietes du fichier */
    Properties Prop;
    
    /* Chargement du fichier */
    Prop.load(Fichier_Param);
    
    /* Fichier contenant les facettes du maillage */
    GET_PARAM("faceset", _Fich_FaceSet);
    
    /* Fichier contenant les masses du maillage */
    GET_PARAM("textures", _Fich_Texture);
    
    // Utilisation des textures ou non
    GET_PARAM("usetexture", _use_texture);
    
    // Position du centre de gravite
    GET_PARAM("positionx", _Position.x);
    GET_PARAM("positiony", _Position.y);
    GET_PARAM("positionz", _Position.z);
    
    // Rotation
    Vector axe_rot;
    float angle_degres;
    
    GET_PARAM("rotationx", axe_rot.x);
    GET_PARAM("rotationy", axe_rot.y);
    GET_PARAM("rotationz", axe_rot.z);
    GET_PARAM("rotationangle", angle_degres);
    
    _Rotation = Matrix::AngleVectorToMatrix(axe_rot, angle_degres);
    
    
    // Quantite mouvement
    GET_PARAM("quantitemouvx", _QuantiteMouvement.x);
    GET_PARAM("quantitemouvy", _QuantiteMouvement.y);
    GET_PARAM("quantitemouvz", _QuantiteMouvement.z);
    
    // Moment cinetique
    GET_PARAM("momentcinetiquex", _MomentCinetique.x);
    GET_PARAM("momentcinetiquey", _MomentCinetique.y);
    GET_PARAM("momentcinetiquez", _MomentCinetique.z);
    
    /* Intervalle de temps */
    GET_PARAM("dt", _delta_t);
    
}//void

