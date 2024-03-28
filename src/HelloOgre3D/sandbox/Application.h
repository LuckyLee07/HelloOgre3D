#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "OgreString.h"
#include "SandboxManager.h"

class Application
{
public:
	Application(const Ogre::String& applicationTitle = "");

    virtual ~Application();

    virtual bool Setup();

    virtual void Draw();

    virtual void Cleanup();
    
    virtual void Update();

};  // class Application

#endif  // __APPLICATION_H__
