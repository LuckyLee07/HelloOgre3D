local function is_vs_action()
	return _ACTION ~= nil and string.find(_ACTION, "vs") == 1
end

local function normalize_list(items)
	if type(items) == "string" then
		return { items }
	end
	if type(items) == "table" then
		return items
	end
	return {}
end

local function filter_msvc_options(items)
	if is_vs_action() then
		return items
	end
	local filtered = {}
	for _, item in ipairs(items) do
		if type(item) ~= "string" or not string.match(item, "^/") then
			table.insert(filtered, item)
		end
	end
	return filtered
end

local function filter_win_defines(items)
	if is_vs_action() then
		return items
	end
	local filtered = {}
	for _, item in ipairs(items) do
		if item ~= "WIN32" and item ~= "_CRT_SECURE_NO_WARNINGS" then
			table.insert(filtered, item)
		end
	end
	return filtered
end

local _buildoptions = buildoptions
buildoptions = function(items)
	local normalized = normalize_list(items)
	local filtered = filter_msvc_options(normalized)
	if #filtered > 0 then
		_buildoptions(filtered)
	end
end

local _linkoptions = linkoptions
linkoptions = function(items)
	local normalized = normalize_list(items)
	local filtered = filter_msvc_options(normalized)
	if #filtered > 0 then
		_linkoptions(filtered)
	end
end

local _defines = defines
defines = function(items)
	local normalized = normalize_list(items)
	local filtered = filter_win_defines(normalized)
	if #filtered > 0 then
		_defines(filtered)
	end
end

if configuration == nil then
	function configuration(terms)
		filter(terms)
	end
end

local macos_deployment_target = "15.6"

local _project = project
project = function(...)
	_project(...)
	filter "system:macosx"
		systemversion(macos_deployment_target)
		xcodebuildsettings {
			["MACOSX_DEPLOYMENT_TARGET"] = macos_deployment_target
		}
	filter {}
end

dofile("samples.lua");

solution( "HelloOgre3D" )
	location( "../build/" )
	configurations( { "Debug", "Release" } )
	platforms( { "x86", "x64" } )
-- configuration shared between all projects
	language( "C++" )
	--includedirs( { "../src/external/%{prj.name}/include/" } )
    warnings( "Extra" )
	multiprocessorcompile "On"
	implicitlink "Off"
	useimportlib "Off"
	incrementallink "Off"
	minimalrebuild "Off"
	editandcontinue "Off"
	staticruntime  "On"

	filter "system:windows"
		characterset ("MBCS")
		buildoptions({ "/wd\"4819\"" })
	filter { "system:not windows" }
		undefines { "WIN32", "_CRT_SECURE_NO_WARNINGS" }
	filter {}

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
	configuration( "x86" )
		vectorextensions( "SSE" )
		vectorextensions( "SSE2" )
-- build for x86-32bit machines
		filter "system:windows"
			linkoptions( "/MACHINE:X86" )
		filter {}
	configuration( "x64" )
