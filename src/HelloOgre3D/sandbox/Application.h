#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include <string>

class ClientManager;
class Application
{
public:
	Application(const std::string& appTitle = "");

    virtual ~Application();

    virtual bool Setup();

    virtual void Draw();

    virtual void Cleanup();
    
    virtual void Update();

private:
    ClientManager* m_pClientManager;
};

#endif  // __APPLICATION_H__
