#include "UIService.h"
#include "GameManager.h"
#include "Object/UIComponent.h"

UIComponent* UIService::CreateUIComponent(unsigned int index)
{
	return g_GameManager->createUIComponent(index);
}

void UIService::SetMarkupColor(unsigned int index, const Ogre::ColourValue& color)
{
	return g_GameManager->setMarkupColor(index, color);
}