#ifndef __HELLO_FAIRY_GUI_SYSTEM_RENDER_HELPERS_H__
#define __HELLO_FAIRY_GUI_SYSTEM_RENDER_HELPERS_H__

#include "ui/fairygui/FairyGuiSystemImpl.h"
#include "ui/fairygui/FairyGuiSystemCommonHelpers.h"
#include "ui/fairygui/FairyGuiSystemFairyIncludes.h"

namespace
{
	const std::string FAIRYGUI_FALLBACK_MATERIAL = "Hello/FairyGUI/Fallback";
	const std::string FAIRYGUI_FALLBACK_TEXTURE = "textures/base/baseWhite.dds";
	const std::string FAIRYGUI_VERTEX_PROGRAM = "gorilla2DVP";
	const std::string FAIRYGUI_FRAGMENT_PROGRAM = "gorilla2DFP";
	float TransformX(const cocos2d::Mat4& transform, const cocos2d::Vec3& value)
	{
		return value.x * transform.m[0] + value.y * transform.m[4] + value.z * transform.m[8] + transform.m[12];
	}

	float TransformY(const cocos2d::Mat4& transform, const cocos2d::Vec3& value)
	{
		return value.x * transform.m[1] + value.y * transform.m[5] + value.z * transform.m[9] + transform.m[13];
	}

	Ogre::ColourValue ToOgreColor(const cocos2d::Color4B& color)
	{
		return Ogre::ColourValue(
			color.r / 255.0f,
			color.g / 255.0f,
			color.b / 255.0f,
			color.a / 255.0f);
	}

	bool HasFairyGuiGpuProgram(const std::string& programName)
	{
		Ogre::HighLevelGpuProgramManager* manager = Ogre::HighLevelGpuProgramManager::getSingletonPtr();
		return manager != nullptr && manager->resourceExists(programName);
	}

	void ConfigureFairyGuiPass(Ogre::Pass* pass, const std::string& textureName)
	{
		if (pass == nullptr)
			return;

		pass->setLightingEnabled(false);
		pass->setDepthCheckEnabled(false);
		pass->setDepthWriteEnabled(false);
		pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
		pass->setCullingMode(Ogre::CULL_NONE);
		pass->setManualCullingMode(Ogre::MANUAL_CULL_NONE);
		pass->setVertexColourTracking(Ogre::TVC_AMBIENT | Ogre::TVC_DIFFUSE);
		if (HasFairyGuiGpuProgram(FAIRYGUI_VERTEX_PROGRAM))
			pass->setVertexProgram(FAIRYGUI_VERTEX_PROGRAM);
		if (HasFairyGuiGpuProgram(FAIRYGUI_FRAGMENT_PROGRAM))
			pass->setFragmentProgram(FAIRYGUI_FRAGMENT_PROGRAM);

		pass->removeAllTextureUnitStates();
		Ogre::TextureUnitState* textureUnit = pass->createTextureUnitState(textureName.empty() ? FAIRYGUI_FALLBACK_TEXTURE : textureName);
		textureUnit->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
		textureUnit->setTextureFiltering(Ogre::TFO_BILINEAR);
	}

	struct ClipVertex
	{
		float x;
		float y;
		float u;
		float v;
		Ogre::ColourValue color;
	};

	enum ClipEdge
	{
		CLIP_LEFT,
		CLIP_RIGHT,
		CLIP_BOTTOM,
		CLIP_TOP
	};

	float GetClipAxisValue(const ClipVertex& vertex, ClipEdge edge)
	{
		return edge == CLIP_LEFT || edge == CLIP_RIGHT ? vertex.x : vertex.y;
	}

	bool IsClipVertexInside(const ClipVertex& vertex, ClipEdge edge, float value)
	{
		if (edge == CLIP_LEFT || edge == CLIP_BOTTOM)
			return GetClipAxisValue(vertex, edge) >= value;
		return GetClipAxisValue(vertex, edge) <= value;
	}

	ClipVertex InterpolateClipVertex(const ClipVertex& from, const ClipVertex& to, float t)
	{
		ClipVertex result;
		result.x = from.x + (to.x - from.x) * t;
		result.y = from.y + (to.y - from.y) * t;
		result.u = from.u + (to.u - from.u) * t;
		result.v = from.v + (to.v - from.v) * t;
		result.color.r = from.color.r + (to.color.r - from.color.r) * t;
		result.color.g = from.color.g + (to.color.g - from.color.g) * t;
		result.color.b = from.color.b + (to.color.b - from.color.b) * t;
		result.color.a = from.color.a + (to.color.a - from.color.a) * t;
		return result;
	}

	ClipVertex IntersectClipEdge(const ClipVertex& from, const ClipVertex& to, ClipEdge edge, float value)
	{
		const float fromValue = GetClipAxisValue(from, edge);
		const float toValue = GetClipAxisValue(to, edge);
		const float delta = toValue - fromValue;
		const float t = delta != 0.0f ? (value - fromValue) / delta : 0.0f;
		return InterpolateClipVertex(from, to, std::max(0.0f, std::min(1.0f, t)));
	}

