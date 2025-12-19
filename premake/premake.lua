
dofile("samples.lua");

solution( "HelloOgre3D" )
	location( "../build/" )
	configurations( { "Debug", "Release" } )
	platforms( { "x32", "x64" } )
	characterset ("MBCS")
-- configuration shared between all projects
	language( "C++" )
	--includedirs( { "../src/External/%{prj.name}/include/" } )
    warnings( "Extra" )
	flags( {
		--"FatalWarnings",
		"MultiProcessorCompile",
		--"NoEditAndContinue",
		"NoImplicitLink",
		"NoImportLib",
		"NoIncrementalLink",
		"NoMinimalRebuild",
		--"StaticRuntime"
	} )
	editandcontinue "Off"
	staticruntime  "On"

	buildoptions({ "/wd\"4819\"" })

-- platform(windows/linux) specific configurations
	configuration( "windows" )
-- fatal linker warnings
--		linkoptions ( { "/WX-" } )
	configuration( "linux" )
	configuration( "*" )

-- solution configuration specific configurations
	configuration( "Debug" )
-- debug symbols
		--flags( { "Symbols" } )
		symbols "On"
	configuration( "Release" )
-- optimized build
		optimize( "Full" )
		--flags( { "Symbols" } )
		symbols "On"
		defines( { "NDEBUG" } )
	configuration( "*" )

-- platform configurations
	configuration( "x32" )
		vectorextensions( "SSE" )
		vectorextensions( "SSE2" )
-- build for x86-32bit machines
		linkoptions( "/MACHINE:X86" )
	configuration( "x64" )
-- build for x86-64bit machine
		linkoptions( "/MACHINE:X64" )
	configuration( "*" )

-- configurations for executables
	configuration( { "ConsoleApp or WindowedApp" } )
		targetdir( "../bin/" )
		libdirs{ "../libs/" }
	configuration( "*" )

-- configurations for static libraries
	configuration( { "StaticLib" } )
		targetdir( "../libs/" )
	configuration( "*")

--------------------------------------------------------------------------------
-- Game application definitions
--------------------------------------------------------------------------------
	--group( "GameDemos" )
	CreateGameProject( "HelloOgre3D" )
	CreateToluaProject( "tolua++" )

--------------------------------------------------------------------------------
-- Open source library definitions
--------------------------------------------------------------------------------
------------Engine------------
	group( "Engine" )
-- ogre3d v1.8.1 static library
	project( "ogre3d" )
		kind( "StaticLib" )
		location( "../build/Engine/ogre3d" )
		pchheader( "OgreStableHeaders.h" )
		pchsource( "../src/Engine/ogre3d/src/OgrePrecompiledHeaders.cpp" )
		buildoptions( {
			"/bigobj",
			"/wd\"4100\"", "/wd\"4127\"", "/wd\"4193\"", "/wd\"4244\"",
			"/wd\"4305\"", "/wd\"4512\"", "/wd\"4706\"", "/wd\"4702\"",
			"/wd\"4245\"", "/wd\"4503\"", "/wd\"4146\"", "/wd\"4565\"",
			"/wd\"4267\"", "/wd\"4996\"", "/wd\"4005\"", "/wd\"4345\"",
			"/Zm198"
		} )
		includedirs( {
			"../src/Engine/ogre3d/include",
			"../src/Engine/ogre3d/include/nedmalloc",
			"../src/Engine/ThirdParty/zlib/include/",
			"../src/Engine/ThirdParty/zzip/include/",
			"../src/Engine/ThirdParty/freeimage/include/",
			"../src/Engine/ThirdParty/freetype/include/"
		} )
		files( {
			"../src/Engine/ogre3d/include/**.h",
			"../src/Engine/ogre3d/src/**.cpp",
			"../src/Engine/ogre3d/resources/**.rc",
			"../src/Engine/ogre3d/resources/**.ico",
			"../src/Engine/ogre3d/resources/**.bmp"
		} )
		configuration( "**/Ogre*.cpp" )
		flags( "ExcludeFromBuild" )
		configuration( "**/OgrePrecompiledHeaders.cpp" )
		removeflags( "ExcludeFromBuild" )
		configuration( "*" )
		defines( {
			"WIN32",
			"_CRT_SECURE_NO_WARNINGS",
			"OGRE_NONCLIENT_BUILD",
			"FREEIMAGE_LIB"
		} )
		vpaths( {
			["resources/*"] = { "**.rc", "**.bmp", "**.ico" }
		} )
	
