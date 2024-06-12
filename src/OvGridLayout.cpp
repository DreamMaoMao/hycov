
#include "globals.hpp"
#include "OvGridLayout.hpp"
#include "dispatchers.hpp"

// find next focus window after remove a window
PHLWINDOW OvGridLayout::getNextWindowCandidate(PHLWINDOW plastWindow) {

    PHLWINDOW targetWindow =  nullptr;
    for (auto &w : g_pCompositor->m_vWindows)
    {
		PHLWINDOW pWindow = w;
        if (pWindow->m_pWorkspace != plastWindow->m_pWorkspace || pWindow->isHidden() || !pWindow->m_bIsMapped || pWindow->m_bFadingOut || pWindow->m_bIsFullscreen)
            continue;
		targetWindow = pWindow; // find the last window that is in same workspace with the remove window
    }

    return targetWindow;

}

SOvGridNodeData *OvGridLayout::getNodeFromWindow(PHLWINDOW pWindow)
{
    for (auto &nd : m_lOvGridNodesData)
    {
        if (nd.pWindow == pWindow)
            return &nd;
    }

    return nullptr;
}


SOldLayoutRecordNodeData *OvGridLayout::getOldLayoutRecordNodeFromWindow(PHLWINDOW pWindow)
{
    for (auto &nd : m_lSOldLayoutRecordNodeData)
    {
        if (nd.pWindow == pWindow)
            return &nd;
    }

    return nullptr;
}

int OvGridLayout::getNodesNumOnWorkspace(const int &ws)
{
    int no = 0;
    for (auto &n : m_lOvGridNodesData)
    {
        if (n.workspaceID == ws)
            no++;
    }

    return no;
}


Vector2D OvGridLayout::predictSizeForNewWindowTiled() {
    return {};
}

void OvGridLayout::resizeNodeSizePos(SOvGridNodeData *node, int x, int y, int width, int height)
{   
    
    int groupbar_height_fix;
    if(node->pWindow->m_sGroupData.pNextWindow.lock()) {
        groupbar_height_fix = g_hycov_groupBarHeight;
    } else {
        groupbar_height_fix = 0;
    }
    node->size = Vector2D(width, height - g_hycov_height_of_titlebar - groupbar_height_fix);
    node->position = Vector2D(x, y + g_hycov_height_of_titlebar + groupbar_height_fix);
    applyNodeDataToWindow(node);
}

