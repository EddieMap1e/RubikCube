#include "GameApp.h"
#include "d3dUtil.h"
using namespace DirectX;
using namespace std::experimental;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	if (!mBasicEffect.InitAll(md3dDevice))
		return false;

	if (!InitResource())
		return false;

	// 初始化鼠标，键盘不需要
	mMouse->SetWindow(mhMainWnd);
	mMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);

	// 初始化滑动延迟时间和点击位置
	mSlideDelay = 0.05f;
	mClickPosX = mClickPosY = -1;

	// 初始化计时器
	mGameTimer.Reset();
	mGameTimer.Stop();
	// 初始化游戏状态
	mGameStatus = GameStatus::Preparing;
	mCurrRotationRecord.dTheta = 0.0f;
	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();

	// 摄像机变更显示
	if (mCamera != nullptr)
	{
		mCamera->SetFrustum(XM_PI * 0.4f, AspectRatio(), 1.0f, 1000.0f);
		mCamera->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);
		mBasicEffect.SetProjMatrix(mCamera->GetProjXM());
	}
}

void GameApp::UpdateScene(float dt)
{
	// 键鼠更新
	if (mGameStatus == GameStatus::Preparing)
	{
		// 播放摄像机动画
		bool animComplete = PlayCameraAnimation(dt);

		if (!mRubik.IsLocked())
		{
			if (!mRotationRecordStack.empty())
			{
				// 打乱
				auto record = mRotationRecordStack.top();
				switch (record.axis)
				{
				case RubikRotationAxis_X: mRubik.RotateX(record.pos, -record.dTheta); break;
				case RubikRotationAxis_Y: mRubik.RotateY(record.pos, -record.dTheta); break;
				case RubikRotationAxis_Z: mRubik.RotateZ(record.pos, -record.dTheta); break;
				}
				mRotationRecordStack.pop();
			}
			else if (animComplete)
			{
				mGameStatus = GameStatus::Ready;
			}
		}
	}
	else
	{
		KeyInput();
		MouseInput(dt);
	}

	// 更新魔方
	mRubik.Update(dt);

}

void GameApp::DrawScene()
{
	// 使用偏紫色的纯色背景
	float backgroundColor[4] = { 0.45882352f, 0.42745098f, 0.51372549f, 1.0f };
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView.Get(), backgroundColor);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 绘制魔方
	mRubik.Draw(md3dImmediateContext, mBasicEffect);

	mSwapChain->Present(0, 0);
}


void GameApp::Shuffle()
{
	// 清栈
	while (!mRotationRecordStack.empty())
		mRotationRecordStack.pop();
	// 往栈上塞30个随机旋转操作用于打乱
	RubikRotationRecord record;
	srand(static_cast<unsigned>(time(nullptr)));
	for (int i = 0; i < 30; ++i)
	{
		record.axis = static_cast<RubikRotationAxis>(rand() % 3);
		record.pos = rand() % 4;
		record.dTheta = XM_PIDIV2 * (rand() % 2 ? 1 : -1);
		mRotationRecordStack.push(record);
	}
}

bool GameApp::PlayCameraAnimation(float dt)
{
	// 获取子类
	auto cam3rd = dynamic_cast<ThirdPersonCamera*>(mCamera.get());

	// 第三人称摄像机的操作

	mAnimationTime += dt;
	float theta, dist;

	theta = -XM_PIDIV2 + XM_PIDIV4 * mAnimationTime * 0.2f;
	dist = 25.0f - mAnimationTime * 2.0f;
	if (theta > -XM_PIDIV4)		//最终把θ角度移到四分之π
		theta = -XM_PIDIV4;
	if (dist < 15.0f)			//最终距离为15
		dist = 15.0f;

	cam3rd->SetRotationY(theta);
	cam3rd->SetDistance(dist);

	// 更新观察矩阵
	mCamera->UpdateViewMatrix();
	mBasicEffect.SetViewMatrix(mCamera->GetViewXM());

	if (fabs(theta + XM_PIDIV4) < 1e-5f && fabs(dist - 15.0f) < 1e-5f)
		return true;
	return false;
}

