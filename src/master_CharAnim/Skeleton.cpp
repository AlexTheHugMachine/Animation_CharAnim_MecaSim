
#include "Skeleton.h"

using namespace chara;

//! Créer un squelette ayant la même structure que définit dans le BVH c'est à dire
//! creer le tableau de SkeletonJoint à la bonne taille, avec les parentId initialsé pour chaque case
void Skeleton::init(const BVH& bvh)
{
    //TODO Done
    int NBJoints = bvh.getNumberOfJoint();
    m_joints.resize(NBJoints);

    for (int i = 0; i < NBJoints; i++)
    {
        m_joints[i].m_parentId = bvh.getJoint(i).getParentId();
        m_joints[i].m_l2w = Transform();
    }
}

//! Renvoie la position de l'articulation i en multipliant le m_l2w par le Point(0,0,0)
Point Skeleton::getJointPosition(int i) const
{
    // TODO Done
    //int ParentID = m_joints[i].m_parentId;
    Transform l2w = m_joints[i].m_l2w;
    //Transform l2p = m_joints[ParentID].m_l2w;

    //l2w = l2p * l2w;

    return l2w(Point(0, 0, 0));
}

//! Renvoie l'identifiant de l'articulation père de l'articulation numéro i
int Skeleton::getParentId(const int i) const
{
    // TODO Done
    return m_joints[i].m_parentId;
}

//! Positionne ce squelette dans la position n du BVH. 
//! Assez proche de la fonction récursive (question 1), mais range la matrice (Transform)
//! dans la case du tableau. Pour obtenir la matrice allant de l'articulation local vers le monde,
//! il faut multiplier la matrice allant de l'articulation vers son père à la matrice du père allant de
//! l'articulation du père vers le monde.
void Skeleton::setPose(const BVH& bvh, int frameNumber, CharacterController& controller, bool racine)
{
    // TODO
    // Parcourir toutes les articulations (SkeletonJoint ou BVHJoint) 
    //     Declarer la matrice l2f (pere<-local)
    //     Init avec la teanslation Sffset
    //     Parcourir tous les channels
    //          Accumuler dans la matrice l2f les translations et rotation en fonction du type de Channel
    // Multiplier la matrice l2f avec la matrice l2w (world<-local) du p�re qui est d�j� stock� dans le tableau 
    // Attention il peut ne pas y avoir de p�re (pour la racine)
    // Stocker la matrice l2w (world<-local) dans le tableau

    for (int i = 0; i < bvh.getNumberOfJoint(); i++)
    {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        bvh.getJoint(i).getOffset(x, y, z);
        Transform l2f = Translation(Vector(x, y, z));

        for (int j = 0; j < bvh.getJoint(i).getNumberOfChannel(); j++)
        {
            BVHChannel::TYPE type = bvh.getJoint(i).getChannel(j).getType();
            float value = bvh.getJoint(i).getChannel(j).getData(frameNumber);

            if (type == BVHChannel::TYPE_TRANSLATION)
            {
                if(bvh.getJoint(i).getChannel(j).getAxis() == AXIS::AXIS_X)
                    l2f = l2f * Translation(Vector(value, 0, 0));
                else if (bvh.getJoint(i).getChannel(j).getAxis() == AXIS::AXIS_Y)
                    l2f = l2f * Translation(Vector(0, value, 0));
                else if (bvh.getJoint(i).getChannel(j).getAxis() == AXIS::AXIS_Z)
                    l2f = l2f * Translation(Vector(0, 0, value));
            }
            if (type == BVHChannel::TYPE_ROTATION)
            {
                if (bvh.getJoint(i).getChannel(j).getAxis() == AXIS::AXIS_X)
                    l2f = l2f * Rotation(Vector(1, 0, 0), value);
                else if (bvh.getJoint(i).getChannel(j).getAxis() == AXIS::AXIS_Y)
                    l2f = l2f * Rotation(Vector(0, 1, 0), value);
                else if (bvh.getJoint(i).getChannel(j).getAxis() == AXIS::AXIS_Z)
                    l2f = l2f * Rotation(Vector(0, 0, 1), value);
            }
        }

        if (m_joints[i].m_parentId != -1)
        {
            m_joints[i].m_l2w = m_joints[getParentId(i)].m_l2w * l2f;
        }   
        else
        {
            if(racine) {
                l2f = Identity();
            }
            m_joints[i].m_l2w = l2f * controller.controller2world();
        }
    }
}

