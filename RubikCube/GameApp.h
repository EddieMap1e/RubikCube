#pragma once

#include "d3dApp.h"
#include "Rubik.h"
#include "Camera.h"
#include <stack>

class GameApp : public D3DApp
{
public:
	enum class GameStatus {
		Preparing,	// ׼����
		Ready,		// ����
		Playing,	// ������
		Finished,	// �����
	};

public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();


private:
	// �����������������ɶ���������true
	bool PlayCameraAnimation(float dt);
	// ���ڲ�������ħ��������
	void Shuffle();

	bool InitResource();

	void KeyInput();
	void MouseInput(float dt);

private:

	Rubik mRubik;								// ħ��

	std::unique_ptr<Camera> mCamera;			// �����˳������

	BasicEffect mBasicEffect;					// ������Ч������

	GameTimer mGameTimer;						// ��Ϸ��ʱ��
	GameStatus mGameStatus;						// ��Ϸ״̬

	float mAnimationTime;						// ��������ʱ��

	int mClickPosX, mClickPosY;					// ���ε��ʱ���λ��
	float mSlideDelay;							// �϶��ӳ���Ӧʱ�� 
	float mCurrDelay;							// ��ǰ�ӳ�ʱ��
	bool mDirectionLocked;						// ������

	RubikRotationRecord mCurrRotationRecord;	// ��ǰ��ת��¼

	std::stack<RubikRotationRecord> mRotationRecordStack;	// ��ת��¼ջ
};