#ifndef __HELLO_FAIRY_GUI_SYSTEM_FAIRY_INCLUDES_H__
#define __HELLO_FAIRY_GUI_SYSTEM_FAIRY_INCLUDES_H__

#include "ui/fairygui/FairyGuiSystem.h"

#include "Controller.h"
#include "cocos2d.h"
#include "GComboBox.h"
#include "GComponent.h"
#include "GGraph.h"
#include "GList.h"
#include "GLoader.h"
#include "GObject.h"
#include "GProgressBar.h"
#include "GRoot.h"
#include "GSlider.h"
#include "GTextField.h"
#include "GTextInput.h"
#include "Transition.h"
#include "UIPackage.h"
#include "event/EventContext.h"
#include "event/InputEvent.h"
#include "event/UIEventType.h"

#include "Ogre.h"
#include "OgreDpiHelper.h"
#include "OgreRenderWindow.h"
#include "ScriptLuaVM.h"
#include "profiling/Profile.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <imm.h>
#pragma comment(lib, "imm32.lib")
#endif

#endif // __HELLO_FAIRY_GUI_SYSTEM_FAIRY_INCLUDES_H__