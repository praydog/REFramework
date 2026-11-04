# A separate assembly imports the normal bridge and compiles the shared native
# ObjectAddress implementation. No fixture code is added to the production DLL.
add_library(ValueTypePinningNative SHARED ScopeTests.cpp)
target_compile_features(ValueTypePinningNative PRIVATE cxx_std_20)
target_compile_options(ValueTypePinningNative PRIVATE /EHa /MD
    "/AI${REFRAMEWORK_DOT_NET_ASSEMBLY_DIR}")
set_target_properties(ValueTypePinningNative PROPERTIES
    OUTPUT_NAME "REFramework.NET.ValueTypePinning.Tests"
    EXCLUDE_FROM_DEFAULT_BUILD TRUE
    EXCLUDE_FROM_ALL TRUE
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/ValueTypePinning/$<0:>"
    COMMON_LANGUAGE_RUNTIME "netcore"
    DOTNET_TARGET_FRAMEWORK "net10.0-windows"
)
# A build-only dependency avoids MSBuild's automatic /FU import for a direct
# managed ProjectReference, which would discard ScopeTests.cpp's as_friend.
add_custom_target(ValueTypePinningBridge DEPENDS csharp-api)
add_dependencies(ValueTypePinningNative ValueTypePinningBridge)

find_program(DOTNET_EXECUTABLE dotnet)
if(NOT DOTNET_EXECUTABLE)
    message(FATAL_ERROR "The .NET SDK is required to run the pinning regression")
endif()
add_custom_target(check-value-pinning
    COMMAND "${DOTNET_EXECUTABLE}" run
        --project "${CMAKE_CURRENT_SOURCE_DIR}/ValueTypePinning.csproj"
        --configuration "$<CONFIG>"
        "-p:BridgeDirectory=$<TARGET_FILE_DIR:csharp-api>"
        "-p:TestDirectory=$<TARGET_FILE_DIR:ValueTypePinningNative>"
    DEPENDS ValueTypePinningNative
    USES_TERMINAL VERBATIM
)
