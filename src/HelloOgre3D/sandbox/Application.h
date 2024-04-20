#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <string>
#include "OgreFrameListener.h"

class ClientManager;
class Application : public Ogre::FrameListener
{
public:
	Application(const std::string& appTitle = "");
    virtual ~Application();

    virtual bool Setup();
    virtual void Cleanup();
    
    virtual void Run();
    virtual void Draw();
    virtual void Update();

public:
    // Ogre::FrameListener
    virtual bool frameEnded(const Ogre::FrameEvent& event);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& event);
    virtual bool frameStarted(const Ogre::FrameEvent& event);

private:
    void CreateFrameListener();

private:
    ClientManager* m_pClientManager;
};

#endif  // __APPLICATION_H__
