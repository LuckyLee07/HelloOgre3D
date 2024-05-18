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
    virtual bool frameEnded(const Ogre::FrameEvent& event) override;
    virtual bool frameRenderingQueued(const Ogre::FrameEvent& event) override;
    virtual bool frameStarted(const Ogre::FrameEvent& event) override;

    // Ogre::WindwoEventListener
    virtual void windowResized(Ogre::RenderWindow* renderWindow) override;
    virtual void windowClosed(Ogre::RenderWindow* renderWindow) override;

private:
    void CreateEventListener();

private:
    ClientManager* m_pClientManager;
};

#endif  // __APPLICATION_H__