-- ogre3d direct3d9 plugin v1.8.1 static library
	project( "ogre3d_direct3d9" )
		kind( "StaticLib" )
		location( "../build/Engine/ogre3d_direct3d9" )
		includedirs( { 
			"../src/Engine/ogre3d/include/",
			"../src/Engine/ogre3d_direct3d9/include/"
		} )
		buildoptions( {
			"/wd\"4100\"", "/wd\"4189\"", "/wd\"4018\"", "/wd\"4193\"",
			"/wd\"4127\"", "/wd\"4389\"", "/wd\"4512\"", "/wd\"4701\"",
			"/wd\"4244\"", "/wd\"4702\"", "/wd\"4267\"", "/wd\"4703\"",
			"/Zm198"
		} )
		linkoptions ( { "/ignore:\"4221\"" } )
		configuration( "windows" )
		buildoptions( { "/I \"$(DXSDK_DIR)/Include/\"" } )
		configuration( "*" )
		files( {
			"../src/Engine/ogre3d_direct3d9/include/**.h",
			"../src/Engine/ogre3d_direct3d9/src/**.cpp"
		} )
		configuration( "**/Ogre*.cpp" )
		flags( "ExcludeFromBuild" )
		configuration( "*" )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )
     
-- ogre3d particlefx plugin v1.8.1 static library
	project( "ogre3d_particlefx" )
		kind( "StaticLib" )
		location( "../build/Engine/ogre3d_particlefx" )
		includedirs( { 
			"../src/Engine/ogre3d/include/",
			"../src/Engine/ogre3d_particlefx/include/"
		} )
		buildoptions( {
			"/wd\"4100\"", "/wd\"4189\"", "/wd\"4018\"", "/wd\"4193\"",
			"/wd\"4127\"", "/wd\"4389\"", "/wd\"4512\"", "/wd\"4701\"",
			"/wd\"4244\"", "/wd\"4702\"", "/wd\"4267\"", "/wd\"4703\"",
			"/Zm198"
		} )
		linkoptions ( { "/ignore:\"4221\"" } )
		configuration( "windows" )
			buildoptions( { "/I \"$(DXSDK_DIR)/Include/\"" } )
		configuration( "*" )
		files( {
			"../src/Engine/ogre3d_particlefx/include/**.h",
			"../src/Engine/ogre3d_particlefx/src/**.cpp"
		} )
		configuration( "**/Ogre*.cpp" )
			flags( "ExcludeFromBuild" )
		configuration( "*" )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- ogre3d procedural v0.2 static library
	project( "ogre3d_procedural" )
		kind( "StaticLib" )
		location( "../build/Engine/ogre3d_procedural" )
		pchheader( "ProceduralStableHeaders.h" )
		pchsource( "../src/Engine/ogre3d_procedural/src/ProceduralPrecompiledHeaders.cpp" )
		includedirs( { 
			"../src/Engine/ogre3d/include/", 
			"../src/Engine/ogre3d_procedural/include/" 
		} )
		buildoptions( {
			"/wd\"4100\"", "/wd\"4127\"", "/wd\"4244\"", "/wd\"4701\"",
			"/wd\"4267\"",
			"/Zm198"
		} )
		files( {
			"../src/Engine/ogre3d_procedural/include/**.h",
			"../src/Engine/ogre3d_procedural/src/**.cpp"
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

	group( "External" )

------------Engine/ThirdParty------------
	group( "Engine/ThirdParty" )
-- freeimage v3.15.4 static library
	project( "freeimage" )
		kind( "StaticLib" )
		location( "../build/Engine/ThirdParty/freeimage" )
		buildoptions( {
			"/wd\"4100\"", "/wd\"4127\"", "/wd\"4189\"", "/wd\"4244\"",
			"/wd\"4611\"", "/wd\"4389\"", "/wd\"4324\"", "/wd\"4702\"",
			"/wd\"4701\"", "/wd\"4789\"", "/wd\"4456\""
		} )
		includedirs( {
			"../src/Engine/ThirdParty/freeimage/include/",
			"../src/Engine/ThirdParty/libjpeg/include/",
			"../src/Engine/ThirdParty/libopenjpeg/include/",
			"../src/Engine/ThirdParty/libpng/include/",
			"../src/Engine/ThirdParty/libraw/include/",
			"../src/Engine/ThirdParty/libtiff4/include/",
			"../src/Engine/ThirdParty/openexr/include",
			"../src/Engine/ThirdParty/openexr/include/half",
			"../src/Engine/ThirdParty/openexr/include/iex",
			"../src/Engine/ThirdParty/openexr/include/ilmimf",
			"../src/Engine/ThirdParty/openexr/include/imath",
			"../src/Engine/ThirdParty/openexr/include/ilmthread",
			"../src/Engine/ThirdParty/zlib/include/"
		} )
		files( {
			"../src/Engine/ThirdParty/freeimage/include/**.h",
			"../src/Engine/ThirdParty/freeimage/src/**.cpp"
		} )
		defines( {
			"WIN32",
			"_CRT_SECURE_NO_WARNINGS",
			"FREEIMAGE_LIB",
			"OPJ_STATIC",
			"LIBRAW_NODLL"
		} )

-- freetype v2.4.12 static library
	project( "freetype" )
		kind( "StaticLib" )
		location( "../build/Engine/ThirdParty/freetype" )
		buildoptions( {
			"/wd\"4100\"", "/wd\"4244\"", "/wd\"4245\"", "/wd\"4701\"",
			"/wd\"4267\"", "/wd\"4324\"", "/wd\"4306\"", "/wd\"4703\""
		} )
		buildoptions( { "/FI \"ft2build.h\"" } )
		defines( { "FT2_BUILD_LIBRARY", "_CRT_SECURE_NO_WARNINGS" } )
		includedirs( { "../src/Engine/ThirdParty/freetype/include/" } )
        -- required to specify only the module level "c" files
		files( {
			"../src/Engine/ThirdParty/freetype/include/**.h",
			"../src/Engine/ThirdParty/freetype/src/**.c"
		} )
		configuration( {
			"../src/Engine/ThirdParty/freetype/src/**.c"
		} )
		flags( "ExcludeFromBuild" )
		configuration( {
			"**/autofit.c or " ..
			"**/bdf.c or " ..
			"**/cff.c or " ..
			"**/fgtlcdfil.c or " ..
			"**/ftbbox.c or " ..
			"**/ftbase.c or " ..
			"**/ftbitmap.c or " ..
			"**/ftcache.c or " ..
			"**/ftdebug.c or " ..
			"**/ftfstype.c or " ..
			"**/ftgasp.c or " ..
			"**/ftglyph.c or " ..
			"**/ftgxval.c or " ..
			"**/ftgzip.c or " ..
			"**/ftinit.c or " ..
			"**/ftlzw.c or " ..
			"**/ftmm.c or " ..
			"**/ftotval.c or " ..
			"**/ftpatent.c or " ..
			"**/ftpfr.c or " ..
			"**/ftstroke.c or " ..
			"**/ftsynth.c or " ..
			"**/ftsystem.c or " ..
			"**/fttype1.c or " ..
			"**/ftwinfnt.c or " ..
			"**/ftxf86.c or " ..
			"**/pcf.c or " ..
			"**/pfr.c or " ..
			"**/psaux.c or " ..
			"**/pshinter.c or " ..
			"**/psmodule.c or " ..
			"**/raster.c or " ..
			"**/sfnt.c or " ..
			"**/smooth.c or " ..
			"**/truetype.c or " ..
			"**/type1.c or " ..
			"**/type1cid.c or " ..
			"**/type42.c or " ..
			"**/winfnt.c"
		} )
		removeflags( "ExcludeFromBuild" )
		configuration( "*" )

-- libjpeg 8d static library
	project( "libjpeg" )
		kind( "StaticLib" )
		location( "../build/Engine/ThirdParty/libjpeg" )
		buildoptions( {
			"/wd\"4100\"", "/wd\"4244\"", "/wd\"4127\"", "/wd\"4267\""
		} )
		includedirs( { "../src/Engine/ThirdParty/libjpeg/include/" } )
		files( { 
			"../src/Engine/ThirdParty/libjpeg/include/**.h", 
			"../src/Engine/ThirdParty/libjpeg/src/**.c" 
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- libopenjpeg v1.5.1 static library
	project( "libopenjpeg" )
		kind( "StaticLib" )
		location( "../build/Engine/ThirdParty/libopenjpeg" )
		buildoptions( {
			"/wd\"4100\"", "/wd\"4244\"", "/wd\"4127\"", "/wd\"4267\"",
			"/wd\"4701\"", "/wd\"4706\""
		} )
		includedirs( { "../src/Engine/ThirdParty/libopenjpeg/include/" } )
		files( {
			"../src/Engine/ThirdParty/libopenjpeg/include/**.h",
			"../src/Engine/ThirdParty/libopenjpeg/src/**.c"
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS", "OPJ_STATIC" } )

-- libpng v1.5.13 static library
	project( "libpng" )
		kind( "StaticLib" )
		location( "../build/Engine/ThirdParty/libpng" )
		buildoptions( { "/wd\"4127\"" } )
		includedirs( { 
			"../src/Engine/ThirdParty/libpng/include/",
			"../src/Engine/ThirdParty/zlib/include/"
		 } )
		files( { 
			"../src/Engine/ThirdParty/libpng/include/**.h", 
			"../src/Engine/ThirdParty/libpng/src/**.c" 
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- libraw v1.5.13 static library
	project( "libraw" )
		kind( "StaticLib" )
		location( "../build/Engine/ThirdParty/libraw" )
		buildoptions( {
			"/wd\"4244\"", "/wd\"4189\"", "/wd\"4101\"", "/wd\"4706\"",
			"/wd\"4100\"", "/wd\"4018\"", "/wd\"4305\"", "/wd\"4309\"",
			"/wd\"4127\"", "/wd\"4389\"", "/wd\"4804\"", "/wd\"4146\"",
			"/wd\"4245\"", "/wd\"4996\"", "/wd\"4702\"", "/wd\"4267\"",
			"/wd\"4701\"", "/wd\"4456\""
		} )
		includedirs( { "../src/Engine/ThirdParty/libraw/include/" } )
		files( {
			"../src/Engine/ThirdParty/libraw/include/**.h",
			"../src/Engine/ThirdParty/libraw/src/**.c",
			"../src/Engine/ThirdParty/libraw/src/**.cpp"
		} )
		excludes( { "../src/Engine/ThirdParty/libraw/src/**dcb_demosaicing.c" } )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS", "LIBRAW_NODLL" } )

-- libtiff4 v4.0.3 static library
	project( "libtiff4" )
		kind( "StaticLib" )
		location( "../build/Engine/ThirdParty/libtiff4" )
		buildoptions( {
			"/wd\"4127\"", "/wd\"4244\"", "/wd\"4706\"", "/wd\"4702\"",
			"/wd\"4701\"", "/wd\"4018\"", "/wd\"4306\"", "/wd\"4305\"",
			"/wd\"4267\"", "/wd\"4324\"", "/wd\"4703\"", "/wd\"4100\"",
			"/wd\"4456\""
		} )
		includedirs( {
			"../src/Engine/ThirdParty/libtiff4/include/",
			"../src/Engine/ThirdParty/libjpeg/include/",
			"../src/Engine/ThirdParty/zlib/include/"
		} )
		files( { 
			"../src/Engine/ThirdParty/libtiff4/include/**.h", 
			"../src/Engine/ThirdParty/libtiff4/src/**.c" 
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- openexr v1.5.13 static library
	project( "openexr" )
		kind( "StaticLib" )
		location( "../build/Engine/ThirdParty/openexr" )
		buildoptions( {
			"/wd\"4244\"", "/wd\"4305\"", "/wd\"4100\"", "/wd\"4127\"",
			"/wd\"4245\"", "/wd\"4512\"", "/wd\"4706\"", "/wd\"4267\"",
			"/wd\"4702\"", "/wd\"4101\"", "/wd\"4800\"", "/wd\"4018\"",
			"/wd\"4701\"", "/wd\"4389\"", "/wd\"4334\"", "/wd\"4722\""
		} )
		linkoptions ( { "/ignore:\"4221\"" } )
		includedirs( {
			"../src/Engine/ThirdParty/openexr/include",
			"../src/Engine/ThirdParty/openexr/include/half",
			"../src/Engine/ThirdParty/openexr/include/iex",
			"../src/Engine/ThirdParty/openexr/include/ilmimf",
			"../src/Engine/ThirdParty/openexr/include/ilmthread",
			"../src/Engine/ThirdParty/openexr/include/imath",
			"../src/Engine/ThirdParty/zlib/include/"
		} )
		files( { 
			"../src/Engine/ThirdParty/openexr/include/**.h", 
			"../src/Engine/ThirdParty/openexr/src/**.cpp" 
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- zlib v1.2.8 static library
	project( "zlib" )
		kind( "StaticLib" )
		location( "../build/Engine/ThirdParty/zlib" )
		buildoptions( {
			"/wd\"4131\"", "/wd\"4996\"", "/wd\"4244\"", "/wd\"4127\""
		} )
		includedirs( { "../src/Engine/ThirdParty/zlib/include/" } )
		files( { 
			"../src/Engine/ThirdParty/zlib/include/**.h", 
			"../src/Engine/ThirdParty/zlib/src/**.c" 
		} )
		defines( { "WIN32" } )

-- zziplib v0.13.62 static library
	project( "zzip" )
		kind( "StaticLib" )
		location( "../build/Engine/ThirdParty/zzip" )
		buildoptions( {
			"/wd\"4127\"", "/wd\"4996\"", "/wd\"4706\"", "/wd\"4244\"",
			"/wd\"4267\"", "/wd\"4028\"", "/wd\"4305\""
		} )
		includedirs( { 
			"../src/Engine/ThirdParty/zlib/include/",
			"../src/Engine/ThirdParty/zzip/include/"
		} )
		files( { 
			"../src/Engine/ThirdParty/zzip/include/**.h", 
			"../src/Engine/ThirdParty/zzip/src/**.c" 
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )


------------External------------
	group( "External" )
-- lua v5.1.4 static library
	project( "lua" )
		kind( "StaticLib" )
		location( "../build/External/lua/lua" )
		buildoptions( {
			"/wd\"4131\"", "/wd\"4996\"", "/wd\"4244\"", "/wd\"4127\""
		} )
		files( { "../src/External/lua/lua/**.h", "../src/External/lua/lua/**.c" } )
		defines( { "WIN32" } )

-- tolua v1.0.92 static library
	project( "tolua" )
		kind( "StaticLib" )
		location( "../build/External/lua/tolua" )
		buildoptions( {
			"/wd\"4131\"", "/wd\"4996\"", "/wd\"4244\"", "/wd\"4127\""
		} )
		includedirs( { "../src/External/lua/lua", "../src/External/lua/tolua"} )
		files( { "../src/External/lua/tolua/**.h", "../src/External/lua/tolua/**.c" } )

-- luasocket v3.0-rc1 static library
	project( "luasocket" )
		kind( "StaticLib" )
		location( "../build/External/lua/luasocket" )
		buildoptions( {
			"/wd\"4131\"", "/wd\"4996\"", "/wd\"4244\"", "/wd\"4127\""
		} )
		includedirs( { "../src/External/lua/lua", "../src/External/lua/luasocket"} )
		files( { "../src/External/lua/luasocket/**.h", 
			"../src/External/lua/luasocket/buffer.c",
			"../src/External/lua/luasocket/io.c",
			"../src/External/lua/luasocket/timeout.c",
			"../src/External/lua/luasocket/select.c",
			"../src/External/lua/luasocket/udp.c",
			"../src/External/lua/luasocket/mime.c",
			"../src/External/lua/luasocket/tcp.c",
			"../src/External/lua/luasocket/auxiliar.c",
			"../src/External/lua/luasocket/inet.c",
			"../src/External/lua/luasocket/luasocket.c",
			"../src/External/lua/luasocket/luasocket_scripts.c",
			"../src/External/lua/luasocket/except.c",
			"../src/External/lua/luasocket/options.c",
		} )

		filter "system:windows" --window平台
			files { "../src/External/lua/luasocket/wsocket.c" }
		filter "system:linux"   --linux平台
			files { "../src/External/lua/luasocket/usocket.c",
					"../src/External/lua/luasocket/serial.c"
			}

-- bullet collision v2.81 revision 2613
	project( "bullet_collision" )
		kind( "StaticLib" )
		location( "../build/External/bullet_collision" )
		buildoptions( {
			"/wd\"4127\"", "/wd\"4100\"", "/wd\"4244\"", "/wd\"4702\"",
			"/wd\"4512\"", "/wd\"4267\"", "/wd\"4456\""
		} )
		includedirs( {
			"../src/External/bullet_collision/include/",
			"../src/External/bullet_collision/include/BulletCollision/BroadphaseCollision",
			"../src/External/bullet_collision/include/BulletCollision/CollisionDispatch",
			"../src/External/bullet_collision/include/BulletCollision/CollisionShapes",
			"../src/External/bullet_collision/include/BulletCollision/Gimpact",
			"../src/External/bullet_collision/include/BulletCollision/NarrowPhaseCollision",
			"../src/External/bullet_linearmath/include"
		} )
		files( {
			"../src/External/bullet_collision/include/**.h",
			"../src/External/bullet_collision/src/**.cpp"
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- bullet dynamics v2.81 revision 2613
	project( "bullet_dynamics" )
		kind( "StaticLib" )
		location( "../build/External/bullet_dynamics" )
		buildoptions( {
			"/wd\"4127\"", "/wd\"4100\"", "/wd\"4244\"", "/wd\"4702\"",
			"/wd\"4512\"", "/wd\"4267\"", "/wd\"4305\"", "/wd\"4456\""
		} )
		includedirs( {
			"../src/External/bullet_collision/include/",
			"../src/External/bullet_dynamics/include/",
			"../src/External/bullet_dynamics/include/BulletDynamics/Character",
			"../src/External/bullet_dynamics/include/BulletDynamics/ConstraintSolver",
			"../src/External/bullet_dynamics/include/BulletDynamics/Dynamics",
			"../src/External/bullet_dynamics/include/BulletDynamics/Vehicle",
			"../src/External/bullet_linearmath/include"
		} )
		files( {
			"../src/External/bullet_dynamics/include/**.h",
			"../src/External/bullet_dynamics/src/**.cpp"
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- bullet linearmath v2.81 revision 2613
	project( "bullet_linearmath" )
		kind( "StaticLib" )
		location( "../build/External/bullet_linearmath" )
		buildoptions( {
			"/wd\"4127\"", "/wd\"4245\"", "/wd\"4244\"", "/wd\"4267\"",
			"/wd\"4701\"", "/wd\"4456\""
		} )
		includedirs( {
			"../src/External/bullet_linearmath/include/LinearMath"
		} )
		files( {
			"../src/External/bullet_linearmath/include/**.h",
			"../src/External/bullet_linearmath/src/**.cpp"
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- ois v1.3 static library
	project( "ois" )
		kind( "StaticLib" )
		location( "../build/External/ois" )
		buildoptions( {
			"/wd\"4512\"", "/wd\"4100\"", "/wd\"4189\""
		} )
		configuration( "windows" )
			buildoptions( { "/I \"$(DXSDK_DIR)/Include/\"" } )
		configuration( "*" )
		includedirs( {"../src/External/ois/include/" } )
		files( { "../src/External/ois/include/**.h", "../src/External/ois/src/**.cpp" } )

-- opensteer revision 190 static library
	project( "opensteer" )
		kind( "StaticLib" )
		location( "../build/External/opensteer" )
		buildoptions( { "/wd\"4701\"", "/wd\"4244\"", "/wd\"4100\"", "/wd\"4458\"" } )
		includedirs( {"../src/External/opensteer/include/" } )
		files( {
			"../src/External/opensteer/include/**.h",
			"../src/External/opensteer/src/**.c",
			"../src/External/opensteer/src/**.cpp"
		} )
		defines( { "WIN32", "HAVE_NO_GLUT" } )

-- ogre3d gorilla ui "master" static library
	project( "ogre3d_gorilla" )
		kind( "StaticLib" )
		location( "../build/Engine/ogre3d_gorilla" )
		includedirs( { 
			"../src/Engine/ogre3d/include/",
			"../src/External/ogre3d_gorilla/include/"
		} )
		buildoptions( {
			"/Zm198"
		} )
		configuration( "*" )
		files( {
			"../src/External/ogre3d_gorilla/include/**.h",
			"../src/External/ogre3d_gorilla/src/**.cpp"
		} )
		defines( { "WIN32" } )