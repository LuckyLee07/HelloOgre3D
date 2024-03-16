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
      "zzip"
    } )
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
      libdirs( {
        "\"../build/external/ogre3d/obj/x32/Debug\"",
        "\"$(DXSDK_DIR)/Lib/x86\"",
      } )
    configuration( { "windows", "x32", "Release" } )
      libdirs( {
        "\"../build/external/ogre3d/obj/x32/Release\"",
        "\"$(DXSDK_DIR)/Lib/x86\"",
      } )
    configuration( { "windows", "x64", "Debug" } )
      libdirs( {
        "\"../build/external/ogre3d/obj/x64/Debug\"",
        "\"$(DXSDK_DIR)/Lib/x64\""
      } )
    configuration( { "windows", "x64", "Release" } )
      libdirs( {
        "\"../build/external/ogre3d/obj/x64/Release\"",
        "\"$(DXSDK_DIR)/Lib/x64\""
      } )
    configuration( "*" )
    includedirs( {
      "../src/%{prj.name}/",
      "../src/%{prj.name}/game",
      "../src/%{prj.name}/sandbox",     
      "../src/external/zzip/include/",
      "../src/external/ogre3d/include/",
      "../src/external/ogre3d_direct3d9/include/",
      "../src/external/ogre3d_particlefx/include/",
      "../src/external/ogre3d_procedural/include/",
    } )
    -- TODO(David Young 6-8-13): the current premake-dev doesn't support
    -- %{prj.name} within "files"
    files( {
      "../src/" .. projectName .. "/**.h",
      "../src/" .. projectName .. "/**.cpp",
      "../src/" .. projectName .. "/game/**.h",
      "../src/" .. projectName .. "/game/**.cpp",
      "../src/" .. projectName .. "/sandbox/**.h",
      "../src/" .. projectName .. "/sandbox/**.cpp",
    } )
end

SandboxGames = {
  "HelloOgre3D",
};
