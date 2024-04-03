cmake_minimum_required (VERSION 3.29.0)


if (MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")
endif ()

project ("ebalpoco")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(Boost_USE_MULTITHREADED ON)

# YOUR TELEGRAM CREDENTIALS
# BEGIN

add_compile_definitions(BOT_TOKEN="PASTE_HERE_YOUR_BOT_TOKEN")
add_compile_definitions(BOT_OWNER_ID="PASTE_HERE_YOUR_TELEGRAM_ID")

# END
# YOUR TELEGRAM CREDENTIALS

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set(BOOST_ROOT C:/boost/boost)
set(Boost_INCLUDE_DIR C:/boost/boost)

set(ZLIB_LIBRARY C:/zlib)
set(ZLIB_INCLUDE_DIR C:/zlib)

include(cmake/FindBoost.cmake)
include(cmake/FindZLIB.cmake)
include(cmake/FindCURL.cmake)

find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)

set(OPENSSL_USE_STATIC_LIBS TRUE)

include_directories(${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})

file(GLOB_RECURSE PROJ_SRC src/*.cpp)

add_library (ebalpoco SHARED "src/main.cpp" "include/utils.h")
target_include_directories(ebalpoco PUBLIC include)

include(cmake/CPM.cmake)

CPMAddPackage("gh:reo7sp/tgbot-cpp#0910c16")

add_subdirectory(debug_loader)

target_link_libraries(ebalpoco PRIVATE TgBot wininet ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES})