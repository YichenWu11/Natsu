#pragma once

namespace Natsu::Runtime {
    class ConfigManager {
    public:
        ConfigManager();

        const std::filesystem::path& GetRootFolder() const;
        const std::filesystem::path& GetAssetFolder() const;
        const std::filesystem::path& GetScriptFolder() const;
        const std::filesystem::path& GetShaderFolder() const;

    private:
        std::filesystem::path m_root_folder;
        std::filesystem::path m_asset_folder;
        std::filesystem::path m_script_folder;
        std::filesystem::path m_shader_folder;
    };
} // namespace Natsu::Runtime