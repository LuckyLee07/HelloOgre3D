#ifndef __DEMO_HELLOWORLD_H__
#define __DEMO_HELLOWORLD_H__

#include "Application.h"

class DemoHelloWorld : public Application
{
public:
	DemoHelloWorld(void);

    virtual ~DemoHelloWorld(void);

    virtual void Run();

private:
    void InitGameScene();
};

#endif  // __DEMO_HELLOWORLD_H__