void OvGridLayout::onWindowCreatedTiling(PHLWINDOW pWindow, eDirection direction)
{
    CMonitor *pTargetMonitor;
    if(g_hycov_forece_display_all_in_one_monitor) {
        pTargetMonitor = g_pCompositor->m_pLastMonitor.get();
    } else {
      pTargetMonitor =  g_pCompositor->getMonitorFromID(pWindow->m_iMonitorID); 
    }

    const auto pNode = &m_lOvGridNodesData.emplace_back(); // make a new node in list back

    auto pActiveWorkspace = pTargetMonitor->activeWorkspace;

    auto pWindowOriWorkspace = pWindow->m_pWorkspace;

    auto oldLayoutRecordNode = getOldLayoutRecordNodeFromWindow(pWindow);
    if(oldLayoutRecordNode) {
        pNode->isInOldLayout = true; //client is taken from the old layout
        m_lSOldLayoutRecordNodeData.remove(*oldLayoutRecordNode);
    }

    //record the previcous window in group
    if(pWindow->m_sGroupData.pNextWindow.lock() && pWindow->getGroupCurrent() == pWindow) {
        pNode->isGroupActive = true;
	}

    pNode->workspaceID = pWindow->m_pWorkspace->m_iID; // encapsulate window objects as node objects to bind more properties
    pNode->pWindow = pWindow;
    pNode->workspaceName = pWindowOriWorkspace->m_szName;
    
    //record the window stats which are used by restore
    pNode->ovbk_windowMonitorId = pWindow->m_iMonitorID;
    pNode->ovbk_windowWorkspaceId = pWindow->m_pWorkspace->m_iID;
    pNode->ovbk_windowFullscreenMode  = pWindowOriWorkspace->m_efFullscreenMode;
    pNode->ovbk_position = pWindow->m_vRealPosition.goal();
    pNode->ovbk_size = pWindow->m_vRealSize.goal();
    pNode->ovbk_windowIsFloating = pWindow->m_bIsFloating;
    pNode->ovbk_windowIsFullscreen = pWindow->m_bIsFullscreen;
    pNode->ovbk_windowWorkspaceName = pWindowOriWorkspace->m_szName;

    //record the window style which are used by restore
    pNode->ovbk_windowIsWithBorder = pWindow->m_sSpecialRenderData.border;
    pNode->ovbk_windowIsWithDecorate = pWindow->m_sSpecialRenderData.decorate;
    pNode->ovbk_windowIsWithRounding = pWindow->m_sSpecialRenderData.rounding;
    pNode->ovbk_windowIsWithShadow = pWindow->m_sSpecialRenderData.shadow;


    //change all client(exclude special workspace) to active worksapce 
    if ((!g_pCompositor->isWorkspaceSpecial(pNode->workspaceID) || g_hycov_show_special) && pNode->isInOldLayout && (pWindowOriWorkspace->m_iID != pActiveWorkspace->m_iID || pWindowOriWorkspace->m_szName != pActiveWorkspace->m_szName) && (!(g_hycov_only_active_workspace || g_hycov_force_display_only_current_workspace) || g_hycov_forece_display_all || g_hycov_forece_display_all_in_one_monitor))    {
        pWindow->m_pWorkspace = pActiveWorkspace;
        pNode->workspaceID = pWindow->m_pWorkspace->m_iID;
        pNode->workspaceName = pActiveWorkspace->m_szName;
        pNode->pWindow->m_iMonitorID = pTargetMonitor->ID;
    }

    // clean fullscreen status
    if (pWindow->m_bIsFullscreen) {   
        pWindow->m_bIsFullscreen = false;
    }

    //clean floating status(only apply to old layout window)
    if (pWindow->m_bIsFloating && pNode->isInOldLayout) {        
        pWindow->m_bIsFloating = false;
        pWindow->updateDynamicRules();
    }

    recalculateMonitor(pWindow->m_iMonitorID);    
}


void OvGridLayout::removeOldLayoutData(PHLWINDOW pWindow) { 

	std::string *configLayoutName = &g_hycov_configLayoutName;
    switchToLayoutWithoutReleaseData(*configLayoutName);
    hycov_log(LOG,"remove data of old layout:{}",*configLayoutName);

    if(*configLayoutName == "dwindle") {
        // disable render client of old layout
        g_hycov_pHyprDwindleLayout_recalculateMonitorHook->hook();
        g_hycov_pHyprDwindleLayout_recalculateWindowHook->hook();
        g_hycov_pSDwindleNodeData_recalcSizePosRecursiveHook->hook();

        // only remove data,not render anything,becaust still in overview
        g_pLayoutManager->getCurrentLayout()->onWindowRemovedTiling(pWindow);

        g_hycov_pSDwindleNodeData_recalcSizePosRecursiveHook->unhook();
        g_hycov_pHyprDwindleLayout_recalculateWindowHook->unhook();
        g_hycov_pHyprDwindleLayout_recalculateMonitorHook->unhook();
    } else if(*configLayoutName == "master") {
        g_hycov_pHyprMasterLayout_recalculateMonitorHook->hook();

        g_pLayoutManager->getCurrentLayout()->onWindowRemovedTiling(pWindow);

        g_hycov_pHyprMasterLayout_recalculateMonitorHook->unhook();
    } else {
        // may be not support other layout
        hycov_log(ERR,"unknow old layout:{}",*configLayoutName);
        g_pLayoutManager->getCurrentLayout()->onWindowRemovedTiling(pWindow);
    }

    switchToLayoutWithoutReleaseData("ovgrid");
}

