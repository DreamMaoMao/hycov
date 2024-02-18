#include <optional>

#include "dispatchers.hpp"
#include "globaleventhook.hpp"

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle)
{
	PHANDLE = handle;

#define CONF(NAME, TYPE, VALUE) \
	HyprlandAPI::addConfigValue(PHANDLE, "plugin:hycov:" NAME, SConfigValue{.TYPE##Value = VALUE})

	CONF("overview_gappo", int, 60);
	CONF("overview_gappi", int, 24);
	CONF("hotarea_size", int, 10);
	CONF("hotarea_monitor", str, "all");
  	CONF("hotarea_pos", int, 1);
	CONF("enable_hotarea", int, 1);
	CONF("swipe_fingers", int, 4);
	CONF("move_focus_distance", int, 100);
	CONF("enable_gesture", int, 0);
	CONF("disable_workspace_change", int, 1);
	CONF("disable_spawn", int, 0);
	CONF("auto_exit", int, 1);
	CONF("auto_fullscreen", int, 0);
	CONF("only_active_workspace", int, 0);
	CONF("only_active_monitor", int, 0);
	CONF("enable_alt_release_exit", int, 0);
	CONF("alt_toggle_auto_next", int, 0);
	CONF("click_in_cursor", int, 1);
	CONF("hight_of_titlebar", int, 0);
	CONF("alt_replace_key", str, "Alt_L");


#undef CONF

	static const auto *pEnable_hotarea_config = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:enable_hotarea")->intValue;
  	static const auto *pHotarea_monitor_config = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:hotarea_monitor")->strValue;
  	static const auto *pHotarea_pos_config = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:hotarea_pos")->intValue;
  	static const auto *pHotarea_size_config = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:hotarea_size")->intValue;
	static const auto *pSwipe_fingers_config = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:swipe_fingers")->intValue;
	static const auto *pMove_focus_distance_config = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:move_focus_distance")->intValue;
	static const auto *pEnable_gesture_config = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:enable_gesture")->intValue;
	static const auto *pDisable_workspace_change_config = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:disable_workspace_change")->intValue;
	static const auto *pDisable_spawn_config = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:disable_spawn")->intValue;
	static const auto *pAuto_exit_config = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:auto_exit")->intValue;
	static const auto *pAuto_fullscreen = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:auto_fullscreen")->intValue;
	static const auto *pOnly_active_workspace = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:only_active_workspace")->intValue;
	static const auto *pOnly_active_monitor = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:only_active_monitor")->intValue;
	static const auto *pEnable_alt_release_exit = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:enable_alt_release_exit")->intValue;
	static const auto *pAlt_toggle_auto_next = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:alt_toggle_auto_next")->intValue;
	static const auto *pClick_in_cursor = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:click_in_cursor")->intValue;
	static const auto *pHight_of_titlebar = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:hight_of_titlebar")->intValue;
	static const auto *pAlt_replace_key = &HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:alt_replace_key")->strValue;


	g_hycov_enable_hotarea = *pEnable_hotarea_config;
	g_hycov_hotarea_monitor = *pHotarea_monitor_config;
	g_hycov_hotarea_pos = *pHotarea_pos_config;
	g_hycov_hotarea_size = *pHotarea_size_config;
	g_hycov_swipe_fingers = *pSwipe_fingers_config;
	g_hycov_move_focus_distance = *pMove_focus_distance_config;
	g_hycov_enable_gesture = *pEnable_gesture_config;
	g_hycov_disable_workspace_change = *pDisable_workspace_change_config;
	g_hycov_disable_spawn = *pDisable_spawn_config;
	g_hycov_auto_exit = *pAuto_exit_config;
	g_hycov_auto_fullscreen = *pAuto_fullscreen;
	g_hycov_only_active_workspace = *pOnly_active_workspace;
	g_hycov_only_active_monitor = *pOnly_active_monitor;
	g_hycov_enable_alt_release_exit = *pEnable_alt_release_exit;
	g_hycov_alt_toggle_auto_next = *pAlt_toggle_auto_next;
	g_hycov_click_in_cursor = *pClick_in_cursor;
	g_hycov_hight_of_titlebar= *pHight_of_titlebar;
	g_hycov_alt_replace_key = *pAlt_replace_key;

	g_hycov_OvGridLayout = std::make_unique<OvGridLayout>();
	HyprlandAPI::addLayout(PHANDLE, "ovgrid", g_hycov_OvGridLayout.get());

	registerGlobalEventHook();
	registerDispatchers();

 	HyprlandAPI::reloadConfig();
	return {"hycov", "overview mode", "DreamMaoMao", "0.2"};
}

APICALL EXPORT void PLUGIN_EXIT() {}
