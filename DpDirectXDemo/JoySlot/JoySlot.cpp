#include "JoySlot.h"
#include "DpTga.h"
#include "DpGeometry.h"

#include <iostream>

const char* s_carName[] = { "chevrolet_corvette_zr1_2009_desc.tga",
"ford_f150_svt_raptor_2010_desc.tga",
"ford_gt_2006_nfs_desc.tga",
"lamborghini_aventador_lp700_4_2011_desc.tga",
"lamborghini_countach_5000qv_1985_desc.tga" };

const char* s_stateDesc[] = { "State_None", "State_SpeedUp", "State_SpeedHold", "State_SlowStart", "State_PreStop", "State_WaitStop", "State_Stop" };

Car::Car()
	: mark(false)
	, carIndex(0)
{

}

void Car::Init()
{
	carIndex = RandRange(0, 4);

	Load(s_carName[carIndex]);
}

void Car::Stop(int _carIndex)
{
	carIndex = _carIndex;
	mark = true;
	Load(s_carName[carIndex]);
}

void Car::SetPos(const dopixel::math::Point& pt)
{
//	assert(pt.y >= CarGroup::ymin);

	Sprite::SetPos(pt);
}

//////////////////////////////////////////////////////////////////////////

float CarGroup::ymin = 0;
float CarGroup::ymax = 0;

const int   CarGroup::maxnums = 5;
const float CarGroup::vLow = 1000;
const float CarGroup::vHigh = 2000;

const float CarGroup::timeHold = 0.8f;

CarGroup::CarGroup()
	: v0(0)
	, a(0)
	, v(0)
	, t(0)
	, x(0)
	, state(State_None)
	, tHold(0)
{

}

void CarGroup::Init(const char* _name, float _x, float _ymin, float _ymax)
{
	x = _x;
	ymin = _ymin;
	ymax = _ymax;

	cars.resize(maxnums);
	for (unsigned int i = 0; i < cars.size(); ++i)
	{
		Car& car = cars[i];
		car.Init();
		car.SetPos(Point(x, ymin + i * car.GetHeight()));
	}

	a = 3000;
	v0 = 1000;

	name = _name;
	state = State_None;
}

void CarGroup::Run(float fElapsedTime)
{
	if (state == State_Stop || state == State_None)
		return;

	Car& car = cars.back();
	if (car.GetPos().y >= ymax)
	{	
		UpdateCar();
	}
	else
	{
		switch (state)
		{
		case State_SpeedUp:
			SpeedUp(fElapsedTime);
			break;
		case State_SpeedHold:
			tHold += fElapsedTime;
			if (tHold >= timeHold)
				state = State_SlowStart;
			break;
		case State_SlowStart:
			SlowDown(fElapsedTime);
			break;
		}

		AlignPos(v * fElapsedTime);
	}
}

void CarGroup::AlignPos(float fDelta)
{
	for (auto& car : cars)
	{
		float y = car.GetPos().y;
		car.SetPos(Point(x, y + fDelta));
	}
}

void CarGroup::AlignByBase()
{
	for (int i = 0; i < cars.size(); ++i)
	{
		Car& car = cars[i];
		float y = car.GetPos().y;
		car.SetPos(Point(x, ymin + i * car.GetHeight()));
	}
}

void CarGroup::UpdateCar()
{
	if (cars.empty())
		return;

	Car car = cars.back();
	cars.erase(cars.end() - 1);

	bool alignBase = false;
	if (state == State_PreStop)
	{
		car.Stop(stopIndex);
		state = State_WaitStop;
	}
	else
	{
		if (state == State_WaitStop)
		{
			// Middle
			int index = maxnums - 3;
			Car& markCar = cars[index];
			if (markCar.IsMark())
			{
				state = State_Stop;

				alignBase = true;
			}
		}

		car.Init();
	}

	float y = cars.front().GetPos().y;

	car.SetPos(Point(x, y - car.GetHeight()));
	cars.insert(cars.begin(), car);

	if (alignBase)
	{
		AlignByBase();
	}
}

void CarGroup::Start()
{
	// Clear
	for (auto& car : cars)
	{
		car.Clear();
	}
	state = State_SpeedUp;
}

void CarGroup::SpeedUp(float fElapsedTime)
{
	if (v < vHigh)
	{
		t += fElapsedTime;
		v = v0 + a * t;
	}
	else
	{
		v = vHigh;
		t = 0;
		state = State_SpeedHold;
	}
}

void CarGroup::SlowDown(float fElapsedTime)
{
	if (v > vLow)
	{
		t += fElapsedTime;
		v = vHigh + (-a * 0.8f) * t;
	}
	else
	{
		v = vLow;
		t = 0;
		state = State_PreStop;
	}
}

void CarGroup::SetStopIndex(int _stopIndex)
{
	stopIndex = _stopIndex;
}

