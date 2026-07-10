#ifndef HELLO_OGRE_CAMERA_CONTROLLER_H
#define HELLO_OGRE_CAMERA_CONTROLLER_H

#include "OgreCamera.h"
#include "OgreFrameListener.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreEntity.h"
#include "OIS.h"

#include <cmath>
#include <limits>
#include <cstdio>
#include <cstdlib>

class OgreCameraController {
public:
    enum CameraStyle {
        CS_FREELOOK,
        CS_ORBIT,
        CS_MANUAL,
        CS_FPS
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
        , mSmoothedMouseDeltaY(0.0f)
        , mEyeHeight(3.5f)
        , mMaxPitchDeg(80.0f)
        , mAccumPitchDeg(0.0f)
		, mHasFpsAnchor(false)
		, mFpsAnchorPosition(Ogre::Vector3::ZERO)
        , mWeaponNode(nullptr)
        , mWeaponEntity(nullptr)
        , mWeaponTried(false)
        , mWpnOffset(0.5f, -0.45f, -1.5f)
        , mWpnYaw(0.0f)
        , mWpnPitch(0.0f)
        , mWpnRoll(0.0f)
        , mWpnScale(1.2f) {
        setCamera(cam);
        setStyle(CS_FREELOOK);
    }

    virtual ~OgreCameraController() {
        destroyFpsWeaponViewModel();
    }

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
        } else if (mStyle != CS_FPS && style == CS_FPS) {
            // 第一人称：贴地、固定眼睛高度、鼠标控朝向、WASD 水平移动、pitch 钳制。
            mCamera->setAutoTracking(false);
            mCamera->setFixedYawAxis(true);
            mGoingForward = mGoingBack = mGoingLeft = mGoingRight = mGoingUp = mGoingDown = false;
            mVelocity = Ogre::Vector3::ZERO;
            resetMouseSmoothing();
			Ogre::Vector3 fpsPos = mHasFpsAnchor ? mFpsAnchorPosition : mCamera->getPosition();
			fpsPos.y = mHasFpsAnchor ? mFpsAnchorPosition.y + mEyeHeight : mEyeHeight;
            mCamera->setPosition(fpsPos);
            mAccumPitchDeg = 0.0f; // 进入 FPS 默认水平视角，由每帧 pitch 修正维持
        }