bool GameApp::InitResource()
{
	// 产生用于打乱魔方的记录
	Shuffle();
	// 初始化魔方
	mRubik.InitResources(md3dDevice, md3dImmediateContext);
	mRubik.SetRotationSpeed(XM_2PI * 1.5f);
	// 初始化特效
	mBasicEffect.SetRenderDefault(md3dImmediateContext);
	// 初始化摄像机
	mCamera.reset(new ThirdPersonCamera);
	auto cam3rd = dynamic_cast<ThirdPersonCamera*>(mCamera.get());
	cam3rd->SetDistance(10.0f);
	cam3rd->SetDistanceMinMax(10.0f, 200.0f);
	cam3rd->SetFrustum(XM_PI * 0.4f, AspectRatio(), 1.0f, 1000.0f);
	cam3rd->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);
	cam3rd->SetTarget(XMFLOAT3(0.0f, 0.0f, 0.0f));
	cam3rd->SetRotationX(XM_PIDIV2 * 0.6f);
	mBasicEffect.SetProjMatrix(cam3rd->GetProjXM());
	mBasicEffect.SetTextureArray(mRubik.GetTexArray());


	return true;
}


void GameApp::KeyInput()
{
	Keyboard::State keyState = mKeyboard->GetState();
	mKeyboardTracker.Update(keyState);


	//
	// 整个魔方旋转
	//

	// 此时正在旋转的话则提前结束
	if (mRubik.IsLocked())
		return;

	// 公式x
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Up))
	{
		mRubik.RotateX(3, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_X, 3, XM_PIDIV2 });
		return;
	}
	// 公式x'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Down))
	{
		mRubik.RotateX(3, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_X, 3, -XM_PIDIV2 });
		return;
	}
	// 公式y
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Left))
	{
		mRubik.RotateY(3, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Y, 3, XM_PIDIV2 });
		return;
	}
	// 公式y'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Right))
	{
		mRubik.RotateY(3, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Y, 3, -XM_PIDIV2 });
		return;
	}
	// 公式z'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::PageUp))
	{
		mRubik.RotateZ(3, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Z, 3, XM_PIDIV2 });
		return;
	}
	// 公式z
	if (mKeyboardTracker.IsKeyPressed(Keyboard::PageDown))
	{
		mRubik.RotateZ(3, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Z, 3, -XM_PIDIV2 });
		return;
	}

	//
	// 双层旋转
	//

	// 公式r
	if (keyState.IsKeyDown(Keyboard::LeftControl) && mKeyboardTracker.IsKeyPressed(Keyboard::I))
	{
		mRubik.RotateX(-2, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_X, -2, XM_PIDIV2 });
		return;
	}
	// 公式r'
	if (keyState.IsKeyDown(Keyboard::LeftControl) && mKeyboardTracker.IsKeyPressed(Keyboard::K))
	{
		mRubik.RotateX(-2, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_X, -2, -XM_PIDIV2 });
		return;
	}
	// 公式u
	if (keyState.IsKeyDown(Keyboard::LeftControl) && mKeyboardTracker.IsKeyPressed(Keyboard::J))
	{
		mRubik.RotateY(-2, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Y, -2, XM_PIDIV2 });
		return;
	}
	// 公式u'
	if (keyState.IsKeyDown(Keyboard::LeftControl) && mKeyboardTracker.IsKeyPressed(Keyboard::L))
	{
		mRubik.RotateY(-2, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Y, -2, -XM_PIDIV2 });
		return;
	}
	// 公式f'
	if (keyState.IsKeyDown(Keyboard::LeftControl) && mKeyboardTracker.IsKeyPressed(Keyboard::U))
	{
		mRubik.RotateZ(-1, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Z, -1, XM_PIDIV2 });
		return;
	}
	// 公式f
	if (keyState.IsKeyDown(Keyboard::LeftControl) && mKeyboardTracker.IsKeyPressed(Keyboard::O))
	{
		mRubik.RotateZ(-1, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Z, -1, -XM_PIDIV2 });
		return;
	}

	// 公式l'
	if (keyState.IsKeyDown(Keyboard::LeftControl) && mKeyboardTracker.IsKeyPressed(Keyboard::W))
	{
		mRubik.RotateX(-1, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_X, -1, XM_PIDIV2 });
		return;
	}
	// 公式l
	if (keyState.IsKeyDown(Keyboard::LeftControl) && mKeyboardTracker.IsKeyPressed(Keyboard::S))
	{
		mRubik.RotateX(-1, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_X, -1, -XM_PIDIV2 });
		return;
	}
	// 公式d'
	if (keyState.IsKeyDown(Keyboard::LeftControl) && mKeyboardTracker.IsKeyPressed(Keyboard::A))
	{
		mRubik.RotateY(-1, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Y, -1, XM_PIDIV2 });
		return;
	}
	// 公式d
	if (keyState.IsKeyDown(Keyboard::LeftControl) && mKeyboardTracker.IsKeyPressed(Keyboard::D))
	{
		mRubik.RotateY(-1, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Y, -1, -XM_PIDIV2 });
		return;
	}
	// 公式b
	if (keyState.IsKeyDown(Keyboard::LeftControl) && mKeyboardTracker.IsKeyPressed(Keyboard::Q))
	{
		mRubik.RotateZ(-2, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Z, -2, XM_PIDIV2 });
		return;
	}
	// 公式b'
	if (keyState.IsKeyDown(Keyboard::LeftControl) && mKeyboardTracker.IsKeyPressed(Keyboard::E))
	{
		mRubik.RotateZ(-2, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Z, -2, -XM_PIDIV2 });
		return;
	}


	//
	// 单层旋转
	//

	// 公式R
	if (mKeyboardTracker.IsKeyPressed(Keyboard::I))
	{
		mRubik.RotateX(2, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_X, 2, XM_PIDIV2 });
		return;
	}
	// 公式R'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::K))
	{
		mRubik.RotateX(2, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_X, 2, -XM_PIDIV2 });
		return;
	}
	// 公式U
	if (mKeyboardTracker.IsKeyPressed(Keyboard::J))
	{
		mRubik.RotateY(2, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Y, 2, XM_PIDIV2 });
		return;
	}
	// 公式U'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::L))
	{
		mRubik.RotateY(2, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Y, 2, -XM_PIDIV2 });
		return;
	}
	// 公式F'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::U))
	{
		mRubik.RotateZ(0, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Z, 0, XM_PIDIV2 });
		return;
	}
	// 公式F
	if (mKeyboardTracker.IsKeyPressed(Keyboard::O))
	{
		mRubik.RotateZ(0, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Z, 0, -XM_PIDIV2 });
		return;
	}

	// 公式L'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::W))
	{
		mRubik.RotateX(0, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_X, 0, XM_PIDIV2 });
		return;
	}
	// 公式L
	if (mKeyboardTracker.IsKeyPressed(Keyboard::S))
	{
		mRubik.RotateX(0, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_X, 0, -XM_PIDIV2 });
		return;
	}
	// 公式D'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::A))
	{
		mRubik.RotateY(0, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Y, 0, XM_PIDIV2 });
		return;
	}
	// 公式D
	if (mKeyboardTracker.IsKeyPressed(Keyboard::D))
	{
		mRubik.RotateY(0, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Y, 0, -XM_PIDIV2 });
		return;
	}
	// 公式B
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Q))
	{
		mRubik.RotateZ(2, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Z, 2, XM_PIDIV2 });
		return;
	}
	// 公式B'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::E))
	{
		mRubik.RotateZ(2, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Z, 2, -XM_PIDIV2 });
		return;
	}

	// 公式M
	if (mKeyboardTracker.IsKeyPressed(Keyboard::T))
	{
		mRubik.RotateX(1, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_X, 1, XM_PIDIV2 });
		return;
	}
	// 公式M'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::G))
	{
		mRubik.RotateX(1, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_X, 1, -XM_PIDIV2 });
		return;
	}
	// 公式E
	if (mKeyboardTracker.IsKeyPressed(Keyboard::F))
	{
		mRubik.RotateY(1, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Y, 1, XM_PIDIV2 });
		return;
	}
	// 公式E'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::H))
	{
		mRubik.RotateY(1, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Y, 1, -XM_PIDIV2 });
		return;
	}
	// 公式S'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::R))
	{
		mRubik.RotateZ(1, XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Z, 1, XM_PIDIV2 });
		return;
	}
	// 公式S
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Y))
	{
		mRubik.RotateZ(1, -XM_PIDIV2);
		mRotationRecordStack.push(RubikRotationRecord{ RubikRotationAxis_Z, 1, -XM_PIDIV2 });
		return;
	}

}

