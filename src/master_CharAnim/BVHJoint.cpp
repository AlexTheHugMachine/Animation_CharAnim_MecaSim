#include "BVHJoint.h"
#include <BVH.h>
#include <iostream>
using namespace std;
namespace chara {


//=============================================================================
BVHJoint::BVHJoint(const std::string& name, int parent, BVH& bvh, int id)
		: m_name(name)
		, m_parentId(parent)
		, m_bvh(bvh)
		, m_id(id)
{
	setOffset(0, 0, 0);
}
//-----------------------------------------------------------------------------
void BVHJoint::removeChannel(const BVHChannel& channel)
{
	std::vector<BVHChannel>::iterator ite=m_channels.begin();
	while (ite!=m_channels.end() && (*ite!=channel) )
		ite++;

	if (ite!=m_channels.end())
	{
		m_channels.erase(ite);
	}

}

const chara::BVHJoint& BVHJoint::getChild(int i) const { return m_bvh[getChildId(i)]; }
void BVHJoint::addChild(int i) { assert(i >= 0); assert(i < m_bvh.getNumberOfJoint()); m_childs.push_back(i); }
const BVHJoint& BVHJoint::getParent(void) const { assert(m_parentId >= 0); assert(m_parentId < m_bvh.getNumberOfJoint()); return m_bvh[m_parentId]; }
void BVHJoint::setParentId(int parent) { assert(parent >= 0); assert(parent < m_bvh.getNumberOfJoint()); m_parentId = parent; }


//-----------------------------------------------------------------------------
void BVHJoint::removeChannel(int i)
{
	int k=0;
	std::vector<BVHChannel>::iterator ite=m_channels.begin();
	while (ite!=m_channels.end() && k!=i)
	{
		ite++;
		k++;
	}

	if (ite!=m_channels.end())
	{
		m_channels.erase(ite);
	}

	//return result;
}

//----------------------------------------------------------------------------
void BVHJoint::scale(float factor)
{
	// scaling offset
	for (int i=0; i<3; ++i)
		m_offset[i]*=factor;

	// scaling translation channels
	for (int i=0; i<getNumberOfChannel(); ++i)
	{
		BVHChannel& curChannel = getChannel(i);
		if (curChannel.isTranslation())
		{
			curChannel.scale(factor);
		}
	}
}

//----------------------------------------------------------------------------
void BVHJoint::rotate90(AXIS axis, bool cw)
{
	// For a 90° ccw rotation transforms
	// an axis 'from' to another axis 'to' and 'to' to -'from'.
	AXIS from;
	AXIS to;
	// 'from' and 'to' depend on the axis of the rotation 'axis'.
	// Let's find these axis :
	switch (axis)
	{
	case AXIS_X:
		from = AXIS_Y;
		to   = AXIS_Z;
		break;
	case AXIS_Y:
		from = AXIS_Z;
		to   = AXIS_X;
		break;
	case AXIS_Z:
		from = AXIS_X;
		to   = AXIS_Y;
		break;
	default:
		assert(0); // bad parameters
		return;
	}

	// Now, let's transform these axis :
	for (int i=0; i<getNumberOfChannel(); ++i)
	{
		BVHChannel& curChannel = getChannel(i);

		if (curChannel.getAxis() == from)
		{
			// 'from' => 'to'
			curChannel.setAxis(to);
			if (cw)
				curChannel.scale(-1.0f);
		}
		else if (curChannel.getAxis() == to)
		{
			// 'to' => -'from'
			curChannel.setAxis(from);
			if (!cw)
				curChannel.scale(-1.0f);
		}
	}

	// offset switching
	float tmp = m_offset[(int)from];
	if (!cw)
	{
		m_offset[(int)from] = -m_offset[(int)to];
		m_offset[(int)to] = tmp;
	}
	else
	{
		m_offset[(int)from] = m_offset[(int)to];
		m_offset[(int)to] = -tmp;
	}
}

//----------------------------------------------------------------------------
static int g_displayIndex = 0;
std::ostream& operator << (std::ostream& os, const BVHJoint& BVHJoint)
{
	for (int i=0; i<g_displayIndex; ++i) os << "  ";
	os <<" id="<<BVHJoint.getId();
	os <<" name="<<BVHJoint.getName();
	os <<" nChannel=" << BVHJoint.getNumberOfChannel() << " nChildren="<<BVHJoint.getNumberOfChildren()<<"(";
	for(int i=0;i<BVHJoint.getNumberOfChildren();++i) os<<BVHJoint.getChildId(i)<<",";
	os<<")";
	//os << " &m_bvh=" << (void*)&BVHJoint.m_bvh;
	os << " parentId=" << BVHJoint.getParentId();
	os<< " offset=("<<BVHJoint.m_offset[0]<<", "<<BVHJoint.m_offset[1]<<", "<<BVHJoint.m_offset[2]<<")";
	os << std::endl;
	g_displayIndex++;
	for (int i = 0; i < BVHJoint.getNumberOfChildren(); ++i)
		//os << "childNumber="<<i<<"  childId=" << (BVHJoint.getChildId(i)) << " " << BVHJoint.getChild(i) << " ";
		os <<BVHJoint.getChild(i) << " ";
	g_displayIndex--;

	return os;
}
//-----------------------------------------------------------------------------
void BVHJoint::initChannel(std::ifstream& stream)
{

	bool result = true;

	BVH::expect("{", stream);
	BVH::expect("OFFSET", stream);
	stream >> m_offset[0] >> m_offset[1] >> m_offset[2];

	BVH::expect("CHANNELS", stream);
	int numChannels;
	stream >> numChannels;

	for (int i=0; i<numChannels; ++i)
	{
		std::string typeStr;
		BVHChannel::TYPE type;
		AXIS axis;

		stream >> typeStr;

		if (typeStr == "Xposition")
		{
			type = BVHChannel::TYPE_TRANSLATION;
			axis = AXIS_X;
		}
		else if (typeStr == "Yposition")
		{
			type = BVHChannel::TYPE_TRANSLATION;
			axis = AXIS_Y;
		}
		else if (typeStr == "Zposition")
		{
			type = BVHChannel::TYPE_TRANSLATION;
			axis = AXIS_Z;
		}
		else if (typeStr == "Xrotation")
		{
			type = BVHChannel::TYPE_ROTATION;
			axis = AXIS_X;
		}
		else if (typeStr == "Yrotation")
		{
			type = BVHChannel::TYPE_ROTATION;
			axis = AXIS_Y;
		}
		else if (typeStr == "Zrotation")
		{
			type = BVHChannel::TYPE_ROTATION;
			axis = AXIS_Z;
		}
		else if (typeStr == "Wrotation")
		{
			type = BVHChannel::TYPE_ROTATION;
			axis = AXIS_W;
		}
		else
		{
			std::cerr << "ERROR : bad channel type : '" << typeStr << "'." << std::endl;
			result = false;
			continue;
		}

		addChannel(BVHChannel(type, axis));
	}

	if (!result)
		std::cerr << "ERROR during the '" << m_name << "' BVHJoint creation." << std::endl;
}

//=============================================================================

} // namespace