        mStyle = style;
    }

    virtual CameraStyle getStyle() {
        return mStyle;
    }

	virtual void setFpsAnchorPosition(const Ogre::Vector3& position) {
		mFpsAnchorPosition = position;
		mHasFpsAnchor = true;
	}

	virtual void clearFpsAnchor() {
		mHasFpsAnchor = false;
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
        updateFpsWeapon();
        if (!mCamera || (mStyle != CS_FREELOOK && mStyle != CS_FPS)) {
            return true;
        }

        const bool fps = (mStyle == CS_FPS);
        Ogre::Vector3 forward = mCamera->getDirection();
        Ogre::Vector3 right = mCamera->getRight();
        if (fps) {
            // 移动限制在水平面：去掉前向/右向的竖直分量，避免"飞起来"。
            forward.y = 0.0f;
            if (forward.squaredLength() > 1e-6f) forward.normalise();
            right.y = 0.0f;
            if (right.squaredLength() > 1e-6f) right.normalise();
        }

        Ogre::Vector3 accel = Ogre::Vector3::ZERO;
        if (mGoingForward) accel += forward;
        if (mGoingBack) accel -= forward;
        if (mGoingRight) accel += right;
        if (mGoingLeft) accel -= right;
        if (!fps) {
            if (mGoingUp) accel += mCamera->getUp();
            if (mGoingDown) accel -= mCamera->getUp();
        }

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

        if (fps) {
            // 每帧把 pitch 修正到累积值（默认 0=水平，鼠标右键拖动可调），
            // 从而覆盖 sample 用 setOrientation 设的俯视朝向；并钳到固定眼睛高度。
            Ogre::Real cy = mCamera->getDirection().y;
            if (cy > 1.0f) cy = 1.0f; else if (cy < -1.0f) cy = -1.0f;
            const Ogre::Real curPitch = std::asin(cy) * 57.2957795f;
            const Ogre::Real corr = mAccumPitchDeg - curPitch;
            if (std::fabs(corr) > 0.05f) mCamera->pitch(Ogre::Degree(corr));
			Ogre::Vector3 pos = mHasFpsAnchor ? mFpsAnchorPosition : mCamera->getPosition();
			pos.y = mHasFpsAnchor ? mFpsAnchorPosition.y + mEyeHeight : mEyeHeight;
            mCamera->setPosition(pos);
        }

        return true;
    }

    virtual void injectKeyDown(const OIS::KeyEvent& evt) {
        if (mStyle != CS_FREELOOK && mStyle != CS_FPS) {
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
        if (mStyle != CS_FREELOOK && mStyle != CS_FPS) {
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

        if (mStyle != CS_FREELOOK && mStyle != CS_FPS) {
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
        Ogre::Real pitchDeg = -mSmoothedMouseDeltaY * mMouseSensitivity;
        if (mStyle == CS_FPS) {
            // pitch 钳制：不让视角翻过头顶/脚底。
            const Ogre::Real newPitch = mAccumPitchDeg + pitchDeg;
            if (newPitch > mMaxPitchDeg) pitchDeg = mMaxPitchDeg - mAccumPitchDeg;
            else if (newPitch < -mMaxPitchDeg) pitchDeg = -mMaxPitchDeg - mAccumPitchDeg;
            mAccumPitchDeg += pitchDeg;
        }
        mCamera->pitch(Ogre::Degree(pitchDeg));
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

    void destroyFpsWeaponViewModel() {
        Ogre::SceneManager* sm = mCamera != nullptr ? mCamera->getSceneManager() : nullptr;
        if (sm != nullptr && mWeaponNode != nullptr) {
            try {
                mWeaponNode->detachAllObjects();
                sm->destroySceneNode(mWeaponNode);
            } catch (...) {
            }
        }
        mWeaponNode = nullptr;

        if (sm != nullptr && mWeaponEntity != nullptr) {
            try {
                sm->destroyEntity(mWeaponEntity);
            } catch (...) {
            }
        }
        mWeaponEntity = nullptr;
    }

    // FPS 武器 view model：把武器 mesh 挂在相机前方右下，跟随相机；非 FPS 隐藏。
    void updateFpsWeapon() {
        if (!mCamera) {
            return;
        }
        const bool fps = (mStyle == CS_FPS);
        if (fps && !mWeaponTried) {
            mWeaponTried = true;
            loadWeaponTuning();
            Ogre::SceneManager* sm = mCamera->getSceneManager();
            if (sm) {
                try {
                    mWeaponEntity = sm->createEntity("FpsWeaponViewModel", "models/futuristic_soldier/soldier_weapon.mesh");
                    mWeaponNode = sm->getRootSceneNode()->createChildSceneNode();
                    mWeaponNode->attachObject(mWeaponEntity);
                    // 尽量画在场景之后，减少被近处墙体遮挡。
                    mWeaponEntity->setRenderQueueGroup(Ogre::RENDER_QUEUE_MAIN + 1);
                } catch (...) {
                    destroyFpsWeaponViewModel();
                }
            }
        }
        if (mWeaponNode) {
            mWeaponNode->setVisible(fps);
            if (fps) {
                const Ogre::Quaternion camOrient = mCamera->getOrientation();
                mWeaponNode->setPosition(mCamera->getPosition() + camOrient * mWpnOffset);
                // 本地朝向修正：先 yaw（绕 Y 让枪管转向前），再 pitch（绕 X 抬/压枪口），再 roll（绕 Z 翻正）。
                const Ogre::Quaternion localRot =
                    Ogre::Quaternion(Ogre::Degree(mWpnYaw), Ogre::Vector3::UNIT_Y) *
                    Ogre::Quaternion(Ogre::Degree(mWpnPitch), Ogre::Vector3::UNIT_X) *
                    Ogre::Quaternion(Ogre::Degree(mWpnRoll), Ogre::Vector3::UNIT_Z);
                mWeaponNode->setOrientation(camOrient * localRot);
                mWeaponNode->setScale(mWpnScale, mWpnScale, mWpnScale);
            }
        }
    }

    // 武器摆放可用环境变量在运行期微调，免去反复重编译：
    //   HELLO_FPS_WPN_OFF   = "x,y,z"            相机空间偏移（右/上/前为正，前=-z）
    //   HELLO_FPS_WPN_ROT   = "yaw,pitch,roll"   本地修正角（度）
    //   HELLO_FPS_WPN_SCALE = "s"                统一缩放
    void loadWeaponTuning() {
        const char* off = std::getenv("HELLO_FPS_WPN_OFF");
        if (off) {
            float x = 0, y = 0, z = 0;
            if (std::sscanf(off, "%f,%f,%f", &x, &y, &z) == 3) {
                mWpnOffset = Ogre::Vector3(x, y, z);
            }
        }
        const char* rot = std::getenv("HELLO_FPS_WPN_ROT");
        if (rot) {
            float yaw = 0, pitch = 0, roll = 0;
            if (std::sscanf(rot, "%f,%f,%f", &yaw, &pitch, &roll) == 3) {
                mWpnYaw = yaw; mWpnPitch = pitch; mWpnRoll = roll;
            }
        }
        const char* sc = std::getenv("HELLO_FPS_WPN_SCALE");
        if (sc) {
            const Ogre::Real s = static_cast<Ogre::Real>(std::atof(sc));
            if (s > 0.0f) mWpnScale = s;
        }
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
    Ogre::Real mEyeHeight;
    Ogre::Real mMaxPitchDeg;
    Ogre::Real mAccumPitchDeg;
	bool mHasFpsAnchor;
	Ogre::Vector3 mFpsAnchorPosition;
    Ogre::SceneNode* mWeaponNode;
    Ogre::Entity* mWeaponEntity;
    bool mWeaponTried;
    Ogre::Vector3 mWpnOffset;
    Ogre::Real mWpnYaw;
    Ogre::Real mWpnPitch;
    Ogre::Real mWpnRoll;
    Ogre::Real mWpnScale;
};

#endif
