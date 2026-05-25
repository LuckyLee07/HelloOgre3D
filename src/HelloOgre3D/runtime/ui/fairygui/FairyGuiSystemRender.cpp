#include "ui/fairygui/FairyGuiSystemInternal.h"

void FairyGuiSystemImpl::handleTrianglesCommand(const cocos2d::TrianglesCommand& command)
{
	if (m_pManualObject == nullptr || m_screenWidth == 0 || m_screenHeight == 0)
		return;

	const cocos2d::TrianglesCommand::Triangles& triangles = command.getTriangles();
	if (triangles.verts == nullptr || triangles.indices == nullptr || triangles.vertCount <= 0 || triangles.indexCount <= 0)
		return;

	if (m_stencilStage == cocos2d::STENCIL_STAGE_WRITE)
	{
		RecordStencilCommand(triangles.indexCount / 3);
		CollectStencilTriangle(command);
		return;
	}

	const std::string& materialName = GetMaterialName(command.getTexture());
	const int submittedTriangleCount = triangles.indexCount / 3;
	const bool hasScissorClip = m_scissorEnabled && m_scissorRect.size.width > 0.0f && m_scissorRect.size.height > 0.0f;
	const bool hasStencilClip = !m_stencilScopes.empty();
	if (hasScissorClip || hasStencilClip)
	{
		struct ActiveStencilScope
		{
			bool inverted;
			bool valid;
			std::vector<std::vector<ClipVertex> > polygons;
		};

		std::vector<ActiveStencilScope> activeScopes;
		activeScopes.reserve(m_stencilScopes.size());
		int stencilClipPolygonCount = 0;
		for (std::vector<StencilClipInfo>::const_iterator scopeIt = m_stencilScopes.begin(); scopeIt != m_stencilScopes.end(); ++scopeIt)
		{
			ActiveStencilScope activeScope;
			activeScope.inverted = scopeIt->inverted;
			activeScope.valid = scopeIt->valid;
			if (scopeIt->valid)
			{
				for (std::vector<StencilClipInfo::Polygon>::const_iterator polygonIt = scopeIt->polygons.begin(); polygonIt != scopeIt->polygons.end(); ++polygonIt)
				{
					std::vector<ClipVertex> polygon = BuildClipPolygon(polygonIt->points);
					if (IsValidClipPolygon(polygon))
						activeScope.polygons.push_back(polygon);
				}
				if (activeScope.polygons.empty())
				{
					std::vector<ClipVertex> rectPolygon = BuildRectClipPolygon(scopeIt->rect);
					if (IsValidClipPolygon(rectPolygon))
						activeScope.polygons.push_back(rectPolygon);
				}
				activeScope.valid = !activeScope.polygons.empty();
			}
			stencilClipPolygonCount += static_cast<int>(activeScope.polygons.size());
			activeScopes.push_back(activeScope);
		}

		const int stencilClipScopeCount = hasStencilClip ? static_cast<int>(activeScopes.size()) : 0;
		const std::vector<ClipVertex> scissorPolygon = hasScissorClip ? BuildRectClipPolygon(m_scissorRect) : std::vector<ClipVertex>();
		std::vector<ClipVertex> outputVertices;
		std::vector<std::vector<ClipVertex> > fragments;
		std::vector<std::vector<ClipVertex> > nextFragments;
		std::vector<ClipVertex> clippedPolygon;
		std::vector<ClipVertex> clipScratch;
		std::vector<ClipVertex> insideScratch;
		std::vector<ClipVertex> outsideScratch;
		outputVertices.reserve(static_cast<size_t>(triangles.indexCount));
		fragments.reserve(8);
		nextFragments.reserve(8);
		clippedPolygon.reserve(8);
		clipScratch.reserve(8);
		insideScratch.reserve(8);
		outsideScratch.reserve(8);
		int outputFragmentCount = 0;
		for (int index = 0; index + 2 < triangles.indexCount; index += 3)
		{
			std::vector<ClipVertex> triangle;
			triangle.resize(3);
			bool validTriangle = true;
			for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
			{
				const int sourceIndex = triangles.indices[index + vertexIndex];
				if (sourceIndex < 0 || sourceIndex >= triangles.vertCount)
				{
					validTriangle = false;
					break;
				}

				const cocos2d::V3F_C4B_T2F& vertex = triangles.verts[sourceIndex];
				triangle[vertexIndex].x = TransformX(command.getTransform(), vertex.vertices);
				triangle[vertexIndex].y = TransformY(command.getTransform(), vertex.vertices);
				triangle[vertexIndex].u = vertex.texCoords.u;
				triangle[vertexIndex].v = vertex.texCoords.v;
				triangle[vertexIndex].color = ToOgreColor(vertex.colors);
			}

			if (!validTriangle || !IsValidClipPolygon(triangle))
				continue;

			fragments.clear();
			fragments.push_back(triangle);
			if (hasScissorClip)
			{
				nextFragments.clear();
				for (std::vector<std::vector<ClipVertex> >::const_iterator fragmentIt = fragments.begin(); fragmentIt != fragments.end(); ++fragmentIt)
				{
					ClipPolygonToConvexPolygon(*fragmentIt, scissorPolygon, clippedPolygon, clipScratch);
					if (IsValidClipPolygon(clippedPolygon))
						nextFragments.push_back(clippedPolygon);
				}
				fragments.swap(nextFragments);
				if (fragments.empty())
					continue;
			}

			for (std::vector<ActiveStencilScope>::const_iterator scopeIt = activeScopes.begin(); scopeIt != activeScopes.end(); ++scopeIt)
			{
				if (!scopeIt->valid)
				{
					if (scopeIt->inverted)
						continue;
					fragments.clear();
					break;
				}

				if (scopeIt->inverted)
				{
					for (std::vector<std::vector<ClipVertex> >::const_iterator maskIt = scopeIt->polygons.begin(); maskIt != scopeIt->polygons.end(); ++maskIt)
					{
						nextFragments.clear();
						for (std::vector<std::vector<ClipVertex> >::const_iterator fragmentIt = fragments.begin(); fragmentIt != fragments.end(); ++fragmentIt)
							SubtractConvexPolygon(*fragmentIt, *maskIt, nextFragments, insideScratch, outsideScratch);
						fragments.swap(nextFragments);
						if (fragments.empty())
							break;
					}
				}
				else
				{
					nextFragments.clear();
					for (std::vector<std::vector<ClipVertex> >::const_iterator fragmentIt = fragments.begin(); fragmentIt != fragments.end(); ++fragmentIt)
					{
						for (std::vector<std::vector<ClipVertex> >::const_iterator maskIt = scopeIt->polygons.begin(); maskIt != scopeIt->polygons.end(); ++maskIt)
						{
							ClipPolygonToConvexPolygon(*fragmentIt, *maskIt, clippedPolygon, clipScratch);
							if (IsValidClipPolygon(clippedPolygon))
								nextFragments.push_back(clippedPolygon);
						}
					}
					fragments.swap(nextFragments);
				}

				if (fragments.empty())
					break;
			}

			outputFragmentCount += static_cast<int>(fragments.size());
			for (std::vector<std::vector<ClipVertex> >::const_iterator fragmentIt = fragments.begin(); fragmentIt != fragments.end(); ++fragmentIt)
				AppendPolygonTriangles(*fragmentIt, outputVertices);
		}

		if (outputVertices.empty())
		{
			RecordCpuClipWork(submittedTriangleCount, 0, outputFragmentCount, stencilClipScopeCount, stencilClipPolygonCount);
			RecordDrawCommand(command.getTexture(), materialName, triangles.vertCount, submittedTriangleCount, 0, true);
			return;
		}

		const int outputTriangleCount = static_cast<int>(outputVertices.size() / 3);
		RecordCpuClipWork(submittedTriangleCount, outputTriangleCount, outputFragmentCount, stencilClipScopeCount, stencilClipPolygonCount);
		RecordDrawCommand(command.getTexture(), materialName, static_cast<int>(outputVertices.size()), submittedTriangleCount, outputTriangleCount, true);
		m_pManualObject->begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
		m_pManualObject->estimateVertexCount(outputVertices.size());
		m_pManualObject->estimateIndexCount(outputVertices.size());
		for (size_t index = 0; index < outputVertices.size(); ++index)
		{
			const ClipVertex& vertex = outputVertices[index];
			const float ndcX = vertex.x / static_cast<float>(m_screenWidth) * 2.0f - 1.0f;
			const float ndcY = vertex.y / static_cast<float>(m_screenHeight) * 2.0f - 1.0f;
			m_pManualObject->position(ndcX, ndcY, 0.0f);
			m_pManualObject->textureCoord(vertex.u, vertex.v);
			m_pManualObject->colour(vertex.color);
			m_pManualObject->index(static_cast<Ogre::uint32>(index));
		}
		m_pManualObject->end();
		return;
	}

	RecordDrawCommand(command.getTexture(), materialName, triangles.vertCount, submittedTriangleCount, submittedTriangleCount, false);
	m_pManualObject->begin(materialName, Ogre::RenderOperation::OT_TRIANGLE_LIST);
	m_pManualObject->estimateVertexCount(static_cast<size_t>(triangles.vertCount));
	m_pManualObject->estimateIndexCount(static_cast<size_t>(triangles.indexCount));

	for (int index = 0; index < triangles.vertCount; ++index)
	{
		const cocos2d::V3F_C4B_T2F& vertex = triangles.verts[index];
		const float x = TransformX(command.getTransform(), vertex.vertices);
		const float y = TransformY(command.getTransform(), vertex.vertices);
		const float ndcX = x / static_cast<float>(m_screenWidth) * 2.0f - 1.0f;
		const float ndcY = y / static_cast<float>(m_screenHeight) * 2.0f - 1.0f;
		m_pManualObject->position(ndcX, ndcY, 0.0f);
		m_pManualObject->textureCoord(vertex.texCoords.u, vertex.texCoords.v);
		m_pManualObject->colour(ToOgreColor(vertex.colors));
	}

	for (int index = 0; index < triangles.indexCount; ++index)
		m_pManualObject->index(static_cast<Ogre::uint32>(triangles.indices[index]));

	m_pManualObject->end();
}

