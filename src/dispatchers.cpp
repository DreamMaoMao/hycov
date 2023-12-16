#include <optional>

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#include "dispatchers.hpp"
#include "globals.hpp"

static const std::string overviewWorksapceName = "OVERVIEW";
static std::string workspaceNameBackup;
static int workspaceIdBackup;

bool want_auto_fullscren(CWindow *pWindow) {
	int nodeNumInTargetWorkspace = 1;
	auto pNode = g_GridLayout->getNodeFromWindow(pWindow);

	// if client is fullscreen before,don't make it fullscreen
	if (pNode->ovbk_windowIsFullscreen) {
		return false;
	}

	// caculate the number of clients that will be in the same workspace with pWindow(don't contain itself)
	for (auto &n : g_GridLayout->m_lGridNodesData) {
		if(n.pWindow != pNode->pWindow && n.ovbk_windowWorkspaceId == pNode->ovbk_windowWorkspaceId) {
			nodeNumInTargetWorkspace++;
		}
	}
	
	// if only one client in workspace(pWindow itself), don't make it fullscreen
	if(nodeNumInTargetWorkspace > 1) {
		return true;
	} else {
		return false;
	}
}

bool isDirection(const std::string& arg) {
    return arg == "l" || arg == "r" || arg == "u" || arg == "d" || arg == "left" || arg == "right" || arg == "up" || arg == "down";
}

std::optional<ShiftDirection> parseShiftArg(std::string arg) {
	if (arg == "l" || arg == "left") return ShiftDirection::Left;
	else if (arg == "r" || arg == "right") return ShiftDirection::Right;
	else if (arg == "u" || arg == "up") return ShiftDirection::Up;
	else if (arg == "d" || arg == "down") return ShiftDirection::Down;
	else return {};
}

