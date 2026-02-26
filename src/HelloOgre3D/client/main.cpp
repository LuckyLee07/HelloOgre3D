
#include "DemoHelloWorld.h"
#include "OgreException.h"
#include <iostream>
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#if defined(__APPLE__)
#include <ApplicationServices/ApplicationServices.h>
extern "C" bool NSApplicationLoad(void);
#endif

int main()
{
#if defined(__APPLE__)
    ProcessSerialNumber psn = {0, kCurrentProcess};
    TransformProcessType(&psn, kProcessTransformToForegroundApplication);
    SetFrontProcess(&psn);
    NSApplicationLoad();
#endif

    DemoHelloWorld application;

    try
    {
        application.Run();
    }
    catch(Ogre::Exception& error)
    {
        const char* pTitle = error.getFullDescription().c_str();
        const char* pCaption = "An exception has occurred!";
#if defined(_WIN32)
        MessageBox(NULL, pTitle, pCaption, MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << pCaption << "\n" << pTitle << std::endl;
#endif
    }

    return 0;
}