void FairyGuiSystemImpl::handleCustomCommand(const cocos2d::CustomCommand& command)
{
	(void)command;
	++m_currentFrameStats.customCommandCount;
	SyncScissorState();
	SyncStencilState();
}

void FairyGuiSystemImpl::BeginOgreRender()
{
	m_stencilScopes.clear();
	m_pendingStencilValid = false;
	m_pendingStencilDepth = 0;
	m_pendingStencilPolygons.clear();
	SyncScissorState();
	SyncStencilState();
	if (m_pManualObject != nullptr)
		m_pManualObject->clear();
}

void FairyGuiSystemImpl::EndOgreRender()
{
	if (m_pManualObject != nullptr && m_pManualNode != nullptr)
	{
		m_pManualObject->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);
		m_pManualNode->_updateBounds();
	}
}

void FairyGuiSystemImpl::SyncScissorState()
{
	cocos2d::GLView* view = cocos2d::Director::getInstance()->getOpenGLView();
	m_scissorEnabled = view != nullptr && view->isScissorEnabled();
	m_scissorRect = view != nullptr ? view->getScissorRect() : cocos2d::Rect::ZERO;
}

void FairyGuiSystemImpl::SyncStencilState()
{
	cocos2d::Renderer* renderer = cocos2d::Director::getInstance()->getRenderer();
	if (renderer == nullptr)
		return;

	const unsigned int revision = renderer->getStencilRevision();
	if (revision == m_stencilRevision)
		return;

	const cocos2d::StencilStage previousStage = m_stencilStage;
	const int previousDepth = m_stencilDepth;
	const cocos2d::StencilStage nextStage = renderer->getStencilStage();
	const int nextDepth = renderer->getStencilDepth();
	const bool nextInverted = renderer->isStencilInverted();

	if (previousStage == cocos2d::STENCIL_STAGE_WRITE && nextStage == cocos2d::STENCIL_STAGE_TEST && m_pendingStencilDepth == nextDepth)
		FinalizeStencilScope(nextDepth);

	TrimStencilScopes(nextDepth);

	if (nextStage == cocos2d::STENCIL_STAGE_WRITE && (previousStage != cocos2d::STENCIL_STAGE_WRITE || previousDepth != nextDepth))
		BeginStencilWrite(nextDepth, nextInverted);

	m_stencilStage = nextStage;
	m_stencilDepth = nextDepth;
	m_stencilRevision = revision;
}

