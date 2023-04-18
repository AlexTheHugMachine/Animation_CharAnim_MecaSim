#ifndef _CHARACTERCONTROLLER_H
#define _CHARACTERCONTROLLER_H


#include "quaternion.h"
#include "Viewer.h"
#include "BVH.h"
#include "Skeleton.h"
#include "TransformQ.h"
#include "MotionGraph.h"

#include <PhysicalWorld.h>
#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
	

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
        };

        void setVelocityMax(const float vmax) {
            m_vMax = vmax;
        };

        std::vector<chara::BVH> mc_bvh;

        const Point position() const { return m_ch2w(Point(0,0,0)); }
        const Vector direction() const { return m_ch2w(Vector(0, 0, 1)); }
        float velocity() const { return m_v; }
        const Transform& controller2world() const { return m_ch2w; }
        void setMotionGraph(MotionGraph &mg) { motionGraph = mg; }
        void setNbFrame(int nb) { nb_frame = nb; }
        MotionGraph getMotionGraph() { return motionGraph; }

        void setBvhMotionGraph() { 
            mc_bvh.resize(4);
            motionGraph.m_BVH.resize(4);
            motionGraph.m_GrapheNode.resize(4);

            mc_bvh[0].init( smart_path("data/bvh/motionGraph/null.bvh") );
            motionGraph.m_BVH[0].init( smart_path("data/bvh/motionGraph/null.bvh") );
            motionGraph.m_GrapheNode.push_back(MotionGraph::GrapheNode());
            motionGraph.m_GrapheNode[0].id_bvh = 0;
            motionGraph.m_GrapheNode[0].frame = nb_frame;
            motionGraph.m_GrapheNode[0].ids_next.resize(2);
            motionGraph.m_GrapheNode[0].ids_next[0] = 1;    // marcher
            motionGraph.m_GrapheNode[0].ids_next[1] = 3;    // frapper

            mc_bvh[1].init( smart_path("data/bvh/motionGraph/marcher.bvh") );
            motionGraph.m_BVH[1].init( smart_path("data/bvh/motionGraph/marcher.bvh") );
            motionGraph.m_GrapheNode.push_back(MotionGraph::GrapheNode());
            motionGraph.m_GrapheNode[1].id_bvh = 1;
            motionGraph.m_GrapheNode[1].frame = nb_frame;
            motionGraph.m_GrapheNode[1].ids_next.resize(3);
            motionGraph.m_GrapheNode[1].ids_next[0] = 2;    // courir
            motionGraph.m_GrapheNode[1].ids_next[1] = 3;    // frapper
            motionGraph.m_GrapheNode[1].ids_next[2] = 0;    // null

            mc_bvh[2].init( smart_path("data/bvh/motionGraph/courir.bvh") );
            motionGraph.m_BVH[2].init( smart_path("data/bvh/motionGraph/courir.bvh") );
            motionGraph.m_GrapheNode.push_back(MotionGraph::GrapheNode());
            motionGraph.m_GrapheNode[2].id_bvh = 2;
            motionGraph.m_GrapheNode[2].frame = nb_frame;
            motionGraph.m_GrapheNode[2].ids_next.resize(2);
            motionGraph.m_GrapheNode[2].ids_next[0] = 1;    // marcher
            motionGraph.m_GrapheNode[2].ids_next[1] = 3;    // frapper

            mc_bvh[3].init( smart_path("data/bvh/motionGraph/frapper.bvh") );
            motionGraph.m_BVH[3].init( smart_path("data/bvh/motionGraph/frapper.bvh") );
            motionGraph.m_GrapheNode.push_back(MotionGraph::GrapheNode());
            motionGraph.m_GrapheNode[3].id_bvh = 3;
            motionGraph.m_GrapheNode[3].frame = nb_frame;
            motionGraph.m_GrapheNode[3].ids_next.resize(1);
            motionGraph.m_GrapheNode[2].ids_next[0] = 0;    // null
        }

        chara::BVH getBVH(int id) { return mc_bvh[id]; }

    protected:
        Transform m_ch2w;   // matrice du character vers le monde
                            // le personnage se déplace vers X
                            // il tourne autour de Y
                            // Z est sa direction droite
      
        float m_v;          // le vecteur vitesse est m_v * m_ch2w * Vector(1,0,0)
        float m_vMax;       // ne peut pas accélérer plus que m_vMax
        MotionGraph motionGraph;
        int nb_frame;
    };

    

#endif