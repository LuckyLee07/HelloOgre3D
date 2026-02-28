#ifndef __OgreDpiHelper_H__
#define __OgreDpiHelper_H__

#include "OgrePrerequisites.h"
#include <cmath>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace Ogre
{
    class DpiHelper
    {
    public:
        static inline Real getScale()
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            static Real sDpiScale = 0.0f;
            if (sDpiScale > 0.0f)
                return sDpiScale;

            UINT dpi = 96;
            HMODULE user32 = GetModuleHandleA("user32.dll");
            if (user32)
            {
                typedef UINT (WINAPI *GetDpiForSystemFn)();
                GetDpiForSystemFn getDpiForSystemFn =
                    (GetDpiForSystemFn)GetProcAddress(user32, "GetDpiForSystem");
                if (getDpiForSystemFn)
                {
                    const UINT systemDpi = getDpiForSystemFn();
                    if (systemDpi > 0)
                        dpi = systemDpi;
                }
            }

            sDpiScale = Real(dpi) / 96.0f;
            if (sDpiScale < 1.0f)
                sDpiScale = 1.0f;
            return sDpiScale;
#else
            return 1.0f;
#endif
        }

        static inline unsigned int toLogicalPixels(unsigned int physicalPixels)
        {
            const Real scale = getScale();
            if (scale <= 1.0f)
                return physicalPixels;
            return static_cast<unsigned int>(std::floor((Real(physicalPixels) / scale) + 0.5f));
        }

        static inline unsigned int toPhysicalPixels(unsigned int logicalPixels)
        {
            const Real scale = getScale();
            if (scale <= 1.0f)
                return logicalPixels;
            return static_cast<unsigned int>(std::floor((Real(logicalPixels) * scale) + 0.5f));
        }

        static inline Real toLogicalPixels(Real physicalPixels)
        {
            return physicalPixels / getScale();
        }

        static inline Real toPhysicalPixels(Real logicalPixels)
        {
            return logicalPixels * getScale();
        }
    };
}

#endif
