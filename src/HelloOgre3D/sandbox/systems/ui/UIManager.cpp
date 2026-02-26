#include "UIManager.h"
#include "UIFrame.h"
#include "AppConfig.h"
#include "GameManager.h"

UIManager::UIManager(GameManager* gameMnanager)
	: m_pGameManager(gameMnanager), m_pUIScene(nullptr), m_pMarkupText(nullptr)
{
	std::fill_n(m_pUILayers, UI_LAYER_COUNT, nullptr);
}

UIManager::~UIManager()
{
	if (m_pUIScene == nullptr)
		return;

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
	if (pSilverback == nullptr)
		return;
	Ogre::Camera* pCamera = m_pGameManager->getCamera();
	m_pUIScene = pSilverback->createScreen(pCamera->getViewport(), DEFAULT_ATLAS);
	if (m_pUIScene == nullptr)
		return;

	Gorilla::Layer* baseLayer = GetUILayer();
	if (baseLayer == nullptr)
		return;

	m_pMarkupText = baseLayer->createMarkupText(
		91, m_pUIScene->getWidth(), m_pUIScene->getHeight(),
		"Learning Game AI Programming. " __TIMESTAMP__);
	if (m_pMarkupText == nullptr)
		return;

	Ogre::Real leftPos = m_pUIScene->getWidth() - m_pMarkupText->maxTextWidth() - 4;
	Ogre::Real topPos = m_pUIScene->getHeight() - m_pUIScene->getAtlas()->getGlyphData(9)->mLineHeight - 4;
	m_pMarkupText->left(leftPos);
	m_pMarkupText->top(topPos);
}

Gorilla::Layer* UIManager::GetUILayer(unsigned int index)
{
	if (index >= UI_LAYER_COUNT) return nullptr;
	if (m_pUIScene == nullptr) return nullptr;

	if (m_pUILayers[index] == nullptr)
	{
		m_pUILayers[index] = m_pUIScene->createLayer(index);
	}
	return m_pUILayers[index];
}

Ogre::Real UIManager::GetScreenWidth()
{
	if (m_pUIScene == nullptr) return 0.0f;
	return m_pUIScene->getWidth();
}

Ogre::Real UIManager::GetScreenHeight()
{
	if (m_pUIScene == nullptr) return 0.0f;
	return m_pUIScene->getHeight();
}

UIFrame* UIManager::CreateUIFrame(unsigned int index)
{
	if (index < UI_LAYER_COUNT)
	{
		Gorilla::Layer* layer = GetUILayer(index);
		if (layer == nullptr)
			return nullptr;

		UIFrame* pFrame = new UIFrame(layer);
		pFrame->Initialize();

		m_uiframes.push_back(pFrame);
		return pFrame;
	}
	return nullptr;
}

void UIManager::SetMarkupColor(unsigned int index, const Ogre::ColourValue& color)
{
	if (m_pUIScene == nullptr)
		return;

	for (size_t layerIndex = 0; layerIndex < UI_LAYER_COUNT; layerIndex++)
	{
		Gorilla::Layer* layer = GetUILayer(layerIndex);
		if (layer != nullptr)
			layer->_getAtlas()->setMarkupColour(index, color);
	}
}

void UIManager::HandleWindowResized(unsigned int width, unsigned int height)
{
	if (m_pUIScene == nullptr || m_pMarkupText == nullptr)
		return;

	Ogre::Real leftPos = width - m_pMarkupText->maxTextWidth() - 4;
	Ogre::Real topPos = height - m_pUIScene->getAtlas()->getGlyphData(9)->mLineHeight - 4;
	m_pMarkupText->left(leftPos);
	m_pMarkupText->top(topPos);
}
