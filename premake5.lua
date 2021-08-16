project "ghh"
	kind "SharedLib"
	language "C"
	cdialect "C99"

	enablewarnings { "all" }
	buildoptions { "-pedantic-errors" }

	targetdir "bin/%{prj.name}"
	objdir "obj/%{prj.name}"

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
