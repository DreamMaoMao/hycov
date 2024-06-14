#pragma once

#include "log.hpp"
#include <hyprland/src/includes.hpp>
#include <any>

#define private public
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>
#include <hyprland/src/devices/Keyboard.hpp>
#include <hyprland/src/devices/IPointer.hpp>
#include <hyprutils/string/String.hpp>
#undef private

#include "OvGridLayout.hpp"


inline HANDLE PHANDLE = nullptr;
inline std::unique_ptr<OvGridLayout> g_hycov_OvGridLayout;

inline bool g_hycov_isOverView;
inline bool g_hycov_isInHotArea;
inline int g_hycov_enable_hotarea;
inline std::string g_hycov_hotarea_monitor;
inline int g_hycov_hotarea_pos;
inline int g_hycov_hotarea_size;
inline unsigned int g_hycov_swipe_fingers;
inline int g_hycov_isGestureBegin;
inline int g_hycov_move_focus_distance;
inline int g_hycov_enable_gesture;
inline int g_hycov_disable_workspace_change;
inline int g_hycov_disable_spawn;
inline int g_hycov_auto_exit;
inline int g_hycov_auto_fullscreen;
inline int g_hycov_only_active_workspace;
inline int g_hycov_only_active_monitor;
inline int g_hycov_enable_alt_release_exit;
inline int g_hycov_alt_toggle_auto_next;
inline int g_hycov_click_in_cursor;
inline int g_hycov_height_of_titlebar;
inline std::string g_hycov_alt_replace_key;
inline int g_hycov_bordersize;
inline int g_hycov_overview_gappo;
inline int g_hycov_overview_gappi;
inline std::string g_hycov_configLayoutName;
inline int g_hycov_show_special;
inline int g_hycov_enable_click_action;
inline int g_hycov_raise_float_to_top;


inline bool g_hycov_isOverViewExiting;
inline bool g_hycov_forece_display_all = false;
inline bool g_hycov_forece_display_all_in_one_monitor = false;
inline bool g_hycov_force_display_only_current_workspace = false;
inline int g_hycov_groupBarHeight;

inline CFunctionHook* g_hycov_pOnSwipeBeginHook = nullptr;
inline CFunctionHook* g_hycov_pOnSwipeEndHook = nullptr;
inline CFunctionHook* g_hycov_pOnSwipeUpdateHook = nullptr;
inline CFunctionHook* g_hycov_pCWindow_onUnmap = nullptr;
inline CFunctionHook* g_hycov_pChangeworkspaceHook = nullptr;
inline CFunctionHook* g_hycov_pMoveActiveToWorkspaceHook = nullptr;
inline CFunctionHook* g_hycov_pSpawnHook = nullptr;
inline CFunctionHook* g_hycov_pStartAnimHook = nullptr;
inline CFunctionHook* g_hycov_pFullscreenActiveHook = nullptr;
inline CFunctionHook* g_hycov_pOnKeyboardKeyHook = nullptr;
inline CFunctionHook* g_hycov_pHyprDwindleLayout_recalculateMonitorHook = nullptr;
inline CFunctionHook* g_hycov_pHyprMasterLayout_recalculateMonitorHook = nullptr;
inline CFunctionHook* g_hycov_pHyprDwindleLayout_recalculateWindowHook = nullptr;
inline CFunctionHook* g_hycov_pSDwindleNodeData_recalcSizePosRecursiveHook = nullptr;
inline CFunctionHook* g_hycov_pCInputManager_onMouseButtonHook = nullptr;
inline CFunctionHook* g_hycov_pCKeybindManager_changeGroupActiveHook = nullptr;  
inline CFunctionHook* g_hycov_pCKeybindManager_toggleGroupHook = nullptr;
inline CFunctionHook* g_hycov_pCKeybindManager_moveOutOfGroupHook = nullptr;
inline CFunctionHook* g_hycov_pCInputManager_mouseMoveUnifiedHook = nullptr;

inline void errorNotif()
{
	HyprlandAPI::addNotificationV2(
		PHANDLE,
		{
			{"text", "Something has gone very wrong. Check the log for details."},
			{"time", (uint64_t)10000},
			{"color", CColor(1.0, 0.0, 0.0, 1.0)},
			{"icon", ICON_ERROR},
		});
}
