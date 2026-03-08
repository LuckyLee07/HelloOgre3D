#ifndef HELLO_OGRE_CAMERA_CONTROLLER_H
#define HELLO_OGRE_CAMERA_CONTROLLER_H

#include "OgreCamera.h"
#include "OgreFrameListener.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OIS.h"

#include <cmath>
#include <limits>

class OgreCameraController {
public:
    enum CameraStyle {
        CS_FREELOOK,
        CS_ORBIT,
        CS_MANUAL
    };

    explicit OgreCameraController(Ogre::Camera* cam)
        : mCamera(nullptr)
        , mStyle(CS_MANUAL)
        , mTarget(nullptr)
        , mOrbiting(false)
        , mZooming(false)
        , mTopSpeed(150.0f)
        , mVelocity(Ogre::Vector3::ZERO)
        , mGoingForward(false)
        , mGoingBack(false)
        , mGoingLeft(false)
        , mGoingRight(false)
        , mGoingUp(false)
        , mGoingDown(false)
        , mFastMove(false)
        , mMouseSensitivity(0.09f)
        , mMouseSmoothing(0.45f)
        , mMouseDeadZone(0.01f)
        , mSmoothedMouseDeltaX(0.0f)
        , mSmoothedMouseDeltaY(0.0f) {
        setCamera(cam);
        setStyle(CS_FREELOOK);
    }

    virtual ~OgreCameraController() {}

    virtual void setCamera(Ogre::Camera* cam) {
        mCamera = cam;
        resetMouseSmoothing();
    }

    virtual Ogre::Camera* getCamera() {
        return mCamera;
    }

    virtual void setTarget(Ogre::SceneNode* target) {
        if (target == mTarget) {
            return;
        }

        mTarget = target;
        if (!mCamera) {
            return;
        }

        if (mTarget) {
            setYawPitchDist(Ogre::Degree(0), Ogre::Degree(15), 150);
            mCamera->setAutoTracking(true, mTarget);
        } else {
            mCamera->setAutoTracking(false);
        }
    }

    virtual Ogre::SceneNode* getTarget() {
        return mTarget;
    }

    virtual void setYawPitchDist(Ogre::Radian yaw, Ogre::Radian pitch, Ogre::Real dist) {
        if (!mCamera || !mTarget) {
            return;
        }

        mCamera->setPosition(mTarget->_getDerivedPosition());
        mCamera->setOrientation(mTarget->_getDerivedOrientation());
        mCamera->yaw(yaw);
        mCamera->pitch(-pitch);
        mCamera->moveRelative(Ogre::Vector3(0, 0, dist));
        resetMouseSmoothing();
    }

    virtual void setTopSpeed(Ogre::Real topSpeed) {
        mTopSpeed = topSpeed;
    }

    virtual Ogre::Real getTopSpeed() {
        return mTopSpeed;
    }

    virtual void setStyle(CameraStyle style) {
        if (!mCamera) {
            mStyle = style;
            return;
        }

        if (mStyle != CS_ORBIT && style == CS_ORBIT) {
            Ogre::SceneNode* orbitTarget = mTarget;
            if (!orbitTarget && mCamera->getSceneManager()) {
                orbitTarget = mCamera->getSceneManager()->getRootSceneNode();
            }
            setTarget(orbitTarget);
            mCamera->setFixedYawAxis(true);
            manualStop();
            setYawPitchDist(Ogre::Degree(0), Ogre::Degree(15), 150);
        } else if (mStyle != CS_FREELOOK && style == CS_FREELOOK) {
            mCamera->setAutoTracking(false);
            mCamera->setFixedYawAxis(true);
            resetMouseSmoothing();
        } else if (mStyle != CS_MANUAL && style == CS_MANUAL) {
            mCamera->setAutoTracking(false);
            manualStop();
        }

        mStyle = style;
    }

    virtual CameraStyle getStyle() {
        return mStyle;
    }

    virtual void manualStop() {
        if (mStyle != CS_FREELOOK) {
            return;
        }

        mGoingForward = false;
        mGoingBack = false;
        mGoingLeft = false;
        mGoingRight = false;
        mGoingUp = false;
        mGoingDown = false;
        mVelocity = Ogre::Vector3::ZERO;
        resetMouseSmoothing();
    }

    virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) {
        if (mStyle != CS_FREELOOK || !mCamera) {
            return true;
        }

        Ogre::Vector3 accel = Ogre::Vector3::ZERO;
        if (mGoingForward) accel += mCamera->getDirection();
        if (mGoingBack) accel -= mCamera->getDirection();
        if (mGoingRight) accel += mCamera->getRight();
        if (mGoingLeft) accel -= mCamera->getRight();
        if (mGoingUp) accel += mCamera->getUp();
        if (mGoingDown) accel -= mCamera->getUp();

        const Ogre::Real topSpeed = mFastMove ? mTopSpeed * 20.0f : mTopSpeed;
        if (accel.squaredLength() > 0.0f) {
            accel.normalise();
            mVelocity += accel * topSpeed * evt.timeSinceLastFrame * 10.0f;
        } else {
            mVelocity -= mVelocity * evt.timeSinceLastFrame * 10.0f;
        }

        const Ogre::Real tooSmall = std::numeric_limits<Ogre::Real>::epsilon();
        if (mVelocity.squaredLength() > topSpeed * topSpeed) {
            mVelocity.normalise();
            mVelocity *= topSpeed;
        } else if (mVelocity.squaredLength() < tooSmall * tooSmall) {
            mVelocity = Ogre::Vector3::ZERO;
        }

        if (mVelocity != Ogre::Vector3::ZERO) {
            mCamera->move(mVelocity * evt.timeSinceLastFrame);
        }

        return true;
    }

    virtual void injectKeyDown(const OIS::KeyEvent& evt) {
        if (mStyle != CS_FREELOOK) {
            return;
        }

        if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = true;
        else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = true;
        else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = true;
        else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = true;
        else if (evt.key == OIS::KC_PGUP) mGoingUp = true;
        else if (evt.key == OIS::KC_PGDOWN) mGoingDown = true;
        else if (evt.key == OIS::KC_LSHIFT) mFastMove = true;
    }

    virtual void injectKeyUp(const OIS::KeyEvent& evt) {
        if (mStyle != CS_FREELOOK) {
            return;
        }

        if (evt.key == OIS::KC_W || evt.key == OIS::KC_UP) mGoingForward = false;
        else if (evt.key == OIS::KC_S || evt.key == OIS::KC_DOWN) mGoingBack = false;
        else if (evt.key == OIS::KC_A || evt.key == OIS::KC_LEFT) mGoingLeft = false;
        else if (evt.key == OIS::KC_D || evt.key == OIS::KC_RIGHT) mGoingRight = false;
        else if (evt.key == OIS::KC_PGUP) mGoingUp = false;
        else if (evt.key == OIS::KC_PGDOWN) mGoingDown = false;
        else if (evt.key == OIS::KC_LSHIFT) mFastMove = false;
    }

    virtual void injectMouseMove(const OIS::MouseEvent& evt) {
        if (!mCamera) {
            return;
        }

        if (mStyle == CS_ORBIT) {
            if (!mTarget) {
                return;
            }

            const Ogre::Real dist = (mCamera->getPosition() - mTarget->_getDerivedPosition()).length();
            if (mOrbiting) {
                mCamera->setPosition(mTarget->_getDerivedPosition());
                mCamera->yaw(Ogre::Degree(-evt.state.X.rel * 0.25f));
                mCamera->pitch(Ogre::Degree(-evt.state.Y.rel * 0.25f));
                mCamera->moveRelative(Ogre::Vector3(0, 0, dist));
            } else if (mZooming) {
                mCamera->moveRelative(Ogre::Vector3(0, 0, evt.state.Y.rel * 0.004f * dist));
            } else if (evt.state.Z.rel != 0) {
                mCamera->moveRelative(Ogre::Vector3(0, 0, -evt.state.Z.rel * 0.0008f * dist));
            }
            return;
        }

        if (mStyle != CS_FREELOOK) {
            return;
        }

        const Ogre::Real rawX = static_cast<Ogre::Real>(evt.state.X.rel);
        const Ogre::Real rawY = static_cast<Ogre::Real>(evt.state.Y.rel);
        if (std::abs(rawX) <= mMouseDeadZone && std::abs(rawY) <= mMouseDeadZone) {
            return;
        }

        mSmoothedMouseDeltaX = mSmoothedMouseDeltaX * mMouseSmoothing + rawX * (1.0f - mMouseSmoothing);
        mSmoothedMouseDeltaY = mSmoothedMouseDeltaY * mMouseSmoothing + rawY * (1.0f - mMouseSmoothing);
        mCamera->yaw(Ogre::Degree(-mSmoothedMouseDeltaX * mMouseSensitivity));
        mCamera->pitch(Ogre::Degree(-mSmoothedMouseDeltaY * mMouseSensitivity));
    }

    virtual void injectMouseDown(const OIS::MouseEvent&, OIS::MouseButtonID id) {
        if (mStyle != CS_ORBIT) {
            return;
        }

        if (id == OIS::MB_Left) mOrbiting = true;
        else if (id == OIS::MB_Right) mZooming = true;
    }

    virtual void injectMouseUp(const OIS::MouseEvent&, OIS::MouseButtonID id) {
        if (mStyle != CS_ORBIT) {
            return;
        }

        if (id == OIS::MB_Left) mOrbiting = false;
        else if (id == OIS::MB_Right) mZooming = false;
    }

protected:
    void resetMouseSmoothing() {
        mSmoothedMouseDeltaX = 0.0f;
        mSmoothedMouseDeltaY = 0.0f;
    }

protected:
    Ogre::Camera* mCamera;
    CameraStyle mStyle;
    Ogre::SceneNode* mTarget;
    bool mOrbiting;
    bool mZooming;
    Ogre::Real mTopSpeed;
    Ogre::Vector3 mVelocity;
    bool mGoingForward;
    bool mGoingBack;
    bool mGoingLeft;
    bool mGoingRight;
    bool mGoingUp;
    bool mGoingDown;
    bool mFastMove;
    Ogre::Real mMouseSensitivity;
    Ogre::Real mMouseSmoothing;
    Ogre::Real mMouseDeadZone;
    Ogre::Real mSmoothedMouseDeltaX;
    Ogre::Real mSmoothedMouseDeltaY;
};

#endif
