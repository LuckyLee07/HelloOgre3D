#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <string>
#include "OgreFrameListener.h"
#include "OgreWindowEventUtilities.h"

class ClientManager;
class Application : public Ogre::FrameListener, public Ogre::WindowEventListener
{
public:
	Application(const std::string& appTitle = "");
    virtual ~Application();

    virtual void Run();
    virtual bool Setup();
    virtual void Cleanup();

public:
    // Ogre::FrameListener
    virtual bool frameEnded(const Ogre::FrameEvent& event);
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& event);
    virtual bool frameStarted(const Ogre::FrameEvent& event);

    // Ogre::WindwoEventListener
    virtual void windowResized(Ogre::RenderWindow* renderWindow);
    virtual void windowClosed(Ogre::RenderWindow* renderWindow);

private:
    void CreateFrameListener();

private:
    ClientManager* m_pClientManager;
};

#endif  // __APPLICATION_H__
