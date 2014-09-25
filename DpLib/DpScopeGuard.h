/********************************************************************
	created:	2014/06/29
	created:	29:6:2014   0:05
	filename: 	F:\SkyDrive\3D\DpLib\DpLib\DpScopeGuard.h
	file path:	F:\SkyDrive\3D\DpLib\DpLib
	file base:	DpScopeGuard
	file ext:	h
	author:		mi2think@gmail.com
	
	purpose:	Scope Guard
*********************************************************************/

#ifndef __DP_SCOPE_GUARD__
#define __DP_SCOPE_GUARD__

#include <functional>

namespace DoPixel
{
	namespace Core
	{
		class ScopeGuard
		{
		public:
			ScopeGuard(std::function<void()> func) : func_(func) {}
			~ScopeGuard() { func_(); }

		private:
			ScopeGuard(const ScopeGuard&);
			ScopeGuard& operator=(const ScopeGuard&);

			std::function<void()> func_;
		};

		#define SCOPEGUARD_NAMELINE_CAT(name, line) name##line
		#define SCOPEGUARD_NAMELINE(name, line) SCOPEGUARD_NAMELINE_CAT(name, line)
		#define ON_SCOPE_EXIT(func) ScopeGuard SCOPEGUARD_NAMELINE(EXIT, __LINE__)(func)
	}
}

#endif