#ifndef __CONFIG_DEFINE__
#define __CONFIG_DEFINE__

#define DEFAULT_MATERIAL "White"

#define DEFAULT_ATLAS "fonts/dejavu/dejavu"

#ifdef _DEBUG
#define APPLICATION_LOG         "Sandbox_d.log"
#define APPLICATION_CONFIG      "Sandbox_d.cfg"
#define APPLICATION_RESOURCES	"SandboxResources_d.cfg"
#else
#define APPLICATION_LOG         "Sandbox.log"
#define APPLICATION_CONFIG      "Sandbox.cfg"
#define APPLICATION_RESOURCES	"SandboxResources.cfg"
#endif

#endif  // __UI_DEFINE__
