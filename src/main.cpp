#include <optional>

#include "dispatchers.hpp"
#include "globaleventhook.hpp"

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle)
{
	PHANDLE = handle;

#define CONF(NAME,VALUE) \
	HyprlandAPI::addConfigValue(PHANDLE, "plugin:hycov:" NAME, Hyprlang::CConfigValue((VALUE)))

	CONF("overview_gappo", (int64_t)60);
	CONF("overview_gappi", (int64_t)24);
	CONF("hotarea_size", (int64_t)10);
	CONF("hotarea_monitor", (char *)"all");
  	CONF("hotarea_pos", (int64_t)1);
	CONF("enable_hotarea", (int64_t)1);
	CONF("swipe_fingers",  (int64_t)4);
	CONF("move_focus_distance",  (int64_t)100);
	CONF("enable_gesture", (int64_t)0);
	CONF("disable_workspace_change", (int64_t)0);
	CONF("disable_spawn", (int64_t)0);
	CONF("auto_exit",  (int64_t)1);
	CONF("auto_fullscreen", (int64_t)0);
	CONF("only_active_workspace", (int64_t)0);
	CONF("only_active_monitor", (int64_t)0);
	CONF("enable_alt_release_exit", (int64_t)0);
	CONF("alt_toggle_auto_next", (int64_t)0);
	CONF("click_in_cursor", (int64_t)1);
	CONF("hight_of_titlebar", (int64_t)0);
	CONF("alt_replace_key", (char *)"Alt_L");


#undef CONF

	// int value
	static const auto *pEnable_hotarea_config = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:enable_hotarea");
  	static const auto *pHotarea_pos_config = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:hotarea_pos");
  	static const auto *pHotarea_size_config = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:hotarea_size");
	static const auto *pSwipe_fingers_config = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:swipe_fingers");
	static const auto *pMove_focus_distance_config = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:move_focus_distance");
	static const auto *pEnable_gesture_config = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:enable_gesture");
	static const auto *pDisable_workspace_change_config = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:disable_workspace_change");
	static const auto *pDisable_spawn_config = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:disable_spawn");
	static const auto *pAuto_exit_config = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:auto_exit");
	static const auto *pAuto_fullscreen = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:auto_fullscreen");
	static const auto *pOnly_active_workspace = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:only_active_workspace");
	static const auto *pOnly_active_monitor = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:only_active_monitor");
	static const auto *pEnable_alt_release_exit = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:enable_alt_release_exit");
	static const auto *pAlt_toggle_auto_next = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:alt_toggle_auto_next");
	static const auto *pClick_in_cursor = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:click_in_cursor");
	static const auto *pHight_of_titlebar = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:hight_of_titlebar");
    static const auto *pBordersize = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "general:border_size");
    static const auto *pOverview_gappo = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:overview_gappo");
    static const auto *pOverview_gappi = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:overview_gappi");


	// string value
	static const auto *pAlt_replace_key = (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:alt_replace_key");
  	static const auto *pHotarea_monitor_config = (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:hotarea_monitor");
	static const auto *pConfigLayoutName = (Hyprlang::STRING const*)HyprlandAPI::getConfigValue(PHANDLE, "general:layout");


	// int value
	g_hycov_enable_hotarea = **pEnable_hotarea_config;
	g_hycov_hotarea_pos = **pHotarea_pos_config;
	g_hycov_hotarea_size = **pHotarea_size_config;
	g_hycov_swipe_fingers = **pSwipe_fingers_config;
	g_hycov_move_focus_distance = **pMove_focus_distance_config;
	g_hycov_enable_gesture = **pEnable_gesture_config;
	g_hycov_disable_workspace_change = **pDisable_workspace_change_config;
	g_hycov_disable_spawn = **pDisable_spawn_config;
	g_hycov_auto_exit = **pAuto_exit_config;
	g_hycov_auto_fullscreen = **pAuto_fullscreen;
	g_hycov_only_active_workspace = **pOnly_active_workspace;
	g_hycov_only_active_monitor = **pOnly_active_monitor;
	g_hycov_enable_alt_release_exit = **pEnable_alt_release_exit;
	g_hycov_alt_toggle_auto_next = **pAlt_toggle_auto_next;
	g_hycov_click_in_cursor = **pClick_in_cursor;
	g_hycov_hight_of_titlebar= **pHight_of_titlebar;
	g_hycov_bordersize = **pBordersize;
	g_hycov_overview_gappo = **pOverview_gappo;
	g_hycov_overview_gappi = **pOverview_gappi;

	// string value
	g_hycov_alt_replace_key = *pAlt_replace_key;
	g_hycov_hotarea_monitor = *pHotarea_monitor_config;
	g_hycov_configLayoutName = *pConfigLayoutName;


	g_hycov_OvGridLayout = std::make_unique<OvGridLayout>();
	HyprlandAPI::addLayout(PHANDLE, "ovgrid", g_hycov_OvGridLayout.get());

	registerGlobalEventHook();
	registerDispatchers();

 	HyprlandAPI::reloadConfig();
	return {"hycov", "overview mode", "DreamMaoMao", "0.3"};
}

APICALL EXPORT void PLUGIN_EXIT() {}