	void ClipPolygonByEdge(const std::vector<ClipVertex>& input, std::vector<ClipVertex>& output, ClipEdge edge, float value)
	{
		output.clear();
		if (input.empty())
			return;

		ClipVertex previous = input.back();
		bool previousInside = IsClipVertexInside(previous, edge, value);
		for (std::vector<ClipVertex>::const_iterator it = input.begin(); it != input.end(); ++it)
		{
			const ClipVertex& current = *it;
			const bool currentInside = IsClipVertexInside(current, edge, value);
			if (currentInside != previousInside)
				output.push_back(IntersectClipEdge(previous, current, edge, value));
			if (currentInside)
				output.push_back(current);
			previous = current;
			previousInside = currentInside;
		}
	}

	void ClipTriangleToRect(const ClipVertex* triangle, const cocos2d::Rect& rect, std::vector<ClipVertex>& output, std::vector<ClipVertex>& scratch)
	{
		scratch.clear();
		output.clear();
		scratch.push_back(triangle[0]);
		scratch.push_back(triangle[1]);
		scratch.push_back(triangle[2]);

		ClipPolygonByEdge(scratch, output, CLIP_LEFT, rect.getMinX());
		ClipPolygonByEdge(output, scratch, CLIP_RIGHT, rect.getMaxX());
		ClipPolygonByEdge(scratch, output, CLIP_BOTTOM, rect.getMinY());
		ClipPolygonByEdge(output, scratch, CLIP_TOP, rect.getMaxY());
		output.swap(scratch);
	}

	bool IntersectRect(const cocos2d::Rect& lhs, const cocos2d::Rect& rhs, cocos2d::Rect& output)
	{
		const float minX = std::max(lhs.getMinX(), rhs.getMinX());
		const float minY = std::max(lhs.getMinY(), rhs.getMinY());
		const float maxX = std::min(lhs.getMaxX(), rhs.getMaxX());
		const float maxY = std::min(lhs.getMaxY(), rhs.getMaxY());
		if (maxX <= minX || maxY <= minY)
			return false;

		output.setRect(minX, minY, maxX - minX, maxY - minY);
		return true;
	}

	void AppendRectIfValid(std::vector<cocos2d::Rect>& rects, float x, float y, float width, float height)
	{
		if (width <= 0.0f || height <= 0.0f)
			return;

		rects.push_back(cocos2d::Rect(x, y, width, height));
	}

	void SubtractRect(const cocos2d::Rect& source, const cocos2d::Rect& cut, std::vector<cocos2d::Rect>& output)
	{
		cocos2d::Rect intersection;
		if (!IntersectRect(source, cut, intersection))
		{
			output.push_back(source);
			return;
		}

		AppendRectIfValid(output, source.getMinX(), source.getMinY(), intersection.getMinX() - source.getMinX(), source.size.height);
		AppendRectIfValid(output, intersection.getMaxX(), source.getMinY(), source.getMaxX() - intersection.getMaxX(), source.size.height);
		AppendRectIfValid(output, intersection.getMinX(), source.getMinY(), intersection.size.width, intersection.getMinY() - source.getMinY());
		AppendRectIfValid(output, intersection.getMinX(), intersection.getMaxY(), intersection.size.width, source.getMaxY() - intersection.getMaxY());
	}

	float PolygonSignedArea(const std::vector<ClipVertex>& polygon)
	{
		if (polygon.size() < 3)
			return 0.0f;

		float area = 0.0f;
		for (size_t index = 0; index < polygon.size(); ++index)
		{
			const ClipVertex& current = polygon[index];
			const ClipVertex& next = polygon[(index + 1) % polygon.size()];
			area += current.x * next.y - next.x * current.y;
		}
		return area * 0.5f;
	}

	bool IsValidClipPolygon(const std::vector<ClipVertex>& polygon)
	{
		return polygon.size() >= 3 && std::abs(PolygonSignedArea(polygon)) > 0.001f;
	}

	float Cross(const ClipVertex& a, const ClipVertex& b, const ClipVertex& point)
	{
		return (b.x - a.x) * (point.y - a.y) - (b.y - a.y) * (point.x - a.x);
	}

	bool IsInsideConvexEdge(const ClipVertex& vertex, const ClipVertex& edgeStart, const ClipVertex& edgeEnd, float orientation, bool keepInside)
	{
		const float side = Cross(edgeStart, edgeEnd, vertex);
		if (orientation >= 0.0f)
			return keepInside ? side >= -0.001f : side < -0.001f;
		return keepInside ? side <= 0.001f : side > 0.001f;
	}

	ClipVertex IntersectConvexEdge(const ClipVertex& from, const ClipVertex& to, const ClipVertex& edgeStart, const ClipVertex& edgeEnd)
	{
		const float fromSide = Cross(edgeStart, edgeEnd, from);
		const float toSide = Cross(edgeStart, edgeEnd, to);
		const float denominator = fromSide - toSide;
		const float t = denominator != 0.0f ? fromSide / denominator : 0.0f;
		return InterpolateClipVertex(from, to, std::max(0.0f, std::min(1.0f, t)));
	}