void OvGridLayout::onWindowRemoved(PHLWINDOW pWindow) {
    const auto pNode = getNodeFromWindow(pWindow);

    if (!pNode)
        return;

    if(pNode->isInOldLayout) { // if client is taken from the old layout
        removeOldLayoutData(pWindow);
    }

    if (pWindow->m_bIsFullscreen)
        g_pCompositor->setWindowFullscreen(pWindow, false, FULLSCREEN_FULL);

    if (!pWindow->m_sGroupData.pNextWindow.expired()) {
        if (pWindow->m_sGroupData.pNextWindow.lock() == pWindow)
            pWindow->m_sGroupData.pNextWindow.reset();
        else {
            // find last window and update
            PHLWINDOW  PWINDOWPREV     = pWindow->getGroupPrevious();
            const auto WINDOWISVISIBLE = pWindow->getGroupCurrent() == pWindow;

            if (WINDOWISVISIBLE)
                PWINDOWPREV->setGroupCurrent(pWindow->m_sGroupData.head ? pWindow->m_sGroupData.pNextWindow.lock() : PWINDOWPREV);

            PWINDOWPREV->m_sGroupData.pNextWindow = pWindow->m_sGroupData.pNextWindow;

            pWindow->m_sGroupData.pNextWindow.reset();

            if (pWindow->m_sGroupData.head) {
                std::swap(PWINDOWPREV->m_sGroupData.pNextWindow.lock()->m_sGroupData.head, pWindow->m_sGroupData.head);
                std::swap(PWINDOWPREV->m_sGroupData.pNextWindow.lock()->m_sGroupData.locked, pWindow->m_sGroupData.locked);
            }

            if (pWindow == m_pLastTiledWindow.lock())
                m_pLastTiledWindow.reset();

            pWindow->setHidden(false);

            pWindow->updateWindowDecos();
            PWINDOWPREV->getGroupCurrent()->updateWindowDecos();
            g_pCompositor->updateWindowAnimatedDecorationValues(pWindow);

            // change node bind window in group
            pNode->pWindow = PWINDOWPREV->getGroupCurrent();
            pNode->pWindow->m_pWorkspace = g_pCompositor->m_pLastMonitor->activeWorkspace;
            applyNodeDataToWindow(pNode);
            pNode->isInOldLayout = false;
            hycov_log(LOG,"change node bind window in group,old:{} new:{}",pWindow,pNode->pWindow);
            
            return;
        }
    }

    if (pWindow->m_bIsFloating) {
        onWindowRemovedFloating(pWindow);
    } else {
        onWindowRemovedTiling(pWindow);
    }

    if (pWindow == m_pLastTiledWindow.lock())
        m_pLastTiledWindow.reset();
}

void OvGridLayout::onWindowRemovedTiling(PHLWINDOW pWindow)
{
    hycov_log(LOG,"remove tiling windwo:{}",pWindow);

    const auto pNode = getNodeFromWindow(pWindow);

    if (!pNode)
        return;

    if(pNode->isInOldLayout) { // if client is taken from the old layout
        removeOldLayoutData(pWindow);
    }

    // if window is in a group,replace it with other window in same group
    //     pNode->pWindow->m_pWorkspace = g_pCompositor->getWorkspaceByID(pNode->workspaceID);
    //     applyNodeDataToWindow(pNode);
    //     pNode->isInOldLayout = false;
    //     g_pCompositor->focusWindow(pNode->pWindow);
    //     return;
    // }

    m_lOvGridNodesData.remove(*pNode);

    if(m_lOvGridNodesData.empty()){
        return;
    }

    recalculateMonitor(pWindow->m_iMonitorID);

}

bool OvGridLayout::isWindowTiled(PHLWINDOW pWindow)
{
    return getNodeFromWindow(pWindow) != nullptr;
}

