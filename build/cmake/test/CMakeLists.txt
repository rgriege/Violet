# Create project
project (Test)

set (Test_ROOT ${Violet_SRC}/test)

# define header and source files for the library
set (HEADER_FILES
    ${Test_ROOT}/core/StatefulTest.h
    ${Test_ROOT}/core/StatelessTestSuite.h
    ${Test_ROOT}/core/StatefulTestSuite.h
    ${Test_ROOT}/core/TestEvaluator.h
    ${Test_ROOT}/core/TestFactory.h
    ${Test_ROOT}/core/StatelessTest.h
    ${Test_ROOT}/game/PathfindingTest.h
    ${Test_ROOT}/lua/LuaTest.h
    ${Test_ROOT}/math/MathTest.h
    ${Test_ROOT}/physics/PhysicsTest.h
    ${Test_ROOT}/serialization/JsonSerializationTest.h
    ${Test_ROOT}/task/TaskTest.h
    ${Test_ROOT}/template/TemplateTest.h
)

set (SOURCE_FILES
    ${Test_ROOT}/main.cpp
    ${Test_ROOT}/core/TestEvaluator.cpp
    ${Test_ROOT}/game/PathfindingTest.cpp
    ${Test_ROOT}/lua/LuaTest.cpp
    ${Test_ROOT}/math/MathTest.cpp
    ${Test_ROOT}/physics/PhysicsTest.cpp
    ${Test_ROOT}/serialization/JsonSerializationTest.cpp
    ${Test_ROOT}/task/TaskTest.cpp
    ${Test_ROOT}/template/TemplateTest.cpp
)

include_directories (${Violet_SRC})

# Setup for library find
set (POSSIBLE_LIBRARIES Lua JsonCpp Engine Game)

gather_libraries ()
add_executable (Test ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (Test ${LIBRARIES})