void FairyGuiSystemImpl::BeginStencilWrite(int depth, bool inverted)
{
	m_pendingStencilDepth = depth;
	m_pendingStencilInverted = inverted;
	m_pendingStencilValid = false;
	m_pendingStencilRect = cocos2d::Rect::ZERO;
	m_pendingStencilPolygons.clear();
}

void FairyGuiSystemImpl::CollectStencilTriangle(const cocos2d::TrianglesCommand& command)
{
	const cocos2d::TrianglesCommand::Triangles& triangles = command.getTriangles();
	for (int index = 0; index + 2 < triangles.indexCount; index += 3)
	{
		StencilClipInfo::Polygon polygon;
		polygon.points.reserve(3);
		bool validTriangle = true;
		for (int vertexIndex = 0; vertexIndex < 3; ++vertexIndex)
		{
			const int sourceIndex = triangles.indices[index + vertexIndex];
			if (sourceIndex < 0 || sourceIndex >= triangles.vertCount)
			{
				validTriangle = false;
				break;
			}

			const cocos2d::V3F_C4B_T2F& vertex = triangles.verts[sourceIndex];
			const float x = TransformX(command.getTransform(), vertex.vertices);
			const float y = TransformY(command.getTransform(), vertex.vertices);
			polygon.points.push_back(cocos2d::Vec2(x, y));
			if (!m_pendingStencilValid)
			{
				m_pendingStencilRect.setRect(x, y, 0.0f, 0.0f);
				m_pendingStencilValid = true;
				continue;
			}

			const float minX = std::min(m_pendingStencilRect.getMinX(), x);
			const float minY = std::min(m_pendingStencilRect.getMinY(), y);
			const float maxX = std::max(m_pendingStencilRect.getMaxX(), x);
			const float maxY = std::max(m_pendingStencilRect.getMaxY(), y);
			m_pendingStencilRect.setRect(minX, minY, maxX - minX, maxY - minY);
		}

		if (validTriangle && polygon.points.size() == 3)
			m_pendingStencilPolygons.push_back(polygon);
	}
}

