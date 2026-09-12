#ifndef HELLO_OGRE_CAMERA_CONTROLLER_H
#define HELLO_OGRE_CAMERA_CONTROLLER_H

#include "OgreCamera.h"
#include "OgreFrameListener.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreMath.h"
#include "OIS.h"

#include <cmath>
#include <limits>

class OgreCameraController {
public:
    enum CameraStyle {
        CS_FREELOOK,
        CS_ORBIT,
        CS_MANUAL,
        CS_FOLLOW
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
        } else if (mStyle != CS_FOLLOW && style == CS_FOLLOW) {
            mCamera->setAutoTracking(false);
            mCamera->setFixedYawAxis(true);
            mHasFollowState = false; // 下次 updateFollow 会 snap 到理想位
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

    // Follow rotation is immediate; only the displayed target translation is filtered.
	void setFollowParams(Ogre::Real horz, Ogre::Real vert, Ogre::Real target,
	                     Ogre::Real eye, Ogre::Real spring) {
		(void)spring;
		mFollowHorzDist = horz;
		mFollowVertDist = vert;
		mFollowTargetDist = target;
		mFollowEyeHeight = eye;
	}

	void enterFollow() { setStyle(CS_FOLLOW); }
	void exitFollow() { mFollowPitchOffset = 0.0f; setStyle(CS_FREELOOK); }
	void resetFollowView() { mFollowPitchOffset = 0.0f; mHasFollowState = false; }

	void rotateFollowView(Ogre::Real yaw, Ogre::Real pitch) {
		if (mStyle != CS_FOLLOW) return;
		mFollowForward = Ogre::Quaternion(Ogre::Radian(yaw), Ogre::Vector3::UNIT_Y) * mFollowForward;
		const Ogre::Real basePitch = Ogre::Math::ATan2(mFollowVertDist - mFollowEyeHeight,
			mFollowHorzDist + mFollowTargetDist).valueRadians();
		const Ogre::Real desiredPitch = basePitch + mFollowPitchOffset + pitch;
		mFollowPitchOffset = Ogre::Math::Clamp(desiredPitch,
			Ogre::Degree(8).valueRadians(), Ogre::Degree(65).valueRadians()) - basePitch;
	}

	// Simulation submits direction; presentation supplies the interpolated physics position.
	void updateFollow(const Ogre::Vector3& targetPos, const Ogre::Vector3& forwardXZ, Ogre::Real dtSec) {
		if (mStyle != CS_FOLLOW || !mCamera) return;
		if (!forwardXZ.isZeroLength() && !forwardXZ.isNaN()) {
			mFollowForward = forwardXZ;
			mFollowForward.y = 0.0f;
			mFollowForward.normalise();
		}
		if (!mHasFollowState || dtSec <= 0.0f) {
			mHasFollowState = false;
			renderFollow(targetPos, 0.0f);
		}
	}

	void renderFollow(const Ogre::Vector3& targetPos, Ogre::Real dtSec) {
		if (mStyle != CS_FOLLOW || !mCamera || targetPos.isNaN()) return;
		if (!mHasFollowState || (targetPos - mFollowTargetPos).squaredLength() > 16.0f) {
			mFollowTargetPos = targetPos;
			mHasFollowState = true;
		} else if (dtSec > 0.0f) {
			const Ogre::Real blend = 1.0f - std::exp(-mFollowTargetSmooth * dtSec);
			mFollowTargetPos += (targetPos - mFollowTargetPos) * blend;
		}
		mFollowLook = mFollowTargetPos + mFollowForward * mFollowTargetDist
			+ Ogre::Vector3::UNIT_Y * mFollowEyeHeight;
		const Ogre::Real horizontal = mFollowHorzDist + mFollowTargetDist;
		const Ogre::Real vertical = mFollowVertDist - mFollowEyeHeight;
		const Ogre::Real radius = Ogre::Math::Sqrt(horizontal * horizontal + vertical * vertical);
		const Ogre::Real pitch = Ogre::Math::ATan2(vertical, horizontal).valueRadians() + mFollowPitchOffset;
		mFollowActualPos = mFollowLook - mFollowForward * (radius * std::cos(pitch))
			+ Ogre::Vector3::UNIT_Y * (radius * std::sin(pitch));
		mCamera->setPosition(mFollowActualPos);
		mCamera->lookAt(mFollowLook);
	}

	const Ogre::Vector3& getFollowForward() const { return mFollowForward; }
	const Ogre::Vector3& getFollowLook() const { return mFollowLook; }
	const Ogre::Vector3& getFollowIdealPosition() const { return mFollowActualPos; }

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

    // CS_FOLLOW 第三人称跟随状态（Sandbox19 尺度初值，PlayerController 可覆盖）。
    Ogre::Vector3 mFollowTargetPos = Ogre::Vector3::ZERO;
    Ogre::Vector3 mFollowForward = Ogre::Vector3::UNIT_Z;
    Ogre::Vector3 mFollowActualPos = Ogre::Vector3::ZERO;
	Ogre::Vector3 mFollowLook = Ogre::Vector3::ZERO;
	Ogre::Real mFollowPitchOffset = 0.0f;
    Ogre::Real mFollowHorzDist = 8.0f;
    Ogre::Real mFollowVertDist = 4.0f;
    Ogre::Real mFollowTargetDist = 3.0f;
    Ogre::Real mFollowEyeHeight = 1.5f;
    Ogre::Real mFollowTargetSmooth = 20.0f; // 被跟随点低通速率（越大越贴、越小越顺）
    bool mHasFollowState = false;
};

#endif
