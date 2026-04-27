#pragma once
#include "../global/base.h"
#include "../math/math.h"
#include <string>

#define NO_MOVE 0
#define MOVE_LEFT 0x001
#define MOVE_RIGHT 0x002
#define MOVE_FRONT 0x004
#define MOVE_BACK 0x008

//1 ���յ��ⲿ���������/���̣�
//2 ���ݲ������ĵ�ǰ״̬���ƶ�״̬/front������
//3 ÿһ֡���û���������update������viewMatrix����

class Camera {
public:
	Camera(float fovy, float aspect, float n, float f, const math::vec3f& top);

	~Camera();

public:
	void onMouseDown(float x, float y);

	void onMouseUp(float x, float y);

	void onMouseMove(float x, float y);

	void onKeyDown(const std::string& key);

	void onKeyUp(const std::string& key);

	void update();

	math::mat4f getViewMatrix()const { return mViewMatrix; }

	math::mat4f getProjectionMatrix()const { return mProjectionMatrix; }

private:
	void pitch(int yoffset);

	void yaw(int xoffset);

private:
	math::mat4f mViewMatrix;
	math::mat4f mProjectionMatrix;

	math::vec3f	mPosition{ 0.0f,0.0f,0.0f };
	math::vec3f	mFront{ 0.0f,0.0f,-1.0f };
	math::vec3f	mTop{ 0.0f,1.0f,0.0f };
	float		mSpeed = { 0.1f };

	float		mPitch{ 0.0f };
	float		mYaw{ -90.0f };
	float		mSensitivity{ 0.1f };

	uint32_t	mMoveState{ NO_MOVE };
	bool		mMouseMoving{ false };
	float		mCurrentMouseX{ 0 };
	float		mCurrentMouseY{ 0 };

};