CWindow  *direction_select(std::string arg){
	CWindow *pTempCWindows[100];
	CWindow *pTempClient =  g_pCompositor->m_pLastWindow;
	CWindow *pTempFocusCWindows = nullptr;
	int last = -1;
	if(!pTempClient){
		return nullptr;
	}else if (pTempClient->m_bIsFullscreen){
		return nullptr;
	}

    if (!isDirection(arg)) {
        hycov_log(ERR, "Cannot move focus in direction {}, unsupported direction. Supported: l/left,r/right,u/up,d/down", arg);
        return nullptr;
    }

    for (auto &w : g_pCompositor->m_vWindows)
    {
		CWindow *pWindow = w.get();
        if (pTempClient == pWindow || pTempClient->m_iWorkspaceID !=pWindow->m_iWorkspaceID || pWindow->isHidden() || !pWindow->m_bIsMapped || pWindow->m_bFadingOut || pWindow->m_bIsFullscreen)
            continue;
			last++;
			pTempCWindows[last] = pWindow;			
    }
	
  	if (last < 0)
  	  return nullptr;
  	int sel_x = pTempClient->m_vRealPosition.goalv().x;
  	int sel_y = pTempClient->m_vRealPosition.goalv().y;
  	long long int distance = LLONG_MAX;
  	// int temp_focus = 0;

	auto values = CVarList(arg);
	auto shift = parseShiftArg(values[0]);
  	switch (shift.value()) {
  	case ShiftDirection::Up:
		// Find the window with the closest coordinates 
		// in the top left corner of the window (is limited to same x)
  		for (int _i = 0; _i <= last; _i++) {
  		  if (pTempCWindows[_i]->m_vRealPosition.goalv().y < sel_y && pTempCWindows[_i]->m_vRealPosition.goalv().x == sel_x) {
  		    int dis_x = pTempCWindows[_i]->m_vRealPosition.goalv().x - sel_x;
  		    int dis_y = pTempCWindows[_i]->m_vRealPosition.goalv().y - sel_y;
  		    long long int tmp_distance = dis_x * dis_x + dis_y * dis_y; 
  		    if (tmp_distance < distance) {
  		      distance = tmp_distance;
  		      pTempFocusCWindows = pTempCWindows[_i];
  		    }
  		  }
  		}
		// if find nothing above 
		// find again(is unlimited to x)
		if(!pTempFocusCWindows){
  			for (int _i = 0; _i <= last; _i++) {
  			  if (pTempCWindows[_i]->m_vRealPosition.goalv().y < sel_y ) {
  			    int dis_x = pTempCWindows[_i]->m_vRealPosition.goalv().x - sel_x;
  			    int dis_y = pTempCWindows[_i]->m_vRealPosition.goalv().y - sel_y;
  			    long long int tmp_distance = dis_x * dis_x + dis_y * dis_y; 
  			    if (tmp_distance < distance) {
  			      distance = tmp_distance;
  			      pTempFocusCWindows = pTempCWindows[_i];
  			    }
  			  }
  			}		
		}
  		break;
  	case ShiftDirection::Down:
  		for (int _i = 0; _i <= last; _i++) {
  		  if (pTempCWindows[_i]->m_vRealPosition.goalv().y > sel_y && pTempCWindows[_i]->m_vRealPosition.goalv().x == sel_x) {
  		    int dis_x = pTempCWindows[_i]->m_vRealPosition.goalv().x - sel_x;
  		    int dis_y = pTempCWindows[_i]->m_vRealPosition.goalv().y - sel_y;
  		    long long int tmp_distance = dis_x * dis_x + dis_y * dis_y; 
  		    if (tmp_distance < distance) {
  		      distance = tmp_distance;
  		      pTempFocusCWindows = pTempCWindows[_i];
  		    }
  		  }
  		}
		if(!pTempFocusCWindows){
  			for (int _i = 0; _i <= last; _i++) {
  			  if (pTempCWindows[_i]->m_vRealPosition.goalv().y > sel_y ) {
  			    int dis_x = pTempCWindows[_i]->m_vRealPosition.goalv().x - sel_x;
  			    int dis_y = pTempCWindows[_i]->m_vRealPosition.goalv().y - sel_y;
  			    long long int tmp_distance = dis_x * dis_x + dis_y * dis_y; 
  			    if (tmp_distance < distance) {
  			      distance = tmp_distance;
  			      pTempFocusCWindows = pTempCWindows[_i];
  			    }
  			  }
  			}		
		}
  		break;
  	case ShiftDirection::Left:
  		for (int _i = 0; _i <= last; _i++) {
  		  if (pTempCWindows[_i]->m_vRealPosition.goalv().x < sel_x && pTempCWindows[_i]->m_vRealPosition.goalv().y == sel_y) {
  		    int dis_x = pTempCWindows[_i]->m_vRealPosition.goalv().x - sel_x;
  		    int dis_y = pTempCWindows[_i]->m_vRealPosition.goalv().y - sel_y;
  		    long long int tmp_distance = dis_x * dis_x + dis_y * dis_y; 
  		    if (tmp_distance < distance) {
  		      distance = tmp_distance;
  		      pTempFocusCWindows = pTempCWindows[_i];
  		    }
  		  }
  		}
		if(!pTempFocusCWindows){
  			for (int _i = 0; _i <= last; _i++) {
  			  if (pTempCWindows[_i]->m_vRealPosition.goalv().x < sel_x) {
  			    int dis_x = pTempCWindows[_i]->m_vRealPosition.goalv().x - sel_x;
  			    int dis_y = pTempCWindows[_i]->m_vRealPosition.goalv().y - sel_y;
  			    long long int tmp_distance = dis_x * dis_x + dis_y * dis_y; 
  			    if (tmp_distance < distance) {
  			      distance = tmp_distance;
  			      pTempFocusCWindows = pTempCWindows[_i];
  			    }
  			  }
  			}		
		}
  		break;
  	case ShiftDirection::Right:
  		for (int _i = 0; _i <= last; _i++) {
  		  if (pTempCWindows[_i]->m_vRealPosition.goalv().x > sel_x  && pTempCWindows[_i]->m_vRealPosition.goalv().y == sel_y) {
  		    int dis_x = pTempCWindows[_i]->m_vRealPosition.goalv().x - sel_x;
  		    int dis_y = pTempCWindows[_i]->m_vRealPosition.goalv().y - sel_y;
  		    long long int tmp_distance = dis_x * dis_x + dis_y * dis_y; 
  		    if (tmp_distance < distance) {
  		      distance = tmp_distance;
  		      pTempFocusCWindows = pTempCWindows[_i];
  		    }
  		  }
  		}
		if(!pTempFocusCWindows){
  			for (int _i = 0; _i <= last; _i++) {
  			  if (pTempCWindows[_i]->m_vRealPosition.goalv().x > sel_x) {
  			    int dis_x = pTempCWindows[_i]->m_vRealPosition.goalv().x - sel_x;
  			    int dis_y = pTempCWindows[_i]->m_vRealPosition.goalv().y - sel_y;
  			    long long int tmp_distance = dis_x * dis_x + dis_y * dis_y; 
  			    if (tmp_distance < distance) {
  			      distance = tmp_distance;
  			      pTempFocusCWindows = pTempCWindows[_i];
  			    }
  			  }
  			}		
		}
  		break;
  	}
  	return pTempFocusCWindows;
}

