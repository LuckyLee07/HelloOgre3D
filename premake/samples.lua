local function AddDebugSuffix(libraries)
  local debugLibraries = {}
  for _, library in ipairs(libraries) do
    table.insert(debugLibraries, library .. "_d")
  end
  return debugLibraries
end

local function LinkProjectLibraries(libraries)
  filter "configurations:Debug"
    links(AddDebugSuffix(libraries))
  filter "configurations:Release"
    links(libraries)
  filter {}
end

function CreateGameProject( projectName )
  project( projectName )
    kind( "ConsoleApp" )
    location( "../build/%{prj.name}" )
    debugdir( "$(OutDir)" )
    -- increase precompiled header allocation limit
    buildoptions( { "/Zm256" } )
	    -- link against all other libraries
	    LinkProjectLibraries( {
	      "freeimage",
	      "freetype",
      "libjpeg",
      "libopenjpeg",
      "libpng",
      "libraw",
      "libtiff4",
      "ois",
      "ogre3d",
      "ogre3d_glsupport",
      "ogre3d_opengl",
      "ogre3d_gl3plus",
      "ogre3d_particlefx",
      "ogre3d_procedural",
      "openexr",
      "zlib",
      "zzip",
      "lua",
      "tolua",
      "luasocket",
      "bullet_collision",
      "bullet_dynamics",
      "bullet_linearmath",
      "ogre3d_gorilla",
      "opensteer",
      "recast",
      "detour",
	      "tracy",
	    } )
	    if HELLO_FAIRYGUI_ENABLED then
	      dependson { "fairygui" }
	      LinkProjectLibraries( { "fairygui" } )
	    end
	    libdirs{ "../libs/" }
    configuration( { "windows" } )
    configuration( { "windows", "Debug" } )
      links { "ogre3d_direct3d9_d" }
    configuration( { "windows", "Release" } )
      links { "ogre3d_direct3d9" }
    configuration( { "windows" } )
      -- add the directx include directory
      buildoptions( { "/I \"$(DXSDK_DIR)/Include/\"" } )
      -- link against directx libraries
      links( {
        "ws2_32",
        "d3d9",
        "dinput8",
        "dxguid",
        "d3dx9",
        "DxErr",
        "advapi32",
        "dbghelp",
        "user32",
        "legacy_stdio_definitions.lib"
      } )
      -- static linking against ogre requires linking against ogre's resource file
      linkoptions( "OgreWin32Resources.res" )
    configuration( { "macosx" } )
      links {
        "Cocoa.framework",
        "Carbon.framework",
        "IOKit.framework"
      }
      linkoptions {
        "-framework Cocoa",
        "-framework Carbon",
        "-framework IOKit",
        "-framework Foundation",
        "-framework AppKit",
        "-framework CoreFoundation",
        "-framework OpenGL"
      }
    configuration( { "linux" } )
      links {
        "pthread",
        "dl"
      }
    filter "system:macosx"
      linkoptions {
        "-framework Cocoa",
        "-framework Carbon",
        "-framework IOKit",
        "-framework Foundation",
        "-framework AppKit",
        "-framework CoreFoundation",
        "-framework OpenGL",
        "-lobjc"
      }
    filter {}
    configuration( { "windows", "x86", "Debug" } )
      defines { "_ITERATOR_DEBUG_LEVEL=2" }
      libdirs( {
        "\"../build/Engine/ogre3d/obj/x86/Debug\"",
        "\"$(DXSDK_DIR)/Lib/x86\"",
      } )
    configuration( { "windows", "x86", "Release" } )
      defines { "_ITERATOR_DEBUG_LEVEL=0" }
      libdirs( {
        "\"../build/Engine/ogre3d/obj/x86/Release\"",
        "\"$(DXSDK_DIR)/Lib/x86\"",
      } )
    configuration( { "windows", "x64", "Debug" } )
      defines { "_ITERATOR_DEBUG_LEVEL=2" }
      libdirs( {
        "\"../build/Engine/ogre3d/obj/x64/Debug\"",
        "\"$(DXSDK_DIR)/Lib/x64\""
      } )
    configuration( { "windows", "x64", "Release" } )
      defines { "_ITERATOR_DEBUG_LEVEL=0" } 
      libdirs( {
        "\"../build/Engine/ogre3d/obj/x64/Release\"",
        "\"$(DXSDK_DIR)/Lib/x64\""
      } )
    configuration( "*" )
    if HELLO_TRACY_ENABLED then
      defines {
        "HELLO_ENABLE_TRACY",
        "TRACY_ENABLE",
        "TRACY_ON_DEMAND",
        "TRACY_ALLOW_SHADOW_WARNING"
      }
    end
    if HELLO_FAIRYGUI_ENABLED then
      defines {
        "HELLO_ENABLE_FGUI"
      }
    end
	    includedirs( {
	      "../src/%{prj.name}/",
	      "../src/%{prj.name}/common",
	      "../src/%{prj.name}/game",
	      "../src/%{prj.name}/runtime",
	      "../src/%{prj.name}/sandbox",
      "../src/%{prj.name}/sandbox/core",
      "../src/external",
      "../src/external/tracy/public",
      "../src/external/lua/lua",
      "../src/external/lua/tolua",
      "../src/external/lua/luasocket",
      "../src/Engine/ogre3d/include/",
      "../src/Engine/ogre3d_glsupport/include/",
      "../src/Engine/ogre3d_opengl/include/",
      "../src/Engine/ogre3d_gl3plus/include/",
      "../src/Engine/ogre3d_gl3plus/include/GLSL/",
      "../src/Engine/ogre3d_glsupport/include/win32/",
      "../src/Engine/ogre3d_particlefx/include/",
      "../src/Engine/ogre3d_procedural/include/",
      "../src/Engine/ThirdParty/zzip/include/",
      "../src/external/bullet_collision/include/",
      "../src/external/bullet_dynamics/include/",
      "../src/external/bullet_linearmath/include/",
	      "../src/external/ois/includes/",
	    } )
    if HELLO_FAIRYGUI_ENABLED then
      includedirs( {
        "../src/%{prj.name}/runtime/ui/fairygui/cocoslite",
        "../src/external/fairygui_cocos2dx/libfairygui/Classes",
      } )
    end
    configuration( { "windows" } )
      includedirs { "../src/Engine/ogre3d_direct3d9/include/" }
    configuration( "*" )
    filter { "system:not windows" }
      buildoptions {
        "-I" .. path.getabsolute("../src/Engine/ThirdParty/zzip/include/")
      }
    filter {}

    files( {
      "../src/" .. projectName .. "/**.h",
      "../src/" .. projectName .. "/**.cpp",
      "../src/" .. projectName .. "/game/**.h",
      "../src/" .. projectName .. "/game/**.cpp",
      "../src/" .. projectName .. "/client/**.h",
      "../src/" .. projectName .. "/client/**.cpp",
      "../src/" .. projectName .. "/common/**.h",
      "../src/" .. projectName .. "/common/**.cpp",
      "../src/" .. projectName .. "/sandbox/**.h",
      "../src/" .. projectName .. "/sandbox/**.cpp",
      "../src/" .. projectName .. "/runtime/**.h",
      "../src/" .. projectName .. "/runtime/**.cpp",
    } )
    if HELLO_FAIRYGUI_ENABLED then
      excludes( {
        "../src/" .. projectName .. "/runtime/ui/fairygui/cocoslite/**.cpp",
      } )
    else
      excludes( {
        "../src/" .. projectName .. "/runtime/ui/fairygui/**.cpp",
      } )
    end
    filter "system:windows"
      postbuildcommands {}
    filter "system:macosx"
      postbuildcommands( {
        "HELLO_BIN_DIR=\"$(cd \"$SRCROOT/../../bin\" && pwd)\"",
        "HELLO_TARGET_DIR=\"$(mkdir -p \"$TARGET_BUILD_DIR\" && cd \"$TARGET_BUILD_DIR\" && pwd)\"",
        "if [ \"$HELLO_BIN_DIR\" != \"$HELLO_TARGET_DIR\" ]; then",
        "  mkdir -p \"$TARGET_BUILD_DIR/res\"",
        "  /bin/cp -f \"$HELLO_BIN_DIR/Sandbox.cfg\" \"$TARGET_BUILD_DIR\"",
        "  /bin/cp -f \"$HELLO_BIN_DIR/Sandbox_d.cfg\" \"$TARGET_BUILD_DIR\"",
        "  /bin/cp -f \"$HELLO_BIN_DIR/SandboxResources.cfg\" \"$TARGET_BUILD_DIR\"",
        "  /bin/cp -f \"$HELLO_BIN_DIR/SandboxResources_d.cfg\" \"$TARGET_BUILD_DIR\"",
        "  /bin/cp -R \"$HELLO_BIN_DIR/res/.\" \"$TARGET_BUILD_DIR/res/\"",
        "fi",
        "HELLO_MEDIA_DIR=\"$(cd \"$SRCROOT/../../media\" && pwd)\"",
        "HELLO_TARGET_MEDIA_DIR=\"$(cd \"$TARGET_BUILD_DIR/..\" && pwd)/media\"",
        "if [ \"$HELLO_MEDIA_DIR\" != \"$HELLO_TARGET_MEDIA_DIR\" ]; then",
        "  /bin/ln -shfn \"$HELLO_MEDIA_DIR\" \"$TARGET_BUILD_DIR/../media\"",
        "fi",
        "/bin/ln -shfn \"$HELLO_MEDIA_DIR\" \"$PROJECT_DIR/../media\""
      } )
    filter {}
end

function CreateToluaProject( projectName )
  project( projectName )
    kind( "ConsoleApp" )
    location( "../build/%{prj.name}" )
    debugdir( "$(OutDir)" )
    -- increase precompiled header allocation limit
    buildoptions( { "/Zm256" } )
    -- link against all other libraries
    LinkProjectLibraries( {
      "lua",
      "luasocket",
    } )
    configuration( { "windows" } )
      links {
        "ws2_32", -- 添加 Winsock 依赖
        "wsock32", -- 兼容旧版本 Winsock
      }
    configuration( "*" )
    libdirs{ "../libs/" }
    includedirs( {
      "../src/external/lua/lua",
      "../src/external/lua/luasocket",
    } )
    files( {
      "../src/external/lua/tolua/**.h",
      "../src/external/lua/tolua/**.c",
    } )
end
