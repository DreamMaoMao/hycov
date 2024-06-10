#include <optional>

#include "dispatchers.hpp"
#include "globaleventhook.hpp"

APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle)
{
	PHANDLE = handle;

#define CONF(NAME,VALUE) \
	HyprlandAPI::addConfigValue(PHANDLE, "plugin:hycov:" NAME, {VALUE})

	CONF("overview_gappo", 60L);
	CONF("overview_gappi", 24L);
	CONF("hotarea_size", 10L);
  	CONF("hotarea_pos", 1L);
	CONF("enable_hotarea", 1L);
	CONF("swipe_fingers",  4L);
	CONF("move_focus_distance",  100L);
	CONF("enable_gesture", 0L);
	CONF("disable_workspace_change", 1L);
	CONF("disable_spawn", 0L);
	CONF("auto_exit",  1L);
	CONF("auto_fullscreen", 0L);
	CONF("only_active_workspace", 0L);
	CONF("only_active_monitor", 0L);
	CONF("enable_alt_release_exit", 0L);
	CONF("alt_toggle_auto_next", 0L);
	CONF("click_in_cursor", 1L);
	CONF("height_of_titlebar", 0L);
	CONF("hotarea_monitor", "all");
	CONF("alt_replace_key", "Alt_L");
	CONF("show_special",  0L);
	CONF("enable_click_action",  1L);
	CONF("raise_float_to_top",  1L);

#undef CONF

	HyprlandAPI::reloadConfig();
	g_pConfigManager->tick();

	// int value
	static const auto *pEnable_hotarea_config = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:enable_hotarea")->getDataStaticPtr());
  	static const auto *pHotarea_pos_config = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:hotarea_pos")->getDataStaticPtr());
  	static const auto *pHotarea_size_config = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:hotarea_size")->getDataStaticPtr());
	static const auto *pSwipe_fingers_config = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:swipe_fingers")->getDataStaticPtr());
	static const auto *pMove_focus_distance_config = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:move_focus_distance")->getDataStaticPtr());
	static const auto *pEnable_gesture_config = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:enable_gesture")->getDataStaticPtr());
	static const auto *pDisable_workspace_change_config = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:disable_workspace_change")->getDataStaticPtr());
	static const auto *pDisable_spawn_config = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:disable_spawn")->getDataStaticPtr());
	static const auto *pAuto_exit_config = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:auto_exit")->getDataStaticPtr());
	static const auto *pAuto_fullscreen = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:auto_fullscreen")->getDataStaticPtr());
	static const auto *pOnly_active_workspace = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:only_active_workspace")->getDataStaticPtr());
	static const auto *pOnly_active_monitor = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:only_active_monitor")->getDataStaticPtr());
	static const auto *pEnable_alt_release_exit = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:enable_alt_release_exit")->getDataStaticPtr());
	static const auto *pAlt_toggle_auto_next = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:alt_toggle_auto_next")->getDataStaticPtr());
	static const auto *pClick_in_cursor = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:click_in_cursor")->getDataStaticPtr());
	static const auto *pHeight_of_titlebar = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:height_of_titlebar")->getDataStaticPtr());
    static const auto *pBordersize = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "general:border_size")->getDataStaticPtr());
    static const auto *pOverview_gappo = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:overview_gappo")->getDataStaticPtr());
    static const auto *pOverview_gappi = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:overview_gappi")->getDataStaticPtr());
	static const auto *pShow_special_config = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:show_special")->getDataStaticPtr());
	static const auto *pEnable_click_action_config = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:enable_click_action")->getDataStaticPtr());
	static const auto *pRaise_float_to_top = (Hyprlang::INT* const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:raise_float_to_top")->getDataStaticPtr());


	// string value
	static const auto *pAlt_replace_key = (Hyprlang::STRING const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:alt_replace_key")->getDataStaticPtr());
  	static const auto *pHotarea_monitor_config = (Hyprlang::STRING const*)(HyprlandAPI::getConfigValue(PHANDLE, "plugin:hycov:hotarea_monitor")->getDataStaticPtr());
	static const auto *pConfigLayoutName = (Hyprlang::STRING const*)(HyprlandAPI::getConfigValue(PHANDLE, "general:layout")->getDataStaticPtr());
	static const auto *pGroupBarHeight = (Hyprlang::STRING const*)(HyprlandAPI::getConfigValue(PHANDLE, "group:groupbar:height")->getDataStaticPtr());


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
	g_hycov_height_of_titlebar= **pHeight_of_titlebar;
	g_hycov_bordersize = **pBordersize;
	g_hycov_overview_gappo = **pOverview_gappo;
	g_hycov_overview_gappi = **pOverview_gappi;
	g_hycov_show_special = **pShow_special_config;
	g_hycov_enable_click_action = **pEnable_click_action_config;
	g_hycov_raise_float_to_top = **pRaise_float_to_top;


	// string value
	g_hycov_alt_replace_key = *pAlt_replace_key;
	g_hycov_hotarea_monitor = *pHotarea_monitor_config;
	g_hycov_configLayoutName = *pConfigLayoutName;

	g_hycov_groupBarHeight= **pGroupBarHeight;


	g_hycov_OvGridLayout = std::make_unique<OvGridLayout>();
	HyprlandAPI::addLayout(PHANDLE, "ovgrid", g_hycov_OvGridLayout.get());

	registerGlobalEventHook();
	registerDispatchers();

 	HyprlandAPI::reloadConfig();
	return {"hycov", "overview mode", "DreamMaoMao", "0.3"};
}

APICALL EXPORT void PLUGIN_EXIT() {}
