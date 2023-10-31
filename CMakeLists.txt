# CMake 최소 버전 명시.
cmake_minimum_required(VERSION 3.26.4)

# 프로젝트 이름 및 버전 명시
set(PROJECT_NAME scop)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 환경 변수 세팅
set(WINDOW_NAME "scop")
set(WINDOW_WIDTH 960)
set(WINDOW_HEIGHT 640)

# Mac에서 돌릴때 필요한 추가적인 Flag들.
if (APPLE)
    set(CMAKE_CXX_FLAGS "-framework Cocoa -framework IOKit -framework OpenGL")
endif()

set(CMAKE_WARN_DEPRECATED OFF CACHE BOOL "" FORCE) # close DEPRECATION WARNING...


# 프로젝트 이름 설정
project(${PROJECT_NAME})
# 실행파일의 이름 설정
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/common.cpp src/common.h
    src/shader.cpp src/shader.h
    src/program.cpp src/program.h
    src/context.cpp src/context.h
    src/buffer.cpp src/buffer.h
    src/vertex_layout.cpp src/vertex_layout.h
    src/image.cpp src/image.h
    src/texture.cpp src/texture.h 
    src/parse.cpp src/parse.h 
    src/glm/glm.cpp src/glm/glm.hpp
    src/glm/mat2x2.cpp src/glm/mat2x2.hpp
    src/glm/mat3x3.cpp src/glm/mat3x3.hpp
    src/glm/mat4x4.cpp src/glm/mat4x4.hpp
    src/glm/type_ptr.cpp src/glm/type_ptr.hpp
    src/glm/vec2.cpp src/glm/vec2.hpp
    src/glm/vec3.cpp src/glm/vec3.hpp
    src/glm/vec4.cpp src/glm/vec4.hpp
    src/glm/compute_vector_relational.hpp
) # 헤더파일을 같이 넣는 이유는 헤더파일이 수정되더라도 다시 컴파일 되도록하기 위함이다.

# ExternalProject 관련 명령어 셋 추가
include(Dependency.cmake)

# 우리 프로젝트에 include / lib 관련 옵션 추가
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
# ${DEP_INCLUDE_DIR} path가 ${PROJECT_NAME}를 컴파일할 때 반드시 필요하다!
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
# ${DEP_LIB_DIR} path가 ${PROJECT_NAME} 를 링크할 때 반드시 필요하다!
# link 과정은 그 라이브러리 안에 들어있는 바이너리 코드들, 컴파일된 코드 뭉치를 가져다가 우리 실행파일에 카피해서 옮기는 것으로 생각하면 된다.
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})
# 실제로 어떤 라이브러리를 사용할지를 결정한다.

# define 매크로로 지정한 값을 실제 컴파일 과정에서 지정
target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
) # define 매크로를 이용해 지정한 것과 똑같은 효과를 나타낸다. #define WINDOW_NAME ${WINDOW_WIDTH} 과 같다.

# Dependency(${DEP_LIST})들이 먼저 build 될 수 있게 관계 설정
add_dependencies(${PROJECT_NAME} ${DEP_LIST})