void FairyGuiSystemImpl::FinalizeStencilScope(int depth)
{
	TrimStencilScopes(depth - 1);

	StencilClipInfo clipInfo;
	clipInfo.rect = m_pendingStencilRect;
	clipInfo.polygons = m_pendingStencilPolygons;
	clipInfo.inverted = m_pendingStencilInverted;
	clipInfo.valid = m_pendingStencilValid && m_pendingStencilRect.size.width > 0.0f && m_pendingStencilRect.size.height > 0.0f;
	m_stencilScopes.push_back(clipInfo);
	m_pendingStencilValid = false;
	m_pendingStencilPolygons.clear();
}

void FairyGuiSystemImpl::TrimStencilScopes(int depth)
{
	if (depth < 0)
		depth = 0;
	while (static_cast<int>(m_stencilScopes.size()) > depth)
		m_stencilScopes.pop_back();
}

void FairyGuiSystemImpl::BuildActiveClipRects(std::vector<cocos2d::Rect>& clipRects) const
{
	if (m_stencilScopes.empty())
	{
		if (m_scissorEnabled && m_scissorRect.size.width > 0.0f && m_scissorRect.size.height > 0.0f)
			clipRects.push_back(m_scissorRect);
		return;
	}

	cocos2d::Rect baseRect;
	if (m_scissorEnabled && m_scissorRect.size.width > 0.0f && m_scissorRect.size.height > 0.0f)
		baseRect = m_scissorRect;
	else
		baseRect.setRect(0.0f, 0.0f, static_cast<float>(m_screenWidth), static_cast<float>(m_screenHeight));

	if (baseRect.size.width <= 0.0f || baseRect.size.height <= 0.0f)
		return;

	clipRects.push_back(baseRect);
	for (std::vector<StencilClipInfo>::const_iterator it = m_stencilScopes.begin(); it != m_stencilScopes.end(); ++it)
	{
		if (!it->valid)
		{
			if (it->inverted)
				continue;
			clipRects.clear();
			return;
		}

		std::vector<cocos2d::Rect> nextRects;
		for (std::vector<cocos2d::Rect>::const_iterator rectIt = clipRects.begin(); rectIt != clipRects.end(); ++rectIt)
		{
			if (it->inverted)
				SubtractRect(*rectIt, it->rect, nextRects);
			else
			{
				cocos2d::Rect intersection;
				if (IntersectRect(*rectIt, it->rect, intersection))
					nextRects.push_back(intersection);
			}
		}
		clipRects.swap(nextRects);
		if (clipRects.empty())
			return;
	}
}


