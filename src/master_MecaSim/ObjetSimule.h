
/** \file ObjetSimule.h
 \brief Structures de donnes communes relatives aux objets simules.
 */

#ifndef OBJET_SIMULE_H
#define OBJET_SIMULE_H



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




/**
 * \brief Structure de donnees de base des objets simules.
 */
class ObjetSimule: public Noeud
{
public:

    /*! Constructeur */
    ObjetSimule(){};

    /*! Lecture des parametres lies au maillage */
    void Param_mesh(std::string fich_param);

    /*! Initialisation des tableaux des sommets a partir du fichier de donnees de l objet */
    virtual void initObjetSimule() = 0;

    /*! Mise a jour du Mesh (pour affichage) de l objet en fonction des nouvelles positions calculees */
    virtual void updateVertex() = 0;

    /*! Operation de creation du maillage (servant a l affichage) */
    virtual void initMeshObjet() = 0;

    /*! Gestion des collisions avec plan (x,y,z) */
    virtual void CollisionPlan(float x, float y, float z) = 0;

    /*! Simulation de l objet */
    virtual void Simulation(Vector gravite, float viscosite, int Tps) = 0;

    /*! Calcul des accelerations (avec ajout de la gravite aux forces) */
    void CalculAccel_ForceGravite(Vector g);

    /*! Calcul de la position et de la vitesse des sommets au temps 0 pour la methode Leap Frog */
    void CalculPositionVitesseTps0(float visco);

    /*! Calcul des positions et des vitesses */
    void CalculPositionVitesse(float visco);

    /*! Interaction avec l utilisateur */
    void Interaction(Vector MousePos);

    /*! Affichage des positions de chaque sommet */
    void AffichagePos(int tps);



protected:

    //Mesh m_mesh;

    /// Fichier de donnees contenant les points
    std::string _Fich_Points;

    /// Fichier de donnees contenant les masses
    std::string _Fich_Masses;

    /// Intervalle de temps
    float _delta_t;

    /// Interaction avec l utilisateur ou non
    std::string _Interaction;

    /// valeur d'absorbtion de la vitesse en cas de collision:
    /// 1=la particule repart aussi vite, 0=elle s'arrete
    float _Friction = 1.0f;


    /** Structures de donnees pour les calculs **/

    /// Declaration du tableau des vitesses
    std::vector<Vector> V;

    /// Declaration du tableau des accelerations
    std::vector<Vector> A;

    /// Declaration du tableau des forces
    std::vector<Vector> Force;

    /// Declaration du tableau des masses
    std::vector<float> M;

};



#endif
