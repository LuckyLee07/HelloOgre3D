#ifndef __AGENT_CONFIG_SERVICE_H__
#define __AGENT_CONFIG_SERVICE_H__

class AgentConfigService //tolua_exports
{ //tolua_exports
public:
	AgentConfigService();
	~AgentConfigService();

	//tolua_begin
	void SetUseCppFsmFlag(bool value);
	bool GetUseCppFsmFlag() const;
	//tolua_end

private:
	bool m_useCppFsmFlag;

}; //tolua_exports

#endif // __AGENT_CONFIG_SERVICE_H__