void FairyGuiSystemImpl::ResetFrameRenderStats()
{
	m_currentFrameStats = FrameRenderStats();
}

void FairyGuiSystemImpl::RecordStencilCommand(int triangleCount)
{
	++m_currentFrameStats.stencilCommandCount;
	m_currentFrameStats.stencilTriangleCount += triangleCount > 0 ? triangleCount : 0;
}

void FairyGuiSystemImpl::RecordCpuClipWork(int sourceTriangleCount, int outputTriangleCount, int fragmentCount, int stencilScopeCount, int stencilPolygonCount)
{
	FrameRenderStats& stats = m_currentFrameStats;
	stats.cpuClipSourceTriangleCount += sourceTriangleCount > 0 ? sourceTriangleCount : 0;
	stats.cpuClipOutputTriangleCount += outputTriangleCount > 0 ? outputTriangleCount : 0;
	stats.cpuClipFragmentCount += fragmentCount > 0 ? fragmentCount : 0;
	if (stencilScopeCount > stats.maxStencilClipScopeCount)
		stats.maxStencilClipScopeCount = stencilScopeCount;
	stats.stencilClipPolygonCount += stencilPolygonCount > 0 ? stencilPolygonCount : 0;
}

void FairyGuiSystemImpl::RecordDrawCommand(cocos2d::Texture2D* texture, const std::string& materialName, int vertexCount, int submittedTriangleCount, int drawTriangleCount, bool clipped)
{
	FrameRenderStats& stats = m_currentFrameStats;
	submittedTriangleCount = submittedTriangleCount > 0 ? submittedTriangleCount : 0;
	drawTriangleCount = drawTriangleCount > 0 ? drawTriangleCount : 0;
	vertexCount = vertexCount > 0 ? vertexCount : 0;

	if (clipped)
	{
		++stats.clippedCommandCount;
		if (submittedTriangleCount > drawTriangleCount)
			stats.clippedTriangleCount += submittedTriangleCount - drawTriangleCount;
	}

	if (drawTriangleCount <= 0)
	{
		if (submittedTriangleCount > 0)
			++stats.culledCommandCount;
		return;
	}

	const std::string textureSource = texture != nullptr ? GetTextureSourceKey(texture) : "fallback";
	const std::string materialKey = !materialName.empty() ? materialName : "-";
	if (!stats.lastMaterialName.empty() && stats.lastMaterialName != materialKey)
		++stats.materialSwitchCount;
	if (!stats.lastTextureSource.empty() && stats.lastTextureSource != textureSource)
		++stats.textureSwitchCount;
	stats.lastMaterialName = materialKey;
	stats.lastTextureSource = textureSource;

	++stats.drawCommandCount;
	stats.drawTriangleCount += drawTriangleCount;
	if (drawTriangleCount > stats.maxBatchTriangles)
		stats.maxBatchTriangles = drawTriangleCount;
	if (vertexCount > stats.maxBatchVertices)
		stats.maxBatchVertices = vertexCount;
	stats.materialCommandCounts[materialKey] += 1;
	stats.textureCommandCounts[textureSource] += 1;
}

void FairyGuiSystemImpl::FinalizeFrameRenderStats()
{
	m_lastFrameStats = m_currentFrameStats;
	m_lastFrameStats.detailString = BuildFrameRenderDetailString(m_lastFrameStats);
}

