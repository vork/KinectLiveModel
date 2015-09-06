#pragma once

#include <Kinect.h>
#include "RigJoint.h"

#define MULT 5

#define ORANGE XMFLOAT3(1.0f, 0.596f, 0.0f)
#define GREEN XMFLOAT3(0.298f, 0.686f, 0.314f)
#define DARKBLUE XMFLOAT3(0.247f, 0.318f, 0.71f)
#define LIGHTBLUE XMFLOAT3(0.012f, 0.663f, 0.957f)
#define YELLOW XMFLOAT3(1.0f, 0.922f, 0.231f)
#include <functional>
#include <memory>

using namespace std;

class KinectHelper
{
public:
	KinectHelper();
	~KinectHelper();

	bool Initialize();
	void Release();


	IKinectSensor* m_p_kinect_sensor() const
	{
		return m_pKinectSensor;
	}

	IBodyFrameReader* m_p_body_frame_reader() const
	{
		return m_pBodyFrameReader;
	}

	ICoordinateMapper* m_p_coordinate_mapper() const
	{
		return m_pCoordinateMapper;
	}

	static shared_ptr<RigJoint> CreateBoneHierarchy();
	static void TraverseBoneHierarchy(shared_ptr<RigJoint>& node,
		function<void(shared_ptr<RigJoint>&)> f);

	static const float lenSpineBaseToSpineMid;
	static const float lenSpineMidToSpineShoulder;
	static const float lenSpineMidToHipRight;
	static const float lenHipRightToKneeRight;
	static const float lenKneeRightToAnkleRight;
	static const float lenAnkleToFoot;
	static const float lenShoulderToNeck;
	static const float lenNeckToHead;
	static const float lenSpineShoulderToShoulder;
	static const float lenShoulderToElbow;
	static const float lenElbowToWrist;
	static const float lenWristToHand;
	static const float lenHandToThumb;
	static const float lenHandToTip;

private:
	IKinectSensor* m_pKinectSensor;
	IBodyFrameReader* m_pBodyFrameReader;
	ICoordinateMapper* m_pCoordinateMapper;
};