void OvGridLayout::calculateWorkspace(const int &ws)
{
    const auto pWorksapce = g_pCompositor->getWorkspaceByID(ws); 
    auto dataSize = m_lOvGridNodesData.size();
    auto pTempNodes = new SOvGridNodeData*[dataSize + 1];
    SOvGridNodeData *pNode;
    int i, n = 0;
    int cx, cy;
    int dx, cw, ch;;
    int cols, rows, overcols,NODECOUNT;

    if (!pWorksapce) {
        delete[] pTempNodes;
        return;
    }

    NODECOUNT = getNodesNumOnWorkspace(pWorksapce->m_iID);          
    const auto pMonitor = g_pCompositor->getMonitorFromID(pWorksapce->m_iMonitorID); 

    if (NODECOUNT == 0) {
        delete[] pTempNodes;
        return;
    }

    static const auto *PBORDERSIZE = &g_hycov_bordersize;
    static const auto *GAPPO = &g_hycov_overview_gappo;
    static const auto *GAPPI = &g_hycov_overview_gappi;

    /*
    m is region that is moniotr,
    w is region that is monitor but don not contain bar  
    */
    int m_x = pMonitor->vecPosition.x;
    int m_y = pMonitor->vecPosition.y;
    int w_x = pMonitor->vecPosition.x + pMonitor->vecReservedTopLeft.x;
    int w_y = pMonitor->vecPosition.y + pMonitor->vecReservedTopLeft.y;
    int m_width = pMonitor->vecSize.x;
    int m_height = pMonitor->vecSize.y;
    int w_width = pMonitor->vecSize.x -  pMonitor->vecReservedTopLeft.x;
    int w_height = pMonitor->vecSize.y - pMonitor->vecReservedTopLeft.y;

    for (auto &node : m_lOvGridNodesData)
    {
        if (node.workspaceID == ws)
        {
            pTempNodes[n] = &node;
            n++;
        }
    }

    pTempNodes[n] = NULL;

    if (NODECOUNT == 0) {
        delete[] pTempNodes;
        return;
    }

    // one client arrange
    if (NODECOUNT == 1)
    {
        pNode = pTempNodes[0];
        cw = (w_width - 2 * (*GAPPO)) * 0.7;
        ch = (w_height - 2 * (*GAPPO)) * 0.8;
        resizeNodeSizePos(pNode, w_x + (int)((m_width - cw) / 2), w_y + (int)((w_height - ch) / 2),
                          cw - 2 * (*PBORDERSIZE), ch - 2 * (*PBORDERSIZE));
        delete[] pTempNodes;
        return;
    }

    // two client arrange
    if (NODECOUNT == 2)
    {
        pNode = pTempNodes[0];
        cw = (w_width - 2 * (*GAPPO) - (*GAPPI)) / 2;
        ch = (w_height - 2 * (*GAPPO)) * 0.65;
        resizeNodeSizePos(pNode, m_x + cw + (*GAPPO) + (*GAPPI), m_y + (m_height - ch) / 2 + (*GAPPO),
                          cw - 2 * (*PBORDERSIZE), ch - 2 * (*PBORDERSIZE));
        resizeNodeSizePos(pTempNodes[1], m_x + (*GAPPO), m_y + (m_height - ch) / 2 + (*GAPPO),
                          cw - 2 * (*PBORDERSIZE), ch - 2 * (*PBORDERSIZE));
        delete[] pTempNodes;
        return;
    }

    //more than two client arrange

    //Calculate the integer part of the square root of the number of nodes
    for (cols = 0; cols <= NODECOUNT / 2; cols++)
        if (cols * cols >= NODECOUNT)
            break;
            
    //The number of rows and columns multiplied by the number of nodes
    // must be greater than the number of nodes to fit all the Windows
    rows = (cols && (cols - 1) * cols >= NODECOUNT) ? cols - 1 : cols;

    //Calculate the width and height of the layout area based on 
    //the number of rows and columns
    ch = (int)((w_height - 2 * (*GAPPO) - (rows - 1) * (*GAPPI)) / rows);
    cw = (int)((w_width - 2 * (*GAPPO) - (cols - 1) * (*GAPPI)) / cols);

    //If the nodes do not exactly fill all rows, 
    //the number of Windows in the unfilled rows is
    overcols = NODECOUNT % cols;

    if (overcols)
        dx = (int)((w_width - overcols * cw - (overcols - 1) * (*GAPPI)) / 2) - (*GAPPO);
    for (i = 0, pNode = pTempNodes[0]; pNode; pNode = pTempNodes[i + 1], i++)
    {
        cx = w_x + (i % cols) * (cw + (*GAPPI));
        cy = w_y + (int)(i / cols) * (ch + (*GAPPI));
        if (overcols && i >= (NODECOUNT-overcols))
        {
            cx += dx;
        }
        resizeNodeSizePos(pNode, cx + (*GAPPO), cy + (*GAPPO), cw - 2 * (*PBORDERSIZE), ch - 2 * (*PBORDERSIZE));
    }
    delete[] pTempNodes;
}

