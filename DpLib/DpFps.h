/********************************************************************
	created:	2014/09/08
	created:	8:9:2014   21:26
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpFps.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpFps
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Fps
*********************************************************************/

#ifndef __DP_FPS__
#define __DP_FPS__

namespace DoPixel
{
	namespace Core
	{
		class Fps
		{
		public:
			Fps() : frameCount(0), time(0), fps(0) {}

			unsigned int GetFrameCount() const { return frameCount; }
			float GetFps() const { return fps; }

			void Run(float fElapsedTime)
			{
				++frameCount;
				time += fElapsedTime;

				if (time >= 1.0f)
				{
					fps = frameCount / time;
					frameCount = 0;
					time = 0.0f;
				}
			}
		private:
			unsigned int frameCount;
			float time;
			float fps;
		};
	}
}


#endif