void GameApp::MouseInput(float dt)
{
	Mouse::State mouseState = mMouse->GetState();
	Mouse::State lastState = mMouseTracker.GetLastState();
	mMouseTracker.Update(mouseState);


	int dx = mouseState.x - lastState.x;
	int dy = mouseState.y - lastState.y;
	// 获取子类
	auto cam3rd = dynamic_cast<ThirdPersonCamera*>(mCamera.get());

	// ******************
	// 第三人称摄像机的操作
	//

	// 绕物体旋转，添加轻微抖动
	cam3rd->SetRotationX(XM_PIDIV2 * 0.6f + (mouseState.y - mClientHeight / 2) *  0.0001f);
	cam3rd->SetRotationY(-XM_PIDIV4 + (mouseState.x - mClientWidth / 2) * 0.0001f);
	cam3rd->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);

	// 更新观察矩阵
	mCamera->UpdateViewMatrix();
	mBasicEffect.SetViewMatrix(mCamera->GetViewXM());

	// 重置滚轮值
	mMouse->ResetScrollWheelValue();

	// ******************
	// 魔方操作
	//

	// 鼠标左键是否点击
	if (mouseState.leftButton)
	{
		// 此时未确定旋转方向
		if (!mDirectionLocked)
		{
			// 此时未记录点击位置
			if (mClickPosX == -1 && mClickPosY == -1)
			{
				// 初次点击
				if (mMouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
				{
					// 记录点击位置
					mClickPosX = mouseState.x;
					mClickPosY = mouseState.y;
				}
			}

			// 仅当记录了点击位置才进行更新
			if (mClickPosX != -1 && mClickPosY != -1)
				mCurrDelay += dt;
			// 未到达滑动延迟时间则结束
			if (mCurrDelay < mSlideDelay)
				return;

			// 未产生运动则不上锁
			if (abs(dx) == abs(dy))
				return;

			// 开始上方向锁
			mDirectionLocked = true;
			// 更新累积的位移变化量
			dx = mouseState.x - mClickPosX;
			dy = mouseState.y - mClickPosY;

			// 找到当前鼠标点击的方块索引
			Ray ray = Ray::ScreenToRay(*mCamera, (float)mouseState.x, (float)mouseState.y);
			float dist;
			XMINT3 pos = mRubik.HitCube(ray, &dist);

			// 判断当前主要是垂直操作还是水平操作
			bool isVertical = abs(dx) < abs(dy);
			// 当前鼠标操纵的是-Z面，根据操作类型决定旋转轴
			if (pos.z == 0 && fabs((ray.origin.z + dist * ray.direction.z) - (-3.0f)) < 1e-5f)
			{
				mCurrRotationRecord.pos = isVertical ? pos.x : pos.y;
				mCurrRotationRecord.axis = isVertical ? RubikRotationAxis_X : RubikRotationAxis_Y;
			}
			// 当前鼠标操纵的是+X面，根据操作类型决定旋转轴
			else if (pos.x == 2 && fabs((ray.origin.x + dist * ray.direction.x) - 3.0f) < 1e-5f)
			{
				mCurrRotationRecord.pos = isVertical ? pos.z : pos.y;
				mCurrRotationRecord.axis = isVertical ? RubikRotationAxis_Z : RubikRotationAxis_Y;
			}
			// 当前鼠标操纵的是+Y面，要判断平移变化量dx和dy的符号来决定旋转方向
			else if (pos.y == 2 && fabs((ray.origin.y + dist * ray.direction.y) - 3.0f) < 1e-5f)
			{
				// 判断异号
				bool diffSign = ((dx & 0x80000000) != (dy & 0x80000000));
				mCurrRotationRecord.pos = diffSign ? pos.x : pos.z;
				mCurrRotationRecord.axis = diffSign ? RubikRotationAxis_X : RubikRotationAxis_Z;
			}
			// 当前鼠标操纵的是空白地区，则对整个魔方旋转
			else
			{
				mCurrRotationRecord.pos = 3;
				// 水平操作是Y轴旋转
				if (!isVertical)
				{
					mCurrRotationRecord.axis = RubikRotationAxis_Y;
				}
				// 屏幕左半部分的垂直操作是X轴旋转
				else if (mouseState.x < mClientWidth / 2)
				{
					mCurrRotationRecord.axis = RubikRotationAxis_X;
				}
				// 屏幕右半部分的垂直操作是Z轴旋转
				else
				{
					mCurrRotationRecord.axis = RubikRotationAxis_Z;
				}
			}
		}

		// 上了方向锁才能进行旋转
		if (mDirectionLocked)
		{
			// 进行旋转
			switch (mCurrRotationRecord.axis)
			{
			case RubikRotationAxis_X: mRubik.RotateX(mCurrRotationRecord.pos, (dx - dy) * 0.008f, true);
				mCurrRotationRecord.dTheta += (dx - dy) * 0.008f;
				break;
			case RubikRotationAxis_Y: mRubik.RotateY(mCurrRotationRecord.pos, -dx * 0.008f, true);
				mCurrRotationRecord.dTheta += (-dx * 0.008f);
				break;
			case RubikRotationAxis_Z: mRubik.RotateZ(mCurrRotationRecord.pos, (-dx - dy) * 0.008f, true);
				mCurrRotationRecord.dTheta += (-dx - dy) * 0.008f;
				break;
			}
		}
	}
	// 鼠标刚释放
	else if (mMouseTracker.leftButton == Mouse::ButtonStateTracker::RELEASED)
	{
		// 释放方向锁
		mDirectionLocked = false;
		// 滑动延迟归零
		mCurrDelay = 0.0f;
		// 坐标移出屏幕
		mClickPosX = mClickPosY = -1;

		// 发送完成指令，进行预旋转
		switch (mCurrRotationRecord.axis)
		{
		case RubikRotationAxis_X: mRubik.RotateX(mCurrRotationRecord.pos, 0.0f); break;
		case RubikRotationAxis_Y: mRubik.RotateY(mCurrRotationRecord.pos, 0.0f); break;
		case RubikRotationAxis_Z: mRubik.RotateZ(mCurrRotationRecord.pos, 0.0f); break;
		}

		// 若这次旋转有意义，记录到栈中
		int times = static_cast<int>(round(mCurrRotationRecord.dTheta / XM_PIDIV2)) % 4;
		if (times != 0)
		{
			mCurrRotationRecord.dTheta = times * XM_PIDIV2;
			mRotationRecordStack.push(mCurrRotationRecord);
		}
		// 旋转值归零
		mCurrRotationRecord.dTheta = 0.0f;
	}
}