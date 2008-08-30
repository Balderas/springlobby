# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Windows)

# which compilers to use for C and C++
SET(CMAKE_C_COMPILER i586-mingw32msvc-gcc)
SET(CMAKE_CXX_COMPILER i586-mingw32msvc-g++)
SET( wxWidgets_ROOT_DIR /var/lib/buildbot/lib/mingw/wx/2.8/ )
# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /usr/bin/i586-mingw32msvc /var/lib/buildbot/lib/mingw /var/lib/buildbot/lib/mingw/wx/2.8/include/wx-2.8)

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search 
# programs in the host environment
# SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
SET( wxWidgets_CONFIG_EXECUTABLE /var/lib/buildbot/lib/mingw/wx/2.8/bin/wx-config )
SET( wxWidgets_INCLUDE_DIRS /var/lib/buildbot/lib/mingw/wx/2.8/include/wx-2.8 /var/lib/buildbot/lib/mingw/wx/2.8/lib/wx/include/i586-mingw32msvc-msw-unicode-release-2.8/)
SET( wxWidgets_LIB_DIR /var/lib/buildbot/lib/mingw/wx/2.8/lib )
SET( wxWidgets_CONFIGURATION mswu )
SET( CMAKE_VERBOSE_MAKEFILE ON )
SET( boost_LIB_DIR /var/lib/buildbot/lib/mingw/lib )
SET( boost_INCLUDE_DIR /var/lib/buildbot/lib/mingw/include )
SET( sdl_INCLUDE_DIR /var/lib/buildbot/lib/mingw/include/SDL )



ADD_DEFINITIONS( -DWXUSINGDLL -D__WXMSW__)