-- build for x86-64bit machine
		filter "system:windows"
			linkoptions( "/MACHINE:X64" )
		filter {}
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
-- ogre3d v1.10.0 static library
	project( "ogre3d" )
		kind( "StaticLib" )
		location( "../build/Engine/ogre3d" )
		pchheader( "OgreStableHeaders.h" )
		pchsource( "../src/Engine/ogre3d/src/OgrePrecompiledHeaders.cpp" )
		filter "system:not windows"
			pchheader ""
			pchsource ""
		filter {}
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
			"../src/Engine/ogre3d/src/nedmalloc",
			"../src/Engine/ThirdParty/zlib/include/",
			"../src/Engine/ThirdParty/zzip/include/",
			"../src/Engine/ThirdParty/freeimage/include/",
			"../src/Engine/ThirdParty/freetype/include/"
		} )
		files( {
			"../src/Engine/ogre3d/include/**.h",
			"../src/Engine/ogre3d/src/**.mm",
			"../src/Engine/ogre3d/src/**.cpp",
			"../src/Engine/ogre3d/resources/**.rc",
			"../src/Engine/ogre3d/resources/**.ico",
			"../src/Engine/ogre3d/resources/**.bmp"
		} )
		removefiles {
			"../src/Engine/ogre3d/src/Threading/OgreDefaultWorkQueueTBB.cpp",
		}
		filter "system:not windows"
			buildoptions {
				"-I" .. path.getabsolute("../src/Engine/ogre3d/include"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d/src/nedmalloc"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/freeimage/include/"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/freetype/include/"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/zlib/include/"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/zzip/include/")
			}
		filter {}
		filter "system:macosx"
			removefiles {
				"../src/Engine/ogre3d/src/WIN32/**",
				"../src/Engine/ogre3d/src/Android/**",
				"../src/Engine/ogre3d/src/Emscripten/**",
				"../src/Engine/ogre3d/src/GLX/**",
				"../src/Engine/ogre3d/src/iOS/**",
				"../src/Engine/ogre3d/src/Threading/*Win.cpp",
				"../src/Engine/ogre3d/src/OgreConfigDialogNoOp.cpp",
				"../src/Engine/ogre3d/src/OgreErrorDialogNoOp.cpp"
			}
			excludes {
				"../src/Engine/ogre3d/src/WIN32/**",
				"../src/Engine/ogre3d/src/Android/**",
				"../src/Engine/ogre3d/src/Emscripten/**",
				"../src/Engine/ogre3d/src/GLX/**",
				"../src/Engine/ogre3d/src/iOS/**",
				"../src/Engine/ogre3d/src/Threading/*Win.cpp"
			}
		filter "system:linux"
			removefiles {
				"../src/Engine/ogre3d/src/WIN32/**",
				"../src/Engine/ogre3d/src/OSX/**"
			}
			excludes {
				"../src/Engine/ogre3d/src/WIN32/**",
				"../src/Engine/ogre3d/src/OSX/**"
			}
		filter "system:windows"
			removefiles {
				"../src/Engine/ogre3d/src/OSX/**",
				"../src/Engine/ogre3d/src/Android/**",
				"../src/Engine/ogre3d/src/Emscripten/**",
				"../src/Engine/ogre3d/src/GLX/**",
				"../src/Engine/ogre3d/src/iOS/**",
				"../src/Engine/ogre3d/src/WIN32/OgreMinGWSupport.cpp",
				"../src/Engine/ogre3d/src/Threading/Ogre*PThreads.cpp",
				"../src/Engine/ogre3d/src/OgreFileSystemLayerNoOp.cpp",
				"../src/Engine/ogre3d/src/OgrePOSIXTimer.cpp",
				"../src/Engine/ogre3d/src/OgreSearchOps.cpp",
				"../src/Engine/ogre3d/src/OgreConfigDialogNoOp.cpp",
				"../src/Engine/ogre3d/src/OgreErrorDialogNoOp.cpp"
			}
			excludes {
				"../src/Engine/ogre3d/src/OSX/**",
				"../src/Engine/ogre3d/src/Android/**",
				"../src/Engine/ogre3d/src/Emscripten/**",
				"../src/Engine/ogre3d/src/GLX/**",
				"../src/Engine/ogre3d/src/iOS/**",
				"../src/Engine/ogre3d/src/WIN32/OgreMinGWSupport.cpp",
				"../src/Engine/ogre3d/src/Threading/Ogre*PThreads.cpp",
				"../src/Engine/ogre3d/src/OgreFileSystemLayerNoOp.cpp",
				"../src/Engine/ogre3d/src/OgrePOSIXTimer.cpp",
				"../src/Engine/ogre3d/src/OgreSearchOps.cpp"
			}
		filter {}
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
	
-- ogre3d direct3d9 plugin v1.10.0 static library
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
		configuration( "*" )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- ogre3d OpenGL plugin v1.10.0git static library
	project( "ogre3d_opengl" )
		kind( "StaticLib" )
		location( "../build/Engine/ogre3d_opengl" )
		includedirs( {
			"../src/Engine/ogre3d/include/",
			"../src/Engine/ogre3d/include/Threading/",
			"../src/Engine/ogre3d_glsupport/include/",
			"../src/Engine/ogre3d_glsupport/include/GLSL/",
			"../src/Engine/ogre3d_opengl/include/",
			"../src/Engine/ogre3d_opengl/src/GLSL/include/",
			"../src/Engine/ogre3d_opengl/src/atifs/include/",
			"../src/Engine/ogre3d_opengl/src/StateCacheManager/",
			"../src/Engine/ogre3d_opengl/src/nvparse/"
		} )
		filter "system:not windows"
			buildoptions {
				"-I" .. path.getabsolute("../src/Engine/ogre3d/include/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d/include/Threading/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d_glsupport/include/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d_glsupport/include/GLSL/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d_opengl/include/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d_opengl/src/GLSL/include/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d_opengl/src/atifs/include/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d_opengl/src/StateCacheManager/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d_opengl/src/nvparse/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d_opengl/include/GL/")
			}
		filter {}
		files( {
			"../src/Engine/ogre3d_opengl/include/**.h",
			"../src/Engine/ogre3d_opengl/src/**.c",
			"../src/Engine/ogre3d_opengl/src/**.cpp",
		} )
		removefiles {
			"../src/Engine/ogre3d_opengl/src/nvparse/ps1.0__test_main.cpp",
		}

-- ogre3d GL3Plus plugin v1.10.0 static library
	project( "ogre3d_gl3plus" )
		kind( "StaticLib" )
		location( "../build/Engine/ogre3d_gl3plus" )
		includedirs( {
			"../src/Engine/ogre3d/include/",
			"../src/Engine/ogre3d/include/Threading/",
			"../src/Engine/ogre3d_glsupport/include/",
			"../src/Engine/ogre3d_glsupport/include/GLSL/",
			"../src/Engine/ogre3d_gl3plus/include/",
			"../src/Engine/ogre3d_gl3plus/include/GLSL/"
		} )
		filter "system:not windows"
			buildoptions {
				"-I" .. path.getabsolute("../src/Engine/ogre3d/include/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d/include/Threading/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d_glsupport/include/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d_glsupport/include/GLSL/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d_gl3plus/include/"),
				"-I" .. path.getabsolute("../src/Engine/ogre3d_gl3plus/include/GL")
			}
		filter {}
		files( {
			"../src/Engine/ogre3d_gl3plus/include/**.h",
			"../src/Engine/ogre3d_gl3plus/src/**.c",
			"../src/Engine/ogre3d_gl3plus/src/**.cpp",
		} )

-- ogre3d GLSupport v1.10.0 static library
	project( "ogre3d_glsupport" )
		kind( "StaticLib" )
		location( "../build/Engine/ogre3d_glsupport" )
		includedirs( {
			"../src/Engine/ogre3d/include/",
			"../src/Engine/ogre3d/include/OSX/",
			"../src/Engine/ogre3d_glsupport/include/",
			"../src/Engine/ogre3d_glsupport/include/GLSL/",
			"../src/Engine/ogre3d_glsupport/include/win32/"
		} )
		files( {
			"../src/Engine/ogre3d_glsupport/include/**.h",
			"../src/Engine/ogre3d_glsupport/src/**.mm",
			"../src/Engine/ogre3d_glsupport/src/**.cpp",
		} )
		filter "system:macosx"
			excludes {
				"../src/Engine/ogre3d_glsupport/src/win32/**",
				"../src/Engine/ogre3d_glsupport/src/GLX/**",
				"../src/Engine/ogre3d_glsupport/src/EGL/**",
				"../src/Engine/ogre3d_glsupport/src/OSX/OgreOSXRenderTexture.cpp",
			}
		filter "system:windows"
			excludes {
				"../src/Engine/ogre3d_glsupport/src/OSX/**",
				"../src/Engine/ogre3d_glsupport/src/GLX/**",
				"../src/Engine/ogre3d_glsupport/src/EGL/**",
			}
		filter "system:linux"
			excludes {
				"../src/Engine/ogre3d_glsupport/src/OSX/**",
				"../src/Engine/ogre3d_glsupport/src/win32/**",
				"../src/Engine/ogre3d_glsupport/src/EGL/**",
			}
		filter {}
     
-- ogre3d particlefx plugin v1.10.0 static library
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
		configuration( "*" )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- ogre3d procedural v0.2 static library
	project( "ogre3d_procedural" )
		kind( "StaticLib" )
		location( "../build/Engine/ogre3d_procedural" )
		pchheader( "ProceduralStableHeaders.h" )
		pchsource( "../src/Engine/ogre3d_procedural/src/ProceduralPrecompiledHeaders.cpp" )
		filter "system:not windows"
			pchheader ""
			pchsource ""
		filter {}
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
		filter "system:not windows"
			buildoptions {
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/freeimage/include/"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/libjpeg/include/"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/libopenjpeg/include/"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/libpng/include/"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/libraw/include/"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/libtiff4/include/"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/openexr/include"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/openexr/include/half"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/openexr/include/iex"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/openexr/include/ilmimf"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/openexr/include/imath"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/openexr/include/ilmthread"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/zlib/include/")
			}
		filter {}
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
		filter "system:not windows"
			buildoptions {
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/freetype/include/"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/zlib/include/")
			}
		filter {}
		-- Build only library sources; drop helper/test sources that break Xcode builds.
		files( {
			"../src/Engine/ThirdParty/freetype/include/**.h",
			"../src/Engine/ThirdParty/freetype/src/**.c"
		} )
		removefiles {
			"../src/Engine/ThirdParty/freetype/src/tools/**",
			"../src/Engine/ThirdParty/freetype/src/gzip/**",
			"../src/Engine/ThirdParty/freetype/src/ftgzip.c",
			"../src/Engine/ThirdParty/freetype/src/gxvalid/gxvfgen.c",
			"../src/Engine/ThirdParty/freetype/src/autofit/aflatin2.c"
		}
		filter "system:windows"
			removefiles {
				"../src/Engine/ThirdParty/freetype/src/base/ftmac.c"
			}
		filter {}

