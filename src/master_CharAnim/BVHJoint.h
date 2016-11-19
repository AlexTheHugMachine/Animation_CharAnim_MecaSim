#ifndef __BVHJOINT_H__
#define __BVHJOINT_H__

#include <string>
#include <vector>
#include <cassert>

#include <iostream>
#include <fstream>

#include "BVH.h"
#include "BVHChannel.h"
#include "BVHAxis.h"

/** @addtogroup BVH
	@{
*/

namespace bvh {


/** @brief Motion capture bone and bone animation

	@remark
		Contains the description and the evolution of a bone.
*/
class BVHJoint
{
	friend class bvh::BVH;
public:
	/** @brief Constructor
		@pre bvh!=0
		@pre offset!=0
	*/
	BVHJoint(const std::string& name, BVHJoint* parent, bvh::BVH* bvh, float* offset);
	//! Destructor (recursive)
	~BVHJoint();

	//! Return the name of the Joint
	std::string getName(void) const;
	//! Modify the name of the Joint
	void setName(const std::string& name);

	//! Return the offset of the joint
	void getOffset(float& x, float& y, float& z) const;
	//! Modify the offset of the joint
	void setOffset(float x, float y, float z);

	//! Return the number of channels in the Joint
	int getNumChannel(void) const;
	//! Return the i-th channel of the Joint
	bvh::BVHChannel* getChannel(int i) const;
	//! modify the i-th channel of the Joint
	void SetChannel(int i, bvh::BVHChannel* channel);
	//! Add channel
	void addChannel(bvh::BVHChannel* channel);
	//! Remove channel
	bvh::BVHChannel* removeChannel(bvh::BVHChannel* channel);
	//! Remove channel
	bvh::BVHChannel* removeChannel(int i);

	//! Return the number of child
	int getNumChild(void) const;
	//! Return a child
	bvh::BVHJoint* getChild(int i) const;
	//! Add child
	void addChild(bvh::BVHJoint* joint);

	//! Return the parent of the Joint
	BVHJoint* getParent(void) const;
	//! Modify the parent of the joint
	void setParent(bvh::BVHJoint* parent);

	//! Return the bvh
	bvh::BVH* getBVH(void) const;
	//! Modify the bvh
	void setBVH(bvh::BVH* bvh);

	//! Scaling the joint
	void scale(float factor);
	//! Rotate the bvh (recursive)
	void rotate90(AXIS axis, bool cw);

	//! dump (recursive)
	friend std::ostream& operator << (std::ostream& os, const BVHJoint& joint);

protected:
	/** @brief Constructor from file stream (recursive)
		@pre bvh!=0
	*/
	BVHJoint(const std::string& name, BVHJoint* parent, bvh::BVH* bvh,
		  std::ifstream& stream, std::vector<BVHChannel*>& channels,
		  bool enableEndSite);
	//! Return the best end name from it parent e.g. RHand from RWrist
	static std::string getEndSiteName(const std::string& parentName);

	//! Joint name
	std::string m_name;
	//! Offset vector
	float m_offset[3];

	//! Channels
	std::vector<bvh::BVHChannel*> m_channels;

	//! Childs
	std::vector<bvh::BVHJoint*> m_childs;
	//! The parent
	bvh::BVHJoint* m_parent;

	//! The BVH
	bvh::BVH* m_bvh;
};

} // namespace

/** @}
*/

#endif //__JOINT_H__
