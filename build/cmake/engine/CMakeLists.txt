# Create project
project (Engine)

set (Engine_ROOT ${Violet_SRC}/violet)

# define header and source files for the library
set (HEADER_FILES
    ${Engine_ROOT}/Defines.h
    ${Engine_ROOT}/Engine.h
    ${Engine_ROOT}/component/Component.h
    ${Engine_ROOT}/component/ComponentDeserializer.h
    ${Engine_ROOT}/component/ComponentManager.h
    ${Engine_ROOT}/component/ComponentPool.h
    ${Engine_ROOT}/component/MappedDataComponent.h
    ${Engine_ROOT}/event/Event.h
    ${Engine_ROOT}/event/EventContext.h
    ${Engine_ROOT}/event/EventContextOwner.h
    ${Engine_ROOT}/graphics/Color.h
    ${Engine_ROOT}/graphics/component/ColorComponent.h
    ${Engine_ROOT}/graphics/component/RenderComponentData.h
    ${Engine_ROOT}/graphics/component/TextComponent.h
    ${Engine_ROOT}/graphics/component/TextureComponent.h
    ${Engine_ROOT}/graphics/font/Font.h
    ${Engine_ROOT}/graphics/Mesh.h
    ${Engine_ROOT}/graphics/shader/Shader.h
    ${Engine_ROOT}/graphics/system/RenderSystem.h
    ${Engine_ROOT}/graphics/texture/Texture.h
    ${Engine_ROOT}/handle/Handle.h
    ${Engine_ROOT}/handle/HandleManager.h
    ${Engine_ROOT}/handle/VersionedHandleManager.h
    ${Engine_ROOT}/input/InputResult.h
    ${Engine_ROOT}/input/Key.h
    ${Engine_ROOT}/input/component/KeyInputComponent.h
    ${Engine_ROOT}/input/component/MouseInputComponent.h
    ${Engine_ROOT}/input/system/InputSystem.h
    ${Engine_ROOT}/log/ConsoleLogTarget.h
    ${Engine_ROOT}/log/ConsoleLogTarget.h
    ${Engine_ROOT}/log/FileLogTarget.h
    ${Engine_ROOT}/log/Log.h
    ${Engine_ROOT}/log/LogTarget.h
    ${Engine_ROOT}/math/AABB.h
    ${Engine_ROOT}/math/Circle.h
    ${Engine_ROOT}/math/Constants.h
    ${Engine_ROOT}/math/Interval.h
    ${Engine_ROOT}/math/LogConst.h
    ${Engine_ROOT}/math/Matrix2.h
    ${Engine_ROOT}/math/Matrix3.h
    ${Engine_ROOT}/math/Matrix4.h
    ${Engine_ROOT}/math/Polygon.h
    ${Engine_ROOT}/math/Vec2.h
    ${Engine_ROOT}/math/Vec3.h
    ${Engine_ROOT}/physics/collision/Intersection.h
    ${Engine_ROOT}/physics/collision/RigidBody.h
    ${Engine_ROOT}/physics/component/PhysicsComponent.h
    ${Engine_ROOT}/physics/system/PhysicsSystem.h
    ${Engine_ROOT}/script/Script.h
    ${Engine_ROOT}/script/ScriptComponent.h
    ${Engine_ROOT}/script/ScriptFactory.h
    ${Engine_ROOT}/script/ScriptUtilities.h
    ${Engine_ROOT}/script/cpp/CppScript.h
    ${Engine_ROOT}/script/cpp/SharedLibrary.h
    ${Engine_ROOT}/script/lua/LuaLibrary.h
    ${Engine_ROOT}/script/lua/LuaScript.h
    ${Engine_ROOT}/serialization/binary/BinaryDeserializer.h
    ${Engine_ROOT}/serialization/binary/BinarySerializer.h
    ${Engine_ROOT}/serialization/Deserializer.h
    ${Engine_ROOT}/serialization/file/FileDeserializerFactory.h
    ${Engine_ROOT}/serialization/file/FileDeserializer.h
    ${Engine_ROOT}/serialization/file/FileSerializerFactory.h
    ${Engine_ROOT}/serialization/file/FileSerializer.h
    ${Engine_ROOT}/serialization/json/JsonDeserializer.h
    ${Engine_ROOT}/serialization/json/JsonSerializer.h
    ${Engine_ROOT}/serialization/SerializationUtilities.h
    ${Engine_ROOT}/serialization/Serializer.h
    ${Engine_ROOT}/structures/CrsGraph.h
    ${Engine_ROOT}/structures/QuadTree.h
    ${Engine_ROOT}/structures/Vector.h
    ${Engine_ROOT}/system/SystemFactory.h
    ${Engine_ROOT}/system/System.h
    ${Engine_ROOT}/task/Task.h
    ${Engine_ROOT}/task/TaskScheduler.h
    ${Engine_ROOT}/task/Thread.h
    ${Engine_ROOT}/template/TemplateUtilities.h
    ${Engine_ROOT}/template/TupleUtilities.h
    ${Engine_ROOT}/time/Time.h
    ${Engine_ROOT}/transform/Transform.h
    ${Engine_ROOT}/transform/component/LocalTransformComponent.h
    ${Engine_ROOT}/transform/component/WorldTransformComponent.h
    ${Engine_ROOT}/transform/system/TransformSystem.h
    ${Engine_ROOT}/ui/UiTextInputBox.h
    ${Engine_ROOT}/update/component/UpdateComponent.h
    ${Engine_ROOT}/update/system/UpdateSystem.h
    ${Engine_ROOT}/utility/Buffer.h
    ${Engine_ROOT}/utility/Factory.h
    ${Engine_ROOT}/utility/FileUtilities.h
    ${Engine_ROOT}/utility/FormattedString.h
    ${Engine_ROOT}/utility/FreeList.h
    ${Engine_ROOT}/utility/Guard.h
    ${Engine_ROOT}/utility/lent_ptr.h
    ${Engine_ROOT}/utility/Profiler.h
    ${Engine_ROOT}/utility/Random.h
    ${Engine_ROOT}/utility/ResourceCache.h
    ${Engine_ROOT}/utility/shared_val.h
    ${Engine_ROOT}/utility/StringUtilities.h
    ${Engine_ROOT}/utility/Tag.h
    ${Engine_ROOT}/utility/TextResource.h
    ${Engine_ROOT}/utility/unique_val.h
    ${Engine_ROOT}/window/MouseButton.h
    ${Engine_ROOT}/window/WindowSystem.h
    ${Engine_ROOT}/window/glut/GlutWindowSystem.h
    ${Engine_ROOT}/window/sdl/SDLWindowSystem.h
)