void OvGridLayout::recalculateMonitor(const int &monid)
{
    const auto pMonitor = g_pCompositor->getMonitorFromID(monid);                       // 根据monitor id获取monitor对象

    if(!pMonitor || !pMonitor->activeWorkspace)
        return;

    g_pHyprRenderer->damageMonitor(pMonitor); // Use local rendering

    if (pMonitor->activeSpecialWorkspaceID()) {
        calculateWorkspace(pMonitor->activeSpecialWorkspaceID());
        return;
    }

    const auto pWorksapce = g_pCompositor->getWorkspaceByID(pMonitor->activeWorkspaceID()); // 获取当前workspace对象
    if (!pWorksapce)
        return;

    calculateWorkspace(pWorksapce->m_iID); // calculate windwo's size and position
}

// set window's size and position
void OvGridLayout::applyNodeDataToWindow(SOvGridNodeData *pNode)
{ 

    const auto pWindow = pNode->pWindow;

    // force disable decorate and shadow
    // pWindow->m_sSpecialRenderData.decorate = false;
    // pWindow->m_sSpecialRenderData.shadow   = false;

    // force enable bordear and rounding
    pWindow->m_sSpecialRenderData.border   = true;
    pWindow->m_sSpecialRenderData.rounding = true;

    pWindow->m_vSize = pNode->size;
    pWindow->m_vPosition = pNode->position;

    auto calcPos = pWindow->m_vPosition;
    auto calcSize = pWindow->m_vSize;

    pWindow->m_vRealSize = calcSize;
    pWindow->m_vRealPosition = calcPos;
    g_pXWaylandManager->setWindowSize(pWindow, calcSize);

    pWindow->updateWindowDecos();
}

void OvGridLayout::recalculateWindow(PHLWINDOW pWindow)
{
    ; // empty
}


void OvGridLayout::resizeActiveWindow(const Vector2D &pixResize, eRectCorner corner, PHLWINDOW pWindow)
{
    ; // empty
}

void OvGridLayout::fullscreenRequestForWindow(PHLWINDOW pWindow, eFullscreenMode mode, bool on)
{
    ; // empty
}

std::any OvGridLayout::layoutMessage(SLayoutMessageHeader header, std::string content)
{
    return "";
}

SWindowRenderLayoutHints OvGridLayout::requestRenderHints(PHLWINDOW pWindow)
{
    return {};
}

void OvGridLayout::switchWindows(PHLWINDOW pWindowA, PHLWINDOW pWindowB)
{
    ; // empty
}

void OvGridLayout::alterSplitRatio(PHLWINDOW pWindow, float delta, bool exact)
{
    ; // empty
}

std::string OvGridLayout::getLayoutName()
{
    return "ovgrid";
}

