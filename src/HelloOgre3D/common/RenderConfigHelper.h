#ifndef __RENDER_CONFIG_HELPER_H__
#define __RENDER_CONFIG_HELPER_H__

#include "OgreRenderSystem.h"
#include "OgreStringConverter.h"
#include "OgreMath.h"

namespace RenderConfigHelper
{
    inline Ogre::String SelectBestMacFsaa(const Ogre::ConfigOptionMap& options)
    {
        Ogre::ConfigOptionMap::const_iterator it = options.find("FSAA");
        if (it == options.end())
            return "0";

        unsigned int bestUpTo4 = 0;
        unsigned int bestAny = 0;
        for (size_t i = 0; i < it->second.possibleValues.size(); ++i)
        {
            Ogre::String v = it->second.possibleValues[i];
            const size_t spacePos = v.find(' ');
            if (spacePos != Ogre::String::npos)
                v = v.substr(0, spacePos);

            const unsigned int samples = Ogre::StringConverter::parseUnsignedInt(v, 0);
            if (samples > bestAny)
                bestAny = samples;
            if (samples > bestUpTo4 && samples <= 4)
                bestUpTo4 = samples;
        }

        if (bestUpTo4 > 0)
            return Ogre::StringConverter::toString(bestUpTo4);
        if (bestAny > 0)
            return Ogre::StringConverter::toString(bestAny);
        if (!it->second.currentValue.empty())
            return it->second.currentValue;
        return "0";
    }

    inline Ogre::String SelectBestMacContentScale(const Ogre::ConfigOptionMap& options)
    {
        Ogre::ConfigOptionMap::const_iterator it = options.find("Content Scaling Factor");
        if (it == options.end())
            return "1.0";

        float bestAbove1 = 1.0f;
        bool has2x = false;
        for (size_t i = 0; i < it->second.possibleValues.size(); ++i)
        {
            const float v = Ogre::StringConverter::parseReal(it->second.possibleValues[i], 1.0f);
            if (v > bestAbove1)
                bestAbove1 = v;
            if (Ogre::Math::RealEqual(v, 2.0f, 1e-3f))
                has2x = true;
        }

        if (has2x)
            return "2.0";
        if (bestAbove1 > 1.0f)
            return Ogre::StringConverter::toString(bestAbove1);
        if (!it->second.currentValue.empty())
            return it->second.currentValue;
        return "1.0";
    }
}

#endif // __RENDER_CONFIG_HELPER_H__