std::string FairyGuiSystemImpl::BuildFrameRenderDetailString(const FrameRenderStats& stats) const
{
	std::ostringstream stream;
	stream << "draw=" << stats.drawCommandCount
		<< " tri=" << stats.drawTriangleCount
		<< " maxBatch=" << stats.maxBatchTriangles << "/" << stats.maxBatchVertices
		<< " switch=" << stats.materialSwitchCount << "/" << stats.textureSwitchCount
		<< " clip=" << stats.clippedCommandCount << "/" << stats.clippedTriangleCount
		<< " cull=" << stats.culledCommandCount
		<< " stencil=" << stats.stencilCommandCount << "/" << stats.stencilTriangleCount
		<< " custom=" << stats.customCommandCount
		<< " backend=" << GetStencilBackendString()
		<< " hwStencil=" << (IsHardwareStencilSupported() ? 1 : 0);

	if (stats.cpuClipSourceTriangleCount > 0)
		stream << " cpuClip=" << stats.cpuClipSourceTriangleCount << "/" << stats.cpuClipOutputTriangleCount << "/" << stats.cpuClipFragmentCount;
	if (stats.maxStencilClipScopeCount > 0 || stats.stencilClipPolygonCount > 0)
		stream << " stencilClip=" << stats.maxStencilClipScopeCount << "/" << stats.stencilClipPolygonCount;

	const std::string materialBrief = BuildCountBrief(stats.materialCommandCounts, 3);
	if (!materialBrief.empty())
		stream << " matTop=" << materialBrief;

	const std::string textureBrief = BuildCountBrief(stats.textureCommandCounts, 3);
	if (!textureBrief.empty())
		stream << " texTop=" << textureBrief;

	return stream.str();
}

bool FairyGuiSystemImpl::IsHardwareStencilSupported() const
{
	Ogre::Root* root = Ogre::Root::getSingletonPtr();
	Ogre::RenderSystem* renderSystem = root != nullptr ? root->getRenderSystem() : nullptr;
	const Ogre::RenderSystemCapabilities* capabilities = renderSystem != nullptr ? renderSystem->getCapabilities() : nullptr;
	return capabilities != nullptr && capabilities->hasCapability(Ogre::RSC_HWSTENCIL);
}

std::string FairyGuiSystemImpl::GetStencilBackendString() const
{
	return "shapeCpu";
}

std::string FairyGuiSystemImpl::GetStencilBackendDetailString() const
{
	std::ostringstream stream;
	stream << "backend=" << GetStencilBackendString()
		<< " hwStencil=" << (IsHardwareStencilSupported() ? 1 : 0)
		<< " gpuPath=customRenderableRequired";
	return stream.str();
}

std::string FairyGuiSystemImpl::GetMaterialDetailString() const
{
	std::ostringstream stream;
	bool first = true;
	for (std::map<std::string, std::string>::const_iterator it = m_materialNamesBySource.begin(); it != m_materialNamesBySource.end(); ++it)
	{
		if (!first)
			stream << "; ";
		stream << it->first << "=" << it->second;
		first = false;
	}
	return stream.str();
}

std::string FairyGuiSystemImpl::GetTextureDetailString() const
{
	std::ostringstream stream;
	bool first = true;
	for (std::map<std::string, TextureDetail>::const_iterator it = m_textureDetailsBySource.begin(); it != m_textureDetailsBySource.end(); ++it)
	{
		if (!first)
			stream << "; ";
		stream << it->first << "=" << it->second.textureName << "(" << it->second.width << "x" << it->second.height << ")";
		first = false;
	}
	return stream.str();
}