set (SOURCE_FILES
    ${Engine_ROOT}/component/Component.cpp
    ${Engine_ROOT}/component/ComponentDeserializer.cpp
    ${Engine_ROOT}/component/ComponentPool.cpp
    ${Engine_ROOT}/component/ComponentManager.cpp
    ${Engine_ROOT}/component/MappedDataComponent.cpp
    ${Engine_ROOT}/Engine.cpp
    ${Engine_ROOT}/event/Event.cpp
    ${Engine_ROOT}/event/EventContext.cpp
    ${Engine_ROOT}/event/EventContextOwner.cpp
    ${Engine_ROOT}/graphics/Color.cpp
    ${Engine_ROOT}/graphics/component/ColorComponent.cpp
    ${Engine_ROOT}/graphics/component/RenderComponentData.cpp
    ${Engine_ROOT}/graphics/component/TextComponent.cpp
    ${Engine_ROOT}/graphics/component/TextureComponent.cpp
    ${Engine_ROOT}/graphics/font/Font.cpp
    ${Engine_ROOT}/graphics/Mesh.cpp
    ${Engine_ROOT}/graphics/shader/Shader.cpp
    ${Engine_ROOT}/graphics/system/RenderSystem.cpp
    ${Engine_ROOT}/graphics/texture/Texture.cpp
    ${Engine_ROOT}/input/Key.cpp
    ${Engine_ROOT}/input/component/KeyInputComponent.cpp
    ${Engine_ROOT}/input/component/MouseInputComponent.cpp
    ${Engine_ROOT}/input/system/InputSystem.cpp
    ${Engine_ROOT}/log/ConsoleLogTarget.cpp
    ${Engine_ROOT}/log/FileLogTarget.cpp
    ${Engine_ROOT}/log/Log.cpp
    ${Engine_ROOT}/log/LogTarget.cpp
    ${Engine_ROOT}/math/AABB.cpp
    ${Engine_ROOT}/math/Circle.cpp
    ${Engine_ROOT}/math/Matrix3.cpp
    ${Engine_ROOT}/math/Matrix4.cpp
    ${Engine_ROOT}/math/Polygon.cpp
    ${Engine_ROOT}/math/Vec2.cpp
    ${Engine_ROOT}/physics/collision/Intersection.cpp
    ${Engine_ROOT}/physics/collision/RigidBody.cpp
    ${Engine_ROOT}/physics/component/PhysicsComponent.cpp
    ${Engine_ROOT}/physics/system/PhysicsSystem.cpp
    ${Engine_ROOT}/script/Script.cpp
    ${Engine_ROOT}/script/ScriptComponent.cpp
    ${Engine_ROOT}/script/ScriptFactory.cpp
    ${Engine_ROOT}/script/cpp/CppScript.cpp
    ${Engine_ROOT}/script/cpp/UnixSharedLibrary.cpp
    ${Engine_ROOT}/script/lua/LuaLibrary.cpp
    ${Engine_ROOT}/script/lua/LuaScript.cpp
    ${Engine_ROOT}/serialization/binary/BinaryDeserializer.cpp
    ${Engine_ROOT}/serialization/binary/BinarySerializer.cpp
    ${Engine_ROOT}/serialization/file/FileDeserializerFactory.cpp
    ${Engine_ROOT}/serialization/file/FileSerializerFactory.cpp
    ${Engine_ROOT}/serialization/json/JsonDeserializer.cpp
    ${Engine_ROOT}/serialization/json/JsonSerializer.cpp
    ${Engine_ROOT}/system/System.cpp
    ${Engine_ROOT}/task/Task.cpp
    ${Engine_ROOT}/task/TaskScheduler.cpp
    ${Engine_ROOT}/time/Time.cpp
    ${Engine_ROOT}/transform/Transform.cpp
    ${Engine_ROOT}/transform/component/LocalTransformComponent.cpp
    ${Engine_ROOT}/transform/component/WorldTransformComponent.cpp
    ${Engine_ROOT}/transform/system/TransformSystem.cpp
    ${Engine_ROOT}/ui/UiTextInputBox.cpp
    ${Engine_ROOT}/update/component/UpdateComponent.cpp
    ${Engine_ROOT}/update/system/UpdateSystem.cpp
    ${Engine_ROOT}/utility/FileUtilities.cpp
    ${Engine_ROOT}/utility/FreeList.cpp
    ${Engine_ROOT}/utility/Profiler.cpp
    ${Engine_ROOT}/utility/Random.cpp
    ${Engine_ROOT}/utility/StringUtilities.cpp
    ${Engine_ROOT}/utility/Tag.cpp
    ${Engine_ROOT}/window/glut/GlutWindowSystem.cpp
    ${Engine_ROOT}/window/sdl/SDLWindowSystem.cpp
)

include_directories (${Violet_SRC})

set (PNG_FIND_VERSION "1.6.18")
set (POSSIBLE_LIBRARIES OpenGL GLEW Freetype PNG Lua JsonCpp SDL2 GLUT)
# Setup for library find
# set (POSSIBLE_LIBRARIES FFmpeg)
# set (FFmpeg_COMMAND FFmpeg REQUIRED COMPONENTS avcodec avutil swscale)
# set (FFmpeg_ADDITIONAL_DEFINITIONS -D__STDC_CONSTANT_MACROS)

gather_libraries ()
add_library (Engine SHARED ${HEADER_FILES} ${SOURCE_FILES})
target_link_libraries (Engine ${LIBRARIES})

