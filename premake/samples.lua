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
      "ogre3d",
      "ogre3d_direct3d9",
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
      "ois",
      "opensteer",
    } )
    libdirs{ "../libs/" }
    configuration( { "windows" } )
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
    configuration( { "windows", "x32", "Debug" } )
      defines { "_ITERATOR_DEBUG_LEVEL=2" }
      libdirs( {
        "\"../build/external/ogre3d/obj/x32/Debug\"",
        "\"$(DXSDK_DIR)/Lib/x86\"",
      } )
    configuration( { "windows", "x32", "Release" } )
      defines { "_ITERATOR_DEBUG_LEVEL=0" }
      libdirs( {
        "\"../build/external/ogre3d/obj/x32/Release\"",
        "\"$(DXSDK_DIR)/Lib/x86\"",
      } )
    configuration( { "windows", "x64", "Debug" } )
      defines { "_ITERATOR_DEBUG_LEVEL=2" }
      libdirs( {
        "\"../build/external/ogre3d/obj/x64/Debug\"",
        "\"$(DXSDK_DIR)/Lib/x64\""
      } )
    configuration( { "windows", "x64", "Release" } )
      defines { "_ITERATOR_DEBUG_LEVEL=0" } 
      libdirs( {
        "\"../build/external/ogre3d/obj/x64/Release\"",
        "\"$(DXSDK_DIR)/Lib/x64\""
      } )
    configuration( "*" )
    includedirs( {
      "../src/%{prj.name}/",
      "../src/%{prj.name}/base",
      "../src/%{prj.name}/game",
      "../src/%{prj.name}/sandbox",
      "../src/%{prj.name}/sandbox/base",
      "../src/%{prj.name}/sandbox/common",
      "../src/%{prj.name}/sandbox/driver",
      "../src/external",
      "../src/external/lua/lua",
      "../src/external/lua/tolua",
      "../src/external/lua/luasocket",
      "../src/external/zzip/include/",
      "../src/external/ogre3d/include/",
      "../src/external/ogre3d_direct3d9/include/",
      "../src/external/ogre3d_particlefx/include/",
      "../src/external/ogre3d_procedural/include/",
      "../src/external/bullet_collision/include/",
      "../src/external/bullet_dynamics/include/",
      "../src/external/bullet_linearmath/include/",
      "../src/external/ois/include/",
    } )

    files( {
      "../src/" .. projectName .. "/**.h",
      "../src/" .. projectName .. "/**.cpp",
      "../src/" .. projectName .. "/base/**.h",
      "../src/" .. projectName .. "/base/**.cpp",
      "../src/" .. projectName .. "/game/**.h",
      "../src/" .. projectName .. "/game/**.cpp",
      "../src/" .. projectName .. "/sandbox/**.h",
      "../src/" .. projectName .. "/sandbox/**.cpp",
    } )
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
      "ws2_32", -- 添加 Winsock 依赖
      "wsock32", -- 兼容旧版本 Winsock
    } )
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