void OvGridLayout::replaceWindowDataWith(PHLWINDOW from, PHLWINDOW to)
{
    ; // empty
}

void OvGridLayout::moveWindowTo(PHLWINDOW , const std::string &dir, bool silent)
{
    ; // empty
}

void OvGridLayout::changeToActivceSourceWorkspace()
{
    PHLWINDOW pWindow = nullptr;
    SOvGridNodeData *pNode;
    PHLWORKSPACE pWorksapce;
    hycov_log(LOG,"changeToActivceSourceWorkspace");
    pWindow = g_pCompositor->m_pLastWindow.lock();
    pNode = getNodeFromWindow(pWindow);
    if(pNode) {
        pWorksapce = g_pCompositor->getWorkspaceByID(pNode->ovbk_windowWorkspaceId); 
    } else if(pWindow) {
        pWorksapce = pWindow->m_pWorkspace; 
    } else {
        pWorksapce = g_pCompositor->m_pLastMonitor->activeWorkspace;
    }
    // pMonitor->changeWorkspace(pWorksapce);
    hycov_log(LOG,"changeToWorkspace:{}",pWorksapce->m_iID);
    g_pEventManager->postEvent(SHyprIPCEvent{"workspace", pWorksapce->m_szName});
    EMIT_HOOK_EVENT("workspace", pWorksapce);
}

void OvGridLayout::moveWindowToSourceWorkspace()
{
    PHLWORKSPACE pWorkspace;
    
    hycov_log(LOG,"moveWindowToSourceWorkspace");

    for (auto &nd : m_lOvGridNodesData)
    {
        if (nd.pWindow && (nd.pWindow->m_pWorkspace->m_iID != nd.ovbk_windowWorkspaceId || nd.workspaceName != nd.ovbk_windowWorkspaceName ))
        {
            pWorkspace = g_pCompositor->getWorkspaceByID(nd.ovbk_windowWorkspaceId);
            if (!pWorkspace){
                hycov_log(LOG,"source workspace no exist");
                g_hycov_pSpawnHook->hook(); // disable on-emptty-create workspace rule
                pWorkspace = g_pCompositor->createNewWorkspace(nd.ovbk_windowWorkspaceId,nd.ovbk_windowMonitorId,nd.ovbk_windowWorkspaceName);
                g_hycov_pSpawnHook->unhook();
                hycov_log(LOG,"create workspace: id:{} monitor:{} name:{}",nd.ovbk_windowWorkspaceId,nd.pWindow->m_iMonitorID,nd.ovbk_windowWorkspaceName);
            }
            nd.pWindow->m_iMonitorID = nd.ovbk_windowMonitorId;
            nd.pWindow->m_pWorkspace = pWorkspace;
            nd.workspaceID = nd.ovbk_windowWorkspaceId;
            nd.workspaceName = nd.ovbk_windowWorkspaceName;
            nd.pWindow->m_vPosition = nd.ovbk_position;
            nd.pWindow->m_vSize = nd.ovbk_size;
            g_pHyprRenderer->damageWindow(nd.pWindow);
        }
    }
}

// it will exec once when change layout enable
void OvGridLayout::onEnable()
{

    for (auto &w : g_pCompositor->m_vWindows)
    {        
        PHLWINDOW pWindow = w;

        if (pWindow->isHidden() || !pWindow->m_bIsMapped || pWindow->m_bFadingOut)
            continue;

        if(pWindow->m_iMonitorID != g_pCompositor->m_pLastMonitor->ID && g_hycov_only_active_monitor && !g_hycov_forece_display_all && !g_hycov_forece_display_all_in_one_monitor)
            continue;

        const auto pNode = &m_lSOldLayoutRecordNodeData.emplace_back();
        pNode->pWindow = pWindow;
        onWindowCreatedTiling(pWindow);
    }
}

// it will exec once when change layout disable
void OvGridLayout::onDisable()
{
    dispatch_leaveoverview("");
}