void CarGroup::Render(float fElapsedTime)
{
	int height = 0;
	for (auto& car : cars)
	{
		car.Render(Point(0,0));
		if (height == 0)
			height = car.GetHeight();
	}

	Color color;
	if (state > State_None && state < State_SlowStart)
		color = Color::green;
	else if (state >= State_SlowStart && state < State_WaitStop)
		color = Color::red;
	else
		color = Color::black;

	Point pt(x, ymin + (maxnums - 3) * height - 20);
	GetD3DDevice()->DrawText(avar("%s:%s", name, s_stateDesc[state]), pt, color);

	pt.y -= 20;
	GetD3DDevice()->DrawText(avar("v:%f", v), pt, Color::black);
}

//////////////////////////////////////////////////////////////////////////

const float JoySlot::interval = 0.15f;

void JoySlot::OnCreate()
{
	car.Load("chevrolet_corvette_zr1_2009_desc.tga");

	frame.Load("Luckdraw_Goldbox.tga");

	select.Load("Luckdraw_choosen.tga");

	carPos = Point(100, 100);
	framePos = Point(82, 115);
	selectPos = Point(89, 185);

	float ymin = carPos.y - car.GetHeight() * (CarGroup::maxnums - 3);
	float ymax = carPos.y + car.GetHeight() * 3;

	leftCars.Init("A", carPos.x, ymin, ymax);
	middleCars.Init("B", carPos.x + car.GetWidth(), ymin, ymax);
	rightCars.Init("C", carPos.x + car.GetWidth() * 2, ymin, ymax);

	leftCars.SetStopIndex(0);
	middleCars.SetStopIndex(0);
	rightCars.SetStopIndex(0);
}

void JoySlot::Run(float fElapsedTime)
{
	D3DDemoApp::Run(fElapsedTime);

	if (state == STOP)
		return;

	leftCars.Run(fElapsedTime);
	middleCars.Run(fElapsedTime);
	rightCars.Run(fElapsedTime);

	if (leftCars.IsRun())
	{
		t += fElapsedTime;

		if (t >= interval)
		{
			if (! middleCars.IsRun())
			{
				middleCars.Start(); 
			}

			if (t >= interval * 1.5f)
			{
				if (! rightCars.IsRun())
				{
					rightCars.Start();
				}
			}
		}
	}

	turnTime += fElapsedTime;

	if (state == RUN)
	{
		if (leftCars.GetState() == CarGroup::State_Stop && middleCars.GetState() == CarGroup::State_Stop && rightCars.GetState() == CarGroup::State_Stop)
		{
			state = STOP;
		}
	}
}


void JoySlot::Render(float fElapsedTime)
{
	D3DDemoApp::Render(fElapsedTime);

	// turn time
	GetD3DDevice()->DrawText(avar("turn time:%f", turnTime), Point(0, 30), Color::black);

	Point pt = carPos;

	// clip Rect
	RectF clip(0, 0, car.GetWidth() * 3.0f, car.GetHeight() * 3.0f);
	clip.OffsetRect(carPos);
	clip.InflateRect(-2, -2, 1, 1);
	GetD3DDevice()->DrawRect(clip, Color(255, 0, 0, 255));

	RectF oldClip;
	if (bclip)
	{
		clip.InflateRect(0, 18, 0, -24);

		oldClip = GetD3DDevice()->SetClipRect(clip);
	}

	// cars
	leftCars.Render(fElapsedTime);
	middleCars.Render(fElapsedTime);
	rightCars.Render(fElapsedTime);
	GetD3DDevice()->FlushQuadTex();

	if (bclip)
	{
		GetD3DDevice()->SetClipRect(oldClip);
	}

	// frame
	frame.Render(framePos);
	GetD3DDevice()->FlushQuadTex();

	// select
	select.Render(selectPos);
	GetD3DDevice()->FlushQuadTex();


}

bool JoySlot::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CHAR:
		switch (wParam)
		{
		case 'w':
			framePos.y -= 1;
			break;
		case 's':
			framePos.y += 1;
			break;
		case 'a':
			framePos.x -= 1;
			break;
		case 'd':
			framePos.x += 1;
			break;

		case 'b':
			bclip = !bclip;
			break;
		case 'r':
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_UP:
			selectPos.y -= 1;
			break;
		case VK_DOWN:
			selectPos.y += 1;
			break;
		case VK_LEFT:
			selectPos.x -= 1;
			break;
		case VK_RIGHT:
			selectPos.x += 1;
			break;
		case VK_SPACE:
			if (state == STOP)
			{
				state = RUN;
				turnTime = 0;
				t = 0;
				leftCars.Start();
			}
			break;
		}
		break;
	}

	return D3DDemoApp::MsgProc(hwnd, msg, wParam, lParam);
}