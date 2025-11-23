#include "UIManager.h"
#include "UIFrame.h"
#include "GameManager.h"
#include "GameDefine.h"
#include "manager/ClientManager.h"

UIManager::UIManager(ClientManager* clientMnanager)
	: m_pClientManager(clientMnanager), m_pUIScene(nullptr), m_pMarkupText(nullptr)
{
	std::fill_n(m_pUILayers, UI_LAYER_COUNT, nullptr);
}

UIManager::~UIManager()
{
	for (size_t index = 0; index < UI_LAYER_COUNT; index++)
	{
		if (m_pUILayers[index] != nullptr)
			m_pUIScene->destroy(m_pUILayers[index]);
		m_pUILayers[index] = nullptr;
	}
	Gorilla::Silverback* pSilverback = Gorilla::Silverback::getSingletonPtr();
	pSilverback->destroyScreen(m_pUIScene);
	m_pUIScene = nullptr;
}

void UIManager::InitConfig()
{
	Gorilla::Silverback* pSilverback = Gorilla::Silverback::getSingletonPtr();
	Ogre::Camera* pCamera = m_pClientManager->getCamera();
	m_pUIScene = pSilverback->createScreen(pCamera->getViewport(), DEFAULT_ATLAS);

	m_pMarkupText = GetUILayer()->createMarkupText(
		91, m_pUIScene->getWidth(), m_pUIScene->getHeight(),
		"Learning Game AI Programming. " __TIMESTAMP__);

	Ogre::Real leftPos = m_pUIScene->getWidth() - m_pMarkupText->maxTextWidth() - 4;
	Ogre::Real topPos = m_pUIScene->getHeight() - m_pUIScene->getAtlas()->getGlyphData(9)->mLineHeight - 4;
	m_pMarkupText->left(leftPos);
	m_pMarkupText->top(topPos);
}

Gorilla::Layer* UIManager::GetUILayer(unsigned int index)
{
	if (index >= UI_LAYER_COUNT) return nullptr;

	if (m_pUILayers[index] == nullptr)
	{
		m_pUILayers[index] = m_pUIScene->createLayer(index);
	}
	return m_pUILayers[index];
}

Ogre::Real UIManager::GetScreenWidth()
{
	return m_pUIScene->getWidth();
}

Ogre::Real UIManager::GetScreenHeight()
{
	return m_pUIScene->getHeight();
}

UIFrame* UIManager::CreateUIFrame(unsigned int index)
{
	if (index < UI_LAYER_COUNT)
	{
		UIFrame* pFrame = new UIFrame(GetUILayer(index));
		pFrame->Initialize();

		m_uiframes.push_back(pFrame);
		return pFrame;
	}
	return nullptr;
}

void UIManager::SetMarkupColor(unsigned int index, const Ogre::ColourValue& color)
{
	for (size_t layerIndex = 0; layerIndex < UI_LAYER_COUNT; layerIndex++)
	{
		GetUILayer(layerIndex)->_getAtlas()->setMarkupColour(index, color);
	}
}

void UIManager::HandleWindowResized(unsigned int width, unsigned int height)
{
	Ogre::Real leftPos = width - m_pMarkupText->maxTextWidth() - 4;
	Ogre::Real topPos = height - m_pUIScene->getAtlas()->getGlyphData(9)->mLineHeight - 4;
	m_pMarkupText->left(leftPos);
	m_pMarkupText->top(topPos);
}