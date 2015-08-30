//------------------------------------------------------------------------------
// <copyright file="BodyBasics.h" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#pragma once
#include <d2d1.h>
#include "Kinect.h"
#include "System.h"


class CBodyBasics
{
public:
	/// <summary>
	/// Constructor
	/// </summary>
	CBodyBasics(Kinect*, System*);

	/// <summary>
	/// Destructor
	/// </summary>
	~CBodyBasics();
private:
	static const int cDepthWidth = 512;
	static const int cDepthHeight = 424;

	Kinect* m_pKinectClass;
	System* m_pSystemClass;

	// Direct2D
	ID2D1Factory* m_pD2DFactory;

	// Body/hand drawing
	ID2D1HwndRenderTarget* m_pRenderTarget;
	ID2D1SolidColorBrush* m_pBrushJointTracked;
	ID2D1SolidColorBrush* m_pBrushJointInferred;
	ID2D1SolidColorBrush* m_pBrushBoneTracked;
	ID2D1SolidColorBrush* m_pBrushBoneInferred;
	ID2D1SolidColorBrush* m_pBrushHandClosed;
	ID2D1SolidColorBrush* m_pBrushHandOpen;
	ID2D1SolidColorBrush* m_pBrushHandLasso;

	/// <summary>
	/// Main processing function
	/// </summary>
	void Update();

	/// <summary>
	/// Handle new body data
	/// <param name="nTime">timestamp of frame</param>
	/// <param name="nBodyCount">body data count</param>
	/// <param name="ppBodies">body data in frame</param>
	/// </summary>
	void ProcessBody(int nBodyCount, IBody** ppBodies);

	/// <summary>
	/// Ensure necessary Direct2d resources are created
	/// </summary>
	/// <returns>S_OK if successful, otherwise an error code</returns>
	HRESULT EnsureDirect2DResources();

	/// <summary>
	/// Dispose Direct2d resources 
	/// </summary>
	void DiscardDirect2DResources();

	/// <summary>
	/// Converts a body point to screen space
	/// </summary>
	/// <param name="bodyPoint">body point to tranform</param>
	/// <param name="width">width (in pixels) of output buffer</param>
	/// <param name="height">height (in pixels) of output buffer</param>
	/// <returns>point in screen-space</returns>
	D2D1_POINT_2F BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height);

	/// <summary>
	/// Draws a body 
	/// </summary>
	/// <param name="pJoints">joint data</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	void DrawBody(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints);

	/// <summary>
	/// Draws a hand symbol if the hand is tracked: red circle = closed, green circle = opened; blue circle = lasso
	/// </summary>
	/// <param name="handState">state of the hand</param>
	/// <param name="handPosition">position of the hand</param>
	void DrawHand(HandState handState, const D2D1_POINT_2F& handPosition);

	/// <summary>
	/// Draws one bone of a body (joint to joint)
	/// </summary>
	/// <param name="pJoints">joint data</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	/// <param name="pJointPoints">joint positions converted to screen space</param>
	/// <param name="joint0">one joint of the bone to draw</param>
	/// <param name="joint1">other joint of the bone to draw</param>
	void DrawBone(const Joint* pJoints, const D2D1_POINT_2F* pJointPoints, JointType joint0, JointType joint1);
};
