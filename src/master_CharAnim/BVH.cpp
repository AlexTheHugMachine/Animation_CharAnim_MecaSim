#include "BVH.h"
#include "BVHJoint.h"
#include <iostream>

using namespace std;
namespace chara {


//=============================================================================
BVH::BVH()
 : m_numFrames(0)
 , m_frameTime(0)
 , m_rootId(-1)
{
}

const chara::BVHJoint& BVH::getRoot(void) const { assert(m_rootId >= 0); assert(m_rootId < m_joints.size()); return m_joints[m_rootId]; }


//-----------------------------------------------------------------------------
void BVH::init(const std::string& filename, bool enableEndSite)
{
    std::ifstream stream(filename.c_str());

    if(stream.is_open())
    {
        //std::cout << "Parsing '" << filename << "'...";

        expect("HIERARCHY", stream);
        expect("ROOT", stream);

        //m_joints.reserve( 100 );

        std::string rootName;
        stream >> rootName;

		m_rootId = addJoint(rootName, -1);
		init(stream, enableEndSite, m_rootId);

        expect("MOTION", stream);
        expect("Frames:", stream);
        stream >> m_numFrames;

        expect("Frame", stream);
        expect("Time:", stream);
        stream >> m_frameTime;

        int i,j,k;
		for (i = 0; i<(int)m_joints.size(); ++i)
			for(j=0; j<(int)m_joints[i].getNumberOfChannel(); ++j)
				m_joints[i].getChannel(j).setDataSize(m_numFrames);

        for(k=0; k<(int)m_numFrames; ++k)
        {
			for (i = 0; i < (int)m_joints.size(); ++i)
			{
				for (j = 0; j < (int)m_joints[i].getNumberOfChannel(); ++j)
				{
					float data;
					stream >> data;
					m_joints[i].getChannel(j).setData(k, data);
				}
			}
        }

    }
    else // file==0
    {
        // can't open m_filename
        std::cerr << "ERROR : Unable to open " << filename << std::endl;
		assert(0);
    }
}


//----------------------------------------------------------------------------
std::string BVH::getEndSiteName(const std::string& parentName)
{
	std::string result;
	if (parentName=="RWrist")
		result="RHand";
	else if (parentName=="LWrist")
		result="LHand";
	else if (parentName=="RAnkle")
		result="RFoot";
	else if (parentName=="LAnkle")
		result="LFoot";
	else
		result=parentName+"_ES";

	return result;
}


void BVH::init(std::ifstream& stream, bool enableEndSite, int id)
{
    m_joints[id].initChannel(stream);

	std::string str;
	stream >> str;

	while (str!="}")
	{
		if (str=="JOINT")
		{
			stream >> str;
			//addChild(new BVHJoint(str, this, bvh, stream, channels, enableEndSite));
			int childId = addJoint(str,id);
			init(stream, enableEndSite, childId);
		}
		else
		if (str=="End")
		{
			BVH::expect("Site", stream);
			BVH::expect("{", stream);
			BVH::expect("OFFSET", stream);
			float x, y, z;
			stream >> x >> y >> z;
			if (enableEndSite)
			{
				std::string nameES = getEndSiteName(m_joints[id].getName());
				//addChild(new BVHJoint(nameES, this, bvh, offset));
				int childId = addJoint(nameES, id);
				(*this)[childId].setOffset(x, y, z);
			}
			BVH::expect("}", stream);
		}
		else
		{
			std::cerr << "ERROR : unexpected word : '" << str << "'." << std::endl;
		}

		stream >> str;
	}
}


int BVH::addJoint(const std::string& name, int parentId)
{
    assert( parentId<(int)m_joints.size() );
	int id = int(m_joints.size());
	m_joints.push_back( BVHJoint(name, parentId, *this, m_joints.size()));
	if (parentId>=0) m_joints[parentId].addChild( id );
	return id;
}

//----------------------------------------------------------------------------
int BVH::getJointId(const std::string& name) const
{
    int i=0;
    while(i<getNumberOfJoint() && getJoint(i).getName()!=name)
        ++i;

    if(i<getNumberOfJoint())
        return i;
    else
        return -1;
}


//----------------------------------------------------------------------------
void BVH::scaleAnimation(float factor)
{
    assert(factor>0);
    m_frameTime*=factor;
}
//----------------------------------------------------------------------------
void BVH::scaleSkeleton(float factor)
{
    assert(factor>0);

    for(int i=0; i<getNumberOfJoint(); ++i)
        getJoint(i).scale(factor);
}
//----------------------------------------------------------------------------
void BVH::rotate90(chara::AXIS axis, bool cw)
{
    for(int i=0; i<getNumberOfJoint(); ++i)
        getJoint(i).rotate90(axis, cw);
}
//----------------------------------------------------------------------------
bool BVH::expect(const std::string& word, std::ifstream& stream)
{
    bool result=true;

    std::string str;
    stream >> str;

    if(str!=word)
    {
        std::cerr << "ERROR : Unexpected string in the file stream : '" << str <<
            "' have been founded where '" << word << "' was expected." << std::endl;
        result =  false;
    }

    return result;
}


//============================================================================
std::ostream& operator << (std::ostream& os, const BVH& bvh)
{
    os << (bvh.getRootId())
       << "Number of frames : " << bvh.getNumberOfFrame() << std::endl
       << "Animation time   : " << bvh.getFrameTime()*(bvh.getNumberOfFrame()-1) << " s" << std::endl;
    os<<bvh.getRoot()<<endl;
    return os;
}


//============================================================================

void BVH::multiResEditAnimation(const std::vector<float>& coef)
{
    unsigned int i;
    int j;
    for(i=0;i<m_joints.size();++i)
    {
        for(j=0;j<m_joints[i].getNumberOfChannel();++j)
            m_joints[i].getChannel(j).regenerateDataFromMultiResolution(coef);
    }
}


} // namespace
