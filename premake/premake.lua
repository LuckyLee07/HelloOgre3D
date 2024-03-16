
dofile("sandboxgames.lua");

solution( "HelloOgre3D" )
	location( "../build/" )
	configurations( { "Debug", "Release" } )
	platforms( { "x32", "x64" } )
	characterset ("MBCS")
-- configuration shared between all projects
	language( "C++" )
	includedirs( { "../src/external/%{prj.name}/include/" } )
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
	group( "GameDemos" )
	CreateGameProject( "HelloOgre3D" )

--------------------------------------------------------------------------------
-- Open source library definitions
--------------------------------------------------------------------------------
	group( "Libraries" )
-- freeimage v3.15.4 static library
	project( "freeimage" )
		kind( "StaticLib" )
		location( "../build/external/freeimage" )
		buildoptions( {
			"/wd\"4100\"", "/wd\"4127\"", "/wd\"4189\"", "/wd\"4244\"",
			"/wd\"4611\"", "/wd\"4389\"", "/wd\"4324\"", "/wd\"4702\"",
			"/wd\"4701\"", "/wd\"4789\""
		} )
		includedirs( {
			"../src/external/libjpeg/include/",
			"../src/external/libopenjpeg/include/",
			"../src/external/libpng/include/",
			"../src/external/libraw/include/",
			"../src/external/libtiff4/include/",
			"../src/external/openexr/include",
			"../src/external/openexr/include/half",
			"../src/external/openexr/include/iex",
			"../src/external/openexr/include/ilmimf",
			"../src/external/openexr/include/imath",
			"../src/external/openexr/include/ilmthread",
			"../src/external/zlib/include/"
		} )
		files( {
			"../src/external/freeimage/include/**.h",
			"../src/external/freeimage/src/**.cpp"
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
		location( "../build/external/freetype" )
		buildoptions( {
			"/wd\"4100\"", "/wd\"4244\"", "/wd\"4245\"", "/wd\"4701\"",
			"/wd\"4267\"", "/wd\"4324\"", "/wd\"4306\"", "/wd\"4703\""
		} )
		buildoptions( { "/FI \"ft2build.h\"" } )
		defines( { "FT2_BUILD_LIBRARY", "_CRT_SECURE_NO_WARNINGS" } )
    -- required to specify only the module level "c" files
		files( {
			"../src/external/freetype/include/**.h",
			"../src/external/freetype/src/**.c"
		} )
		configuration( {
			"../src/external/freetype/src/**.c"
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
		location( "../build/external/libjpeg" )
		buildoptions( {
			"/wd\"4100\"", "/wd\"4244\"", "/wd\"4127\"", "/wd\"4267\""
		} )
		files( { "../src/external/libjpeg/include/**.h", "../src/external/libjpeg/src/**.c" } )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- libopenjpeg v1.5.1 static library
	project( "libopenjpeg" )
		kind( "StaticLib" )
		location( "../build/external/libopenjpeg" )
		buildoptions( {
			"/wd\"4100\"", "/wd\"4244\"", "/wd\"4127\"", "/wd\"4267\"",
			"/wd\"4701\"", "/wd\"4706\""
		} )
		files( {
			"../src/external/libopenjpeg/include/**.h",
			"../src/external/libopenjpeg/src/**.c"
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS", "OPJ_STATIC" } )

-- libpng v1.5.13 static library
	project( "libpng" )
		kind( "StaticLib" )
		location( "../build/external/libpng" )
		buildoptions( { "/wd\"4127\"" } )
		includedirs( { "../src/external//zlib/include/" } )
		files( { "../src/external/libpng/include/**.h", "../src/external/libpng/src/**.c" } )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- libraw v1.5.13 static library
	project( "libraw" )
		kind( "StaticLib" )
		location( "../build/external/libraw" )
		buildoptions( {
			"/wd\"4244\"", "/wd\"4189\"", "/wd\"4101\"", "/wd\"4706\"",
			"/wd\"4100\"", "/wd\"4018\"", "/wd\"4305\"", "/wd\"4309\"",
			"/wd\"4127\"", "/wd\"4389\"", "/wd\"4804\"", "/wd\"4146\"",
			"/wd\"4245\"", "/wd\"4996\"", "/wd\"4702\"", "/wd\"4267\"",
			"/wd\"4701\""
		} )
		files( {
			"../src/external/libraw/include/**.h",
			"../src/external/libraw/src/**.c",
			"../src/external/libraw/src/**.cpp"
		} )
		excludes( { "../src/external/libraw/src/**dcb_demosaicing.c" } )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS", "LIBRAW_NODLL" } )

-- libtiff4 v4.0.3 static library
	project( "libtiff4" )
		kind( "StaticLib" )
		location( "../build/external/libtiff4" )
		buildoptions( {
			"/wd\"4127\"", "/wd\"4244\"", "/wd\"4706\"", "/wd\"4702\"",
			"/wd\"4701\"", "/wd\"4018\"", "/wd\"4306\"", "/wd\"4305\"",
			"/wd\"4267\"", "/wd\"4324\"", "/wd\"4703\"", "/wd\"4100\""
		} )
		includedirs( {
			"../src/external/libjpeg/include/",
			"../src/external/zlib/include/"
		} )
		files( { "../src/external/libtiff4/include/**.h", "../src/external/libtiff4/src/**.c" } )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- ogre3d v1.8.1 static library
	project( "ogre3d" )
		kind( "StaticLib" )
		location( "../build/external/ogre3d" )
		pchheader( "OgreStableHeaders.h" )
		pchsource( "../src/external/ogre3d/src/OgrePrecompiledHeaders.cpp" )
		buildoptions( {
			"/bigobj",
			"/wd\"4100\"", "/wd\"4127\"", "/wd\"4193\"", "/wd\"4244\"",
			"/wd\"4305\"", "/wd\"4512\"", "/wd\"4706\"", "/wd\"4702\"",
			"/wd\"4245\"", "/wd\"4503\"", "/wd\"4146\"", "/wd\"4565\"",
			"/wd\"4267\"", "/wd\"4996\"", "/wd\"4005\"", "/wd\"4345\"",
			"/Zm198"
		} )
		includedirs( {
			"../src/external/freeimage/include/",
			"../src/external/freetype/include/",
			"../src/external/ogre3d/include/nedmalloc",
			"../src/external/zlib/include/",
			"../src/external/zzip/include/"
		} )
		files( {
			"../src/external/ogre3d/include/**.h",
			"../src/external/ogre3d/src/**.cpp",
			"../src/external/ogre3d/resources/**.rc",
			"../src/external/ogre3d/resources/**.ico",
			"../src/external/ogre3d/resources/**.bmp"
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
		location( "../build/external/ogre3d_direct3d9" )
		includedirs( { "../src/external/ogre3d/include/" } )
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
			"../src/external/ogre3d_direct3d9/include/**.h",
			"../src/external/ogre3d_direct3d9/src/**.cpp"
		} )
		configuration( "**/Ogre*.cpp" )
		flags( "ExcludeFromBuild" )
		configuration( "*" )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )
     
-- ogre3d particlefx plugin v1.8.1 static library
	project( "ogre3d_particlefx" )
		kind( "StaticLib" )
		location( "../build/external/ogre3d_particlefx" )
		includedirs( { "../src/external/ogre3d/include/" } )
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
			"../src/external/ogre3d_particlefx/include/**.h",
			"../src/external/ogre3d_particlefx/src/**.cpp"
		} )
		configuration( "**/Ogre*.cpp" )
			flags( "ExcludeFromBuild" )
		configuration( "*" )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- ogre3d procedural v0.2 static library
	project( "ogre3d_procedural" )
		kind( "StaticLib" )
		location( "../build/external/ogre3d_procedural" )
		pchheader( "ProceduralStableHeaders.h" )
		pchsource( "../src/external/ogre3d_procedural/src/ProceduralPrecompiledHeaders.cpp" )
		includedirs( { "../src/external/ogre3d/include/" } )
		buildoptions( {
			"/wd\"4100\"", "/wd\"4127\"", "/wd\"4244\"", "/wd\"4701\"",
			"/wd\"4267\"",
			"/Zm198"
		} )
		files( {
			"../src/external/ogre3d_procedural/include/**.h",
			"../src/external/ogre3d_procedural/src/**.cpp"
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- openexr v1.5.13 static library
	project( "openexr" )
		kind( "StaticLib" )
		location( "../build/external/openexr" )
		buildoptions( {
			"/wd\"4244\"", "/wd\"4305\"", "/wd\"4100\"", "/wd\"4127\"",
			"/wd\"4245\"", "/wd\"4512\"", "/wd\"4706\"", "/wd\"4267\"",
			"/wd\"4702\"", "/wd\"4101\"", "/wd\"4800\"", "/wd\"4018\"",
			"/wd\"4701\"", "/wd\"4389\"", "/wd\"4334\"", "/wd\"4722\""
		} )
		linkoptions ( { "/ignore:\"4221\"" } )
		includedirs( {
			"../src/external/openexr/include/half",
			"../src/external/openexr/include/iex",
			"../src/external/openexr/include/ilmimf",
			"../src/external/openexr/include/ilmthread",
			"../src/external/openexr/include/imath",
			"../src/external/zlib/include/"
		} )
		files( { "../src/external/openexr/include/**.h", "../src/external/openexr/src/**.cpp" } )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- zlib v1.2.8 static library
	project( "zlib" )
		kind( "StaticLib" )
		location( "../build/external/zlib" )
		buildoptions( {
			"/wd\"4131\"", "/wd\"4996\"", "/wd\"4244\"", "/wd\"4127\""
		} )
		files( { "../src/external/zlib/include/**.h", "../src/external/zlib/src/**.c" } )
		defines( { "WIN32" } )

-- zziplib v0.13.62 static library
	project( "zzip" )
		kind( "StaticLib" )
		location( "../build/external/zzip" )
		buildoptions( {
			"/wd\"4127\"", "/wd\"4996\"", "/wd\"4706\"", "/wd\"4244\"",
			"/wd\"4267\"", "/wd\"4028\"", "/wd\"4305\""
		} )
		includedirs( { "../src/external/zlib/include/" } )
		files( { "../src/external/zzip/include/**.h", "../src/external/zzip/src/**.c" } )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )
