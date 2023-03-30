#ifndef _CHARACTERCONTROLLER_H
#define _CHARACTERCONTROLLER_H


#include "quaternion.h"
#include "Viewer.h"
#include "BVH.h"
#include "Skeleton.h"
#include "TransformQ.h"

#include <PhysicalWorld.h>
#include <cstdio>
#include <iostream>

class CharacterController
    {
    public:
        CharacterController() {
            m_v = 0.0f;
            m_vMax = 50.0f;
        };
        void update(const float dt) {
            m_ch2w = m_ch2w * Translation(m_v * Vector(0, 0, 1));
        };

        void turnXZ(const float& rot_angle_v) {
            m_ch2w = m_ch2w * RotationY(rot_angle_v);
        };

        void accelerate(const float& speed_inc) {
            m_v += speed_inc;

            if(m_v > m_vMax) {
                m_v = m_vMax;
            }
            if(m_v < 0) {
                m_v = 0;
            }

            /*if(m_v > 0 && m_v <= 2) {
                mc_bvh.init( smart_path("data/bvh/motionGraph/marcher.bvh") );
            }
            if(m_v > 2) {
                mc_bvh.init( smart_path("data/bvh/motionGraph/courir.bvh") );
            }*/
        };

        void setVelocityMax(const float vmax) {
            m_vMax = vmax;
        };

        const Point position() const { return m_ch2w(Point(0,0,0)); }
        const Vector direction() const { return m_ch2w(Vector(0, 0, 1)); }
        float velocity() const { return m_v; }
        const Transform& controller2world() const { return m_ch2w; }

        chara::BVH mc_bvh;
        //const chara::BVH setBVH(chara::BVH &&bvh) { mc_bvh = bvh; }

    protected:
        Transform m_ch2w;   // matrice du character vers le monde
                            // le personnage se déplace vers X
                            // il tourne autour de Y
                            // Z est sa direction droite
      
        float m_v;          // le vecteur vitesse est m_v * m_ch2w * Vector(1,0,0)
        float m_vMax;       // ne peut pas accélérer plus que m_vMax
    };

    

#endif