void dispatch_focusdir(std::string arg)
{
	CWindow *pWindow;
	try {
		pWindow = direction_select(arg);
		if(pWindow){
			g_pCompositor->focusWindow(pWindow);
			g_pCompositor->warpCursorTo(pWindow->middle());
			g_pInputManager->m_pForcedFocus = pWindow;
        	g_pInputManager->simulateMouseMovement();
        	g_pInputManager->m_pForcedFocus = nullptr;
		}
    } catch (std::bad_any_cast& e) { HyprlandAPI::addNotification(PHANDLE, "focusdir", CColor{0.f, 0.5f, 1.f, 1.f}, 5000); }

}

void dispatch_toggleoverview(std::string arg)
{
	hycov_log(LOG,"toggle overview");
	if (g_isOverView) {
		dispatch_leaveoverview(arg);
	} else {
		dispatch_enteroverview(arg);
	}
}

void dispatch_enteroverview(std::string arg)
{ 
	//ali clients exit fullscreen status before enter overview
	CWindow *pFullscreenWindow;
	CWindow *pActiveWindow = g_pCompositor->m_pLastWindow;
	CWorkspace *pActiveWorkspace;
	CMonitor *pActiveMonitor;

	bool isNoShouldTileWindow = true;

    for (auto &w : g_pCompositor->m_vWindows)
    {
		CWindow *pWindow = w.get();
        if (pWindow->isHidden() || !pWindow->m_bIsMapped || pWindow->m_bFadingOut || g_pCompositor->isWorkspaceSpecial(pWindow->m_iWorkspaceID))
            continue;
		isNoShouldTileWindow = false;
	}

	//if no clients, forbit enter overview 
	if(isNoShouldTileWindow){
		return;
	}

	hycov_log(LOG,"enter overview");
	g_isOverView = true;

	//make all fullscreen window exit fullscreen state
	for (auto &w : g_pCompositor->m_vWorkspaces)
	{
		CWorkspace *pWorkspace = w.get();
		if (pWorkspace->m_bHasFullscreenWindow)
		{
			pFullscreenWindow = g_pCompositor->getFullscreenWindowOnWorkspace(pWorkspace->m_iID);
			g_pCompositor->setWindowFullscreen(pFullscreenWindow, false, FULLSCREEN_FULL);

			//let overview know the client is a fullscreen before
			pFullscreenWindow->m_bIsFullscreen = true;
		}
	}

	//enter overview layout
	g_pLayoutManager->switchToLayout("grid");

	//change workspace name to OVERVIEW
	pActiveMonitor	= g_pCompositor->m_pLastMonitor;
	pActiveWorkspace = g_pCompositor->getWorkspaceByID(pActiveMonitor->activeWorkspace);
	workspaceNameBackup = pActiveWorkspace->m_szName;
	workspaceIdBackup = pActiveWorkspace->m_iID;
	g_pCompositor->renameWorkspace(pActiveMonitor->activeWorkspace,overviewWorksapceName);

	//Preserve window focus
	if(pActiveWindow){
		g_pCompositor->focusWindow(pActiveWindow); //restore the focus to before active window

	} else {
		auto node = g_GridLayout->m_lGridNodesData.back();
		g_pCompositor->focusWindow(node.pWindow);
	}

	//disable changeworkspace
	if(g_disable_workspace_change) {
  		g_pChangeworkspaceHook->hook();
		g_pMoveActiveToWorkspaceHook->hook();
	}

	//disable spawn
	if(g_disable_spawn) {
		g_pSpawnHook->hook();
	}

	return;
}

