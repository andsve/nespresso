solution "nespresso"
   configurations { "Debug", "Release" }
   location "build"
   targetdir "bin"
   debugdir "./runtime/"
   platforms { "x64" }


   -- nespresso_debugger
   -- ============
   project "nespresso"
      language "C++"
      files { "src/*.cpp" }
      includedirs { "include/", "thirdparty/minifb/include/" }

      links { "minifb_internal" }

      if os.get() == "windows" then

      elseif (os.get() == "macosx") then
        buildoptions { "-Wall",
                       "-Werror",
                       "-fcolor-diagnostics"
                     }
      end

      configuration "Debug"
         kind "ConsoleApp"
         defines { "DEBUG" }
         flags { "Symbols", "StaticRuntime" }

      configuration "Release"
         kind "WindowedApp"
         defines { "NDEBUG" }
         flags { "Optimize", "StaticRuntime" }
         -- flags { "OptimizeSize", "StaticRuntime" }

   -- minifb lib
   -- ===========
   project "minifb_internal"
      targetname ("minifb_internal")
      kind "StaticLib"
      language "C++"

      buildoptions_cpp {
          "-std=c++11",
      }

      includedirs { "thirdparty/minifb/include/", "thirdparty/minifb/src/" }

      files {
        "thirdparty/minifb/src/MiniFB_common.c",
        "thirdparty/minifb/src/MiniFB_cpp.cpp",
        "thirdparty/minifb/src/MiniFB_internal.c",
        "thirdparty/minifb/src/MiniFB_internal.h",
        "thirdparty/minifb/src/MiniFB_timer.c",
        "thirdparty/minifb/src/WindowData.h",
      }

      if os.get() == "windows" then
      elseif (os.get() == "macosx") then
        defines { "USE_METAL_API" }
        files {
          "thirdparty/minifb/src/macosx/MacMiniFB.m",
          "thirdparty/minifb/src/macosx/OSXWindow.m",
          "thirdparty/minifb/src/macosx/OSXView.m",
          "thirdparty/minifb/src/macosx/OSXViewDelegate.m",
          "thirdparty/minifb/src/macosx/WindowData_OSX.h",
        }

        includedirs { "thirdparty/minifb/macosx/" }

        links
        {
          "Cocoa.framework",
          "QuartzCore.framework",
          "Metal.framework",
          "MetalKit.framework",
        }
      end

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols", "StaticRuntime" }

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize", "StaticRuntime" }