-- libjpeg v8d static library
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

-- libraw v0.14.7 static library
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
		filter "system:not windows"
			buildoptions { "-Wno-c++11-narrowing" }
		filter {}
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
		filter "system:not windows"
			excludes { "../src/Engine/ThirdParty/libtiff4/src/tif_win32.c" }
		filter {}
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- openexr v1.5.0 (ilmbase v0.9.0) static library
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
		filter "system:not windows"
			buildoptions {
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/openexr/include"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/openexr/include/half"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/openexr/include/iex"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/openexr/include/ilmimf"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/openexr/include/ilmthread"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/openexr/include/imath"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/zlib/include/")
			}
		filter {}
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
		filter "system:not windows"
			defines { "HAVE_UNISTD_H" }
		filter {}

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
		filter "system:not windows"
			buildoptions {
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/zlib/include/"),
				"-I" .. path.getabsolute("../src/Engine/ThirdParty/zzip/include/")
			}
		filter {}
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
		files( { "../src/external/lua/lua/**.h", "../src/external/lua/lua/**.c" } )
		defines( { "WIN32" } )

-- tolua v1.0.92 static library
	project( "tolua" )
		kind( "StaticLib" )
		location( "../build/External/lua/tolua" )
		buildoptions( {
			"/wd\"4131\"", "/wd\"4996\"", "/wd\"4244\"", "/wd\"4127\""
		} )
		includedirs( { "../src/external/lua/lua", "../src/external/lua/tolua"} )
		files( { "../src/external/lua/tolua/**.h", "../src/external/lua/tolua/**.c" } )

