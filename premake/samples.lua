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
      "../src/External",
      "../src/External/lua/lua",
      "../src/External/lua/tolua",
      "../src/External/lua/luasocket",
      "../src/Engine/ogre3d/include/",
      "../src/Engine/ogre3d_direct3d9/include/",
      "../src/Engine/ogre3d_particlefx/include/",
      "../src/Engine/ogre3d_procedural/include/",
      "../src/Engine/ThirdParty/zzip/include/",
      "../src/External/bullet_collision/include/",
      "../src/External/bullet_dynamics/include/",
      "../src/External/bullet_linearmath/include/",
      "../src/External/ois/include/",
    } )

    files( {
      "../src/" .. projectName .. "/**.h",
      "../src/" .. projectName .. "/**.cpp",
      "../src/" .. projectName .. "/common/**.h",
      "../src/" .. projectName .. "/common/**.cpp",
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
      "../src/External/lua/lua",
      "../src/External/lua/luasocket",
    } )
    files( {
      "../src/External/lua/tolua/**.h",
      "../src/External/lua/tolua/**.c",
    } )
end
