#include "natsu_runtime/resource/config_manager/config_manager.h"

#include "natsu_runtime/core/base/macro.h"

namespace Natsu::Runtime {
    ConfigManager::ConfigManager() {
        m_root_folder   = std::filesystem::path(NATSU_XSTR(ROOT_DIR));
        m_asset_folder  = m_root_folder / "asset";
        m_script_folder = m_root_folder / "script";
        m_shader_folder = m_root_folder / "natsu/shader";
    }

    const std::filesystem::path& ConfigManager::GetRootFolder() const { return m_root_folder; }
    const std::filesystem::path& ConfigManager::GetAssetFolder() const { return m_asset_folder; }
    const std::filesystem::path& ConfigManager::GetScriptFolder() const { return m_script_folder; }
    const std::filesystem::path& ConfigManager::GetShaderFolder() const { return m_shader_folder; }
} // namespace Natsu::Runtime