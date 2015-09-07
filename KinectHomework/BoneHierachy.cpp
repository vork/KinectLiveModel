#include "pch.h"
#include "BoneHierachy.h"

const float BoneHierachy::lenSpineBaseToSpineMid = MULT*0.2f;
const float BoneHierachy::lenSpineMidToSpineShoulder = MULT*0.2f;
const float BoneHierachy::lenSpineMidToHipRight = MULT*0.1f;
const float BoneHierachy::lenHipRightToKneeRight = MULT*0.3f;
const float BoneHierachy::lenKneeRightToAnkleRight = MULT*0.2f;
const float BoneHierachy::lenAnkleToFoot = MULT*0.1f;
const float BoneHierachy::lenShoulderToNeck = MULT*0.05f;
const float BoneHierachy::lenNeckToHead = MULT*0.1f;
const float BoneHierachy::lenSpineShoulderToShoulder = MULT*0.2f;
const float BoneHierachy::lenShoulderToElbow = MULT*0.3f;
const float BoneHierachy::lenElbowToWrist = MULT*0.2f;
const float BoneHierachy::lenWristToHand = MULT*0.05f;
const float BoneHierachy::lenHandToThumb = MULT*0.05f;
const float BoneHierachy::lenHandToTip = MULT*0.05f;

//Create a hierachy for the affine transformations
shared_ptr<BoneJoint> BoneHierachy::CreateHierarchy()
{
	auto boneHierarchy = make_shared<BoneJoint>(JointType::SpineBase, LIGHTBLUE, 1.0f, L"Spine Base");

	boneHierarchy->AddChild(make_shared<BoneJoint>(JointType::HipRight, YELLOW, lenSpineMidToHipRight, L"Hip Right"))
		->AddChild(make_shared<BoneJoint>(JointType::KneeRight, YELLOW, lenHipRightToKneeRight, L"Knee Right"))
		->AddChild(make_shared<BoneJoint>(JointType::AnkleRight, YELLOW, lenKneeRightToAnkleRight, L"Ankle Right"))
		->AddChild(make_shared<BoneJoint>(JointType::FootRight, YELLOW, lenAnkleToFoot, L"Foot Right"));

	boneHierarchy->AddChild(make_shared<BoneJoint>(JointType::HipLeft, YELLOW, lenSpineMidToHipRight, L"Hip Left"))
		->AddChild(make_shared<BoneJoint>(JointType::KneeLeft, YELLOW, lenHipRightToKneeRight, L"Knee Left"))
		->AddChild(make_shared<BoneJoint>(JointType::AnkleLeft, YELLOW, lenKneeRightToAnkleRight, L"Ankle Left"))
		->AddChild(make_shared<BoneJoint>(JointType::FootLeft, YELLOW, lenAnkleToFoot, L"Foot Left"));

	auto spineShoulder = boneHierarchy->AddChild(make_shared<BoneJoint>(JointType::SpineMid, GREEN, lenSpineBaseToSpineMid, L"Spine Mid"))
		->AddChild(make_shared<BoneJoint>(JointType::SpineShoulder, GREEN, lenSpineMidToSpineShoulder, L"Spine Shoulder"));

	spineShoulder->AddChild(make_shared<BoneJoint>(JointType::Neck, DARKBLUE, lenShoulderToNeck, L"Neck"))
		->AddChild(make_shared<BoneJoint>(JointType::Head, DARKBLUE, lenNeckToHead, L"Head"));

	auto wristRight = spineShoulder->AddChild(make_shared<BoneJoint>(JointType::ShoulderRight, ORANGE, lenSpineShoulderToShoulder, L"Shoulder Right"))
		->AddChild(make_shared<BoneJoint>(JointType::ElbowRight, ORANGE, lenShoulderToElbow, L"Elbow Right"))
		->AddChild(make_shared<BoneJoint>(JointType::WristRight, ORANGE, lenElbowToWrist, L"Wrist Right"));

	auto handRight = wristRight->AddChild(make_shared<BoneJoint>(JointType::HandRight, ORANGE, lenWristToHand, L"Hand Right"));

	wristRight->AddChild(make_shared<BoneJoint>(JointType::ThumbRight, ORANGE, lenHandToThumb, L"Thumb Right"));
	handRight->AddChild(make_shared<BoneJoint>(JointType::HandTipRight, ORANGE, lenHandToTip, L"Hand Tip Right"));

	auto wristLeft = spineShoulder->AddChild(make_shared<BoneJoint>(JointType::ShoulderLeft, ORANGE, lenSpineShoulderToShoulder, L"Shoulder Left"))
		->AddChild(make_shared<BoneJoint>(JointType::ElbowLeft, ORANGE, lenShoulderToElbow, L"Elbow Left"))
		->AddChild(make_shared<BoneJoint>(JointType::WristLeft, ORANGE, lenElbowToWrist, L"Wrist Left"));

	auto handLeft = wristLeft->AddChild(make_shared<BoneJoint>(JointType::HandLeft, ORANGE, lenWristToHand, L"Hand Left"));
	wristLeft->AddChild(make_shared<BoneJoint>(JointType::ThumbLeft, ORANGE, lenHandToThumb, L"Thumb Left"));
	handLeft->AddChild(make_shared<BoneJoint>(JointType::HandTipLeft, ORANGE, lenHandToTip, L"Hand Tip Left"));

	return boneHierarchy;
}

//Traverse through the bone hierachy and execute a fucntion on each bone
void BoneHierachy::TraverseHierarchy(shared_ptr<BoneJoint>& node, std::function<void(shared_ptr<BoneJoint>&)> f)
{
	if (!node)
		return;

	f(node);

	for (auto child : node->Children())
	{
		TraverseHierarchy(child, f);
	}
}

