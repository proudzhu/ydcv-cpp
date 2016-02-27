set(BUILD_CPR_TESTS OFF CACHE BOOL "Don't build cpr tests")
set(USE_SYSTEM_CURL ON CACHE BOOL "Use the system curl for faster builds")

add_subdirectory(cpr)
