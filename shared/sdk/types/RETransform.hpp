#pragma once
#include "REComponent.hpp"

class REScene;
class REArrayBase;
class JointMatrices;

#pragma pack(push, 1)
class REJointArray {
public:
    REArrayBase* data;        // 0x0000
    JointMatrices* matrices;  // 0x0008
    char pad_0010[256];       // 0x0010
}; // Size: 0x0110
static_assert(sizeof(REJointArray) == 0x110);

class RETransform : public REComponent {
public:
#ifdef REFRAMEWORK_UNIVERSAL
    // REJointArray offset varies: 0xD0 (RE2/RE3 TDB70), 0xD8 (RE8+), 0xE8 (RE7 TDB49, out of scope).
    REJointArray& get_joints();
    const REJointArray& get_joints() const;
#else
    REJointArray& get_joints() { return m_joints; }
    const REJointArray& get_joints() const { return m_joints; }
#endif

    // Stable-offset accessors (same across all supported games)
    Vector4f& get_position() { return m_position; }
    const Vector4f& get_position() const { return m_position; }
    void set_position(const Vector4f& v) { m_position = v; }

    Vector4f& get_angles() { return m_angles; }
    const Vector4f& get_angles() const { return m_angles; }
    void set_angles(const Vector4f& v) { m_angles = v; }

    Vector4f& get_scale() { return m_scale; }
    const Vector4f& get_scale() const { return m_scale; }

    Matrix4x4f& get_world_transform() { return m_worldTransform; }
    const Matrix4x4f& get_world_transform() const { return m_worldTransform; }
    void set_world_transform(const Matrix4x4f& m) { m_worldTransform = m; }

    RETransform* get_child() const { return m_child; }
    RETransform* get_next() const { return m_next; }
    RETransform* get_parent_transform() const { return m_parentTransform; }
    REScene* get_scene() const { return m_scene; }

private:
    Vector4f       m_position;          // 0x0030
    Vector4f       m_angles;            // 0x0040
    Vector4f       m_scale;             // 0x0050
    REScene*       m_scene;             // 0x0060
    RETransform*   m_child;             // 0x0068
    RETransform*   m_next;              // 0x0070
    RETransform*   m_parentTransform;   // 0x0078
    Matrix4x4f     m_worldTransform;    // 0x0080
    void*          m_unk_00C0;          // 0x00C0
    int32_t        m_unk_00C8;          // 0x00C8
    uint32_t       m_tickCount;         // 0x00CC
    bool           m_sameJointsConstraint; // 0x00D0
    bool           m_unk_00D1;          // 0x00D1
    char           _pad_00D2[1];        // 0x00D2
    bool           m_absoluteScaling;   // 0x00D3
    char           _pad_00D4[4];        // 0x00D4
    REJointArray   m_joints;            // 0x00D8
    char           _pad_01E8[8];        // 0x01E8
}; // Size: 0x01F0
static_assert(sizeof(RETransform) == 0x1F0);
#pragma pack(pop)