-- luasocket v3.0-rc1 static library
	project( "luasocket" )
		kind( "StaticLib" )
		location( "../build/External/lua/luasocket" )
		buildoptions( {
			"/wd\"4131\"", "/wd\"4996\"", "/wd\"4244\"", "/wd\"4127\""
		} )
		includedirs( { "../src/external/lua/lua", "../src/external/lua/luasocket"} )
		files( { "../src/external/lua/luasocket/**.h", 
			"../src/external/lua/luasocket/buffer.c",
			"../src/external/lua/luasocket/io.c",
			"../src/external/lua/luasocket/timeout.c",
			"../src/external/lua/luasocket/select.c",
			"../src/external/lua/luasocket/udp.c",
			"../src/external/lua/luasocket/mime.c",
			"../src/external/lua/luasocket/tcp.c",
			"../src/external/lua/luasocket/auxiliar.c",
			"../src/external/lua/luasocket/inet.c",
			"../src/external/lua/luasocket/luasocket.c",
			"../src/external/lua/luasocket/luasocket_scripts.c",
			"../src/external/lua/luasocket/except.c",
			"../src/external/lua/luasocket/options.c",
		} )

		filter "system:windows" --window平台
			files { "../src/external/lua/luasocket/wsocket.c" }
		filter "system:linux"   --linux平台
			files { "../src/external/lua/luasocket/usocket.c",
					"../src/external/lua/luasocket/serial.c"
			}
		filter "system:macosx"  --macOS平台
			files { "../src/external/lua/luasocket/usocket.c",
					"../src/external/lua/luasocket/unix.c"
			}
		filter {}

