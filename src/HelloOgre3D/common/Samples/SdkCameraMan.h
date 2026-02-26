#ifndef HELLO_OGRE_SDK_CAMERA_MAN_H
#define HELLO_OGRE_SDK_CAMERA_MAN_H

#include "OgreCamera.h"
#include "OgreFrameListener.h"
#include "OIS.h"

namespace OgreBites {

class SdkCameraMan {
public:
    explicit SdkCameraMan(Ogre::Camera* cam)
        : mCamera(cam), mTopSpeed(1.0f) {}

    void setTopSpeed(Ogre::Real speed) { mTopSpeed = speed; }

    bool frameRenderingQueued(const Ogre::FrameEvent&) { return true; }

    void injectKeyDown(const OIS::KeyEvent&) {}
    void injectKeyUp(const OIS::KeyEvent&) {}
    void injectMouseMove(const OIS::MouseEvent&) {}
    void injectMouseDown(const OIS::MouseEvent&, OIS::MouseButtonID) {}
    void injectMouseUp(const OIS::MouseEvent&, OIS::MouseButtonID) {}

private:
    Ogre::Camera* mCamera;
    Ogre::Real mTopSpeed;
};

} // namespace OgreBites

#endif