const std::string& FairyGuiSystemImpl::GetMaterialName(cocos2d::Texture2D* texture)
{
	if (texture == nullptr || texture->getImageData().empty())
	{
		if (!Ogre::MaterialManager::getSingleton().resourceExists(FAIRYGUI_FALLBACK_MATERIAL))
		{
			Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(FAIRYGUI_FALLBACK_MATERIAL, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			ConfigureFairyGuiPass(material->getTechnique(0)->getPass(0), FAIRYGUI_FALLBACK_TEXTURE);
		}
		return FAIRYGUI_FALLBACK_MATERIAL;
	}

	std::map<cocos2d::Texture2D*, std::string>::iterator it = m_materialNames.find(texture);
	if (it != m_materialNames.end())
		return it->second;

	const std::string sourceKey = GetTextureSourceKey(texture);
	std::map<std::string, std::string>::iterator sourceIt = m_materialNamesBySource.find(sourceKey);
	if (sourceIt != m_materialNamesBySource.end())
	{
		m_materialNames[texture] = sourceIt->second;
		return m_materialNames[texture];
	}

	const std::string textureName = CreateOgreTexture(texture);
	const std::string materialName = "Hello/FairyGUI/Material/" + std::to_string(++m_materialCounter);
	Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(materialName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	material->setReceiveShadows(false);

	Ogre::Technique* technique = material->getTechnique(0);
	Ogre::Pass* pass = technique->getPass(0);
	ConfigureFairyGuiPass(pass, textureName);

	m_materialNames[texture] = materialName;
	m_materialNamesBySource[sourceKey] = materialName;
	return m_materialNames[texture];
}

std::string FairyGuiSystemImpl::GetTextureSourceKey(cocos2d::Texture2D* texture) const
{
	if (texture == nullptr)
		return std::string();

	if (!texture->getFilePath().empty())
		return "file:" + texture->getFilePath();

	std::ostringstream stream;
	stream << "ptr:" << texture;
	return stream.str();
}

std::string FairyGuiSystemImpl::CreateOgreTexture(cocos2d::Texture2D* texture)
{
	std::map<cocos2d::Texture2D*, std::string>::iterator it = m_textureNames.find(texture);
	if (it != m_textureNames.end())
		return it->second;

	const std::vector<unsigned char>& data = texture->getImageData();
	if (data.empty())
		return std::string();

	const std::string sourceKey = GetTextureSourceKey(texture);
	std::map<std::string, std::string>::iterator sourceIt = m_textureNamesBySource.find(sourceKey);
	if (sourceIt != m_textureNamesBySource.end())
	{
		m_textureNames[texture] = sourceIt->second;
		return sourceIt->second;
	}

	const std::string textureName = "Hello/FairyGUI/Texture/" + std::to_string(m_textureNames.size() + 1);
	std::string extension = GetFileExtension(texture->getFilePath());
	if (extension.empty())
		extension = "png";
	try
	{
		Ogre::DataStreamPtr stream(new Ogre::MemoryDataStream((void*)&data[0], data.size(), false, true));
		Ogre::Image image;
		image.load(stream, extension);
		Ogre::TextureManager::getSingleton().loadImage(textureName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, image);
		m_textureNames[texture] = textureName;
		m_textureNamesBySource[sourceKey] = textureName;
		TextureDetail detail;
		detail.textureName = textureName;
		detail.width = texture->getPixelsWide();
		detail.height = texture->getPixelsHigh();
		m_textureDetailsBySource[sourceKey] = detail;
		return textureName;
	}
	catch (const Ogre::Exception&)
	{
		return std::string();
	}
}

void FairyGuiSystemImpl::DestroyOgreResources()
{
	if (m_pManualObject != nullptr && m_pSceneManager != nullptr)
	{
		if (m_pManualNode != nullptr)
			m_pManualNode->detachObject(m_pManualObject);
		m_pSceneManager->destroyManualObject(m_pManualObject);
	}

	if (m_pManualNode != nullptr && m_pSceneManager != nullptr)
		m_pSceneManager->destroySceneNode(m_pManualNode);

	for (std::map<std::string, std::string>::iterator it = m_materialNamesBySource.begin(); it != m_materialNamesBySource.end(); ++it)
	{
		if (!it->second.empty() && Ogre::MaterialManager::getSingleton().resourceExists(it->second))
			Ogre::MaterialManager::getSingleton().remove(it->second);
	}

	for (std::map<std::string, std::string>::iterator it = m_textureNamesBySource.begin(); it != m_textureNamesBySource.end(); ++it)
	{
		if (!it->second.empty() && Ogre::TextureManager::getSingleton().resourceExists(it->second))
			Ogre::TextureManager::getSingleton().remove(it->second);
	}

	if (Ogre::MaterialManager::getSingleton().resourceExists(FAIRYGUI_FALLBACK_MATERIAL))
		Ogre::MaterialManager::getSingleton().remove(FAIRYGUI_FALLBACK_MATERIAL);

	m_materialNames.clear();
	m_textureNames.clear();
	m_materialNamesBySource.clear();
	m_textureNamesBySource.clear();
	m_textureDetailsBySource.clear();
	m_materialCounter = 0;
}

