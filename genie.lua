solution "delauncher"
   configurations { "Debug", "Release" }
   location "build"
   targetdir "bin"
   debugdir "./runtime/"
   platforms { "x64" }


   -- App project
   -- ============
   project "delauncher"
      language "C++"
      files { "src/*.cpp" }

      includedirs { "third-party/inih/",
                    "third-party/libui/"
                  }

      links { "ui", "inih" }

      if os.get() == "windows" then
        links { "opengl32",
                "imm32",
               }

      elseif (os.get() == "macosx") then
        links { "Foundation.framework",
                 "AppKit.framework" }

        buildoptions { "-Wall",
                       "-Werror",
                       "-std=c++11"
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
   --]]

   -- libui
   project "ui"
      targetname ("ui")
      kind "StaticLib"
      language "C"
      files { "third-party/libui/common/attribute.c",
              "third-party/libui/common/attrlist.c",
              "third-party/libui/common/attrstr.c",
              "third-party/libui/common/areaevents.c",
              "third-party/libui/common/control.c",
              "third-party/libui/common/debug.c",
              "third-party/libui/common/matrix.c",
              "third-party/libui/common/opentype.c",
              "third-party/libui/common/shouldquit.c",
              "third-party/libui/common/tablemodel.c",
              "third-party/libui/common/tablevalue.c",
              "third-party/libui/common/userbugs.c",
              "third-party/libui/common/utf.c", }

      if os.get() == "windows" then
        -- defines { "WIN32", "WIN32_LEAN_AND_MEAN", "VC_EXTRALEAN" }
        -- files { "libs/Boxer/src/boxer_win.c" }
        -- links { "opengl32" }
      elseif os.get() == "macosx" then
        files { "third-party/libui/darwin/aat.m",
                "third-party/libui/darwin/alloc.m",
                "third-party/libui/darwin/area.m",
                "third-party/libui/darwin/areaevents.m",
                "third-party/libui/darwin/attrstr.m",
                "third-party/libui/darwin/autolayout.m",
                "third-party/libui/darwin/box.m",
                "third-party/libui/darwin/button.m",
                "third-party/libui/darwin/checkbox.m",
                "third-party/libui/darwin/colorbutton.m",
                "third-party/libui/darwin/combobox.m",
                "third-party/libui/darwin/control.m",
                "third-party/libui/darwin/datetimepicker.m",
                "third-party/libui/darwin/debug.m",
                "third-party/libui/darwin/draw.m",
                "third-party/libui/darwin/drawtext.m",
                "third-party/libui/darwin/editablecombo.m",
                "third-party/libui/darwin/entry.m",
                "third-party/libui/darwin/fontbutton.m",
                "third-party/libui/darwin/fontmatch.m",
                "third-party/libui/darwin/fonttraits.m",
                "third-party/libui/darwin/fontvariation.m",
                "third-party/libui/darwin/form.m",
                "third-party/libui/darwin/future.m",
                "third-party/libui/darwin/graphemes.m",
                "third-party/libui/darwin/grid.m",
                "third-party/libui/darwin/group.m",
                "third-party/libui/darwin/image.m",
                "third-party/libui/darwin/label.m",
                "third-party/libui/darwin/main.m",
                "third-party/libui/darwin/map.m",
                "third-party/libui/darwin/menu.m",
                "third-party/libui/darwin/multilineentry.m",
                "third-party/libui/darwin/opentype.m",
                "third-party/libui/darwin/progressbar.m",
                "third-party/libui/darwin/radiobuttons.m",
                "third-party/libui/darwin/scrollview.m",
                "third-party/libui/darwin/separator.m",
                "third-party/libui/darwin/slider.m",
                "third-party/libui/darwin/spinbox.m",
                "third-party/libui/darwin/stddialogs.m",
                "third-party/libui/darwin/tab.m",
                "third-party/libui/darwin/table.m",
                "third-party/libui/darwin/tablecolumn.m",
                "third-party/libui/darwin/text.m",
                "third-party/libui/darwin/undocumented.m",
                "third-party/libui/darwin/util.m",
                "third-party/libui/darwin/window.m",
                "third-party/libui/darwin/winmoveresize.m" }

        links { "Foundation.framework",
                 "AppKit.framework" }
      else
        -- files { "libs/Boxer/src/boxer_linux.c" }
      end

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols", "StaticRuntime" }

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize", "StaticRuntime" }

   -- inih - ini loader
   project "inih"
      targetname ("inih_internal")
      kind "StaticLib"
      language "C"
      files { "third-party/inih/ini.c" }

      configuration "Debug"
         defines { "DEBUG" }
         flags { "Symbols", "StaticRuntime" }

      configuration "Release"
         defines { "NDEBUG" }
         flags { "Optimize", "StaticRuntime" }
