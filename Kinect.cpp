#include "Kinect.h"
#include "System.h"

const float KinectHelper::lenSpineBaseToSpineMid = MULT*0.2f;
const float KinectHelper::lenSpineMidToSpineShoulder = MULT*0.2f;
const float KinectHelper::lenSpineMidToHipRight = MULT*0.1f;
const float KinectHelper::lenHipRightToKneeRight = MULT*0.3f;
const float KinectHelper::lenKneeRightToAnkleRight = MULT*0.2f;
const float KinectHelper::lenAnkleToFoot = MULT*0.1f;
const float KinectHelper::lenShoulderToNeck = MULT*0.05f;
const float KinectHelper::lenNeckToHead = MULT*0.1f;
const float KinectHelper::lenSpineShoulderToShoulder = MULT*0.2f;
const float KinectHelper::lenShoulderToElbow = MULT*0.3f;
const float KinectHelper::lenElbowToWrist = MULT*0.2f;
const float KinectHelper::lenWristToHand = MULT*0.05f;
const float KinectHelper::lenHandToThumb = MULT*0.05f;
const float KinectHelper::lenHandToTip = MULT*0.05f;

KinectHelper::KinectHelper()
{
}


KinectHelper::~KinectHelper()
{
}

bool KinectHelper::Initialize()
{
	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return false;
	}

	if (m_pKinectSensor)
	{
		// Initialize the Kinect and get coordinate mapper and the body reader
		IBodyFrameSource* pBodyFrameSource = NULL;

		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}

		/*if (pBodyFrameSource != NULL) //TODO Don't tidy up. It crashes.
		{
			delete pBodyFrameSource;
			pBodyFrameSource = NULL;
		}*/
	}

	if (!m_pKinectSensor || FAILED(hr))
	{
		//SetStatusMessage(L"No ready Kinect found!", 10000, true); //TODO add error
		return false;
	}

	return true;
}

void KinectHelper::Release()
{
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}

	SafeRelease(m_pKinectSensor);
}

shared_ptr<BoneJoint> KinectHelper::CreateHierarchy()
{
	auto boneHierarchy = make_shared<BoneJoint>(JointType_SpineBase, LIGHTBLUE, 1.0f, L"Spine Base");

	boneHierarchy->AddChild(make_shared<BoneJoint>(JointType_HipRight, YELLOW, lenSpineMidToHipRight, L"Hip Right"))
		->AddChild(make_shared<BoneJoint>(JointType_KneeRight, YELLOW, lenHipRightToKneeRight, L"Knee Right"))
		->AddChild(make_shared<BoneJoint>(JointType_AnkleRight, YELLOW, lenKneeRightToAnkleRight, L"Ankle Right"))
		->AddChild(make_shared<BoneJoint>(JointType_FootRight, YELLOW, lenAnkleToFoot, L"Foot Right"));

	boneHierarchy->AddChild(make_shared<BoneJoint>(JointType_HipLeft, YELLOW, lenSpineMidToHipRight, L"Hip Left"))
		->AddChild(make_shared<BoneJoint>(JointType_KneeLeft, YELLOW, lenHipRightToKneeRight, L"Knee Left"))
		->AddChild(make_shared<BoneJoint>(JointType_AnkleLeft, YELLOW, lenKneeRightToAnkleRight, L"Ankle Left"))
		->AddChild(make_shared<BoneJoint>(JointType_FootLeft, YELLOW, lenAnkleToFoot, L"Foot Left"));

	auto spineShoulder = boneHierarchy->AddChild(make_shared<BoneJoint>(JointType_SpineMid, GREEN, lenSpineBaseToSpineMid, L"Spine Mid"))
		->AddChild(make_shared<BoneJoint>(JointType_SpineShoulder, GREEN, lenSpineMidToSpineShoulder, L"Spine Shoulder"));

	spineShoulder->AddChild(make_shared<BoneJoint>(JointType_Neck, DARKBLUE, lenShoulderToNeck, L"Neck"))
		->AddChild(make_shared<BoneJoint>(JointType_Head, DARKBLUE, lenNeckToHead, L"Head"));

	auto wristRight = spineShoulder->AddChild(make_shared<BoneJoint>(JointType_ShoulderRight, ORANGE, lenSpineShoulderToShoulder, L"Shoulder Right"))
		->AddChild(make_shared<BoneJoint>(JointType_ElbowRight, ORANGE, lenShoulderToElbow, L"Elbow Right"))
		->AddChild(make_shared<BoneJoint>(JointType_WristRight, ORANGE, lenElbowToWrist, L"Wrist Right"));

	auto handRight = wristRight->AddChild(make_shared<BoneJoint>(JointType_HandRight, ORANGE, lenWristToHand, L"Hand Right"));

	wristRight->AddChild(make_shared<BoneJoint>(JointType_ThumbRight, ORANGE, lenHandToThumb, L"Thumb Right"));
	handRight->AddChild(make_shared<BoneJoint>(JointType_HandTipRight, ORANGE, lenHandToTip, L"Hand Tip Right"));

	auto wristLeft = spineShoulder->AddChild(make_shared<BoneJoint>(JointType_ShoulderLeft, ORANGE, lenSpineShoulderToShoulder, L"Shoulder Left"))
		->AddChild(make_shared<BoneJoint>(JointType_ElbowLeft, ORANGE, lenShoulderToElbow, L"Elbow Left"))
		->AddChild(make_shared<BoneJoint>(JointType_WristLeft, ORANGE, lenElbowToWrist, L"Wrist Left"));

	auto handLeft = wristLeft->AddChild(make_shared<BoneJoint>(JointType_HandLeft, ORANGE, lenWristToHand, L"Hand Left"));
	wristLeft->AddChild(make_shared<BoneJoint>(JointType_ThumbLeft, ORANGE, lenHandToThumb, L"Thumb Left"));
	handLeft->AddChild(make_shared<BoneJoint>(JointType_HandTipLeft, ORANGE, lenHandToTip, L"Hand Tip Left"));

	return boneHierarchy;
}

void KinectHelper::TraverseHierarchy(shared_ptr<BoneJoint>& node, function<void(shared_ptr<BoneJoint>&)> f)
{
	if (!node)
		return;

	f(node);

	for (auto child : node->Children())
	{
		TraverseHierarchy(child, f);
	}
}
