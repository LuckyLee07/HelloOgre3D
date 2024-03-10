#include "chapter_0_helloworld/include/DemoHelloWorld.h"
#include "ogre3d/include/OgreException.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

int main()
{
    DemoHelloWorld application;

    try
    {
        application.Run();
    }
    catch(Ogre::Exception& error)
    {
        MessageBox(
            NULL,
            error.getFullDescription().c_str(),
            "An exception has occurred!",
            MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }

    return 0;
}