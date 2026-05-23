#include "cocos2d.h"

#include <fstream>
#include <sstream>

namespace cocos2d
{
	const std::string STD_STRING_EMPTY;
	const Vec2 Vec2::ZERO(0, 0);
	const Vec2 Vec2::ONE(1, 1);
	const Vec2 Vec2::ANCHOR_MIDDLE(0.5f, 0.5f);
	const Vec2 Vec2::ANCHOR_BOTTOM_LEFT(0, 0);
	const Vec3 Vec3::ZERO(0, 0, 0);
	const Size Size::ZERO(0, 0);
	const Rect Rect::ZERO(0, 0, 0, 0);
	const Color3B Color3B::WHITE(255, 255, 255);
	const Color3B Color3B::BLACK(0, 0, 0);
	const Color4B Color4B::WHITE(255, 255, 255, 255);
	const Color4B Color4B::BLACK(0, 0, 0, 255);
	const Color4F Color4F::WHITE(1, 1, 1, 1);
	const Color4F Color4F::BLACK(0, 0, 0, 1);
	const Mat4 Mat4::IDENTITY;
	const Value Value::Null;
	const ValueMap ValueMapNull;
	const char* GLProgram::SHADER_NAME_POSITION_GRAYSCALE = "position_grayscale";
	const char* GLProgram::SHADER_NAME_POSITION_TEXTURE_COLOR_NO_MVP = "position_texture_color_no_mvp";
	const char* GLProgram::SHADER_NAME_POSITION_TEXTURE_ALPHA_TEST_NO_MV = "position_texture_alpha_test_no_mv";
	const char* GLProgram::UNIFORM_NAME_ALPHA_TEST_VALUE = "u_alpha_value";
	const char* Director::EVENT_RESET = "director_reset";
	const char* GLViewImpl::EVENT_WINDOW_RESIZED = "glview_window_resized";

	static Mat4 MultiplyMat4(const Mat4& lhs, const Mat4& rhs)
	{
		Mat4 result;
		for (int column = 0; column < 4; ++column)
		{
			for (int row = 0; row < 4; ++row)
			{
				result.m[column * 4 + row] =
					lhs.m[0 * 4 + row] * rhs.m[column * 4 + 0] +
					lhs.m[1 * 4 + row] * rhs.m[column * 4 + 1] +
					lhs.m[2 * 4 + row] * rhs.m[column * 4 + 2] +
					lhs.m[3 * 4 + row] * rhs.m[column * 4 + 3];
			}
		}
		return result;
	}

		static Mat4 BuildNodeTransform(float x, float y, float scaleX, float scaleY, float rotation, const Vec2& anchorPoint, const Size& contentSize)
		{
			const float radians = MATH_DEG_TO_RAD(rotation);
			const float sinValue = std::sin(radians);
		const float cosValue = std::cos(radians);
		const float anchorX = anchorPoint.x * contentSize.width;
		const float anchorY = anchorPoint.y * contentSize.height;

		Mat4 transform;
		transform.m[0] = cosValue * scaleX;
		transform.m[1] = sinValue * scaleX;
		transform.m[4] = -sinValue * scaleY;
		transform.m[5] = cosValue * scaleY;
		transform.m[12] = x - anchorX * transform.m[0] - anchorY * transform.m[4];
			transform.m[13] = y - anchorX * transform.m[1] - anchorY * transform.m[5];
			return transform;
		}

		static Vec2 TransformPoint(const Mat4& transform, const Vec2& point)
		{
			return Vec2(
				point.x * transform.m[0] + point.y * transform.m[4] + transform.m[12],
				point.x * transform.m[1] + point.y * transform.m[5] + transform.m[13]);
		}

		static Vec2 InverseTransformPoint(const Mat4& transform, const Vec2& point)
		{
			const float a = transform.m[0];
			const float b = transform.m[1];
			const float c = transform.m[4];
			const float d = transform.m[5];
			const float det = a * d - b * c;
			if (std::fabs(det) <= 0.00001f)
				return point;

			const float px = point.x - transform.m[12];
			const float py = point.y - transform.m[13];
			const float invDet = 1.0f / det;
			return Vec2(
				(d * px - c * py) * invDet,
				(-b * px + a * py) * invDet);
		}

	static unsigned int ReadBigEndianUInt(const unsigned char* data)
	{
		return (static_cast<unsigned int>(data[0]) << 24) |
			(static_cast<unsigned int>(data[1]) << 16) |
			(static_cast<unsigned int>(data[2]) << 8) |
			static_cast<unsigned int>(data[3]);
	}

	static bool ReadPngSize(const unsigned char* data, ssize_t size, int& width, int& height)
	{
		static const unsigned char pngSignature[] = { 137, 80, 78, 71, 13, 10, 26, 10 };
		if (data == nullptr || size < 24 || std::memcmp(data, pngSignature, sizeof(pngSignature)) != 0)
			return false;

		width = static_cast<int>(ReadBigEndianUInt(data + 16));
		height = static_cast<int>(ReadBigEndianUInt(data + 20));
		return width > 0 && height > 0;
	}

	static bool FileExists(const std::string& filename)
	{
		std::ifstream file(filename.c_str(), std::ios::binary);
		return file.good();
	}

	Vec2::Vec2() : x(0), y(0)
	{
	}

	Vec2::Vec2(float ax, float ay) : x(ax), y(ay)
	{
	}

	bool Vec2::equals(const Vec2& other) const
	{
		return std::fabs(x - other.x) < FLT_EPSILON && std::fabs(y - other.y) < FLT_EPSILON;
	}

	float Vec2::distance(const Vec2& other) const
	{
		float dx = x - other.x;
		float dy = y - other.y;
		return std::sqrt(dx * dx + dy * dy);
	}

	Vec2 Vec2::getNormalized() const
	{
		float len = std::sqrt(x * x + y * y);
		if (len < FLT_EPSILON)
			return Vec2::ZERO;
		return Vec2(x / len, y / len);
	}

	void Vec2::normalize()
	{
		*this = getNormalized();
	}

	Vec2 Vec2::lerp(const Vec2& other, float alpha) const
	{
		return Vec2(x + (other.x - x) * alpha, y + (other.y - y) * alpha);
	}

	Vec2 Vec2::rotateByAngle(const Vec2& pivot, float angle) const
	{
		float sinAngle = std::sin(angle);
		float cosAngle = std::cos(angle);
		Vec2 result = *this - pivot;
		return Vec2(result.x * cosAngle - result.y * sinAngle + pivot.x,
			result.x * sinAngle + result.y * cosAngle + pivot.y);
	}

