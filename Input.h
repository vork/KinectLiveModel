#pragma once
class Input
{
public:
	Input();
	~Input();

	void Initialize();

	void Release()
	{
		//This is just here to be deleteable by SafeRelease
	}

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool m_keys[256];
};
