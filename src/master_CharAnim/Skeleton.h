#ifndef _SKELETON_H
#define _SKELETON_H

#include "BVH.h"
#include "mat.h"

class Skeleton
{
public:

   struct SkeletonJoint
   {
      int m_parentId;    // Le numéro du père dans le tableau de CAJoint de CASkeleton
      Transform m_l2w;  // La matrice passant du repère de l'articulation vers le monde
   };

   Skeleton() {}

   //! Créer un squelette ayant la même structure que définit dans le BVH c'est à dire
   //! creer le tableau de SkeletonJoint à la bonne taille, avec les parentId initialsé pour chaque case
   void init(const chara::BVH& bvh);

   //! Renvoie la position de l'articulation i en multipliant le m_l2w par le Point(0,0,0)
   Point getJointPosition(int i) const;

  //! Renvoie l'identifiant de l'articulation père de l'articulation numéro i
   int getParentId(const int i) const;

   //! Renvoie le nombre d'articulation
   int numberOfJoint() const { return (int)m_joints.size();  }

   //! Positionne ce squelette dans la position n du BVH.
   //! Assez proche de la fonction récursive (question 1), mais range la matrice (Transform)
   //! dans la case du tableau. Pour obtenir la matrice allant de l'articulation local vers le monde,
   //! il faut multiplier la matrice allant de l'articulation vers son père à la matrice du père allant de
   //! l'articulation du père vers le monde.
   void setPose(const chara::BVH& bvh, int frameNumber);


   //! Positionne ce squelette entre la position frameNbSrc du BVH Src et la position frameNbDst du bvh Dst
   //void setPoseInterpolation(const BVH& bvhSrc, int frameNbSrc, const BVH& bvhDst, int frameNbDst, float t);

   //! Positionne ce squelette entre la position frameNbSrc du BVH Src et la position frameNbDst du bvh Dst
   //! idem à setPoseInterpolation mais interpole avec des quaternions sur chaque articulations
   //void setPoseInterpolationQ(const BVH& bvhSrc, int frameNbSrc, const BVH& bvhDst, int frameNbDst, float t);

   //! Calcule la distance entre deux poses
   //! precond: les deux squelettes doivent avoir le
   //! même nombre d'articulations (même structure d'arbre)
   //! ==> Sera utile lors de la construction du graphe d'animation
   // friend float distance(const CASkeleton& a, const CASkeleton& b);


protected:
   //! L'ensemble des articulations.
   //! Remarque : la notion de hiérarchie (arbre) n'est plus nécessaire ici,
   //! pour tracer les os on utilise l'information "parentID" de la class CAJoint
   std::vector<SkeletonJoint> m_joints;
};

#endif