	bool Vec2::isLineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d, float* s, float* t)
	{
		Vec2 ba = b - a;
		Vec2 dc = d - c;
		float denom = dc.y * ba.x - dc.x * ba.y;
		if (std::fabs(denom) < FLT_EPSILON)
			return false;

		if (s)
			*s = (dc.x * (a.y - c.y) - dc.y * (a.x - c.x)) / denom;
		if (t)
			*t = (ba.x * (a.y - c.y) - ba.y * (a.x - c.x)) / denom;
		return true;
	}

	Vec2 operator+(const Vec2& lhs, const Vec2& rhs)
	{
		return Vec2(lhs.x + rhs.x, lhs.y + rhs.y);
	}

	Vec2 operator-(const Vec2& value)
	{
		return Vec2(-value.x, -value.y);
	}

	Vec2 operator-(const Vec2& lhs, const Vec2& rhs)
	{
		return Vec2(lhs.x - rhs.x, lhs.y - rhs.y);
	}

	Vec2 operator*(const Vec2& lhs, float value)
	{
		return Vec2(lhs.x * value, lhs.y * value);
	}

	Vec2 operator*(float value, const Vec2& rhs)
	{
		return rhs * value;
	}

	Vec2 operator/(const Vec2& lhs, float value)
	{
		return Vec2(lhs.x / value, lhs.y / value);
	}

	Vec2& operator+=(Vec2& lhs, const Vec2& rhs)
	{
		lhs.x += rhs.x;
		lhs.y += rhs.y;
		return lhs;
	}

	Vec2& operator-=(Vec2& lhs, const Vec2& rhs)
	{
		lhs.x -= rhs.x;
		lhs.y -= rhs.y;
		return lhs;
	}

	bool operator==(const Vec2& lhs, const Vec2& rhs)
	{
		return lhs.equals(rhs);
	}

	bool operator!=(const Vec2& lhs, const Vec2& rhs)
	{
		return !lhs.equals(rhs);
	}

	Vec3::Vec3() : x(0), y(0), z(0)
	{
	}

	Vec3::Vec3(float ax, float ay, float az) : x(ax), y(ay), z(az)
	{
	}

	float Vec3::distance(const Vec3& other) const
	{
		float dx = x - other.x;
		float dy = y - other.y;
		float dz = z - other.z;
		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}

	Vec3 Vec3::lerp(const Vec3& other, float alpha) const
	{
		return Vec3(x + (other.x - x) * alpha, y + (other.y - y) * alpha, z + (other.z - z) * alpha);
	}

	Vec3 operator+(const Vec3& lhs, const Vec3& rhs)
	{
		return Vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
	}

	Vec3 operator-(const Vec3& lhs, const Vec3& rhs)
	{
		return Vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
	}

	Vec3 operator*(const Vec3& lhs, float value)
	{
		return Vec3(lhs.x * value, lhs.y * value, lhs.z * value);
	}

	Vec3 operator*(float value, const Vec3& rhs)
	{
		return rhs * value;
	}

	Vec4::Vec4() : x(0), y(0), z(0), w(0)
	{
	}

	Vec4::Vec4(float ax, float ay, float az, float aw) : x(ax), y(ay), z(az), w(aw)
	{
	}

	bool operator==(const Vec4& lhs, const Vec4& rhs)
	{
		return std::fabs(lhs.x - rhs.x) < FLT_EPSILON
			&& std::fabs(lhs.y - rhs.y) < FLT_EPSILON
			&& std::fabs(lhs.z - rhs.z) < FLT_EPSILON
			&& std::fabs(lhs.w - rhs.w) < FLT_EPSILON;
	}

	bool operator!=(const Vec4& lhs, const Vec4& rhs)
	{
		return !(lhs == rhs);
	}

	Size::Size() : width(0), height(0)
	{
	}

	Size::Size(float aw, float ah) : width(aw), height(ah)
	{
	}

	Size::Size(const Vec2& value) : width(value.x), height(value.y)
	{
	}

	Size& Size::operator=(const Vec2& value)
	{
		width = value.x;
		height = value.y;
		return *this;
	}

	bool Size::equals(const Size& other) const
	{
		return std::fabs(width - other.width) < FLT_EPSILON && std::fabs(height - other.height) < FLT_EPSILON;
	}

	Rect::Rect() : origin(), size()
	{
	}

	Rect::Rect(float x, float y, float width, float height) : origin(x, y), size(width, height)
	{
	}

	Rect::Rect(const Vec2& aOrigin, const Size& aSize) : origin(aOrigin), size(aSize)
	{
	}

	bool Rect::equals(const Rect& other) const
	{
		return origin.equals(other.origin) && size.equals(other.size);
	}

	void Rect::setRect(float x, float y, float width, float height)
	{
		origin = Vec2(x, y);
		size = Size(width, height);
	}

	bool Rect::containsPoint(const Vec2& point) const
	{
		return point.x >= origin.x && point.x <= origin.x + size.width
			&& point.y >= origin.y && point.y <= origin.y + size.height;
	}

	Color3B::Color3B() : r(255), g(255), b(255)
	{
	}

	Color3B::Color3B(uint8_t ar, uint8_t ag, uint8_t ab) : r(ar), g(ag), b(ab)
	{
	}

	Color3B::Color3B(const Color4B& color) : r(color.r), g(color.g), b(color.b)
	{
	}

	bool operator==(const Color3B& lhs, const Color3B& rhs)
	{
		return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
	}

	bool operator!=(const Color3B& lhs, const Color3B& rhs)
	{
		return !(lhs == rhs);
	}

	Color4B::Color4B() : r(255), g(255), b(255), a(255)
	{
	}

	Color4B::Color4B(uint8_t ar, uint8_t ag, uint8_t ab, uint8_t aa) : r(ar), g(ag), b(ab), a(aa)
	{
	}

	Color4B::Color4B(const Color3B& color, uint8_t aa) : r(color.r), g(color.g), b(color.b), a(aa)
	{
	}

	bool operator==(const Color4B& lhs, const Color4B& rhs)
	{
		return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
	}

	bool operator!=(const Color4B& lhs, const Color4B& rhs)
	{
		return !(lhs == rhs);
	}

	Color4F::Color4F() : r(1), g(1), b(1), a(1)
	{
	}

	Color4F::Color4F(float ar, float ag, float ab, float aa) : r(ar), g(ag), b(ab), a(aa)
	{
	}

	Color4F::Color4F(const Color3B& color, float aa) : r(color.r / 255.0f), g(color.g / 255.0f), b(color.b / 255.0f), a(aa)
	{
	}

	Color4F::Color4F(const Color4B& color) : r(color.r / 255.0f), g(color.g / 255.0f), b(color.b / 255.0f), a(color.a / 255.0f)
	{
	}

	Color4F::operator Color3B() const
	{
		return Color3B(static_cast<uint8_t>(r * 255), static_cast<uint8_t>(g * 255), static_cast<uint8_t>(b * 255));
	}

	Color4F::operator Color4B() const
	{
		return Color4B(static_cast<uint8_t>(r * 255), static_cast<uint8_t>(g * 255), static_cast<uint8_t>(b * 255), static_cast<uint8_t>(a * 255));
	}

	Tex2F::Tex2F() : u(0), v(0)
	{
	}

	Tex2F::Tex2F(float au, float av) : u(au), v(av)
	{
	}

	BlendFunc::BlendFunc() : src(1), dst(0)
	{
	}

	BlendFunc::BlendFunc(int asrc, int adst) : src(asrc), dst(adst)
	{
	}

	Mat4::Mat4()
	{
		std::memset(m, 0, sizeof(m));
		m[0] = 1;
		m[5] = 1;
		m[10] = 1;
		m[15] = 1;
	}

	AffineTransform::AffineTransform() : a(1), b(0), c(0), d(1), tx(0), ty(0)
	{
	}

	Ref::Ref() : _referenceCount(1)
	{
	}

	Ref::~Ref()
	{
	}

	void Ref::retain()
	{
		++_referenceCount;
	}

	void Ref::release()
	{
		if (_referenceCount > 0)
			--_referenceCount;
	}

	Ref* Ref::autorelease()
	{
		return this;
	}

	unsigned int Ref::getReferenceCount() const
	{
		return _referenceCount;
	}

	Value::Value() : _type(Type::NONE), _number(0), _bool(false)
	{
	}

	Value::Value(int value) : _type(Type::INTEGER), _number(value), _bool(value != 0)
	{
	}

	Value::Value(unsigned int value) : _type(Type::INTEGER), _number(value), _bool(value != 0)
	{
	}

	Value::Value(float value) : _type(Type::FLOAT), _number(value), _bool(value != 0)
	{
	}

	Value::Value(double value) : _type(Type::DOUBLE), _number(value), _bool(value != 0)
	{
	}

	Value::Value(bool value) : _type(Type::BOOLEAN), _number(value ? 1 : 0), _bool(value)
	{
	}

	Value::Value(const char* value) : _type(Type::STRING), _number(0), _bool(false), _string(value ? value : "")
	{
	}

	Value::Value(const std::string& value) : _type(Type::STRING), _number(0), _bool(false), _string(value)
	{
	}

	Value::Value(const ValueVector& value) : _type(Type::VECTOR), _number(0), _bool(false), _vector(value)
	{
	}

	Value::Value(const ValueMap& value) : _type(Type::MAP), _number(0), _bool(false), _map(value)
	{
	}

	int Value::asInt() const
	{
		return static_cast<int>(_number);
	}

	unsigned int Value::asUnsignedInt() const
	{
		return static_cast<unsigned int>(_number);
	}

	float Value::asFloat() const
	{
		return static_cast<float>(_number);
	}

	double Value::asDouble() const
	{
		return _number;
	}

	bool Value::asBool() const
	{
		return _type == Type::BOOLEAN ? _bool : _number != 0;
	}

	std::string Value::asString() const
	{
		if (_type == Type::STRING)
			return _string;
		if (_type == Type::BOOLEAN)
			return _bool ? "true" : "false";
		return std::to_string(_number);
	}

	ValueVector& Value::asValueVector()
	{
		return _vector;
	}

	const ValueVector& Value::asValueVector() const
	{
		return _vector;
	}

	ValueMap& Value::asValueMap()
	{
		return _map;
	}

	const ValueMap& Value::asValueMap() const
	{
		return _map;
	}

	Data::Data() : _bytes(nullptr), _size(0)
	{
	}

	Data::~Data()
	{
		clear();
	}

	Data::Data(const Data& other) : _bytes(nullptr), _size(0)
	{
		if (other._bytes && other._size > 0)
		{
			_bytes = new unsigned char[static_cast<size_t>(other._size)];
			std::memcpy(_bytes, other._bytes, static_cast<size_t>(other._size));
			_size = other._size;
		}
	}

	Data& Data::operator=(const Data& other)
	{
		if (this != &other)
		{
			clear();
			if (other._bytes && other._size > 0)
			{
				_bytes = new unsigned char[static_cast<size_t>(other._size)];
				std::memcpy(_bytes, other._bytes, static_cast<size_t>(other._size));
				_size = other._size;
			}
		}
		return *this;
	}

	Data::Data(Data&& other) : _bytes(other._bytes), _size(other._size)
	{
		other._bytes = nullptr;
		other._size = 0;
	}

	Data& Data::operator=(Data&& other)
	{
		if (this != &other)
		{
			clear();
			_bytes = other._bytes;
			_size = other._size;
			other._bytes = nullptr;
			other._size = 0;
		}
		return *this;
	}

	bool Data::isNull() const
	{
		return _bytes == nullptr || _size <= 0;
	}

	unsigned char* Data::getBytes() const
	{
		return _bytes;
	}

	ssize_t Data::getSize() const
	{
		return _size;
	}

	unsigned char* Data::takeBuffer(ssize_t* size)
	{
		if (size)
			*size = _size;
		unsigned char* bytes = _bytes;
		_bytes = nullptr;
		_size = 0;
		return bytes;
	}

	void Data::clear()
	{
		delete[] _bytes;
		_bytes = nullptr;
		_size = 0;
	}

	void Data::fastSet(unsigned char* bytes, ssize_t size)
	{
		clear();
		_bytes = bytes;
		_size = size;
	}

	GLProgramCache* GLProgramCache::getInstance()
	{
		static GLProgramCache cache;
		return &cache;
	}

	GLProgram* GLProgramCache::getGLProgram(const std::string&)
	{
		static GLProgram program;
		return &program;
	}

	GLProgramState* GLProgramState::getOrCreateWithGLProgramName(const std::string&, Texture2D*)
	{
		static GLProgramState state;
		return &state;
	}

	Texture2D::TexParams::TexParams() : minFilter(GL_LINEAR), magFilter(GL_LINEAR), wrapS(GL_REPEAT), wrapT(GL_REPEAT)
	{
	}

	Texture2D::TexParams::TexParams(int aminFilter, int amagFilter, int awrapS, int awrapT) : minFilter(aminFilter), magFilter(amagFilter), wrapS(awrapS), wrapT(awrapT)
	{
	}

	Texture2D::TexParams::TexParams(backend::SamplerFilter, backend::SamplerFilter, backend::SamplerAddressMode, backend::SamplerAddressMode) : minFilter(GL_LINEAR), magFilter(GL_LINEAR), wrapS(GL_REPEAT), wrapT(GL_REPEAT)
	{
	}

	Texture2D::Texture2D() : _width(0), _height(0), _filePath(), _imageData()
	{
	}

	bool Texture2D::initWithData(const void* data, ssize_t size, PixelFormat, int width, int height, const Size&)
	{
		_width = width;
		_height = height;
		_filePath.clear();
		if (data != nullptr && size > 0)
		{
			const unsigned char* bytes = static_cast<const unsigned char*>(data);
			_imageData.assign(bytes, bytes + size);
		}
		return true;
	}

	bool Texture2D::initWithImage(Image* image)
	{
		if (image)
		{
			_width = image->getWidth();
			_height = image->getHeight();
			_filePath = image->getFilePath();
			if (image->getData() != nullptr && image->getDataLen() > 0)
				_imageData.assign(image->getData(), image->getData() + image->getDataLen());
		}
		return true;
	}

	void Texture2D::updateWithImage(Image* image, PixelFormat, int, int)
	{
		if (image)
		{
			_width = image->getWidth();
			_height = image->getHeight();
			_filePath = image->getFilePath();
			if (image->getData() != nullptr && image->getDataLen() > 0)
				_imageData.assign(image->getData(), image->getData() + image->getDataLen());
		}
	}

	void Texture2D::updateWithImage(Image* image, PixelFormat format, int index, TextureFormatEXT)
	{
		updateWithImage(image, format, index, 0);
	}

	void Texture2D::setTexParameters(const TexParams&)
	{
	}

	SpriteFrame* SpriteFrame::createWithTexture(Texture2D* texture)
	{
		Size size = texture ? texture->getContentSize() : Size::ZERO;
		return createWithTexture(texture, Rect(Vec2::ZERO, size), false, Vec2::ZERO, size);
	}

	SpriteFrame* SpriteFrame::createWithTexture(Texture2D* texture, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize)
	{
		SpriteFrame* frame = new (std::nothrow) SpriteFrame();
		if (frame && frame->initWithTexture(texture, rect, rotated, offset, originalSize))
		{
			frame->autorelease();
			return frame;
		}
		delete frame;
		return nullptr;
	}

	bool SpriteFrame::initWithTexture(Texture2D* texture, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize)
	{
		_texture = texture;
		_rect = rect;
		_rotated = rotated;
		_offset = offset;
		_originalSize = originalSize;
		return true;
	}

	Image::Image() : _width(0), _height(0), _format(Format::UNKNOWN), _filePath()
	{
	}

	bool Image::initWithImageFile(const std::string& filename)
	{
		Data data = FileUtils::getInstance()->getDataFromFile(filename);
		if (data.isNull())
			return false;
		_filePath = filename;
		return initWithImageData(data.getBytes(), data.getSize());
	}

	bool Image::initWithRawData(const unsigned char* data, ssize_t size, int width, int height, int, bool)
	{
		_width = width;
		_height = height;
		return initWithImageData(data, size);
	}

	bool Image::initWithImageData(const unsigned char* data, ssize_t size)
	{
		if (data && size > 0)
			_data.assign(data, data + size);
		if (_width == 0 || _height == 0)
			ReadPngSize(data, size, _width, _height);
		_width = _width == 0 ? 1 : _width;
		_height = _height == 0 ? 1 : _height;
		_format = Format::PNG;
		return true;
	}

	Texture2D* TextureCache::addImage(const std::string& filename)
	{
		const std::string fullPath = FileUtils::getInstance()->fullPathForFilename(filename);
		std::map<std::string, Texture2D*>::iterator it = _textures.find(fullPath);
		if (it != _textures.end())
			return it->second;

		Image image;
		if (!image.initWithImageFile(fullPath))
			return nullptr;
		Texture2D* texture = new (std::nothrow) Texture2D();
		if (texture)
		{
			texture->initWithImage(&image);
			texture->autorelease();
			_textures[fullPath] = texture;
		}
		return texture;
	}

	Texture2D* TextureCache::addImage(Image* image, const std::string& key)
	{
		std::string cacheKey = key.empty() && image != nullptr ? image->getFilePath() : key;
		if (!cacheKey.empty())
		{
			cacheKey = "image:" + cacheKey;
			std::map<std::string, Texture2D*>::iterator it = _textures.find(cacheKey);
			if (it != _textures.end())
				return it->second;
		}

		Texture2D* texture = new (std::nothrow) Texture2D();
		if (texture)
		{
			texture->initWithImage(image);
			texture->autorelease();
			if (!cacheKey.empty())
				_textures[cacheKey] = texture;
		}
		return texture;
	}

	FileUtils* FileUtils::getInstance()
	{
		static FileUtils fileUtils;
		return &fileUtils;
	}

	bool FileUtils::isFileExist(const std::string& filename) const
	{
		return FileExists(fullPathForFilename(filename));
	}

	bool FileUtils::isAbsolutePath(const std::string& path) const
	{
		return (path.size() > 2 && path[1] == ':') || (!path.empty() && (path[0] == '/' || path[0] == '\\'));
	}

	std::string FileUtils::fullPathForFilename(const std::string& filename) const
	{
		if (filename.empty() || isAbsolutePath(filename) || FileExists(filename))
			return filename;

		const char* searchRoots[] =
		{
			"../",
			"../../",
			"../../../"
		};
		for (size_t index = 0; index < sizeof(searchRoots) / sizeof(searchRoots[0]); ++index)
		{
			const std::string candidate = std::string(searchRoots[index]) + filename;
			if (FileExists(candidate))
				return candidate;
		}
		return filename;
	}

	std::string FileUtils::getStringFromFile(const std::string& filename) const
	{
		const std::string fullPath = fullPathForFilename(filename);
		std::ifstream file(fullPath.c_str(), std::ios::binary);
		if (!file.good())
			return "";
		std::ostringstream stream;
		stream << file.rdbuf();
		return stream.str();
	}

	Data FileUtils::getDataFromFile(const std::string& filename) const
	{
		Data data;
		const std::string fullPath = fullPathForFilename(filename);
		std::ifstream file(fullPath.c_str(), std::ios::binary | std::ios::ate);
		if (!file.good())
			return data;

		std::ifstream::pos_type size = file.tellg();
		if (size <= 0)
			return data;

		file.seekg(0, std::ios::beg);
		unsigned char* bytes = new unsigned char[static_cast<size_t>(size)];
		file.read(reinterpret_cast<char*>(bytes), size);
		data.fastSet(bytes, static_cast<ssize_t>(size));
		return data;
	}

	FileUtils::Status FileUtils::getContents(const std::string& filename, Data* data) const
	{
		if (!data)
			return Status::OpenFailed;
		*data = getDataFromFile(filename);
		return data->isNull() ? Status::NotExists : Status::OK;
	}

	std::string FileUtils::getFileExtension(const std::string& path) const
	{
		size_t pos = path.find_last_of('.');
		return pos == std::string::npos ? "" : path.substr(pos);
	}

	Touch::Touch() : _id(0), _location(), _previousLocation()
	{
	}

	void Touch::setTouchInfo(int id, float x, float y, float previousX, float previousY)
	{
		_id = id;
		_location.set(x, y);
		_previousLocation.set(previousX, previousY);
	}

	EventMouse::EventMouse() : _button(MouseButton::BUTTON_UNSET), _x(0), _y(0), _scrollX(0), _scrollY(0)
	{
	}

	EventListenerMouse* EventListenerMouse::create()
	{
		EventListenerMouse* listener = new (std::nothrow) EventListenerMouse();
		if (listener)
			listener->autorelease();
		return listener;
	}

	EventListenerTouchOneByOne* EventListenerTouchOneByOne::create()
	{
		EventListenerTouchOneByOne* listener = new (std::nothrow) EventListenerTouchOneByOne();
		if (listener)
		{
			listener->_swallowTouches = false;
			listener->autorelease();
		}
		return listener;
	}

	EventListenerKeyboard* EventListenerKeyboard::create()
	{
		EventListenerKeyboard* listener = new (std::nothrow) EventListenerKeyboard();
		if (listener)
			listener->autorelease();
		return listener;
	}

	Action::Action() : _target(nullptr)
	{
	}

	FiniteTimeAction::FiniteTimeAction() : _duration(0)
	{
	}

	AnimationFrame* AnimationFrame::create(SpriteFrame* spriteFrame, float delayUnits, const ValueMap& userInfo)
	{
		AnimationFrame* frame = new (std::nothrow) AnimationFrame();
		if (frame)
		{
			frame->_spriteFrame = spriteFrame;
			frame->_delayUnits = delayUnits;
			frame->_userInfo = userInfo;
			frame->autorelease();
		}
		return frame;
	}

	Animation* Animation::createWithAnimationFrames(const Vector<AnimationFrame*>& frames, float delayPerUnit, unsigned int loops)
	{
		Animation* animation = new (std::nothrow) Animation();
		if (animation)
		{
			animation->initWithAnimationFrames(frames, delayPerUnit, loops);
			animation->autorelease();
		}
		return animation;
	}

	Animation* Animation::create()
	{
		Animation* animation = new (std::nothrow) Animation();
		if (animation)
			animation->autorelease();
		return animation;
	}

	bool Animation::initWithAnimationFrames(const Vector<AnimationFrame*>& frames, float delayPerUnit, unsigned int loops)
	{
		_frames = frames;
		_delayPerUnit = delayPerUnit;
		_loops = loops;
		return true;
	}

	Animation* Animation::clone() const
	{
		Animation* animation = new (std::nothrow) Animation();
		if (animation)
		{
			animation->_frames = _frames;
			animation->_delayPerUnit = _delayPerUnit;
			animation->_loops = _loops;
			animation->autorelease();
		}
		return animation;
	}

	Animate* Animate::create(Animation* animation)
	{
		Animate* action = new (std::nothrow) Animate();
		if (action)
		{
			action->_animation = animation;
			action->autorelease();
		}
		return action;
	}

	RepeatForever* RepeatForever::create(Action* action)
	{
		RepeatForever* repeat = new (std::nothrow) RepeatForever();
		if (repeat)
		{
			repeat->_action = action;
			repeat->autorelease();
		}
		return repeat;
	}

	DelayTime* DelayTime::create(float duration)
	{
		DelayTime* delay = new (std::nothrow) DelayTime();
		if (delay)
		{
			delay->setDuration(duration);
			delay->autorelease();
		}
		return delay;
	}

	CallFunc* CallFunc::create(const std::function<void()>& func)
	{
		CallFunc* action = new (std::nothrow) CallFunc();
		if (action)
		{
			action->_func = func;
			action->autorelease();
		}
		return action;
	}

	Sequence* Sequence::create(FiniteTimeAction* action1, FiniteTimeAction* action2, std::nullptr_t)
	{
		return createWithTwoActions(action1, action2);
	}

	Sequence* Sequence::createWithTwoActions(FiniteTimeAction* action1, FiniteTimeAction* action2)
	{
		Sequence* sequence = new (std::nothrow) Sequence();
		if (sequence)
		{
			sequence->_actions.push_back(action1);
			sequence->_actions.push_back(action2);
			sequence->autorelease();
		}
		return sequence;
	}

	Renderer::Renderer() :
		_commandSink(nullptr),
		_triangleCommandCount(0),
		_submittedTriangleCount(0),
		_stencilStage(STENCIL_STAGE_DISABLED),
		_stencilInverted(false),
		_stencilDepth(0),
		_stencilRevision(0)
	{
	}

	void Renderer::beginFrame()
	{
		_triangleCommandCount = 0;
		_submittedTriangleCount = 0;
		if (_stencilStage != STENCIL_STAGE_DISABLED || _stencilDepth != 0 || _stencilInverted)
			setStencilState(STENCIL_STAGE_DISABLED, false, 0);
		Director::getInstance()->getOpenGLView()->setScissorTestEnabled(false);
	}

	void Renderer::setScissorTest(bool enabled)
	{
		Director::getInstance()->getOpenGLView()->setScissorTestEnabled(enabled);
	}

	void Renderer::setStencilState(StencilStage stage, bool inverted, int depth)
	{
		_stencilStage = stage;
		_stencilInverted = inverted;
		_stencilDepth = depth;
		++_stencilRevision;
	}

	void Renderer::addCommand(RenderCommand* command)
	{
		if (command == nullptr)
			return;

		CustomCommand* customCommand = dynamic_cast<CustomCommand*>(command);
		if (customCommand != nullptr)
		{
			if (customCommand->func)
				customCommand->func();
			if (_commandSink != nullptr)
				_commandSink->handleCustomCommand(*customCommand);
			return;
		}

		TrianglesCommand* trianglesCommand = dynamic_cast<TrianglesCommand*>(command);
		if (trianglesCommand != nullptr)
		{
			++_triangleCommandCount;
			_submittedTriangleCount += trianglesCommand->getTriangleCount();
			if (_commandSink != nullptr)
				_commandSink->handleTrianglesCommand(*trianglesCommand);
		}
	}

	GroupCommand::GroupCommand() : _renderQueueId(0)
	{
	}

	void GroupCommand::init(float globalOrder)
	{
		_renderQueueId = static_cast<int>(globalOrder);
	}

	TrianglesCommand::Triangles::Triangles() : verts(nullptr), vertCount(0), indices(nullptr), indexCount(0)
	{
	}

	TrianglesCommand::TrianglesCommand() :
		_globalOrder(0),
		_texture(nullptr),
		_programState(nullptr),
		_blendFunc(),
		_triangles(),
		_transform(),
		_flags(0)
	{
	}

	void TrianglesCommand::init(float globalOrder, Texture2D* texture, GLProgramState* programState, const BlendFunc& blendFunc, const Triangles& triangles, const Mat4& transform, uint32_t flags)
	{
		_globalOrder = globalOrder;
		_texture = texture;
		_programState = programState;
		_blendFunc = blendFunc;
		_triangles = triangles;
		_transform = transform;
		_flags = flags;
	}

	void TrianglesCommand::init(float globalOrder, Texture2D* texture, const BlendFunc& blendFunc, const Triangles& triangles, const Mat4& transform, uint32_t flags)
	{
		init(globalOrder, texture, nullptr, blendFunc, triangles, transform, flags);
	}

	GLView::GLView() :
		_scissorEnabled(false),
		_scissorRect(Rect::ZERO),
		_frameSize(Size::ZERO)
	{
	}

	void GLView::setScissorTestEnabled(bool enabled)
	{
		_scissorEnabled = enabled;
	}

	void GLView::setScissorInPoints(float x, float y, float width, float height)
	{
		_scissorRect = Rect(x, y, width, height);
	}

	void GLView::setFrameSize(float width, float height)
	{
		_frameSize = Size(width, height);
	}

	Camera* Camera::getVisitingCamera()
	{
		return getDefaultCamera();
	}

	Camera* Camera::getDefaultCamera()
	{
		static Camera camera;
		return &camera;
	}

	Node::Node() :
		_parent(nullptr),
		_position(),
		_anchorPoint(0, 0),
		_contentSize(),
		_modelViewTransform(),
		_visible(true),
		_running(false),
		_isTransitionFinished(true),
		_transformUpdated(true),
		_contentSizeDirty(true),
		_globalZOrder(0),
		_localZOrder(0),
		_scaleX(1),
		_scaleY(1),
		_rotationZ(0),
		_skewX(0),
		_skewY(0),
		_displayedOpacity(255),
		_displayedColor(Color3B::WHITE),
		_userData(nullptr),
		_eventDispatcher(nullptr),
		_glProgram(nullptr),
		_glProgramState(nullptr)
	{
	}

	Node::~Node()
	{
	}

	bool Node::init()
	{
		return true;
	}

	void Node::onEnter()
	{
		_running = true;
		if (_onEnterCallback)
			_onEnterCallback();
	}

	void Node::onEnterTransitionDidFinish()
	{
		_isTransitionFinished = true;
	}

	void Node::onExitTransitionDidStart()
	{
	}

	void Node::onExit()
	{
		if (_onExitCallback)
			_onExitCallback();
		_running = false;
	}

	void Node::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
	{
		if (!_visible)
			return;

		uint32_t flags = processParentFlags(parentTransform, parentFlags);
		int index = 0;

		if (!_children.empty())
		{
			sortAllChildren();
			for (int size = static_cast<int>(_children.size()); index < size; ++index)
			{
				Node* child = _children[index];
				if (child != nullptr && child->getLocalZOrder() < 0)
					child->visit(renderer, _modelViewTransform, flags);
				else
					break;
			}

			draw(renderer, _modelViewTransform, flags);

			for (std::vector<Node*>::const_iterator it = _children.begin() + index; it != _children.end(); ++it)
			{
				if (*it != nullptr)
					(*it)->visit(renderer, _modelViewTransform, flags);
			}
		}
		else
			draw(renderer, _modelViewTransform, flags);
	}

	void Node::draw(Renderer*, const Mat4&, uint32_t)
	{
	}

	void Node::setCameraMask(unsigned short, bool)
	{
	}

	void Node::setGlobalZOrder(float globalZOrder)
	{
		_globalZOrder = globalZOrder;
	}

	void Node::setContentSize(const Size& size)
	{
		_contentSize = size;
		_contentSizeDirty = true;
	}

	void Node::addChild(Node* child)
	{
		addChild(child, child ? child->_localZOrder : 0);
	}

	void Node::addChild(Node* child, int localZOrder)
	{
		if (!child)
			return;
		child->_parent = this;
		child->_localZOrder = localZOrder;
		_children.push_back(child);
	}

	void Node::removeChild(Node* child)
	{
		auto it = std::find(_children.begin(), _children.end(), child);
		if (it != _children.end())
		{
			(*it)->_parent = nullptr;
			_children.erase(it);
		}
	}

	void Node::removeChild(Node* child, bool cleanup)
	{
		(void)cleanup;
		removeChild(child);
	}

	void Node::removeFromParent()
	{
		if (_parent)
			_parent->removeChild(this);
	}

	void Node::removeAllChildren()
	{
		for (auto child : _children)
		{
			if (child)
				child->_parent = nullptr;
		}
		_children.clear();
	}

	void Node::sortAllChildren()
	{
		std::sort(_children.begin(), _children.end(), [](Node* lhs, Node* rhs) {
			return lhs->getLocalZOrder() < rhs->getLocalZOrder();
		});
	}

	void Node::setPosition(const Vec2& position)
	{
		_position = position;
		_transformUpdated = true;
	}

	void Node::setPosition(float x, float y)
	{
		setPosition(Vec2(x, y));
	}

	void Node::setPositionX(float x)
	{
		_position.x = x;
		_transformUpdated = true;
	}

	void Node::setPositionY(float y)
	{
		_position.y = y;
		_transformUpdated = true;
	}

	void Node::setScale(float scale)
	{
		setScale(scale, scale);
	}

	void Node::setScale(float scaleX, float scaleY)
	{
		_scaleX = scaleX;
		_scaleY = scaleY;
		_transformUpdated = true;
	}

		Vec2 Node::convertToNodeSpace(const Vec2& worldPoint) const
		{
			Vec2 parentPoint = worldPoint;
			if (_parent != nullptr)
				parentPoint = _parent->convertToNodeSpace(worldPoint);

			const Mat4 localTransform = BuildNodeTransform(_position.x, _position.y, _scaleX, _scaleY, _rotationZ, _anchorPoint, _contentSize);
			return InverseTransformPoint(localTransform, parentPoint);
		}

	Vec2 Node::convertToNodeSpaceAR(const Vec2& worldPoint) const
	{
		return convertToNodeSpace(worldPoint) + Vec2(_anchorPoint.x * _contentSize.width, _anchorPoint.y * _contentSize.height);
	}

		Vec2 Node::convertToWorldSpace(const Vec2& nodePoint) const
		{
			const Mat4 localTransform = BuildNodeTransform(_position.x, _position.y, _scaleX, _scaleY, _rotationZ, _anchorPoint, _contentSize);
			Vec2 parentPoint = TransformPoint(localTransform, nodePoint);
			return _parent != nullptr ? _parent->convertToWorldSpace(parentPoint) : parentPoint;
		}

	Vec2 Node::convertToWorldSpaceAR(const Vec2& nodePoint) const
	{
		return convertToWorldSpace(nodePoint - Vec2(_anchorPoint.x * _contentSize.width, _anchorPoint.y * _contentSize.height));
	}

	AffineTransform Node::getNodeToWorldAffineTransform() const
	{
		AffineTransform transform;
		transform.a = _scaleX;
		transform.d = _scaleY;
		transform.tx = _position.x;
		transform.ty = _position.y;
		return transform;
	}

	uint32_t Node::processParentFlags(const Mat4& parentTransform, uint32_t parentFlags)
	{
		Mat4 localTransform = BuildNodeTransform(_position.x, _position.y, _scaleX, _scaleY, _rotationZ, _anchorPoint, _contentSize);
		_modelViewTransform = MultiplyMat4(parentTransform, localTransform);
		_transformUpdated = false;
		_contentSizeDirty = false;
		return parentFlags | FLAGS_TRANSFORM_DIRTY;
	}

	Action* Node::runAction(Action* action)
	{
		return action;
	}

	void Node::stopAction(Action*)
	{
	}

	void Node::stopAllActions()
	{
	}

	EventDispatcher* Node::getEventDispatcher()
	{
		if (_eventDispatcher)
			return _eventDispatcher;
		return Director::getInstance()->getEventDispatcher();
	}

	Scene* Node::getScene() const
	{
		const Node* node = this;
		while (node && node->_parent)
			node = node->_parent;
		return const_cast<Scene*>(dynamic_cast<const Scene*>(node));
	}

	static uint8_t ClampColorByte(float value)
	{
		if (value <= 0.0f)
			return 0;
		if (value >= 1.0f)
			return 255;
		return static_cast<uint8_t>(value * 255.0f + 0.5f);
	}

	static Color4B ToColor4B(const Color4F& color)
	{
		return Color4B(ClampColorByte(color.r), ClampColorByte(color.g), ClampColorByte(color.b), ClampColorByte(color.a));
	}

	static Color4B TintVertexColor(const Color4B& color, const Color3B& displayedColor, uint8_t displayedOpacity)
	{
		return Color4B(
			static_cast<uint8_t>(static_cast<int>(color.r) * static_cast<int>(displayedColor.r) / 255),
			static_cast<uint8_t>(static_cast<int>(color.g) * static_cast<int>(displayedColor.g) / 255),
			static_cast<uint8_t>(static_cast<int>(color.b) * static_cast<int>(displayedColor.b) / 255),
			static_cast<uint8_t>(static_cast<int>(color.a) * static_cast<int>(displayedOpacity) / 255));
	}

	DrawNode::DrawNode() :
		_triangles(),
		_blendFunc(),
		_lineWidth(1.0f)
	{
	}

	void DrawNode::clear()
	{
		_vertices.clear();
		_indices.clear();
	}

	void DrawNode::appendVertex(const Vec2& point, const Color4F& color)
	{
		if (_vertices.size() >= 65535)
			return;

		V3F_C4B_T2F vertex;
		vertex.vertices = Vec3(point.x, point.y, 0.0f);
		vertex.colors = ToColor4B(color);
		vertex.texCoords = Tex2F(0.0f, 0.0f);
		_vertices.push_back(vertex);
	}

	void DrawNode::appendTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color4F& color)
	{
		if (_vertices.size() + 3 > 65535)
			return;

		const unsigned short baseIndex = static_cast<unsigned short>(_vertices.size());
		appendVertex(p1, color);
		appendVertex(p2, color);
		appendVertex(p3, color);
		_indices.push_back(baseIndex);
		_indices.push_back(static_cast<unsigned short>(baseIndex + 1));
		_indices.push_back(static_cast<unsigned short>(baseIndex + 2));
	}

	void DrawNode::appendLine(const Vec2& from, const Vec2& to, float width, const Color4F& color)
	{
		const float dx = to.x - from.x;
		const float dy = to.y - from.y;
		const float length = std::sqrt(dx * dx + dy * dy);
		if (length <= 0.0001f || width <= 0.0f)
			return;

		const float halfWidth = width * 0.5f;
		const float nx = -dy / length * halfWidth;
		const float ny = dx / length * halfWidth;
		const Vec2 p1(from.x + nx, from.y + ny);
		const Vec2 p2(to.x + nx, to.y + ny);
		const Vec2 p3(to.x - nx, to.y - ny);
		const Vec2 p4(from.x - nx, from.y - ny);
		appendTriangle(p1, p2, p3, color);
		appendTriangle(p1, p3, p4, color);
	}

	void DrawNode::drawTriangle(const Vec2& p1, const Vec2& p2, const Vec2& p3, const Color4F& color)
	{
		appendTriangle(p1, p2, p3, color);
	}

	void DrawNode::drawCircle(const Vec2& center, float radius, float angle, unsigned int segments, bool drawLineToCenter, float scaleX, float scaleY, const Color4F& color)
	{
		if (radius <= 0.0f)
			return;

		const unsigned int segmentCount = std::max(8u, std::min(segments, 96u));
		const float delta = static_cast<float>(M_PI * 2.0 / static_cast<double>(segmentCount));
		Vec2 first;
		Vec2 previous;
		for (unsigned int index = 0; index <= segmentCount; ++index)
		{
			const float currentAngle = angle + delta * static_cast<float>(index);
			const Vec2 point(
				center.x + std::cos(currentAngle) * radius * scaleX,
				center.y + std::sin(currentAngle) * radius * scaleY);
			if (index == 0)
			{
				first = point;
				previous = point;
				continue;
			}
			appendLine(previous, point, _lineWidth, color);
			previous = point;
		}
		if (drawLineToCenter)
			appendLine(center, first, _lineWidth, color);
	}

	void DrawNode::drawSolidCircle(const Vec2& center, float radius, float angle, unsigned int segments, float scaleX, float scaleY, const Color4F& color)
	{
		if (radius <= 0.0f)
			return;

		const unsigned int segmentCount = std::max(8u, std::min(segments, 96u));
		const float delta = static_cast<float>(M_PI * 2.0 / static_cast<double>(segmentCount));
		Vec2 previous(
			center.x + std::cos(angle) * radius * scaleX,
			center.y + std::sin(angle) * radius * scaleY);
		for (unsigned int index = 1; index <= segmentCount; ++index)
		{
			const float currentAngle = angle + delta * static_cast<float>(index);
			const Vec2 point(
				center.x + std::cos(currentAngle) * radius * scaleX,
				center.y + std::sin(currentAngle) * radius * scaleY);
			appendTriangle(center, previous, point, color);
			previous = point;
		}
	}

	void DrawNode::drawPolygon(const Vec2* points, int count, const Color4F& fillColor, float borderWidth, const Color4F& borderColor)
	{
		if (points == nullptr || count < 3)
			return;

		for (int index = 1; index + 1 < count; ++index)
			appendTriangle(points[0], points[index], points[index + 1], fillColor);

		if (borderWidth > 0.0f)
		{
			for (int index = 0; index < count; ++index)
				appendLine(points[index], points[(index + 1) % count], borderWidth, borderColor);
		}
	}

	void DrawNode::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
	{
		if (renderer == nullptr || _vertices.empty() || _indices.empty())
			return;

		std::vector<V3F_C4B_T2F> tintedVertices;
		const V3F_C4B_T2F* commandVertices = &_vertices[0];
		if (_displayedOpacity != 255 || _displayedColor != Color3B::WHITE)
		{
			tintedVertices = _vertices;
			for (std::vector<V3F_C4B_T2F>::iterator it = tintedVertices.begin(); it != tintedVertices.end(); ++it)
				it->colors = TintVertexColor(it->colors, _displayedColor, _displayedOpacity);
			commandVertices = &tintedVertices[0];
		}

		_triangles.verts = const_cast<V3F_C4B_T2F*>(commandVertices);
		_triangles.vertCount = static_cast<int>(_vertices.size());
		_triangles.indices = &_indices[0];
		_triangles.indexCount = static_cast<int>(_indices.size());
		_trianglesCommand.init(_globalZOrder, nullptr, getGLProgramState(), _blendFunc, _triangles, transform, flags);
		renderer->addCommand(&_trianglesCommand);
	}

	Sprite::Sprite() :
		_texture(nullptr),
		_spriteFrame(nullptr),
		_rect(),
		_rectRotated(false),
		_originalContentSize(),
		_unflippedOffsetPositionFromCenter(),
		_centerRectNormalized(0, 0, 1, 1),
		_centerRect(),
		_flippedX(false),
		_flippedY(false),
		_blendFunc(),
		_trianglesCommand(),
		_quadVerts(),
		_quadIndices(),
		_quadTriangles()
	{
		_quadIndices[0] = 0;
		_quadIndices[1] = 1;
		_quadIndices[2] = 2;
		_quadIndices[3] = 2;
		_quadIndices[4] = 1;
		_quadIndices[5] = 3;
		_quadTriangles.verts = _quadVerts;
		_quadTriangles.vertCount = 4;
		_quadTriangles.indices = _quadIndices;
		_quadTriangles.indexCount = 6;
	}

	Sprite::~Sprite()
	{
	}

	Sprite* Sprite::createWithSpriteFrame(SpriteFrame* spriteFrame)
	{
		Sprite* sprite = new (std::nothrow) Sprite();
		if (sprite && sprite->initWithSpriteFrame(spriteFrame))
		{
			sprite->autorelease();
			return sprite;
		}
		delete sprite;
		return nullptr;
	}

	Sprite* Sprite::createWithTexture(Texture2D* texture)
	{
		Sprite* sprite = new (std::nothrow) Sprite();
		if (sprite && sprite->initWithTexture(texture))
		{
			sprite->autorelease();
			return sprite;
		}
		delete sprite;
		return nullptr;
	}

	bool Sprite::initWithSpriteFrame(SpriteFrame* spriteFrame)
	{
		setSpriteFrame(spriteFrame);
		return true;
	}

	bool Sprite::initWithTexture(Texture2D* texture)
	{
		setTexture(texture);
		return true;
	}

	void Sprite::setTexture(Texture2D* texture)
	{
		_texture = texture;
		if (texture)
		{
			_originalContentSize = Size(static_cast<float>(texture->getPixelsWide()), static_cast<float>(texture->getPixelsHigh()));
			_contentSize = _originalContentSize;
			_rect = Rect(0, 0, _originalContentSize.width, _originalContentSize.height);
			_rectRotated = false;
		}
	}

	void Sprite::setSpriteFrame(SpriteFrame* spriteFrame)
	{
		_spriteFrame = spriteFrame;
		if (spriteFrame)
		{
			_texture = spriteFrame->getTexture();
			_rect = spriteFrame->getRect();
			_rectRotated = spriteFrame->isRotated();
			_originalContentSize = spriteFrame->getOriginalSize();
			if (_originalContentSize.equals(Size::ZERO))
				_originalContentSize = _rect.size;
			_contentSize = _originalContentSize;
			_unflippedOffsetPositionFromCenter = spriteFrame->getOffset();
		}
	}

	void Sprite::setTextureRect(const Rect& rect)
	{
		setTextureRect(rect, false, rect.size);
	}

	void Sprite::setTextureRect(const Rect& rect, bool rotated, const Size& untrimmedSize)
	{
		_rect = rect;
		_rectRotated = rotated;
		_originalContentSize = untrimmedSize;
		_contentSize = untrimmedSize;
	}

	V3F_C4B_T2F_Quad Sprite::getQuad() const
	{
		V3F_C4B_T2F_Quad quad;
		Color4B color(_displayedColor, _displayedOpacity);
		quad.bl.vertices = Vec3(0, 0, 0);
		quad.br.vertices = Vec3(_contentSize.width, 0, 0);
		quad.tl.vertices = Vec3(0, _contentSize.height, 0);
		quad.tr.vertices = Vec3(_contentSize.width, _contentSize.height, 0);
		quad.bl.colors = color;
		quad.br.colors = color;
		quad.tl.colors = color;
		quad.tr.colors = color;
		float left = 0;
		float right = 1;
		float top = 0;
		float bottom = 1;
		if (_texture != nullptr && _texture->getPixelsWide() > 0 && _texture->getPixelsHigh() > 0 && _rect.size.width > 0 && _rect.size.height > 0)
		{
			const float textureWidth = static_cast<float>(_texture->getPixelsWide());
			const float textureHeight = static_cast<float>(_texture->getPixelsHigh());
			left = _rect.origin.x / textureWidth;
			right = (_rect.origin.x + _rect.size.width) / textureWidth;
			top = _rect.origin.y / textureHeight;
			bottom = (_rect.origin.y + _rect.size.height) / textureHeight;
		}
		if (_flippedX)
			std::swap(left, right);
		if (_flippedY)
			std::swap(top, bottom);
		if (_rectRotated)
		{
			quad.bl.texCoords = Tex2F(left, top);
			quad.br.texCoords = Tex2F(left, bottom);
			quad.tl.texCoords = Tex2F(right, top);
			quad.tr.texCoords = Tex2F(right, bottom);
		}
		else
		{
			quad.bl.texCoords = Tex2F(left, bottom);
			quad.br.texCoords = Tex2F(right, bottom);
			quad.tl.texCoords = Tex2F(left, top);
			quad.tr.texCoords = Tex2F(right, top);
		}
		return quad;
	}

	void Sprite::updateColor()
	{
	}

	void Sprite::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
	{
		if (renderer == nullptr || _texture == nullptr || _contentSize.width <= 0 || _contentSize.height <= 0)
			return;

		V3F_C4B_T2F_Quad quad = getQuad();
		_quadVerts[0] = quad.bl;
		_quadVerts[1] = quad.br;
		_quadVerts[2] = quad.tl;
		_quadVerts[3] = quad.tr;
		_quadTriangles.verts = _quadVerts;
		_quadTriangles.vertCount = 4;
		_quadTriangles.indices = _quadIndices;
		_quadTriangles.indexCount = 6;

		_trianglesCommand.init(_globalZOrder, _texture, getGLProgramState(), _blendFunc, _quadTriangles, transform, flags);
		renderer->addCommand(&_trianglesCommand);
	}

	int* Font::getHorizontalKerningForTextUTF32(const std::u32string&, int& outNumLetters) const
	{
		outNumLetters = 0;
		return nullptr;
	}

	FontAtlas* Font::createFontAtlas()
	{
		return new FontAtlas(*this);
	}

	FontLetterDefinition::FontLetterDefinition() :
		charID(0),
		width(0),
		height(0),
		offsetX(0),
		offsetY(0),
		xAdvance(0),
		U(0),
		V(0),
		validDefinition(false),
		textureID(0)
	{
	}

	FontAtlas::FontAtlas() : _font(nullptr), _lineHeight(0)
	{
	}

	FontAtlas::FontAtlas(Font& font) : _font(&font), _lineHeight(0)
	{
	}

	Label::Label() :
		_currentLabelType(LabelType::STRING_TEXTURE),
		_fontAtlas(nullptr),
		_bmFontSize(12),
		_bmfontScale(1),
		_systemFontSize(12),
		_lineSpacing(0),
		_horizontalAlignment(TextHAlignment::LEFT),
		_verticalAlignment(TextVAlignment::TOP),
		_dimensions(),
		_overflow(Overflow::NONE),
		_lineBreakWithoutSpace(false),
		_wrapEnabled(false),
		_maxLineWidth(0)
	{
	}

	Label::~Label()
	{
	}

	void Label::setString(const std::string& value)
	{
		_string = value;
	}

	bool Label::setBMFontFilePath(const std::string& bmfontFilePath, const Vec2&, float fontSize)
	{
		_bmFontPath = bmfontFilePath;
		_bmFontSize = fontSize;
		_currentLabelType = LabelType::BMFONT;
		return true;
	}

	void Label::updateBMFontScale()
	{
		_bmfontScale = 1.0f;
	}

	StencilStateManager::StencilStateManager() :
		_alphaThreshold(1),
		_inverted(false),
		_previousStage(STENCIL_STAGE_DISABLED),
		_previousInverted(false),
		_previousDepth(0),
		_hasPreviousState(false)
	{
	}

	void StencilStateManager::onBeforeVisit()
	{
		Renderer* renderer = Director::getInstance()->getRenderer();
		_previousStage = renderer->getStencilStage();
		_previousInverted = renderer->isStencilInverted();
		_previousDepth = renderer->getStencilDepth();
		_hasPreviousState = true;
		renderer->setStencilState(STENCIL_STAGE_WRITE, _inverted, _previousDepth + 1);
	}

	void StencilStateManager::onAfterDrawStencil()
	{
		Renderer* renderer = Director::getInstance()->getRenderer();
		renderer->setStencilState(STENCIL_STAGE_TEST, _inverted, renderer->getStencilDepth());
	}

	void StencilStateManager::onAfterVisit()
	{
		if (!_hasPreviousState)
			return;

		Director::getInstance()->getRenderer()->setStencilState(_previousStage, _previousInverted, _previousDepth);
		_hasPreviousState = false;
	}

	Director::Director() : _totalFrames(0), _deltaTime(0)
	{
		_view.setFrameSize(800, 600);
	}

	Director* Director::getInstance()
	{
		static Director director;
		return &director;
	}

	void Director::mainLoop(float dt)
	{
		_deltaTime = dt;
		++_totalFrames;
	}

	namespace ui
	{
		EditBox::EditBox() :
			_inputMode(InputMode::ANY),
			_inputFlag(InputFlag::SENSITIVE),
			_maxLength(0),
			_touchEnabled(false),
			_delegate(nullptr)
		{
		}

		EditBox* EditBox::create(const Size& size, Scale9Sprite*)
		{
			EditBox* editBox = new (std::nothrow) EditBox();
			if (editBox && editBox->init())
			{
				editBox->setContentSize(size);
				editBox->autorelease();
				return editBox;
			}
			delete editBox;
			return nullptr;
		}
	}
}

void glEnable(unsigned int capability)
{
	if (capability == GL_SCISSOR_TEST)
		cocos2d::Director::getInstance()->getOpenGLView()->setScissorTestEnabled(true);
}

void glDisable(unsigned int capability)
{
	if (capability == GL_SCISSOR_TEST)
		cocos2d::Director::getInstance()->getOpenGLView()->setScissorTestEnabled(false);
}
