#pragma once

#include <Kinect.h>

class Kinect
{
public:
	Kinect();
	~Kinect();

	bool Initialize();
	void Shutdown();


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

private:
	IKinectSensor* m_pKinectSensor;
	IBodyFrameReader* m_pBodyFrameReader;
	ICoordinateMapper* m_pCoordinateMapper;
};
