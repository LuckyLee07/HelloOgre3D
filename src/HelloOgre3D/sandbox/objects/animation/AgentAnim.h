#ifndef __AGENT_ANIM_H__
#define __AGENT_ANIM_H__

namespace Ogre
{
	class AnimationState;
}

class AgentAnim //tolua_exports
{ //tolua_exports
public:
	AgentAnim(Ogre::AnimationState* pAnimState);
    ~AgentAnim();

    //tolua_begin
    void Init(float startTime = 0.0f);
    void Clear();

    void SetEnabled(bool enable);
    void SetLooping(bool looping);
    void AddTime(float deltaTime);

    float GetTime();
    float GetLength();
    float GetWeight();
    void SetWeight(float weight);

    //tolua_end
private:
    Ogre::AnimationState* m_pAnimState;

}; //tolua_exports

#endif  // __AGENT_ANIM_H__
