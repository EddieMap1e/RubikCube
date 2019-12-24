#pragma once

#include "d3dApp.h"
#include "Rubik.h"
#include "Camera.h"
#include <stack>

class GameApp : public D3DApp
{
public:
	enum class GameStatus {
		Preparing,	// 准备中
		Ready,		// 就绪
		Playing,	// 游玩中
		Finished,	// 已完成
	};

public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();


private:
	// 播放摄像机动画，完成动画将返回true
	bool PlayCameraAnimation(float dt);
	// 用于产生打乱魔方的序列
	void Shuffle();

	bool InitResource();

	void KeyInput();
	void MouseInput(float dt);

private:

	Rubik mRubik;								// 魔方

	std::unique_ptr<Camera> mCamera;			// 第三人称摄像机

	BasicEffect mBasicEffect;					// 基础特效管理类

	GameTimer mGameTimer;						// 游戏计时器
	GameStatus mGameStatus;						// 游戏状态

	float mAnimationTime;						// 动画经过时间

	int mClickPosX, mClickPosY;					// 初次点击时鼠标位置
	float mSlideDelay;							// 拖动延迟响应时间 
	float mCurrDelay;							// 当前延迟时间
	bool mDirectionLocked;						// 方向锁

	RubikRotationRecord mCurrRotationRecord;	// 当前旋转记录

	std::stack<RubikRotationRecord> mRotationRecordStack;	// 旋转记录栈
};