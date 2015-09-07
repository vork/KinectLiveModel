#include "BoneJoint.h"

BoneJoint::BoneJoint(_JointType type, XMFLOAT3 colour, float boneLength, wstring name) :
m_jointType(type), m_color(colour), m_boneLength(boneLength), m_name(name)
{
}

const shared_ptr<BoneJoint> BoneJoint::AddChild(shared_ptr<BoneJoint> child)
{
	child->SetParent(shared_ptr<BoneJoint>(this));
	m_children.push_back(child);
	return child;
}