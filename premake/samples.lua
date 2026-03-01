function CreateGameProject( projectName )
  project( projectName )
    kind( "ConsoleApp" )
    location( "../build/%{prj.name}" )
    debugdir( "$(OutDir)" )
    -- increase precompiled header allocation limit
    buildoptions( { "/Zm256" } )
    -- link against all other libraries
    links( {
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
    } )
    libdirs{ "../libs/" }
    configuration( { "windows" } )
      links { "ogre3d_direct3d9" }
      -- add the directx include directory
      buildoptions( { "/I \"$(DXSDK_DIR)/Include/\"" } )
      -- link against directx libraries
      links( {
        "d3d9",
        "dinput8",
        "dxguid",
        "d3dx9",
        "DxErr",
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
    configuration( { "windows", "x32", "Debug" } )
      defines { "_ITERATOR_DEBUG_LEVEL=2" }
      libdirs( {
        "\"../build/Engine/ogre3d/obj/x32/Debug\"",
        "\"$(DXSDK_DIR)/Lib/x86\"",
      } )
    configuration( { "windows", "x32", "Release" } )
      defines { "_ITERATOR_DEBUG_LEVEL=0" }
      libdirs( {
        "\"../build/Engine/ogre3d/obj/x32/Release\"",
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
    includedirs( {
      "../src/%{prj.name}/",
      "../src/%{prj.name}/common",
      "../src/%{prj.name}/game",
      "../src/%{prj.name}/sandbox",
      "../src/%{prj.name}/sandbox/core",
      "../src/external",
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
    } )
    filter "system:windows"
      postbuildcommands {}
    filter "system:macosx"
      postbuildcommands( {
        "{MKDIR} \"$TARGET_BUILD_DIR/res\"",
        "{COPYFILE} ../../bin/Sandbox.cfg \"$TARGET_BUILD_DIR\"",
        "{COPYFILE} ../../bin/Sandbox_d.cfg \"$TARGET_BUILD_DIR\"",
        "{COPYFILE} ../../bin/SandboxResources.cfg \"$TARGET_BUILD_DIR\"",
        "{COPYFILE} ../../bin/SandboxResources_d.cfg \"$TARGET_BUILD_DIR\"",
        "{COPYDIR} ../../bin/res \"$TARGET_BUILD_DIR/res\"",
        "{MKDIR} \"$TARGET_BUILD_DIR/res/scripts\"",
        "/bin/cp -R \"$SRCROOT/../../bin/res/scripts/.\" \"$TARGET_BUILD_DIR/res/scripts/\"",
        "/bin/ln -sfn \"$SRCROOT/../../media\" \"$TARGET_BUILD_DIR/../media\"",
        "/bin/ln -sfn \"$SRCROOT/../../media\" \"$PROJECT_DIR/../media\""
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
    links( {
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
