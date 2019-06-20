#include "scene_object.h"
#include <cassert>

namespace Baikal
{
    static std::uint32_t g_next_id = 0;
    static int g_scene_controller_id = -1;

    SceneObject::SceneObject()
        : m_dirty(), m_id(g_next_id++)
    {
    }

    bool SceneObject::IsDirty() const
    {
        assert(g_scene_controller_id >= 0);

        return m_dirty[g_scene_controller_id];

    }

    void SceneObject::SetDirty(bool dirty) const
    {
        if (dirty)
        {
            // Set all bits to 1
            m_dirty.set();
        }
        else
        {
            assert(g_scene_controller_id >= 0);
            // Unset corresponging bit
            m_dirty.set(g_scene_controller_id, false);
        }
    }

    void SceneObject::ResetId()
    {
        g_next_id = 0;
    }

    void SceneObject::SetSceneControllerId(std::uint32_t controller_id)
    {
        assert(controller_id < kMaxDirtyBits);
        g_scene_controller_id = static_cast<int>(controller_id);
    }

    void SceneObject::ResetSceneControllerId()
    {
        g_scene_controller_id = -1;
    }

}
