#pragma once
namespace regenny::via {
#pragma pack(push, 1)
enum class CameraType {
    Game = 0,
    Debug = 1,
    Scene = 2,
    SceneXY = 3,
    SceneYZ = 4,
    SceneXZ = 5,
    Preview = 6,
};
#pragma pack(pop)
}