-- bullet collision v2.81 revision 2613
	project( "bullet_collision" )
		kind( "StaticLib" )
		location( "../build/External/bullet_collision" )
		buildoptions( {
			"/wd\"4127\"", "/wd\"4100\"", "/wd\"4244\"", "/wd\"4702\"",
			"/wd\"4512\"", "/wd\"4267\"", "/wd\"4456\""
		} )
		includedirs( {
			"../src/external/bullet_collision/include/",
			"../src/external/bullet_collision/include/BulletCollision/BroadphaseCollision",
			"../src/external/bullet_collision/include/BulletCollision/CollisionDispatch",
			"../src/external/bullet_collision/include/BulletCollision/CollisionShapes",
			"../src/external/bullet_collision/include/BulletCollision/Gimpact",
			"../src/external/bullet_collision/include/BulletCollision/NarrowPhaseCollision",
			"../src/external/bullet_linearmath/include"
		} )
		files( {
			"../src/external/bullet_collision/include/**.h",
			"../src/external/bullet_collision/src/**.cpp"
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
			"../src/external/bullet_collision/include/",
			"../src/external/bullet_dynamics/include/",
			"../src/external/bullet_dynamics/include/BulletDynamics/Character",
			"../src/external/bullet_dynamics/include/BulletDynamics/ConstraintSolver",
			"../src/external/bullet_dynamics/include/BulletDynamics/Dynamics",
			"../src/external/bullet_dynamics/include/BulletDynamics/Vehicle",
			"../src/external/bullet_linearmath/include"
		} )
		files( {
			"../src/external/bullet_dynamics/include/**.h",
			"../src/external/bullet_dynamics/src/**.cpp"
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
			"../src/external/bullet_linearmath/include/LinearMath"
		} )
		files( {
			"../src/external/bullet_linearmath/include/**.h",
			"../src/external/bullet_linearmath/src/**.cpp"
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- ois v1.5.0 static library
	project( "ois" )
		kind( "StaticLib" )
		location( "../build/External/ois" )
		buildoptions( {
			"/wd\"4512\"", "/wd\"4100\"", "/wd\"4189\""
		} )
		includedirs( {
			"../src/external/ois/includes/",
			"../src/external/ois/includes/win32/",
			"../src/external/ois/includes/mac/",
			"../src/external/ois/includes/linux/",
			"../src/external/ois/includes/iphone/"
		} )
		files( {
			"../src/external/ois/includes/**.h",
			"../src/external/ois/src/**.cpp",
			"../src/external/ois/src/**.mm"
		} )
		filter "system:windows"
			buildoptions( { "/I \"$(DXSDK_DIR)/Include/\"" } )
			includedirs { "../src/external/ois/includes/win32/" }
			excludes {
				"../src/external/ois/src/mac/**",
				"../src/external/ois/src/linux/**",
				"../src/external/ois/src/iphone/**",
				"../src/external/ois/src/SDL/**",
				"../src/external/ois/src/extras/**"
			}
		filter "system:macosx"
			includedirs { "../src/external/ois/includes/mac/" }
			excludes {
				"../src/external/ois/src/win32/**",
				"../src/external/ois/src/linux/**",
				"../src/external/ois/src/iphone/**",
				"../src/external/ois/src/SDL/**",
				"../src/external/ois/src/extras/**"
			}
			removefiles { "../src/external/ois/src/OISInputManager.cpp" }
		filter "system:linux"
			includedirs { "../src/external/ois/includes/linux/" }
			excludes {
				"../src/external/ois/src/win32/**",
				"../src/external/ois/src/mac/**",
				"../src/external/ois/src/iphone/**",
				"../src/external/ois/src/SDL/**"
			}
		filter {}

-- opensteer v0.8.2 static library
	project( "opensteer" )
		kind( "StaticLib" )
		location( "../build/External/opensteer" )
		buildoptions( { "/wd\"4701\"", "/wd\"4244\"", "/wd\"4100\"", "/wd\"4458\"" } )
		includedirs( {"../src/external/opensteer/include/" } )
		files( {
			"../src/external/opensteer/include/**.h",
			"../src/external/opensteer/src/**.c",
			"../src/external/opensteer/src/**.cpp"
		} )
		defines( { "WIN32", "HAVE_NO_GLUT" } )

-- ogre3d gorilla ui snapshot (2013-12-26) static library
	project( "ogre3d_gorilla" )
		kind( "StaticLib" )
		location( "../build/External/ogre3d_gorilla" )
		includedirs( { 
			"../src/Engine/ogre3d/include/",
			"../src/external/ogre3d_gorilla/include/"
		} )
		buildoptions( {
			"/Zm198"
		} )
		configuration( "*" )
		files( {
			"../src/external/ogre3d_gorilla/include/**.h",
			"../src/external/ogre3d_gorilla/src/**.cpp"
		} )
		defines( { "WIN32" } )

-- recast v1.4 static library
	project( "recast" )
		kind( "StaticLib" )
		location( "../build/External/recast" )
		includedirs( {"../src/external/recast/include/" } )
		files( { 
			"../src/external/recast/include/**.h", 
			"../src/external/recast/src/**.cpp"
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )

-- detour v1.4 static library
	project( "detour" )
		kind( "StaticLib" )
		location( "../build/External/detour" )
		includedirs( {"../src/external/detour/include/" } )
		files( { 
			"../src/external/detour/include/**.h", 
			"../src/external/detour/src/**.cpp"
		} )
		defines( { "WIN32", "_CRT_SECURE_NO_WARNINGS" } )
