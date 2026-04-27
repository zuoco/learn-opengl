#include "camera.h"

Camera::Camera(float fovy, float aspect, float n, float f, const math::vec3f& top) {
	mTop = top;
	mProjectionMatrix = math::perspective(fovy, aspect, n, f);
	update();
}

Camera::~Camera() {}

void Camera::onMouseDown(float x, float y) {
	mMouseMoving = true;
	mCurrentMouseX = x;
	mCurrentMouseY = y;
}

void Camera::onMouseUp(float x, float y) {
	mMouseMoving = false;
}

void Camera::onMouseMove(float x, float y) {
	if (mMouseMoving) {
		int xOffset = static_cast<int>(x - mCurrentMouseX);
		int yOffset = static_cast<int>(y - mCurrentMouseY);

		mCurrentMouseX = x;
		mCurrentMouseY = y;

		pitch(-yOffset);
		yaw(xOffset);
	}
}

void Camera::onKeyDown(const std::string& key) {
	if (key == "w" || key == "W") {
		mMoveState |= MOVE_FRONT;
	} else if (key == "a" || key == "A") {
		mMoveState |= MOVE_LEFT;
	} else if (key == "s" || key == "S") {
		mMoveState |= MOVE_BACK;
	} else if (key == "d" || key == "D") {
		mMoveState |= MOVE_RIGHT;
	}
}

void Camera::onKeyUp(const std::string& key) {
	if (key == "w" || key == "W") {
		mMoveState &= ~MOVE_FRONT;
	} else if (key == "a" || key == "A") {
		mMoveState &= ~MOVE_LEFT;
	} else if (key == "s" || key == "S") {
		mMoveState &= ~MOVE_BACK;
	} else if (key == "d" || key == "D") {
		mMoveState &= ~MOVE_RIGHT;
	}
}

void Camera::pitch(int yoffset) {
	mPitch += yoffset * mSensitivity;

	if (mPitch >= 89.0f)
	{
		mPitch = 89.0f;
	}

	if (mPitch <= -89.0f)
	{
		mPitch = -89.0f;
	}

	mFront.y = sin(DEG2RAD(mPitch));
	mFront.x = cos(DEG2RAD(mYaw)) * cos(DEG2RAD(mPitch));
	mFront.z = sin(DEG2RAD(mYaw)) * cos(DEG2RAD(mPitch));

	mFront = math::normalize(mFront);
}

void Camera::yaw(int xoffset) {
	mYaw += xoffset * mSensitivity;

	mFront.y = sin(DEG2RAD(mPitch));
	mFront.x = cos(DEG2RAD(mYaw)) * cos(DEG2RAD(mPitch));
	mFront.z = sin(DEG2RAD(mYaw)) * cos(DEG2RAD(mPitch));

	mFront = math::normalize(mFront);
}

void Camera::update() {
	//����������Ŀ��ܽ��л��ܣ��õ�һ�������ƶ�����
	math::vec3f moveDirection = {0.0f, 0.0f, 0.0f};

	math::vec3f front = mFront;
	math::vec3f right = math::normalize(cross(mFront, mTop));

	if (mMoveState & MOVE_FRONT) {
		moveDirection += front;
	}

	if (mMoveState & MOVE_BACK) {
		moveDirection += -front;
	}

	if (mMoveState & MOVE_LEFT) {
		moveDirection += -right;
	}

	if (mMoveState & MOVE_RIGHT) {
		moveDirection += right;
	}

	if (math::lengthSquared(moveDirection) != 0) {
		moveDirection = math::normalize(moveDirection);
		mPosition += mSpeed * moveDirection;
	}

	mViewMatrix = math::lookAt<float>(mPosition, mPosition + mFront, mTop);
}

