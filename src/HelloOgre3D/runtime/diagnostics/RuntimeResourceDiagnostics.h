#ifndef __RUNTIME_RESOURCE_DIAGNOSTICS_H__
#define __RUNTIME_RESOURCE_DIAGNOSTICS_H__

#include <string>

class RuntimeResourceDiagnostics
{
public:
	static std::string BuildResourceDump(int maxEntriesPerType);
};

#endif // __RUNTIME_RESOURCE_DIAGNOSTICS_H__
