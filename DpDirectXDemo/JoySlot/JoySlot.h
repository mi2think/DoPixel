#ifndef __JOY_SLOT__
#define __JOY_SLOT__

#include "D3DDemoApp.h"
#include "DpSprite.h"

class Car : public Sprite
{
public:
	Car();

	void Init();

	void Stop(int _carIndex);

	bool IsMark() const { return mark; }

	void Clear() {  mark = false; }

	void SetPos(const DoPixel::Math::Point& pt);
private:
	int carIndex;
	bool mark;
};

class CarGroup
{
public:
	enum State
	{
		State_None,

		// v: v0 -> vHigh
		State_SpeedUp,

		// v: vHigh
		State_SpeedHold,

		// v: vHigh -> vLow, a -> -a
		State_SlowStart,

		// Add stop car to top
		State_PreStop,

		// v: vLow, wait for stopIndex car come in
		State_WaitStop,

		// Stop
		State_Stop,
	};

	CarGroup();

	void Init(const char* _name, float _x, float _ymin, float _ymax);

	void Run(float fElapsedTime);

	void Render(float fElapsedTime);

	void AlignPos(float fDelta);

	void UpdateCar();

	void AlignByBase();

	State GetState() const { return state; }

	bool IsRun() const { return state != State_None &&  state != State_Stop; }

	void Start();

	void SpeedUp(float fElapsedTime);

	void SlowDown(float fElapsedTime);

	void PreStop();

	void SetStopIndex(int _stopIndex);
private:
	const char* name;
	float v0;
	float a;
	float v;
	float t;
	std::vector<Car> cars;

	float x;
	int	stopIndex;
	State state;

	float tHold;

public:
	// Pos
	static float ymin;
	static float ymax;
	static const int   maxnums;
	// Speed
	static const float vLow;
	static const float vHigh;
	// Time
	static const float timeHold;
};

class JoySlot : public D3DDemoApp
{
public:
	JoySlot() : state(STOP), bclip(false), t(0), turnTime(0){}
	~JoySlot() {}

	void OnCreate();

	void Run(float fElapsedTime);

	void Render(float fElapsedTime);

	bool MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	enum STATE 
	{
		RUN,
		STOP,
	};
private:
	Car car;
	Sprite frame;
	Sprite select;

	Point carPos;
	Point framePos;
	Point selectPos;

	CarGroup leftCars;
	CarGroup middleCars;
	CarGroup rightCars;

	float t;
	float turnTime;
	
	STATE state;

	bool bclip;

	static const float interval;
};

#endif