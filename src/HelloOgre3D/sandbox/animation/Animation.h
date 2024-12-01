#ifndef __ANIMATION_H__
#define __ANIMATION_H__

namespace Ogre
{
	class AnimationState;
}

namespace Fancy //tolua_exports
{ //tolua_exports
    class Animation //tolua_exports
    { //tolua_exports
    public:
        Animation(Ogre::AnimationState* pAnimState);
        ~Animation();

        //tolua_begin
        void Init();
        void Clear();

        void SetEnabled(bool enable);
        void SetLooping(bool looping);
        void AddTime(float deltaTime);

        float GetTime();
        float GetLength();
        float GetWeight();
        //tolua_end
    private:
        Ogre::AnimationState* m_pAnimState;

    }; //tolua_exports

} //tolua_exports

#endif  // __ANIMATION_H__
