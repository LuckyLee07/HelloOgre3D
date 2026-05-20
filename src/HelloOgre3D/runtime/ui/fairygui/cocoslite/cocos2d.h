#ifndef HELLO_FAIRYGUI_COCOS2D_H
#define HELLO_FAIRYGUI_COCOS2D_H

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <functional>
#include <map>
#include <memory>
#include <new>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#if defined(_MSC_VER)
typedef intptr_t ssize_t;
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define COCOS2D_VERSION 0x00031702
#define CC_PLATFORM_PC 1
#define CC_PLATFORM_WIN32 2
#define CC_PLATFORM_MAC 3
#define CC_PLATFORM_LINUX 4
#define CC_TARGET_PLATFORM CC_PLATFORM_WIN32
#define CC_ENABLE_SCRIPT_BINDING 0
#define CC_ENABLE_GC_FOR_NATIVE_OBJECTS 0
#define CC_USE_CULLING 0
#define CC_DLL
#define NS_CC_BEGIN namespace cocos2d {
#define NS_CC_END }
#define USING_NS_CC using namespace cocos2d

#define CCASSERT(_cond, _msg) do { (void)sizeof(_cond); } while (0)
#define CCLOGWARN(...) do {} while (0)
#define CCLOG(...) do {} while (0)
#define CCLOGERROR(...) do {} while (0)
#define CC_ASSERT(_cond) CCASSERT(_cond, "")
#define CC_RECT_PIXELS_TO_POINTS(_rect) (_rect)
#define CC_SIZE_PIXELS_TO_POINTS(_size) (_size)
#define CC_SAFE_DELETE(_p) do { delete (_p); (_p) = nullptr; } while (0)
#define CC_SAFE_FREE(_p) do { free(_p); (_p) = nullptr; } while (0)
#define CC_SAFE_RETAIN(_p) do { if ((_p) != nullptr) { (_p)->retain(); } } while (0)
#define CC_SAFE_RELEASE(_p) do { if ((_p) != nullptr) { (_p)->release(); } } while (0)
#define CC_SAFE_RELEASE_NULL(_p) do { if ((_p) != nullptr) { (_p)->release(); (_p) = nullptr; } } while (0)
#define CC_CONTENT_SCALE_FACTOR() 1.0f
#define MATH_DEG_TO_RAD(_angle) ((_angle) * 0.01745329252f)
#define MIN(_a, _b) (((_a) < (_b)) ? (_a) : (_b))
#define MAX(_a, _b) (((_a) > (_b)) ? (_a) : (_b))
#define GL_SCISSOR_TEST 0x0C11
#define GL_LINEAR 0x2601
#define GL_REPEAT 0x2901

typedef unsigned char GLubyte;
typedef int GLint;

void glEnable(unsigned int capability);
void glDisable(unsigned int capability);
inline GLint glGetUniformLocation(unsigned int, const char*) { return 0; }

#define CREATE_FUNC(__TYPE__) \
static __TYPE__* create() \
{ \
	__TYPE__* pRet = new (std::nothrow) __TYPE__(); \
	if (pRet && pRet->init()) \
	{ \
		pRet->autorelease(); \
		return pRet; \
	} \
	delete pRet; \
	return nullptr; \
}

#define CC_CALLBACK_0(__selector__, __target__, ...) std::bind(&__selector__, __target__, ##__VA_ARGS__)
#define CC_CALLBACK_1(__selector__, __target__, ...) std::bind(&__selector__, __target__, std::placeholders::_1, ##__VA_ARGS__)
#define CC_CALLBACK_2(__selector__, __target__, ...) std::bind(&__selector__, __target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define CC_SCHEDULE_SELECTOR(_SELECTOR) (&_SELECTOR)
#define CC_DISALLOW_COPY_AND_ASSIGN(__TYPE__) \
	__TYPE__(const __TYPE__&) = delete; \
	__TYPE__& operator=(const __TYPE__&) = delete

namespace cocos2d
{
	extern const std::string STD_STRING_EMPTY;

	enum class TextHAlignment
	{
		LEFT,
		CENTER,
		RIGHT
	};

	enum class TextVAlignment
	{
		TOP,
		CENTER,
		BOTTOM
	};

	enum class LabelEffect
	{
		UNDERLINE,
		ITALICS,
		BOLD,
		OUTLINE,
		SHADOW
	};

	enum class MATRIX_STACK_TYPE
	{
		MATRIX_STACK_MODELVIEW
	};

	enum StencilStage
	{
		STENCIL_STAGE_DISABLED,
		STENCIL_STAGE_WRITE,
		STENCIL_STAGE_TEST
	};

	struct Vec2
	{
		float x;
		float y;

		static const Vec2 ZERO;
		static const Vec2 ONE;
		static const Vec2 ANCHOR_MIDDLE;
		static const Vec2 ANCHOR_BOTTOM_LEFT;

		Vec2();
		Vec2(float ax, float ay);

		bool equals(const Vec2& other) const;
		void set(float ax, float ay) { x = ax; y = ay; }
		void setZero() { x = 0; y = 0; }
		float distance(const Vec2& other) const;
		Vec2 getNormalized() const;
		void normalize();
		Vec2 lerp(const Vec2& other, float alpha) const;
		Vec2 rotateByAngle(const Vec2& pivot, float angle) const;

