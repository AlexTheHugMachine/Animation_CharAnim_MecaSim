#ifndef _MOTIONGRAPH_H
#define _MOTIONGRAPH_H

#include "quaternion.h"
#include "Viewer.h"
#include "BVH.h"
#include "Skeleton.h"
#include "TransformQ.h"

#include <PhysicalWorld.h>
#include <cstdio>
#include <iostream>

class MotionGraph
{
public:
   //! L'ensemble des BVH du graphe d'animation
   std::vector<chara::BVH> m_BVH;

   //! Un noeud du graphe d'animation est repéré par un entier=un identifiant
   typedef int GrapheNodeID;

   //! Une animation BVH est repérée par un identifiant=un entier 
   typedef int BVH_ID;
   
   //! Un noeud du graphe contient l'identifiant de l'animation, le numéro 
   //! de la frame et les identifiants des noeuds successeurs 
   //! Remarque : du code plus "joli" aurait créer une classe CAGrapheNode
   class GrapheNode
   {
    public:
     BVH_ID id_bvh;
     int frame = 0;
     std::vector<GrapheNodeID> ids_next;     //! Liste des nœuds successeurs 
   };


   //! Tous les noeuds du graphe d'animation
   std::vector<GrapheNode> m_GrapheNode;

};

#endif