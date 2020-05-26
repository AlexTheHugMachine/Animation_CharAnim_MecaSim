
#include "Skeleton.h"

using namespace chara;

void Skeleton::init(const BVH& bvh)
{
    //TODO
}


Point Skeleton::getJointPosition(int i) const
{
    // TODO
    return Point(0, 0, 0);
}


int Skeleton::getParentId(const int i) const
{
    // TODO
    return 0;
}


void Skeleton::setPose(const BVH& bvh, int frameNumber)
{
    // TODO
    // Parcourir toutes les articulations (SkeletonJoint ou BVHJoint) 
    //     Declarer la matrice l2f (pere<-local)
    //     Init avec la teanslation Sffset
    //     Parcourir tous les channels
    //          Accumuler dans la matrice l2f les translations et rotation en fonction du type de Channel
    // Multiplier la matrice l2f avec la matrice l2w (world<-local) du père qui est déjà stocké dans le tableau 
    // Attention il peut ne pas y avoir de père (pour la racine)
                
}
