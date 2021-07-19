project "ghh"
	kind "StaticLib"
	language "C"
	cdialect "c99"
	buildoptions { "-pedantic" }

	targetdir ("bin/%{prj.name}")
	objdir ("obj/%{prj.name}")

	files {
		"./include/**.h",
		"./src/**.c"
	}

	includedirs {
		"./include/"
	}

	if os.target() == "windows" then
		makesettings { "CC=gcc" }
		links { "mingw32" }
	end

	filter "configurations:debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:release"
		defines { "NDEBUG" }
		optimize "On"