		static bool isLineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d, float* s, float* t);
	};

	Vec2 operator+(const Vec2& lhs, const Vec2& rhs);
	Vec2 operator-(const Vec2& value);
	Vec2 operator-(const Vec2& lhs, const Vec2& rhs);
	Vec2 operator*(const Vec2& lhs, float value);
	Vec2 operator*(float value, const Vec2& rhs);
	Vec2 operator/(const Vec2& lhs, float value);
	Vec2& operator+=(Vec2& lhs, const Vec2& rhs);
	Vec2& operator-=(Vec2& lhs, const Vec2& rhs);
	bool operator==(const Vec2& lhs, const Vec2& rhs);
	bool operator!=(const Vec2& lhs, const Vec2& rhs);

	struct Vec3
	{
		float x;
		float y;
		float z;

		static const Vec3 ZERO;

		Vec3();
		Vec3(float ax, float ay, float az);
		float distance(const Vec3& other) const;
		Vec3 lerp(const Vec3& other, float alpha) const;
	};

	Vec3 operator+(const Vec3& lhs, const Vec3& rhs);
	Vec3 operator-(const Vec3& lhs, const Vec3& rhs);
	Vec3 operator*(const Vec3& lhs, float value);
	Vec3 operator*(float value, const Vec3& rhs);

	struct Vec4
	{
		float x;
		float y;
		float z;
		float w;

		Vec4();
		Vec4(float ax, float ay, float az, float aw);
	};

	bool operator==(const Vec4& lhs, const Vec4& rhs);
	bool operator!=(const Vec4& lhs, const Vec4& rhs);

	struct Size
	{
		float width;
		float height;

		static const Size ZERO;

		Size();
		Size(float aw, float ah);
		Size(const Vec2& value);
		Size& operator=(const Vec2& value);
		bool equals(const Size& other) const;
		void setSize(float aw, float ah) { width = aw; height = ah; }
		operator Vec2() const { return Vec2(width, height); }
	};

	struct Rect
	{
		Vec2 origin;
		Size size;

		static const Rect ZERO;

		Rect();
		Rect(float x, float y, float width, float height);
		Rect(const Vec2& aOrigin, const Size& aSize);
		bool equals(const Rect& other) const;
		void setRect(float x, float y, float width, float height);
		bool containsPoint(const Vec2& point) const;
		float getMinX() const { return origin.x; }
		float getMaxX() const { return origin.x + size.width; }
		float getMinY() const { return origin.y; }
		float getMaxY() const { return origin.y + size.height; }
	};

	struct Color3B
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;

		static const Color3B WHITE;
		static const Color3B BLACK;

		Color3B();
		Color3B(uint8_t ar, uint8_t ag, uint8_t ab);
		explicit Color3B(const struct Color4B& color);
	};

	bool operator==(const Color3B& lhs, const Color3B& rhs);
	bool operator!=(const Color3B& lhs, const Color3B& rhs);

	struct Color4B
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;

		static const Color4B WHITE;
		static const Color4B BLACK;

		Color4B();
		Color4B(uint8_t ar, uint8_t ag, uint8_t ab, uint8_t aa);
		Color4B(const Color3B& color, uint8_t aa = 255);
	};

	bool operator==(const Color4B& lhs, const Color4B& rhs);
	bool operator!=(const Color4B& lhs, const Color4B& rhs);

	struct Color4F
	{
		float r;
		float g;
		float b;
		float a;

		static const Color4F WHITE;
		static const Color4F BLACK;

		Color4F();
		Color4F(float ar, float ag, float ab, float aa);
		Color4F(const Color3B& color, float aa = 1.0f);
		Color4F(const Color4B& color);
		operator Color3B() const;
		operator Color4B() const;
	};

	struct Tex2F
	{
		float u;
		float v;

		Tex2F();
		Tex2F(float au, float av);
	};

	struct V3F_C4B_T2F
	{
		Vec3 vertices;
		Color4B colors;
		Tex2F texCoords;
	};

	struct V3F_C4B_T2F_Quad
	{
		V3F_C4B_T2F bl;
		V3F_C4B_T2F br;
		V3F_C4B_T2F tl;
		V3F_C4B_T2F tr;
	};

	struct BlendFunc
	{
		int src;
		int dst;

		BlendFunc();
		BlendFunc(int asrc, int adst);
	};

	struct Mat4
	{
		float m[16];

		Mat4();
		static const Mat4 IDENTITY;
	};

	struct AffineTransform
	{
		float a;
		float b;
		float c;
		float d;
		float tx;
		float ty;

		AffineTransform();
	};

	class Ref
	{
	public:
		Ref();
		virtual ~Ref();

		void retain();
		void release();
		Ref* autorelease();
		unsigned int getReferenceCount() const;

	private:
		unsigned int _referenceCount;
	};

	template <class T>
	class Vector
	{
	public:
		typedef typename std::vector<T>::iterator iterator;
		typedef typename std::vector<T>::const_iterator const_iterator;

		Vector() {}
		explicit Vector(size_t capacity) { _data.reserve(capacity); }
		Vector(const Vector& other) : _data(other._data) {}
		Vector& operator=(const Vector& other) { _data = other._data; return *this; }

		size_t size() const { return _data.size(); }
		bool empty() const { return _data.empty(); }
		void clear() { _data.clear(); }
		void reserve(size_t capacity) { _data.reserve(capacity); }
		void pushBack(T object) { _data.push_back(object); }
		void push_back(T object) { _data.push_back(object); }
		void popBack() { _data.pop_back(); }
		void insert(ssize_t index, T object) { _data.insert(_data.begin() + index, object); }
		iterator insert(iterator it, T object) { return _data.insert(it, object); }
		T at(size_t index) const { return _data.at(index); }
		T front() const { return _data.front(); }
		T back() const { return _data.back(); }
		bool contains(T object) const { return std::find(_data.begin(), _data.end(), object) != _data.end(); }
		ssize_t getIndex(T object) const
		{
			auto it = std::find(_data.begin(), _data.end(), object);
			return it == _data.end() ? -1 : static_cast<ssize_t>(std::distance(_data.begin(), it));
		}
		void eraseObject(T object, bool removeAll = false)
		{
			auto it = _data.begin();
			while ((it = std::find(it, _data.end(), object)) != _data.end())
			{
				it = _data.erase(it);
				if (!removeAll)
					break;
			}
		}
		void erase(ssize_t index)
		{
			if (index >= 0 && static_cast<size_t>(index) < _data.size())
				_data.erase(_data.begin() + index);
		}
		iterator erase(iterator it) { return _data.erase(it); }
		iterator erase(iterator first, iterator last) { return _data.erase(first, last); }
		T operator[](size_t index) const { return _data[index]; }
		iterator begin() { return _data.begin(); }
		iterator end() { return _data.end(); }
		const_iterator begin() const { return _data.begin(); }
		const_iterator end() const { return _data.end(); }
		const_iterator cbegin() const { return _data.cbegin(); }
		const_iterator cend() const { return _data.cend(); }

	private:
		std::vector<T> _data;
	};

	template <class K, class V>
	using Map = std::unordered_map<K, V>;

	class Value;
	typedef std::vector<Value> ValueVector;
	typedef std::unordered_map<std::string, Value> ValueMap;

	class Value
	{
	public:
		enum class Type
		{
			NONE,
			BYTE,
			INTEGER,
			FLOAT,
			DOUBLE,
			BOOLEAN,
			STRING,
			VECTOR,
			MAP
		};

		static const Value Null;

		Value();
		Value(int value);
		Value(unsigned int value);
		Value(float value);
		Value(double value);
		Value(bool value);
		Value(const char* value);
		Value(const std::string& value);
		Value(const ValueVector& value);
		Value(const ValueMap& value);

		Type getType() const { return _type; }
		int asInt() const;
		unsigned int asUnsignedInt() const;
		float asFloat() const;
		double asDouble() const;
		bool asBool() const;
		std::string asString() const;
		ValueVector& asValueVector();
		const ValueVector& asValueVector() const;
		ValueMap& asValueMap();
		const ValueMap& asValueMap() const;

	private:
		Type _type;
		double _number;
		bool _bool;
		std::string _string;
		ValueVector _vector;
		ValueMap _map;
	};

	extern const ValueMap ValueMapNull;

	class Data
	{
	public:
		Data();
		~Data();
		Data(const Data& other);
		Data& operator=(const Data& other);
		Data(Data&& other);
		Data& operator=(Data&& other);

		bool isNull() const;
		unsigned char* getBytes() const;
		ssize_t getSize() const;
		unsigned char* takeBuffer(ssize_t* size);
		void clear();
		void fastSet(unsigned char* bytes, ssize_t size);

	private:
		unsigned char* _bytes;
		ssize_t _size;
	};

	class Event;
	class Renderer;
	class RenderCommand;
	class RenderCommandSink;
	class CustomCommand;
	class TrianglesCommand;
	class GLProgram;
	class GLProgramState;
	class Texture2D;
	class SpriteFrame;
	class Action;
	class EventDispatcher;
	class Camera;
	class Scene;

	class GLProgram : public Ref
	{
	public:
		static const char* SHADER_NAME_POSITION_GRAYSCALE;
		static const char* SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP;
		static const char* SHADER_NAME_POSITION_TEXTURE_ALPHA_TEST_NO_MV;
		static const char* UNIFORM_NAME_ALPHA_TEST_VALUE;

		unsigned int getProgram() const { return 0; }
		void use() {}
		void setUniformLocationWith1f(GLint, float) {}
	};

	class GLProgramCache
	{
	public:
		static GLProgramCache* getInstance();
		GLProgram* getGLProgram(const std::string&);
	};

	class GLProgramState : public Ref
	{
	public:
		static GLProgramState* getOrCreateWithGLProgramName(const std::string&, Texture2D* = nullptr);
	};

	namespace backend
	{
		enum class SamplerFilter
		{
			LINEAR
		};

		enum class SamplerAddressMode
		{
			REPEAT,
			CLAMP_TO_EDGE
		};

		enum class ProgramType
		{
			POSITION_TEXTURE_COLOR_ALPHA_TEST
		};

		class Program
		{
		public:
			static Program* getBuiltinProgram(ProgramType) { static Program program; return &program; }
		};

		class ProgramState : public Ref
		{
		public:
			explicit ProgramState(Program*) {}
			int getUniformLocation(const std::string&) const { return 0; }
			void setUniform(int, const void*, size_t) {}
		};
	}

	enum class TextureFormatEXT
	{
		ETC1_ALPHA
	};

	class Texture2D : public Ref
	{
	public:
		enum class PixelFormat
		{
			RGBA8888
		};

		struct TexParams
		{
			int minFilter;
			int magFilter;
			int wrapS;
			int wrapT;

			TexParams();
			TexParams(int aminFilter, int amagFilter, int awrapS, int awrapT);
			TexParams(backend::SamplerFilter, backend::SamplerFilter, backend::SamplerAddressMode, backend::SamplerAddressMode);
		};

		Texture2D();
		bool initWithData(const void*, ssize_t, PixelFormat, int, int, const Size&);
		bool initWithImage(class Image*);
		void updateWithImage(class Image*, PixelFormat = PixelFormat::RGBA8888, int = 0, int = 0);
		void updateWithImage(class Image*, PixelFormat, int, TextureFormatEXT);
		void setAlphaTexture(Texture2D*) {}
		void setTexParameters(const TexParams&);
		int getPixelsWide() const { return _width; }
		int getPixelsHigh() const { return _height; }
		Size getContentSize() const { return Size(static_cast<float>(_width), static_cast<float>(_height)); }
		const std::string& getFilePath() const { return _filePath; }
		const std::vector<unsigned char>& getImageData() const { return _imageData; }
		bool getAlphaTextureName() const { return false; }
		static PixelFormat getDefaultAlphaPixelFormat() { return PixelFormat::RGBA8888; }

	private:
		int _width;
		int _height;
		std::string _filePath;
		std::vector<unsigned char> _imageData;
	};

	class SpriteFrame : public Ref
	{
	public:
		static SpriteFrame* createWithTexture(Texture2D* texture);
		static SpriteFrame* createWithTexture(Texture2D* texture, const Rect& rect, bool rotated = false, const Vec2& offset = Vec2::ZERO, const Size& originalSize = Size::ZERO);
		bool initWithTexture(Texture2D* texture, const Rect& rect, bool rotated = false, const Vec2& offset = Vec2::ZERO, const Size& originalSize = Size::ZERO);
		Texture2D* getTexture() const { return _texture; }
		const Rect& getRect() const { return _rect; }
		const Rect& getRectInPixels() const { return _rect; }
		void setOriginalSizeInPixels(const Size& size) { _originalSize = size; }
		void setOriginalSize(const Size& size) { _originalSize = size; }
		void setOffset(const Vec2& offset) { _offset = offset; }
		bool isRotated() const { return _rotated; }
		const Vec2& getOffset() const { return _offset; }
		const Size& getOriginalSize() const { return _originalSize; }

	private:
		Texture2D* _texture;
		Rect _rect;
		bool _rotated;
		Vec2 _offset;
		Size _originalSize;
	};

	class Image
	{
	public:
		enum class Format
		{
			UNKNOWN,
			PNG,
			ETC
		};

		Image();
		bool initWithRawData(const unsigned char*, ssize_t, int width, int height, int, bool);
		bool initWithImageFile(const std::string&);
		bool initWithImageData(const unsigned char*, ssize_t);
		unsigned char* getData() const { return _data.empty() ? nullptr : const_cast<unsigned char*>(_data.data()); }
		ssize_t getDataLen() const { return static_cast<ssize_t>(_data.size()); }
		int getWidth() const { return _width; }
		int getHeight() const { return _height; }
		const std::string& getFilePath() const { return _filePath; }
		Format getFileType() const { return _format; }
		static void setPNGPremultipliedAlphaEnabled(bool) {}

	private:
		std::vector<unsigned char> _data;
		int _width;
		int _height;
		Format _format;
		std::string _filePath;
	};

	class TextureCache
	{
	public:
		Texture2D* addImage(const std::string&);
		Texture2D* addImage(Image*, const std::string&);
		void removeTexture(Texture2D*) {}
	};

	class FileUtils
	{
	public:
		enum class Status
		{
			OK,
			NotExists,
			OpenFailed
		};

		static FileUtils* getInstance();
		bool isFileExist(const std::string&) const;
		bool isAbsolutePath(const std::string&) const;
		bool isPopupNotify() const { return _popupNotify; }
		void setPopupNotify(bool value) { _popupNotify = value; }
		std::string fullPathForFilename(const std::string& filename) const;
		std::string getStringFromFile(const std::string& filename) const;
		Data getDataFromFile(const std::string& filename) const;
		Status getContents(const std::string& filename, Data* data) const;
		std::string getFileExtension(const std::string& path) const;

	private:
		bool _popupNotify = true;
	};

	class Event : public Ref
	{
	};

	class EventCustom : public Event
	{
	};

	class Touch : public Ref
	{
	public:
		Touch();
		int getID() const { return _id; }
		const Vec2& getLocation() const { return _location; }
		const Vec2& getPreviousLocation() const { return _previousLocation; }
		void setTouchInfo(int id, float x, float y, float previousX, float previousY);

	private:
		int _id;
		Vec2 _location;
		Vec2 _previousLocation;
	};

	class EventMouse : public Event
	{
	public:
		enum class MouseButton
		{
			BUTTON_UNSET = -1,
			BUTTON_LEFT = 0,
			BUTTON_RIGHT,
			BUTTON_MIDDLE
		};

		EventMouse();
		MouseButton getMouseButton() const { return _button; }
		float getCursorX() const { return _x; }
		float getCursorY() const { return _y; }
		float getScrollX() const { return _scrollX; }
		float getScrollY() const { return _scrollY; }
		void setCursorPosition(float x, float y) { _x = x; _y = y; }
		void setScrollData(float scrollX, float scrollY) { _scrollX = scrollX; _scrollY = scrollY; }

	private:
		MouseButton _button;
		float _x;
		float _y;
		float _scrollX;
		float _scrollY;
	};

	class EventKeyboard : public Event
	{
	public:
		enum class KeyCode
		{
			KEY_0,
			KEY_LEFT_CTRL,
			KEY_RIGHT_CTRL,
			KEY_LEFT_ALT,
			KEY_RIGHT_ALT,
			KEY_LEFT_SHIFT,
			KEY_RIGHT_SHIFT
		};
	};

	class EventListener : public Ref
	{
	};

	class EventListenerMouse : public EventListener
	{
	public:
		std::function<void(EventMouse*)> onMouseDown;
		std::function<void(EventMouse*)> onMouseUp;
		std::function<void(EventMouse*)> onMouseMove;
		std::function<void(EventMouse*)> onMouseScroll;

		static EventListenerMouse* create();
	};

	class EventListenerTouchOneByOne : public EventListener
	{
	public:
		std::function<bool(Touch*, Event*)> onTouchBegan;
		std::function<void(Touch*, Event*)> onTouchMoved;
		std::function<void(Touch*, Event*)> onTouchEnded;
		std::function<void(Touch*, Event*)> onTouchCancelled;

		static EventListenerTouchOneByOne* create();
		void setSwallowTouches(bool swallow) { _swallowTouches = swallow; }

	private:
		bool _swallowTouches;
	};

	class EventListenerKeyboard : public EventListener
	{
	public:
		std::function<void(EventKeyboard::KeyCode, Event*)> onKeyPressed;
		std::function<void(EventKeyboard::KeyCode, Event*)> onKeyReleased;

		static EventListenerKeyboard* create();
	};

	class EventDispatcher
	{
	public:
		void addEventListenerWithSceneGraphPriority(EventListener*, class Node*) {}
		EventListener* addCustomEventListener(const std::string&, const std::function<void(EventCustom*)>&) { return nullptr; }
		void removeEventListener(EventListener*) {}
	};

	class Action : public Ref
	{
	public:
		Action();
		virtual Action* clone() const { return new Action(*this); }
		virtual Action* reverse() const { return clone(); }
		virtual bool isDone() const { return true; }
		virtual void step(float) {}
		virtual void update(float) {}
		virtual void startWithTarget(class Node* target) { _target = target; }
		virtual void stop() { _target = nullptr; }

	protected:
		class Node* _target;
	};

	class FiniteTimeAction : public Action
	{
	public:
		FiniteTimeAction();
		float getDuration() const { return _duration; }
		void setDuration(float duration) { _duration = duration; }

	protected:
		float _duration;
	};

	class AnimationFrame : public Ref
	{
	public:
		static AnimationFrame* create(SpriteFrame* spriteFrame, float delayUnits, const ValueMap& userInfo);
		SpriteFrame* getSpriteFrame() const { return _spriteFrame; }
		float getDelayUnits() const { return _delayUnits; }

	private:
		SpriteFrame* _spriteFrame;
		float _delayUnits;
		ValueMap _userInfo;
	};

	class Animation : public Ref
	{
	public:
		static Animation* create();
		static Animation* createWithAnimationFrames(const Vector<AnimationFrame*>& frames, float delayPerUnit, unsigned int loops);
		bool initWithAnimationFrames(const Vector<AnimationFrame*>& frames, float delayPerUnit, unsigned int loops);
		Animation* clone() const;
		const Vector<AnimationFrame*>& getFrames() const { return _frames; }
		void setDelayPerUnit(float value) { _delayPerUnit = value; }
		float getDelayPerUnit() const { return _delayPerUnit; }
		void setLoops(unsigned int loops) { _loops = loops; }
		unsigned int getLoops() const { return _loops; }

	private:
		Vector<AnimationFrame*> _frames;
		float _delayPerUnit;
		unsigned int _loops;
	};

	class Animate : public FiniteTimeAction
	{
	public:
		static Animate* create(Animation* animation);
		Animation* getAnimation() const { return _animation; }

	private:
		Animation* _animation;
	};

	class RepeatForever : public Action
	{
	public:
		static RepeatForever* create(Action* action);

	private:
		Action* _action;
	};

	class DelayTime : public FiniteTimeAction
	{
	public:
		static DelayTime* create(float duration);
	};

	class CallFunc : public FiniteTimeAction
	{
	public:
		static CallFunc* create(const std::function<void()>& func);

	private:
		std::function<void()> _func;
	};

	class Sequence : public FiniteTimeAction
	{
	public:
		static Sequence* create(FiniteTimeAction* action1, FiniteTimeAction* action2, std::nullptr_t);
		static Sequence* createWithTwoActions(FiniteTimeAction* action1, FiniteTimeAction* action2);

	private:
		std::vector<FiniteTimeAction*> _actions;
	};

	class Scheduler
	{
	public:
		template <class T>
		bool isScheduled(void (T::*)(float), T*) const { return false; }

		template <class T>
		void schedule(void (T::*)(float), T*, float, bool) {}

		template <class T>
		void unschedule(void (T::*)(float), T*) {}

		void scheduleUpdate(void*, int, bool) {}
	};

	class Renderer
	{
	public:
		Renderer();

		void beginFrame();
		void addCommand(RenderCommand* command);
		void pushGroup(int) {}
		void popGroup() {}
		void setScissorTest(bool enabled);
		void setStencilState(StencilStage stage, bool inverted, int depth);
		bool checkVisibility(const Mat4&, const Size&) const { return true; }
		void setCommandSink(RenderCommandSink* sink) { _commandSink = sink; }
		int getTriangleCommandCount() const { return _triangleCommandCount; }
		int getSubmittedTriangleCount() const { return _submittedTriangleCount; }
		StencilStage getStencilStage() const { return _stencilStage; }
		bool isStencilInverted() const { return _stencilInverted; }
		int getStencilDepth() const { return _stencilDepth; }
		unsigned int getStencilRevision() const { return _stencilRevision; }

	private:
		RenderCommandSink* _commandSink;
		int _triangleCommandCount;
		int _submittedTriangleCount;
		StencilStage _stencilStage;
		bool _stencilInverted;
		int _stencilDepth;
		unsigned int _stencilRevision;
	};

	class RenderCommand
	{
	public:
		virtual ~RenderCommand() {}
	};

	class GroupCommand : public RenderCommand
	{
	public:
		GroupCommand();
		void init(float globalOrder);
		int getRenderQueueID() const { return _renderQueueId; }

	private:
		int _renderQueueId;
	};

	class CustomCommand : public RenderCommand
	{
	public:
		std::function<void()> func;
		void init(float globalOrder) { (void)globalOrder; }
	};

	class CallbackCommand : public CustomCommand
	{
	};

	class RenderCommandSink
	{
	public:
		virtual ~RenderCommandSink() {}
		virtual void handleTrianglesCommand(const TrianglesCommand& command) = 0;
		virtual void handleCustomCommand(const CustomCommand& command) { (void)command; }
	};

	class TrianglesCommand : public RenderCommand
	{
	public:
		struct Triangles
		{
			V3F_C4B_T2F* verts;
			int vertCount;
			unsigned short* indices;
			int indexCount;

			Triangles();
		};

		TrianglesCommand();
		void init(float globalOrder, Texture2D* texture, GLProgramState* programState, const BlendFunc& blendFunc, const Triangles& triangles, const Mat4& transform, uint32_t flags);
		void init(float globalOrder, Texture2D* texture, const BlendFunc& blendFunc, const Triangles& triangles, const Mat4& transform, uint32_t flags);
		float getGlobalOrder() const { return _globalOrder; }
		Texture2D* getTexture() const { return _texture; }
		GLProgramState* getGLProgramState() const { return _programState; }
		const BlendFunc& getBlendFunc() const { return _blendFunc; }
		const Triangles& getTriangles() const { return _triangles; }
		const Mat4& getTransform() const { return _transform; }
		uint32_t getFlags() const { return _flags; }
		int getTriangleCount() const { return _triangles.indexCount / 3; }

	private:
		float _globalOrder;
		Texture2D* _texture;
		GLProgramState* _programState;
		BlendFunc _blendFunc;
		Triangles _triangles;
		Mat4 _transform;
		uint32_t _flags;
	};

	class GLView
	{
	public:
		GLView();
		bool isScissorEnabled() const { return _scissorEnabled; }
		Rect getScissorRect() const { return _scissorRect; }
		void setScissorTestEnabled(bool enabled);
		void setScissorInPoints(float x, float y, float width, float height);
		const Size& getFrameSize() const { return _frameSize; }
		const Size& getDesignResolutionSize() const { return _frameSize; }
		void setFrameSize(float width, float height);

	private:
		bool _scissorEnabled;
		Rect _scissorRect;
		Size _frameSize;
	};

	class Camera : public Ref
	{
	public:
		static Camera* getVisitingCamera();
		static Camera* getDefaultCamera();
		bool isViewProjectionUpdated() const { return true; }
	};

	class Node : public Ref
	{
	public:
		static const uint32_t FLAGS_DIRTY_MASK = 0xffffffff;
		static const uint32_t FLAGS_TRANSFORM_DIRTY = 1;

		Node();
		virtual ~Node();

		CREATE_FUNC(Node);

		virtual bool init();
		virtual void onEnter();
		virtual void onEnterTransitionDidFinish();
		virtual void onExitTransitionDidStart();
		virtual void onExit();
		virtual void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags);
		virtual void draw(Renderer* renderer, const Mat4& transform, uint32_t flags);
		virtual void setCameraMask(unsigned short mask, bool applyChildren = true);
		virtual void setGlobalZOrder(float globalZOrder);
		virtual void setContentSize(const Size& size);

		void addChild(Node* child);
		void addChild(Node* child, int localZOrder);
		void removeChild(Node* child);
		void removeChild(Node* child, bool cleanup);
		void removeFromParent();
		void removeAllChildren();
		void removeAllChildrenWithCleanup(bool cleanup) { (void)cleanup; removeAllChildren(); }
		void sortAllChildren();
		const std::vector<Node*>& getChildren() const { return _children; }
		Node* getParent() const { return _parent; }

		void setPosition(const Vec2& position);
		void setPosition(float x, float y);
		void setPositionX(float x);
		void setPositionY(float y);
		const Vec2& getPosition() const { return _position; }
		float getPositionX() const { return _position.x; }
		float getPositionY() const { return _position.y; }
		void setScale(float scale);
		void setScale(float scaleX, float scaleY);
		void setScaleX(float scaleX) { _scaleX = scaleX; }
		void setScaleY(float scaleY) { _scaleY = scaleY; }
		float getScaleX() const { return _scaleX; }
		float getScaleY() const { return _scaleY; }
		void setRotation(float rotation) { _rotationZ = rotation; }
		float getRotation() const { return _rotationZ; }
		void setSkewX(float skewX) { _skewX = skewX; }
		void setSkewY(float skewY) { _skewY = skewY; }
		void setRotationSkewX(float skewX) { _skewX = skewX; }
		void setRotationSkewY(float skewY) { _skewY = skewY; }
		float getSkewX() const { return _skewX; }
		float getSkewY() const { return _skewY; }
		void setAnchorPoint(const Vec2& anchorPoint) { _anchorPoint = anchorPoint; }
		const Vec2& getAnchorPoint() const { return _anchorPoint; }
		virtual const Size& getContentSize() const { return _contentSize; }
		float getGlobalZOrder() const { return _globalZOrder; }
		int getLocalZOrder() const { return _localZOrder; }
		void setLocalZOrder(int zOrder) { _localZOrder = zOrder; }
		void setVisible(bool visible) { _visible = visible; }
		bool isVisible() const { return _visible; }
		bool isRunning() const { return _running; }
		void setCascadeOpacityEnabled(bool) {}
		void setCascadeColorEnabled(bool) {}
		void setOpacity(uint8_t opacity) { _displayedOpacity = opacity; }
		uint8_t getOpacity() const { return _displayedOpacity; }
		virtual void setColor(const Color3B& color) { _displayedColor = color; }
		const Color3B& getColor() const { return _displayedColor; }
		void setName(const std::string& name) { _name = name; }
		const std::string& getName() const { return _name; }
		void setUserData(void* userData) { _userData = userData; }
		void* getUserData() const { return _userData; }
		void setOnEnterCallback(const std::function<void()>& callback) { _onEnterCallback = callback; }
		void setOnExitCallback(const std::function<void()>& callback) { _onExitCallback = callback; }
		Scene* getScene() const;

		Vec2 convertToNodeSpace(const Vec2& worldPoint) const;
		Vec2 convertToNodeSpaceAR(const Vec2& worldPoint) const;
		Vec2 convertToWorldSpace(const Vec2& nodePoint) const;
		Vec2 convertToWorldSpaceAR(const Vec2& nodePoint) const;
		AffineTransform getNodeToWorldAffineTransform() const;
		uint32_t processParentFlags(const Mat4&, uint32_t parentFlags);
		bool isVisitableByVisitingCamera() const { return _visible; }

		Action* runAction(Action* action);
		void stopAction(Action* action);
		void stopAllActions();
		EventDispatcher* getEventDispatcher();

		GLProgram* getGLProgram() const { return _glProgram; }
		void setGLProgram(GLProgram* program) { _glProgram = program; }
		GLProgramState* getGLProgramState() const { return _glProgramState; }
		void setGLProgramState(GLProgramState* state) { _glProgramState = state; }

	protected:
		Node* _parent;
		std::vector<Node*> _children;
		Vec2 _position;
		Vec2 _anchorPoint;
		Size _contentSize;
		Mat4 _modelViewTransform;
		bool _visible;
		bool _running;
		bool _isTransitionFinished;
		bool _transformUpdated;
		bool _contentSizeDirty;
		float _globalZOrder;
		int _localZOrder;
		float _scaleX;
		float _scaleY;
		float _rotationZ;
		float _skewX;
		float _skewY;
		uint8_t _displayedOpacity;
		Color3B _displayedColor;
		std::string _name;
		void* _userData;
		std::function<void()> _onEnterCallback;
		std::function<void()> _onExitCallback;
		EventDispatcher* _eventDispatcher;
		GLProgram* _glProgram;
		GLProgramState* _glProgramState;
	};

	class Scene : public Node
	{
	public:
		CREATE_FUNC(Scene);
	};

	class DrawNode : public Node
	{
	public:
		CREATE_FUNC(DrawNode);

		void clear() {}
		void setLineWidth(float) {}
		void drawTriangle(const Vec2&, const Vec2&, const Vec2&, const Color4F&) {}
		void drawCircle(const Vec2&, float, float, unsigned int, bool, float, float, const Color4F&) {}
		void drawSolidCircle(const Vec2&, float, float, unsigned int, float, float, const Color4F&) {}
		void drawPolygon(const Vec2*, int, const Color4F&, float, const Color4F&) {}
	};

	class Sprite : public Node
	{
	public:
		Sprite();
		virtual ~Sprite();

		CREATE_FUNC(Sprite);

		static Sprite* createWithSpriteFrame(SpriteFrame* spriteFrame);
		static Sprite* createWithTexture(Texture2D* texture);
		virtual bool initWithSpriteFrame(SpriteFrame* spriteFrame);
		virtual bool initWithTexture(Texture2D* texture);
		void setTexture(Texture2D* texture);
		Texture2D* getTexture() const { return _texture; }
		void setSpriteFrame(SpriteFrame* spriteFrame);
		SpriteFrame* getSpriteFrame() const { return _spriteFrame; }
		void setTextureRect(const Rect& rect);
		void setTextureRect(const Rect& rect, bool rotated, const Size& untrimmedSize);
		void setCenterRectNormalized(const Rect& rect) { _centerRectNormalized = rect; }
		void setCenterRect(const Rect& rect) { _centerRect = rect; }
		void setFlippedX(bool value) { _flippedX = value; }
		void setFlippedY(bool value) { _flippedY = value; }
		bool isFlippedX() const { return _flippedX; }
		bool isFlippedY() const { return _flippedY; }
		bool isTextureRectRotated() const { return _rectRotated; }
		V3F_C4B_T2F_Quad getQuad() const;
		virtual void updateColor();

	protected:
		virtual void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;

		Texture2D* _texture;
		SpriteFrame* _spriteFrame;
		Rect _rect;
		bool _rectRotated;
		Size _originalContentSize;
		Vec2 _unflippedOffsetPositionFromCenter;
		Rect _centerRectNormalized;
		Rect _centerRect;
		bool _flippedX;
		bool _flippedY;
		BlendFunc _blendFunc;
		TrianglesCommand _trianglesCommand;
		V3F_C4B_T2F _quadVerts[4];
		unsigned short _quadIndices[6];
		TrianglesCommand::Triangles _quadTriangles;
	};

	struct FontDefinition
	{
		std::string fontFilePath;
		float fontSize;

		FontDefinition() : fontSize(12) {}
	};

	class Font : public Ref
	{
	public:
		virtual int* getHorizontalKerningForTextUTF32(const std::u32string&, int& outNumLetters) const;
		virtual class FontAtlas* createFontAtlas();
	};

	struct FontLetterDefinition
	{
		unsigned short charID;
		float width;
		float height;
		float offsetX;
		float offsetY;
		float xAdvance;
		float U;
		float V;
		bool validDefinition;
		int textureID;

		FontLetterDefinition();
	};

	class FontAtlas : public Ref
	{
	public:
		FontAtlas();
		explicit FontAtlas(Font& font);
		Font* getFont() const { return _font; }
		void addLetterDefinition(unsigned int, const FontLetterDefinition&) {}
		void addTexture(Texture2D*, int) {}
		void setLineHeight(float lineHeight) { _lineHeight = lineHeight; }
		float getLineHeight() const { return _lineHeight; }

	private:
		Font* _font;
		float _lineHeight;
	};

	class Label : public Node
	{
	public:
		enum class LabelType
		{
			STRING_TEXTURE,
			BMFONT,
			TTF
		};

		enum class Overflow
		{
			NONE,
			CLAMP,
			SHRINK,
			RESIZE_HEIGHT
		};

		Label();
		virtual ~Label();

		CREATE_FUNC(Label);

		const std::string& getString() const { return _string; }
		void setString(const std::string& value);
		void setSystemFontName(const std::string& name) { _systemFontName = name; _currentLabelType = LabelType::STRING_TEXTURE; }
		void setSystemFontSize(float size) { _systemFontSize = size; }
		void setTTFConfig(const FontDefinition& config) { _fontConfig = config; _currentLabelType = LabelType::TTF; }
		virtual bool setBMFontFilePath(const std::string& bmfontFilePath, const Vec2& imageOffset = Vec2::ZERO, float fontSize = 0);
		void setBMFontSize(float size) { _bmFontSize = size; }
		void setFontAtlas(FontAtlas* fontAtlas) { _fontAtlas = fontAtlas; }
		void setTextColor(const Color4B& color) { setColor(Color3B(color.r, color.g, color.b)); }
		void enableUnderline() {}
		void enableItalics() {}
		void enableBold() {}
		void enableOutline(const Color4B&, int = 1) {}
		void enableShadow(const Color4B&, const Size&) {}
		void disableEffect(LabelEffect) {}
		void setLineSpacing(float lineSpacing) { _lineSpacing = lineSpacing; }
		void setHorizontalAlignment(TextHAlignment alignment) { _horizontalAlignment = alignment; }
		void setVerticalAlignment(TextVAlignment alignment) { _verticalAlignment = alignment; }
		void setDimensions(float width, float height) { _dimensions = Size(width, height); setContentSize(_dimensions); }
		const Size& getDimensions() const { return _dimensions; }
		void setOverflow(Overflow overflow) { _overflow = overflow; }
		Overflow getOverflow() const { return _overflow; }
		void reset() { _string.clear(); }
		void setLineBreakWithoutSpace(bool value) { _lineBreakWithoutSpace = value; }
		void setMaxLineWidth(float width) { _maxLineWidth = width; }
		float getMaxLineWidth() const { return _maxLineWidth; }
		void enableWrap(bool value) { _wrapEnabled = value; }
		bool isWrapEnabled() const { return _wrapEnabled; }
		virtual void updateBMFontScale();

	protected:
		std::string _string;
		LabelType _currentLabelType;
		FontDefinition _fontConfig;
		FontAtlas* _fontAtlas;
		float _bmFontSize;
		std::string _bmFontPath;
		float _bmfontScale;
		std::string _systemFontName;
		float _systemFontSize;
		float _lineSpacing;
		TextHAlignment _horizontalAlignment;
		TextVAlignment _verticalAlignment;
		Size _dimensions;
		Overflow _overflow;
		bool _lineBreakWithoutSpace;
		bool _wrapEnabled;
		float _maxLineWidth;
	};

	class SAXDelegator
	{
	public:
		virtual ~SAXDelegator() {}
		virtual void startElement(void*, const char*, const char**) {}
		virtual void endElement(void*, const char*) {}
		virtual void textHandler(void*, const char*, size_t) {}
	};

	class SAXParser
	{
	public:
		void setDelegator(SAXDelegator* delegator) { _delegator = delegator; }
		bool parse(const char*, size_t) { return true; }
		bool parse(const std::string&) { return true; }
		bool parseIntrusive(char*, size_t) { return true; }

	private:
		SAXDelegator* _delegator = nullptr;
	};

	class StencilStateManager
	{
	public:
		StencilStateManager();
		float getAlphaThreshold() const { return _alphaThreshold; }
		void setAlphaThreshold(float value) { _alphaThreshold = value; }
		bool isInverted() const { return _inverted; }
		void setInverted(bool value) { _inverted = value; }
		void onBeforeVisit();
		void onAfterDrawStencil();
		void onAfterVisit();

	private:
		float _alphaThreshold;
		bool _inverted;
		StencilStage _previousStage;
		bool _previousInverted;
		int _previousDepth;
		bool _hasPreviousState;
	};

	class Director
	{
	public:
		static const char* EVENT_RESET;
		static Director* getInstance();
		Scheduler* getScheduler() { return &_scheduler; }
		Renderer* getRenderer() { return &_renderer; }
		GLView* getOpenGLView() { return &_view; }
		TextureCache* getTextureCache() { return &_textureCache; }
		EventDispatcher* getEventDispatcher() { return &_eventDispatcher; }
		unsigned int getTotalFrames() const { return _totalFrames; }
		Size getWinSize() const { return _view.getFrameSize(); }
		float getContentScaleFactor() const { return 1.0f; }
		float getDeltaTime() const { return _deltaTime; }
		void pushMatrix(MATRIX_STACK_TYPE) {}
		void loadMatrix(MATRIX_STACK_TYPE, const Mat4&) {}
		void popMatrix(MATRIX_STACK_TYPE) {}
		void mainLoop(float dt);

	private:
		Director();

		Scheduler _scheduler;
		Renderer _renderer;
		GLView _view;
		TextureCache _textureCache;
		EventDispatcher _eventDispatcher;
		unsigned int _totalFrames;
		float _deltaTime;
	};

	class GLViewImpl
	{
	public:
		static const char* EVENT_WINDOW_RESIZED;
	};

	class MathUtil
	{
	public:
		static float lerp(float from, float to, float alpha) { return from + (to - from) * alpha; }
	};

	inline float clampf(float value, float minValue, float maxValue)
	{
		return std::max(minValue, std::min(value, maxValue));
	}

	inline float rand_0_1()
	{
		return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
	}

	class StringUtils
	{
	public:
		static bool UTF8ToUTF32(const std::string& source, std::u32string& target)
		{
			target.clear();
			for (char ch : source)
				target.push_back(static_cast<unsigned char>(ch));
			return true;
		}

		static bool UTF32ToUTF8(const std::u32string& source, std::string& target)
		{
			target.clear();
			for (char32_t ch : source)
				target.push_back(static_cast<char>(ch));
			return true;
		}
	};

	namespace ui
	{
		class Widget : public Node
		{
		public:
			enum class TouchEventType
			{
				BEGAN,
				MOVED,
				ENDED,
				CANCELED
			};
		};

		class Scale9Sprite : public Sprite
		{
		public:
			CREATE_FUNC(Scale9Sprite);
			static Scale9Sprite* createWithTexture(Texture2D*)
			{
				Scale9Sprite* sprite = new (std::nothrow) Scale9Sprite();
				if (sprite && sprite->init())
				{
					sprite->autorelease();
					return sprite;
				}
				delete sprite;
				return nullptr;
			}
		};

		class EditBox;

		class EditBoxDelegate
		{
		public:
			virtual ~EditBoxDelegate() {}
			virtual void editBoxReturn(EditBox*) {}
			virtual void editBoxTextChanged(EditBox*, const std::string&) {}
		};

		class EditBox : public Widget
		{
		public:
			enum class InputMode
			{
				ANY,
				SINGLE_LINE
			};

			enum class InputFlag
			{
				SENSITIVE,
				PASSWORD
			};

			EditBox();
			virtual ~EditBox() {}

			static EditBox* create(const Size& size, Scale9Sprite* normalSprite = nullptr);
			bool initWithSizeAndBackgroundSprite(const Size& size, Scale9Sprite*)
			{
				setContentSize(size);
				return true;
			}
			const char* getText() const { return _text.c_str(); }
			void setText(const char* text) { _text = text ? text : ""; }
			void setPlaceHolder(const char* text) { _placeHolder = text ? text : ""; }
			void setFontName(const char*) {}
			void setFontSize(int) {}
			void setFontColor(const Color3B&) {}
			void setPlaceholderFontName(const char*) {}
			void setPlaceholderFontSize(int) {}
			void setPlaceholderFontColor(const Color3B&) {}
			void setPlaceholderFontColor(const Color4B&) {}
			void setInputMode(InputMode mode) { _inputMode = mode; }
			InputMode getInputMode() const { return _inputMode; }
			void setInputFlag(InputFlag flag) { _inputFlag = flag; }
			void setMaxLength(int value) { _maxLength = value; }
			void setReturnType(int) {}
			void openKeyboard() {}
			void setTouchEnabled(bool value) { _touchEnabled = value; }
			void addTouchEventListener(const std::function<void(Ref*, Widget::TouchEventType)>& callback) { _touchCallback = callback; }
			void setDelegate(EditBoxDelegate* delegate) { _delegate = delegate; }

		private:
			std::string _text;
			std::string _placeHolder;
			InputMode _inputMode;
			InputFlag _inputFlag;
			int _maxLength;
			bool _touchEnabled;
			EditBoxDelegate* _delegate;
			std::function<void(Ref*, Widget::TouchEventType)> _touchCallback;
		};
	}
}

#endif
