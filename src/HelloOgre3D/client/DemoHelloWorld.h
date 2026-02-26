#ifndef __DEMO_HELLOWORLD_H__
#define __DEMO_HELLOWORLD_H__

#include "Application.h"

class DemoHelloWorld : public Application
{
public:
	DemoHelloWorld(void);

    virtual ~DemoHelloWorld(void);

    virtual void Run() override;
    virtual bool frameEnded(const Ogre::FrameEvent& event) override;

private:
    void InitGameScene();

private:
    bool m_sceneInitDone;
};

#endif  // __DEMO_HELLOWORLD_H__
