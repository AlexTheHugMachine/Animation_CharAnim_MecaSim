
/** \file ObjetSimuleRigidBody.h
 \brief Structures de donnes relatives aux objets rigides.
 */

#ifndef OBJET_SIMULE_RIGID_BODY_H
#define OBJET_SIMULE_RIGID_BODY_H



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
#include "Matrix.h"
#include "Properties.h"
#include "ObjetSimule.h"
#include "ObjetSimuleMSS.h"


/**
 * \brief Structure de donnees pour un objet rigide.
 */
class ObjetSimuleRigidBody: public ObjetSimuleMSS
{
public:
    
    /*! Constructeur */
    ObjetSimuleRigidBody(std::string fich_param);
    
    /*! Lecture des parametres lies a un objet rigid */
    void Param_rigid(std::string fich_param);
    
    /*! Initialisation des tableaux des sommets
     a partir du fichier de donnees de l objet */
    void initObjetSimule();
    
    /*! Calcul de la masse de l objet rigide */
    void CalculMasse();
    
    /*! Calcul du IBody - partie constante du tenseur d'inertie */
    void CalculIBody();
    
    /*! Calcul de l etat de l objet rigide */
    void CalculStateX();
    
    /*! Calcul de d/dt X(t) */
    void CalculDeriveeStateX(Vector gravite);
    
    /*! Schema integration pour obtenir X(t+dt) */
    void Solve(float visco);
    
    /*! Simulation de l objet */
    void Simulation(Vector gravite, float viscosite, int Tps);
    
    /*! Gestion des collisions. Rajoutez parametre si besoin. */
    void Collision();
    
    /*! Creation du maillage (pour l affichage) de l objet simule de type RigidBody */
    void initMeshObjet();
    
    /*! Mise a jour du Mesh (pour affichage) de l objet 
     en fonction des nouvelles positions calculees */
    void updateVertex();
    
    
    ///---- Info relatives a l etat du systeme
    /// Etat du systeme : X(t)
    // - position : x(t) - position du baycentre
    // - rotation : R(t)
    // - quantite de mouvement : P(t) = m v(t)
    // - moment cinetique : L(t) = I(t) w(t)
    
    /// Position du centre de masse
    Vector _Position;
    Vector _BaryCentre;
    
    /// Position des particules i :
    // r0i : position constante de la particule i dans l objet
    // ri(t) : position dans le repere monde
    // ri(t) = R(t) r0i + x(t)
    std::vector<Vector> _ROi;
    
    /// Matrice de rotation R(t)
    Matrix _Rotation;
    
    /// Quantite de mouvement : P(t) = masse * vitesse
    Vector _QuantiteMouvement;
    
    /// Moment Cinetique : L(t)
    Vector _MomentCinetique;
    
    ///--- On a aussi besoin
    // masse : m
    // tenseur inertie : I(t)
    // vitesse angulaire : w(t)
    
    /// Masse de l objet (constante pendant la simulation)
    float _Mass;
    
    /// Tenseur d inertie de l objet rigide - Partie constante
    Matrix _Ibody;
    
    /// Inverse du tenseur d inertie - Partie constante
    Matrix _IbodyInv;
    
    /// Inverse du tenseur d inertie
    Matrix _InertieTenseurInv;
    
    /// Vitesse angulaire w(t)
    Vector _VitesseAngulaire;
    
    
    ///---- Info relatives a la derivee de l etat du systeme
    // Derivee de l etat du systeme : X'(t) = d/dt X(t)
    // - d/dt x(t) = v(t) - vitesse du baycentre
    // - d/dt R(t) = R'(t) = w(t) x R(t)
    // - d/dt P(t) = F(t) - les forces
    // - d/dt L(t) = moment de la force (torque) = sum_i (ri(t) - x(t)) x Fi(t)
    

    /// x'(t) = vitesse du centre de masse
    Vector _Vitesse;
    
    /// Derivee de la rotation
    Matrix _RotationDerivee;
    
    /// P'(t) = Force appliquee
    Vector _Force;
    
    /// L'(t) = Moment de la force
    Vector _Torque;
    
    // Intervalle de temps
    float _delta_t;
    
};



#endif