void dispatch_leaveoverview(std::string arg)
{ 
	// get default layout
	std::string *configLayoutName = &HyprlandAPI::getConfigValue(PHANDLE, "general:layout")->strValue;

	if(!g_isOverView){
		return;
	}
	
	hycov_log(LOG,"leave overview");
	g_isOverView = false;
	//mark exiting overview mode
	g_isOverViewExiting = true;
	
	//restore workspace name
	g_pCompositor->renameWorkspace(workspaceIdBackup,workspaceNameBackup);

	//enable changeworkspace
	if(g_disable_workspace_change) {
  		g_pChangeworkspaceHook->unhook();
		g_pMoveActiveToWorkspaceHook->unhook();
	}

	//enable spawn
	if(g_disable_spawn) {
		g_pSpawnHook->unhook();
	}

	// if no clients, just exit overview, don't restore client's state
	if (g_GridLayout->m_lGridNodesData.empty())
	{
		g_pLayoutManager->switchToLayout(*configLayoutName);	
		g_GridLayout->m_lGridNodesData.clear();
		g_isOverViewExiting = false;
		return;
	}

	//move clients to it's original workspace 
	g_GridLayout->moveWindowToSourceWorkspace();
	// go to the workspace where the active client was before
	g_GridLayout->changeToActivceSourceWorkspace();
	
	for (auto &n : g_GridLayout->m_lGridNodesData)
	{	
		if (n.ovbk_windowIsFloating)
		{
			//make floating client restore it's floating status
			n.pWindow->m_bIsFloating = true;
			g_pLayoutManager->getCurrentLayout()->onWindowCreatedFloating(n.pWindow);

			// make floating client restore it's position and size
			n.pWindow->m_vRealSize = n.ovbk_size;
			n.pWindow->m_vRealPosition = n.ovbk_position;

			auto calcPos = n.ovbk_position;
			auto calcSize = n.ovbk_size;

			n.pWindow->m_vRealSize = calcSize;
			n.pWindow->m_vRealPosition = calcPos;

			g_pXWaylandManager->setWindowSize(n.pWindow, calcSize);

		} else if(!n.ovbk_windowIsFloating && !n.ovbk_windowIsFullscreen) {
			// make nofloating client restore it's position and size
			n.pWindow->m_vRealSize = n.ovbk_size;
			n.pWindow->m_vRealPosition = n.ovbk_position;

			auto calcPos = n.ovbk_position;
			auto calcSize = n.ovbk_size;

			n.pWindow->m_vRealSize = calcSize;
			n.pWindow->m_vRealPosition = calcPos;

			g_pXWaylandManager->setWindowSize(n.pWindow, calcSize);			
		}
	}

	//exit overview layout,go back to old layout
	CWindow *pActiveWindow = g_pCompositor->m_pLastWindow;
	g_pCompositor->focusWindow(nullptr);
	g_pLayoutManager->switchToLayout(*configLayoutName);

	//Preserve window focus
	if(pActiveWindow){
		g_pCompositor->focusWindow(pActiveWindow); //restore the focus to before active window
		if(pActiveWindow->m_bIsFloating)
			g_pCompositor->changeWindowZOrder(pActiveWindow, true);
		else if(g_auto_fullscreen) { // if enale auto_fullscreen after exit overview
			if(want_auto_fullscren(pActiveWindow))
				g_pCompositor->setWindowFullscreen(pActiveWindow,true,FULLSCREEN_MAXIMIZED);
		}

	} else {
		auto node = g_GridLayout->m_lGridNodesData.back();
		g_pCompositor->focusWindow(node.pWindow);
	}

	for (auto &n : g_GridLayout->m_lGridNodesData)
	{
		//make all fullscrenn windwo restore it's status
		if (n.ovbk_windowIsFullscreen)
		{
			if (n.pWindow != g_pCompositor->m_pLastWindow && n.pWindow->m_iWorkspaceID == g_pCompositor->m_pLastWindow->m_iWorkspaceID)
			{
				continue;
			}	
			g_pCompositor->setWindowFullscreen(n.pWindow, true, n.ovbk_windowFullscreenMode );
		}
	}

	//clean overview layout node date
	g_GridLayout->m_lGridNodesData.clear();

	//mark has exited overview mode
	g_isOverViewExiting = false;
	return;
}

void registerDispatchers()
{
	HyprlandAPI::addDispatcher(PHANDLE, "hycov:enteroverview", dispatch_enteroverview);
	HyprlandAPI::addDispatcher(PHANDLE, "hycov:leaveoverview", dispatch_leaveoverview);
	HyprlandAPI::addDispatcher(PHANDLE, "hycov:toggleoverview", dispatch_toggleoverview);
	HyprlandAPI::addDispatcher(PHANDLE, "hycov:movefocus", dispatch_focusdir);
}