// Positionne ce skelette entre la position frameNbSrc du BVH Src et la position frameNbDst du bvh Dst
void Skeleton::setPoseInterpolation(const chara::BVH& bvhSrc, int frameNbSrc, const chara::BVH& bvhDst, int frameNbDst, float t, CharacterController& controller, bool racine) {

    // Interpolate between the source and destination frames
    for (int i = 0; i < bvhSrc.getNumberOfJoint(); i++) {
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        
        // Get the offset for the current joint
        bvhSrc.getJoint(i).getOffset(x, y, z);
        Transform l2f = Translation(Vector(x, y, z));

        Skeleton skSrc;
        Skeleton skDst;
        skSrc.init(bvhSrc);
        skSrc.setPose(bvhSrc, frameNbSrc, controller, racine);
        skDst.init(bvhDst);
        skDst.setPose(bvhDst, frameNbDst, controller, racine);
        //t = distance(skSrc, skDst) / 1000;
        //std::cout << "t : " << t << std::endl;
        if(distance(skSrc, skDst) < 120) {
            t = 0;
        }
        else if(distance(skSrc, skDst) < 140 && distance(skSrc, skDst) > 120) {
            t = 0.25;
        }
        else if(distance(skSrc, skDst) < 160 && distance(skSrc, skDst) > 140) {
            t = 0.5;
        }
        else if(distance(skSrc, skDst) < 180 && distance(skSrc, skDst) > 160) {
            t = 0.75;
        }
        else {
            t = 1;
        }

        // Interpolate the channel data for the current joint
        for (int j = 0; j < bvhSrc.getJoint(i).getNumberOfChannel(); j++) {
            BVHChannel::TYPE type = bvhSrc.getJoint(i).getChannel(j).getType();
            float valueSrc = bvhSrc.getJoint(i).getChannel(j).getData(frameNbSrc);
            float valueDst = bvhDst.getJoint(i).getChannel(j).getData(frameNbDst);
            float frameNumperInterp = frameNbSrc * (1 - t) + frameNbDst * t;
            float value = bvhSrc.getJoint(i).getChannel(j).getData(frameNumperInterp);
            
            if (type == BVHChannel::TYPE_TRANSLATION) {
                if (bvhSrc.getJoint(i).getChannel(j).getAxis() == AXIS::AXIS_X) {
                    l2f = l2f * Translation(Vector(value, 0, 0));
                }
                else if (bvhSrc.getJoint(i).getChannel(j).getAxis() == AXIS::AXIS_Y) {
                    l2f = l2f * Translation(Vector(0, value, 0));
                }
                else if (bvhSrc.getJoint(i).getChannel(j).getAxis() == AXIS::AXIS_Z) {
                    l2f = l2f * Translation(Vector(0, 0, value));
                }
            }
            else if (type == BVHChannel::TYPE_ROTATION) {
                if (bvhSrc.getJoint(i).getChannel(j).getAxis() == AXIS::AXIS_X) {
                    l2f = l2f * Rotation(Vector(1, 0, 0), value);
                }
                else if (bvhSrc.getJoint(i).getChannel(j).getAxis() == AXIS::AXIS_Y) {
                    l2f = l2f * Rotation(Vector(0, 1, 0), value);
                }
                else if (bvhSrc.getJoint(i).getChannel(j).getAxis() == AXIS::AXIS_Z) {
                    l2f = l2f * Rotation(Vector(0, 0, 1), value);
                }
            }
        }

        // Compute the local-to-world transformation for the current joint
        if (m_joints[i].m_parentId != -1) {
            m_joints[i].m_l2w = m_joints[getParentId(i)].m_l2w * l2f;
        }
        else {
            if (racine) {
                l2f = Identity();
            }
            m_joints[i].m_l2w = l2f * controller.controller2world();
        }
    }
}


void Skeleton::setPoseInterpolationQ(const chara::BVH& bvhSrc, int frameNbSrc, const chara::BVH& bvhDst, int frameNbDst, float t, CharacterController& controller, bool racine) {

    Skeleton skSrc;
    Skeleton skDst;
    skSrc.init(bvhSrc);
    skSrc.setPose(bvhSrc, frameNbSrc, controller, racine);
    skDst.init(bvhDst);
    skDst.setPose(bvhDst, frameNbDst, controller, racine);

    float moyFrame = (frameNbSrc + frameNbDst) / 2.0f;
    float dist = distance(skSrc, skDst);
    std::cout << "dist : " << dist << std::endl;
    std::cout << "moyFrame : " << moyFrame << std::endl;

    for(int i = 0; i < skSrc.m_joints.size(); i++) {
        Transform l2w = skSrc.m_joints[i].m_l2w;
        Transform l2wDst = skDst.m_joints[i].m_l2w;
        Transform l2wInterp = l2w * (1 - t);
        l2wInterp(l2wDst * t);
        m_joints[i].m_l2w = l2wInterp;
    }
}