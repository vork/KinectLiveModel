#pragma once
#define MULT 4
#include "BoneJoint.h"

using namespace std;

#define ORANGE XMFLOAT3(1.0f, 0.596f, 0.0f)
#define GREEN XMFLOAT3(0.298f, 0.686f, 0.314f)
#define DARKBLUE XMFLOAT3(0.247f, 0.318f, 0.71f)
#define LIGHTBLUE XMFLOAT3(0.012f, 0.663f, 0.957f)
#define YELLOW XMFLOAT3(1.0f, 0.922f, 0.231f)

class BoneHierachy
{
public:
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

	static shared_ptr<BoneJoint>	CreateHierarchy();
	static void TraverseHierarchy(shared_ptr<BoneJoint>& node,
		function<void(shared_ptr<BoneJoint>&)> f);
};