	void ClipPolygonByConvexEdge(const std::vector<ClipVertex>& input, std::vector<ClipVertex>& output, const ClipVertex& edgeStart, const ClipVertex& edgeEnd, float orientation, bool keepInside)
	{
		output.clear();
		if (input.empty())
			return;

		ClipVertex previous = input.back();
		bool previousInside = IsInsideConvexEdge(previous, edgeStart, edgeEnd, orientation, keepInside);
		for (std::vector<ClipVertex>::const_iterator it = input.begin(); it != input.end(); ++it)
		{
			const ClipVertex& current = *it;
			const bool currentInside = IsInsideConvexEdge(current, edgeStart, edgeEnd, orientation, keepInside);
			if (currentInside != previousInside)
				output.push_back(IntersectConvexEdge(previous, current, edgeStart, edgeEnd));
			if (currentInside)
				output.push_back(current);
			previous = current;
			previousInside = currentInside;
		}
	}

	void ClipPolygonToConvexPolygon(const std::vector<ClipVertex>& input, const std::vector<ClipVertex>& clipPolygon, std::vector<ClipVertex>& output, std::vector<ClipVertex>& scratch)
	{
		output.clear();
		if (!IsValidClipPolygon(input) || !IsValidClipPolygon(clipPolygon))
			return;

		const float orientation = PolygonSignedArea(clipPolygon);
		scratch = input;
		for (size_t index = 0; index < clipPolygon.size(); ++index)
		{
			const ClipVertex& edgeStart = clipPolygon[index];
			const ClipVertex& edgeEnd = clipPolygon[(index + 1) % clipPolygon.size()];
			ClipPolygonByConvexEdge(scratch, output, edgeStart, edgeEnd, orientation, true);
			if (!IsValidClipPolygon(output))
			{
				output.clear();
				return;
			}
			scratch.swap(output);
		}
		output.swap(scratch);
	}

	void SubtractConvexPolygon(const std::vector<ClipVertex>& source, const std::vector<ClipVertex>& cut, std::vector<std::vector<ClipVertex> >& output, std::vector<ClipVertex>& insideScratch, std::vector<ClipVertex>& outsideScratch)
	{
		if (!IsValidClipPolygon(source))
			return;
		if (!IsValidClipPolygon(cut))
		{
			output.push_back(source);
			return;
		}

		const float orientation = PolygonSignedArea(cut);
		insideScratch = source;
		for (size_t index = 0; index < cut.size(); ++index)
		{
			const ClipVertex& edgeStart = cut[index];
			const ClipVertex& edgeEnd = cut[(index + 1) % cut.size()];
			ClipPolygonByConvexEdge(insideScratch, outsideScratch, edgeStart, edgeEnd, orientation, false);
			if (IsValidClipPolygon(outsideScratch))
				output.push_back(outsideScratch);

			std::vector<ClipVertex> nextInside;
			ClipPolygonByConvexEdge(insideScratch, nextInside, edgeStart, edgeEnd, orientation, true);
			if (!IsValidClipPolygon(nextInside))
				return;
			insideScratch.swap(nextInside);
		}
	}

	ClipVertex MakeClipVertex(float x, float y)
	{
		ClipVertex vertex;
		vertex.x = x;
		vertex.y = y;
		vertex.u = 0.0f;
		vertex.v = 0.0f;
		vertex.color = Ogre::ColourValue::White;
		return vertex;
	}

	std::vector<ClipVertex> BuildClipPolygon(const std::vector<cocos2d::Vec2>& points)
	{
		std::vector<ClipVertex> polygon;
		polygon.reserve(points.size());
		for (std::vector<cocos2d::Vec2>::const_iterator it = points.begin(); it != points.end(); ++it)
			polygon.push_back(MakeClipVertex(it->x, it->y));
		if (!IsValidClipPolygon(polygon))
			polygon.clear();
		return polygon;
	}

	std::vector<ClipVertex> BuildRectClipPolygon(const cocos2d::Rect& rect)
	{
		std::vector<ClipVertex> polygon;
		if (rect.size.width <= 0.0f || rect.size.height <= 0.0f)
			return polygon;

		polygon.push_back(MakeClipVertex(rect.getMinX(), rect.getMinY()));
		polygon.push_back(MakeClipVertex(rect.getMaxX(), rect.getMinY()));
		polygon.push_back(MakeClipVertex(rect.getMaxX(), rect.getMaxY()));
		polygon.push_back(MakeClipVertex(rect.getMinX(), rect.getMaxY()));
		return polygon;
	}

	void AppendPolygonTriangles(const std::vector<ClipVertex>& polygon, std::vector<ClipVertex>& outputVertices)
	{
		if (!IsValidClipPolygon(polygon))
			return;

		for (size_t index = 1; index + 1 < polygon.size(); ++index)
		{
			outputVertices.push_back(polygon[0]);
			outputVertices.push_back(polygon[index]);
			outputVertices.push_back(polygon[index + 1]);
		}
	}
}

#endif // __HELLO_FAIRY_GUI_SYSTEM_RENDER_HELPERS_H__
