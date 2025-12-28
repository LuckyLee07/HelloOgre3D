#include "UIService.h"
#include "GameManager.h"
#include "systems/ui/UIFrame.h"
#include "systems/ui/UIManager.h"

UIService::UIService(UIManager* pMananger)
	: m_pUIManager(pMananger)
{
}

UIFrame* UIService::CreateUIFrame(unsigned int index)
{
	return m_pUIManager->CreateUIFrame(index);
}

void UIService::SetMarkupColor(unsigned int index, const Ogre::ColourValue& color)
{
	return m_pUIManager->SetMarkupColor(index, color);
}