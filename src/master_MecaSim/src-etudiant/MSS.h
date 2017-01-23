/*
 * MSS.h : Systeme masses-resorts (base sur des maillages triangulaires)
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

/** \file MSS.h
* \brief Structures de donnes relatives a la construction du maillage triangulaire relatif au systeme masse-ressort.
*/

#ifndef MSS_H
#define MSS_H

/** Librairie de base **/
#include <stdio.h>
#include <vector>
#include <string.h>


#include "vec.h"

#include "Noeuds.h"




/**
* \brief Structure de donnees d une particule : 
 * reliee a un ensemble de ressorts 
 * et caracterisee par sa position et son id.
 */
class Particule;


/**
* \brief Structure pour definir les caracteristiques physiques
 * des ressorts : raideur, amortissement, facteur d amortissement 
 * et longueur au repos.
 */
class Spring
{
public:
	
	/*! Ajout d un constructeur */ 
	Spring() : _Raideur(0.0), _Nu(0.0), _L0(0.0), _Amorti(0.0) {}
	
	/*! Ajout d un constructeur de copies physiques */
	Spring(const Spring & R) : 
	_Raideur(R._Raideur), _Nu(R._Nu), _L0(R._L0), _Amorti(R._Amorti) {}
	
	/*! Surcharge de = */
	inline Spring &operator=(const Spring & R)
	{
		_Raideur = R._Raideur;
		_Nu = R._Nu;
		_L0 = R._L0;
		_Amorti = R._Amorti;
		
		return *this; 
	}
	
	/*! Modification du facteur d amortissement du ressort */
	inline void SetFactAmorti()
	{
		if (_Raideur != 0)
			_Amorti = _Nu / _Raideur;
		else
			_Amorti = 1000;
	}
	
	
public:
	
	/// Raideur
	float _Raideur;    
	
	/// Amortissement
	float _Nu;
	
	/// Longueur au repos
	float _L0;
	
	/// Facteur d amortissement
	float _Amorti;
	
};


/**
* \brief Structure de donnees d un ressort : 
 * constitue de deux particules a chacune de ses extremites, d un id  
 * et caracterise par ses parametres physiques.
 */
class Ressort
{
public:
	
	/*! Constructeur vide */
	inline Ressort() 
    {
		_ParticuleA = NULL; 
		_ParticuleB = NULL;
    }
	
	/*! Ajout d un constructeur */
	Ressort(Particule *p1, Particule *p2, Spring *R)
    {
		// Particules connectees par le ressort
		_ParticuleA = p1; 
		_ParticuleB = p2; 
		
		// R : nu, raideur, amortissement
		_Spring = new Spring(*R);
		
		// Calcul de la longueur au repos du ressort
		SetLrepos();
        
    }
	
	/*! Ajout d un constructeur de copies physiques */
	Ressort(Ressort &R)
    {
		_ParticuleA = R.GetParticuleA();
		_ParticuleB = R.GetParticuleB(); 
		_Spring = R.GetSpring();
    }
	
	/*! Pour acceder a l identificateur */
	inline int GetId() {return _Id;}
	
	/*! Pour acceder a la particuleB */
	inline Particule * GetParticuleA() {return _ParticuleA;}
	
	/*! Pour acceder a la particuleA */
	inline Particule * GetParticuleB() {return _ParticuleB;}
	
	/*! Pour acceder aux caracteristiques du ressort */
	inline Spring * GetSpring() {return _Spring;}
	
	/*! Pour acceder a la raideur */
	inline float GetRaideur() {return _Spring->_Raideur;}
	
	/*! Pour acceder a l amortissement */
	inline float GetAmortissement() {return _Spring->_Nu;}
	
	/*! Pour acceder au facteur d amortissement */
	inline float GetFactAmorti() {return _Spring->_Amorti;}
	
	/*! Pour acceder a la longueur au repos */
	inline float GetLrepos() {return _Spring->_L0;}
	
	/*! Modification des donnees */
	inline void SetId(int id) {_Id = id;}
	
	/*! Modification de l amortissement du ressort */
	inline void SetAmortiss(float nu) {_Spring->_Nu = nu;}
	
	/*! Modification de la raideur du ressort */
	inline void SetRaideur(float raideur) {_Spring->_Raideur = raideur;}
	
	/*! Modification du facteur d amortissement du ressort */
	inline void SetFactAmorti() {_Spring->SetFactAmorti();}
	
	/*! Modification de la longueur au repos */
	void SetLrepos();
	
	
public:
	
	/// Identificateur
	int _Id; 
	
	/// Particule A du ressort 
	Particule *_ParticuleA;
	
	/// Particule B du ressort   
	Particule *_ParticuleB;
	
