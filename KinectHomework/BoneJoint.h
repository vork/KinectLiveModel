#pragma once

using namespace std;
using namespace WindowsPreview::Kinect;
using namespace DirectX;

class BoneJoint
{
public:
	BoneJoint(JointType type, XMFLOAT3 colour, float boneLength, wstring name);

	const shared_ptr<BoneJoint> AddChild(shared_ptr<BoneJoint> child);

	const vector<shared_ptr<BoneJoint>>& Children()
	{
		return m_children;
	}

	XMFLOAT3 getColor()
	{
		return m_color;
	}

	void setColor(XMFLOAT3 col)
	{
		m_color = col;
	}

	const wstring& Name()
	{
		return m_name;
	}

	void SetParent(shared_ptr<BoneJoint> parent)
	{
		m_parent = parent;
	}

	const shared_ptr<BoneJoint>& Parent()
	{
		return m_parent;
	}

	float BoneLength()
	{
		return m_boneLength;
	}

	const JointType& JointType()
	{
		return m_jointType;
	}

	XMVECTOR m_transformed;
	JointOrientation m_orientation;

private:
	shared_ptr<BoneJoint> m_parent;
	vector<shared_ptr<BoneJoint>> m_children;
	float m_boneLength;
	XMFLOAT3 m_color;
	WindowsPreview::Kinect::JointType m_jointType;
	wstring m_name;
};

