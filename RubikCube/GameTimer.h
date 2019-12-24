#pragma once
#include <windows.h>

class GameTimer
{
public:
	GameTimer();

	float TotalTime()const;		// ����Ϸʱ��
	float DeltaTime()const;		// ֡���ʱ��

	void Reset(); // ����Ϣѭ��֮ǰ����
	void Start(); // ��ȡ����ͣ��ʱ�����
	void Stop();  // ����ͣ��ʱ�����
	void Tick();  // ��ÿһ֡��ʱ�����

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};