	/// Caracteristique du ressort (longueur au repos, raideur, nu, amortis.)
	Spring *_Spring;
	
};


/**
* \brief Structure de donnees d une particule : 
 * reliee a un ensemble de ressorts 
 * et caracterisee par sa position et son id.
 */
class Particule
{
public:
	
	/*! Constructeur */
	inline Particule(const Vector &C) {_Id = 0; _Pos = C;}
	
	/*! Ajout d un constructeur vide */
	Particule() {}
	
	/*! Ajout d un constructeur de copies physiques */
	Particule(Particule &Part)
    {
		_Id = Part.GetId(); 
		_RessortList = Part.GetRessortList(); 
		_Pos = Part.GetPosition(); 
    }
	
	/*! Pour acceder a l identificateur */
	inline int GetId() {return _Id;}
	
	/*! Pour acceder a la liste des ressorts de la particule */
	inline std::vector<Ressort *> &GetRessortList() {return _RessortList;}
	
	/*! Pour acceder a la position de la particule */
	inline const Vector& GetPosition() const {return _Pos;}
    
    /*! Pour acceder a la masse de la particule */
    inline float GetMass() const {return _Mass;}
	
	/*! Pour connaitre le nombre de voisins d une particule */
	inline int GetNbVoisins() {return _RessortList.size();}
	
	/*! Modification de l identificateur */
	inline void SetId(int id) {_Id = id;}
	
	/*! Modification de la position */
	inline void SetPosition(const Vector &C) {_Pos = C;}
    
    /*! Modification de la masse */
    inline void SetMass(float M) {_Mass = M;}
	
	/*! Ajout d un ressort dans la liste des ressorts lies a la particule */
	inline void AddRessort(Ressort *R) {_RessortList.push_back(R);}
	
	
public:
	
	/// Identificateur  
	int _Id;      
	
	/// Ressorts relies a la particule
	std::vector<Ressort *> _RessortList;
	
	/// Position de la particule
	Vector _Pos;
    
    /// Masse de la particule
    float _Mass;
	
};


/**
 * \brief Classe de base d un systeme masses-ressorts.
 * Il est constitue d un ensemble de particules et de ressorts.
 */
class MSS
{
public:
	
	/*! Constructeur vide */
	inline MSS() {};
	
	/*! Ajout d un destructeur : liberation de la memoire */
	inline ~MSS();
	
	
	/**
	* Methodes permettant d acceder aux donnees de la classe.
	 */
	
	/*! Recuperation d une particule */
	inline Particule* GetParticule(int id) {return _ParticuleList[id];}
	
	/*! Recuperation le nombre de particule */
	inline int GetNbParticule() const {return _ParticuleList.size(); }
	
	/*! Pour acceder a la liste des particules du maillage */
	inline std::vector<Particule *> &GetPartList() {return _ParticuleList;}
	
	/*! Pour acceder a la liste des ressorts du maillage */
	inline std::vector<Ressort *> &GetRessortList() {return _RessortList;}
	
	/*! Pour acceder a la liste des particules du maillage */
	inline const std::vector<Particule *> &GetPartListConst() const 
	{return _ParticuleList;}
	
	/*! Pour acceder a la liste des ressorts du maillage */
	inline const std::vector<Ressort *> &GetRessortListConst() const 
	{return _RessortList;}
		
	/*! Recuperation du nombre de ressorts dans le maillage */
	inline int GetNbRessort() const {return _RessortList.size();}
			
	
	/**
	* Methodes permettant de creer le maillage a partir des donnees
    */
	
	/*! Ajout d une particule dans le maillage */
	inline void AddParticule(Particule *P) {_ParticuleList.push_back(P);}
	
	/*! Ajout d un ressort dans le maillage */
	inline void AddRessort(Ressort *R) {_RessortList.push_back(R);}
	
	/*! Modification du maillage : creation d une face */
	void MakeFace(Particule *p1, Particule *p2, Particule *p3, Spring *R);
	
	/*! Modification du maillage : creation d une arete */
	void MakeEdge(Particule *p1, Particule *p2, Spring *R);
	
	
	
public:
	
	/// Particules du maillage
	std::vector<Particule *> _ParticuleList;
	
	/// Ressorts du maillage
	std::vector<Ressort *> _RessortList;
    
    /// Caracteristiques des ressorts
    Spring _RessOS;
    

};


/**
* Destructeur de la classe MSS.
 */
MSS::~MSS()
{
	// Destruction des particules du maillage
	_ParticuleList.~vector<Particule *>();
	
	// Destruction des ressorts du maillage
	_RessortList.~vector<Ressort *>();

}

#endif
