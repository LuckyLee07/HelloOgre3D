#ifndef CHAPTER_0_HELLOWORLD_H
#define CHAPTER_0_HELLOWORLD_H

#include "demo_framework/include/SandboxApplication.h"

class DemoHelloWorld : public SandboxApplication
{
public:
    DemoHelloWorld(void);

    virtual ~DemoHelloWorld(void);

    virtual void Initialize();

    virtual void Update();

private:
    DemoHelloWorld(const DemoHelloWorld&);
    DemoHelloWorld& operator=(const DemoHelloWorld&);
};

#endif  // CHAPTER_0_HELLOWORLD_H
