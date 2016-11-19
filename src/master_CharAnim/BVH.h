#ifndef __BVH_H__
#define __BVH_H__

#include <memory.h>
#include <string>
#include <vector>
#include <cassert>

#include <fstream>
#include <iostream>

#include "BVHAxis.h"

/** @addtogroup BVH
@{
*/

namespace bvh {

	class BVHJoint;


	/** @brief Motion capture skeleton and animation
	*/
	class BVH
	{
		friend class bvh::BVHJoint;
	public:
		//! Default constructor
		BVH();
		//! Destructor
		BVH(const std::string& filename, bool enableEndSite=false)  { init(filename,enableEndSite); }
		~BVH();

		//! init
		void init(const std::string& filename, bool enableEndSite=false);

		//! Return the number of frames
		int getNumFrame(void) const;
		//! Return the dt between 2 frames
		float getFrameTime(void) const;

		//! Return the root joint
		bvh::BVHJoint* getRoot(void) const;
		//! Modify the root
		void setRoot(bvh::BVHJoint* joint);

		//! Return the number of joint
		int getNumJoint(void) const;
		//! Return the i-th joint
		bvh::BVHJoint* getJoint(int i) const;
		//! Return a joint
		bvh::BVHJoint* getJoint(const std::string& name) const;
		//! Return a joint number
		int getJointNumber(const std::string& name) const;

		//! Scaling the animation time
		void scaleAnimation(float factor);
		//! Scaling the skeleton
		void scaleSkeleton(float factor);
		//! Rotate the BVH
		void rotate90(bvh::AXIS axis, bool cw);
		//! Edit the animation in a multi resolution way
		void multiResEditAnimation(const std::vector<float>& coef);

		//! dump
		friend std::ostream& operator << (std::ostream& os, const BVH& bvh);

	protected:
		//! Number of frames
		int m_numFrames;
		//! Time between 2 frames
		float m_frameTime;

		//! Vector of joint
		std::vector<bvh::BVHJoint*> m_joints;

		//! Root Joint
		bvh::BVHJoint* m_root;

	private:
		//! Check if the next word in stream is word
		static bool expect(const std::string& word, std::ifstream& stream);
	};

} // namespace

/** @}
*/

#endif //__BVH_H__
