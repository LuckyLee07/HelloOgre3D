#ifndef HELLO_FAIRYGUI_TINYXML2_H
#define HELLO_FAIRYGUI_TINYXML2_H

#include <cstddef>

namespace tinyxml2
{
	class XMLElement
	{
	public:
		const char* Attribute(const char*) const { return ""; }
		const char* GetText() const { return ""; }
		XMLElement* FirstChildElement(const char*) { return nullptr; }
		XMLElement* NextSiblingElement(const char*) { return nullptr; }
	};

	class XMLDocument
	{
	public:
		int Parse(const char*, size_t) { return 0; }
		XMLElement* RootElement() { return &_root; }

	private:
		XMLElement _root;
	};
}

#endif
