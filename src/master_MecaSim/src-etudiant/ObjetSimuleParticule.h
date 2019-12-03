
/** \file ObjetSimuleParticule.h
 \brief Structures de donnes relatives aux objets simules de type ensemble de particules.
 */

#ifndef OBJET_SIMULE_PART_H
#define OBJET_SIMULE_PART_H



/** Librairies de base **/
#include <stdio.h>
#include <vector>
#include <string.h>
#include <fstream>

// Fichiers de gkit2light
#include "vec.h"
#include "mesh.h"

// Fichiers de master_meca_sim
#include "MSS.h"
#include "Noeuds.h"
#include "Properties.h"
#include "SolveurExpl.h"






/**
 * \brief Structure de donnees pour un ensemble de particules.
 */
class ObjetSimuleParticule: public ObjetSimule
{
public:
    
    /*! Constructeur */
    ObjetSimuleParticule(std::string fich_param);
    
    /*  Lecture des parametres de l execution relatfs au systeme de particules */
    void Param_particule(std::string Fichier_Param);
    
    /*! Initialisation a partir des fichiers de donnees.*/
    void initObjetSimule();
    
    /*! Creation du maillage (pour affichage) de l objet simule */
    void initMeshObjet();
    
    /*! Simulation de l objet */
    void Simulation(Vector gravite, float viscosite, int Tps);
  
    /*! Gestion des collisions */
    void Collision();
    
    /*! Mise a jour du Mesh (pour affichage) de l objet en fonction des nouvelles positions calculees */
    void updateVertex();
    
    /// SolveurExpl : schema d integration semi-implicite 
    SolveurExpl *_SolveurExpl;

};



#endif
