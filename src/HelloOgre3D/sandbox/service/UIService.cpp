#include "UIService.h"
#include "GameManager.h"
#include "object/UIComponent.h"

UIService::UIService(GameManager* pMananger)
	: m_gameManager(pMananger)
{
}

UIComponent* UIService::CreateUIComponent(unsigned int index)
{
	return m_gameManager->createUIComponent(index);
}

void UIService::SetMarkupColor(unsigned int index, const Ogre::ColourValue& color)
{
	return m_gameManager->setMarkupColor(index, color);
}