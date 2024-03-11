
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
		const char* pTitle = error.getFullDescription().c_str();
		const char* pCaption = "An exception has occurred!";
        MessageBox(NULL, pTitle, pCaption, MB_OK | MB_ICONERROR | MB_TASKMODAL);
    }

    return 0;
}