
#include "globals.hpp"
#include "OvGridLayout.hpp"
#include "dispatchers.hpp"

// find next focus window after remove a window
CWindow *OvGridLayout::getNextWindowCandidate(CWindow* plastWindow) {

    CWindow *targetWindow =  nullptr;
    for (auto &w : g_pCompositor->m_vWindows)
    {
		CWindow *pWindow = w.get();
        if ((g_pCompositor->m_pLastMonitor->specialWorkspaceID != 0 && !g_pCompositor->isWorkspaceSpecial(pWindow->m_iWorkspaceID)) || (g_pCompositor->m_pLastMonitor->specialWorkspaceID == 0 && g_pCompositor->isWorkspaceSpecial(pWindow->m_iWorkspaceID)) || pWindow->m_iWorkspaceID != plastWindow->m_iWorkspaceID || pWindow->isHidden() || !pWindow->m_bIsMapped || pWindow->m_bFadingOut || pWindow->m_bIsFullscreen)
            continue;
		targetWindow = pWindow; // find the last window that is in same workspace with the remove window
    }

    return targetWindow;

}

SOvGridNodeData *OvGridLayout::getNodeFromWindow(CWindow *pWindow)
{
    for (auto &nd : m_lOvGridNodesData)
    {
        if (nd.pWindow == pWindow)
            return &nd;
    }

    return nullptr;
}


SOldLayoutRecordNodeData *OvGridLayout::getOldLayoutRecordNodeFromWindow(CWindow *pWindow)
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

void OvGridLayout::resizeNodeSizePos(SOvGridNodeData *node, int x, int y, int width, int height)
{   
    node->size = Vector2D(width, height - g_hycov_hight_of_titlebar);
    node->position = Vector2D(x, y);
    applyNodeDataToWindow(node);
}

void OvGridLayout::onWindowCreatedTiling(CWindow *pWindow, eDirection direction)
{
    CMonitor *pTargetMonitor;
    if(g_hycov_forece_display_all_in_one_monitor) {
        pTargetMonitor = g_pCompositor->m_pLastMonitor;
    } else {
      pTargetMonitor =  g_pCompositor->getMonitorFromID(pWindow->m_iMonitorID); 
    }

    const auto pNode = &m_lOvGridNodesData.emplace_back(); // make a new node in list back

    const auto pActiveWorkspace = g_pCompositor->getWorkspaceByID(pTargetMonitor->activeWorkspace); 

    const auto pWindowOriWorkspace = g_pCompositor->getWorkspaceByID(pWindow->m_iWorkspaceID);


    auto oldLayoutRecordNode = getOldLayoutRecordNodeFromWindow(pWindow);
    if(oldLayoutRecordNode) {
        pNode->isInOldLayout = true; //client is taken from the old layout
        m_lSOldLayoutRecordNodeData.remove(*oldLayoutRecordNode);
    }

    pNode->workspaceID = pWindow->m_iWorkspaceID; // encapsulate window objects as node objects to bind more properties
    pNode->pWindow = pWindow;
    pNode->workspaceName = pWindowOriWorkspace->m_szName;
    
    //record the window stats which are used by restore
    pNode->ovbk_windowMonitorId = pWindow->m_iMonitorID;
    pNode->ovbk_windowWorkspaceId = pWindow->m_iWorkspaceID;
    pNode->ovbk_windowFullscreenMode  = pWindowOriWorkspace->m_efFullscreenMode;
    pNode->ovbk_position = pWindow->m_vRealPosition.goalv();
    pNode->ovbk_size = pWindow->m_vRealSize.goalv();
    pNode->ovbk_windowIsFloating = pWindow->m_bIsFloating;
    pNode->ovbk_windowIsFullscreen = pWindow->m_bIsFullscreen;
    pNode->ovbk_windowWorkspaceName = pWindowOriWorkspace->m_szName;

    //record the window style which are used by restore
    pNode->ovbk_windowIsWithBorder = pWindow->m_sSpecialRenderData.border;
    pNode->ovbk_windowIsWithDecorate = pWindow->m_sSpecialRenderData.decorate;
    pNode->ovbk_windowIsWithRounding = pWindow->m_sSpecialRenderData.rounding;
    pNode->ovbk_windowIsWithShadow = pWindow->m_sSpecialRenderData.shadow;


    //change all client(exclude special workspace) to active worksapce 
    if ( !g_pCompositor->isWorkspaceSpecial(pWindow->m_iWorkspaceID) && pNode->isInOldLayout && (pWindowOriWorkspace->m_iID != pActiveWorkspace->m_iID || pWindowOriWorkspace->m_szName != pActiveWorkspace->m_szName) && (!g_hycov_only_active_workspace || g_hycov_forece_display_all || g_hycov_forece_display_all_in_one_monitor))    {
        pNode->workspaceID = pWindow->m_iWorkspaceID = pActiveWorkspace->m_iID;
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


void OvGridLayout::removeOldLayoutData(CWindow *pWindow) { 

    switchToLayoutWithoutReleaseData(g_hycov_configLayoutName);
    hycov_log(LOG,"remove data of old layout:{}",g_hycov_configLayoutName);

    if(g_hycov_configLayoutName == "dwindle") {
        // disable render client of old layout
        g_hycov_pHyprDwindleLayout_recalculateMonitor->hook();
        g_hycov_pHyprDwindleLayout_recalculateWindow->hook();
        g_hycov_pSDwindleNodeData_recalcSizePosRecursive->hook();

        // only remove data,not render anything,becaust still in overview
        g_pLayoutManager->getCurrentLayout()->onWindowRemovedTiling(pWindow);

        g_hycov_pSDwindleNodeData_recalcSizePosRecursive->unhook();
        g_hycov_pHyprDwindleLayout_recalculateWindow->unhook();
        g_hycov_pHyprDwindleLayout_recalculateMonitor->unhook();
    } else if(g_hycov_configLayoutName == "master") {
        g_hycov_pHyprMasterLayout_recalculateMonitor->hook();

        g_pLayoutManager->getCurrentLayout()->onWindowRemovedTiling(pWindow);

        g_hycov_pHyprMasterLayout_recalculateMonitor->unhook();
    } else {
        // may be not support other layout
        hycov_log(ERR,"unknow old layout:{}",g_hycov_configLayoutName);
        g_pLayoutManager->getCurrentLayout()->onWindowRemovedTiling(pWindow);
    }

    switchToLayoutWithoutReleaseData("ovgrid");
}

void OvGridLayout::onWindowRemovedTiling(CWindow *pWindow)
{
    hycov_log(LOG,"remove tiling windwo:{}",pWindow);

    const auto pNode = getNodeFromWindow(pWindow);

    if (!pNode)
        return;

    if(pNode->isInOldLayout) { // if client is taken from the old layout
        removeOldLayoutData(pWindow);
    }

    m_lOvGridNodesData.remove(*pNode);

    if(m_lOvGridNodesData.empty()){
        return;
    }

    recalculateMonitor(pWindow->m_iMonitorID);

}

bool OvGridLayout::isWindowTiled(CWindow *pWindow)
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
        cw = (w_width - 2 * (g_hycov_overview_gappo)) * 0.7;
        ch = (w_height - 2 * (g_hycov_overview_gappo)) * 0.8;
        resizeNodeSizePos(pNode, w_x + (int)((m_width - cw) / 2), w_y + (int)((w_height - ch) / 2),
                          cw - 2 * (g_hycov_bordersize), ch - 2 * (g_hycov_bordersize));
        delete[] pTempNodes;
        return;
    }

    // two client arrange
    if (NODECOUNT == 2)
    {
        pNode = pTempNodes[0];
        cw = (w_width - 2 * (g_hycov_overview_gappo) - (g_hycov_overview_gappi)) / 2;
        ch = (w_height - 2 * (g_hycov_overview_gappo)) * 0.65;
        resizeNodeSizePos(pNode, m_x + cw + (g_hycov_overview_gappo) + (g_hycov_overview_gappi), m_y + (m_height - ch) / 2 + (g_hycov_overview_gappo),
                          cw - 2 * (g_hycov_bordersize), ch - 2 * (g_hycov_bordersize));
        resizeNodeSizePos(pTempNodes[1], m_x + (g_hycov_overview_gappo), m_y + (m_height - ch) / 2 + (g_hycov_overview_gappo),
                          cw - 2 * (g_hycov_bordersize), ch - 2 * (g_hycov_bordersize));
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
    ch = (int)((w_height - 2 * (g_hycov_overview_gappo) - (rows - 1) * (g_hycov_overview_gappi)) / rows);
    cw = (int)((w_width - 2 * (g_hycov_overview_gappo) - (cols - 1) * (g_hycov_overview_gappi)) / cols);

    //If the nodes do not exactly fill all rows, 
    //the number of Windows in the unfilled rows is
    overcols = NODECOUNT % cols;

    if (overcols)
        dx = (int)((w_width - overcols * cw - (overcols - 1) * (g_hycov_overview_gappi)) / 2) - (g_hycov_overview_gappo);
    for (i = 0, pNode = pTempNodes[0]; pNode; pNode = pTempNodes[i + 1], i++)
    {
        cx = w_x + (i % cols) * (cw + (g_hycov_overview_gappi));
        cy = w_y + (int)(i / cols) * (ch + (g_hycov_overview_gappi));
        if (overcols && i >= (NODECOUNT-overcols))
        {
            cx += dx;
        }
        resizeNodeSizePos(pNode, cx + (g_hycov_overview_gappo), cy + (g_hycov_overview_gappo), cw - 2 * (g_hycov_bordersize), ch - 2 * (g_hycov_bordersize));
    }
    delete[] pTempNodes;
}

void OvGridLayout::recalculateMonitor(const int &monid)
{
    const auto pMonitor = g_pCompositor->getMonitorFromID(monid);                       // 根据monitor id获取monitor对象
    g_pHyprRenderer->damageMonitor(pMonitor); // Use local rendering

    if (pMonitor->specialWorkspaceID) {
        calculateWorkspace(pMonitor->specialWorkspaceID);
        return;
    }

    const auto pWorksapce = g_pCompositor->getWorkspaceByID(pMonitor->activeWorkspace); // 获取当前workspace对象
    if (!pWorksapce)
        return;

    calculateWorkspace(pWorksapce->m_iID); // calculate windwo's size and position
}

// set window's size and position
void OvGridLayout::applyNodeDataToWindow(SOvGridNodeData *pNode)
{ 

    const auto pWindow = pNode->pWindow;

    const auto WORKSPACERULE = g_pConfigManager->getWorkspaceRuleFor(g_pCompositor->getWorkspaceByID(pWindow->m_iWorkspaceID));
    pWindow->updateSpecialRenderData();
    // force disable decorate and shadow
    // pWindow->m_sSpecialRenderData.decorate = false;
    // pWindow->m_sSpecialRenderData.shadow   = false;

    // force enable bordear and rounding
    pWindow->m_sSpecialRenderData.border   = true;
    pWindow->m_sSpecialRenderData.rounding = true;
    pWindow->m_sSpecialRenderData.borderSize = WORKSPACERULE.borderSize.value_or(-1);

    pWindow->m_vSize = pNode->size;
    pWindow->m_vPosition = pNode->position;

    auto calcPos = pWindow->m_vPosition;
    auto calcSize = pWindow->m_vSize;

    pWindow->m_vRealSize = calcSize;
    pWindow->m_vRealPosition = calcPos;
    g_pXWaylandManager->setWindowSize(pWindow, calcSize);

    pWindow->updateWindowDecos();
}

void OvGridLayout::recalculateWindow(CWindow *pWindow)
{
    ; // empty
}


void OvGridLayout::resizeActiveWindow(const Vector2D &pixResize, eRectCorner corner, CWindow *pWindow)
{
    ; // empty
}

void OvGridLayout::fullscreenRequestForWindow(CWindow *pWindow, eFullscreenMode mode, bool on)
{
    ; // empty
}

std::any OvGridLayout::layoutMessage(SLayoutMessageHeader header, std::string content)
{
    return "";
}

SWindowRenderLayoutHints OvGridLayout::requestRenderHints(CWindow *pWindow)
{
    return {};
}

void OvGridLayout::switchWindows(CWindow *pWindowA, CWindow *pWindowB)
{
    ; // empty
}

void OvGridLayout::alterSplitRatio(CWindow *pWindow, float delta, bool exact)
{
    ; // empty
}

std::string OvGridLayout::getLayoutName()
{
    return "ovgrid";
}

void OvGridLayout::replaceWindowDataWith(CWindow *from, CWindow *to)
{
    ; // empty
}

void OvGridLayout::moveWindowTo(CWindow *, const std::string &dir)
{
    ; // empty
}

void OvGridLayout::changeToActivceSourceWorkspace()
{
    CWindow *pWindow = nullptr;
    SOvGridNodeData *pNode;
    CWorkspace *pWorksapce;
    hycov_log(LOG,"changeToActivceSourceWorkspace");
    pWindow = g_pCompositor->m_pLastWindow;
    pNode = getNodeFromWindow(pWindow);
    if(pNode) {
        pWorksapce = g_pCompositor->getWorkspaceByID(pNode->ovbk_windowWorkspaceId); 
    } else if(pWindow) {
        pWorksapce = g_pCompositor->getWorkspaceByID(pWindow->m_iWorkspaceID); 
    } else {
        pWorksapce = g_pCompositor->getWorkspaceByID(g_pCompositor->m_pLastMonitor->activeWorkspace);
    }
    // pMonitor->changeWorkspace(pWorksapce);
    hycov_log(LOG,"changeToWorkspace:{}",pWorksapce->m_iID);
    g_pEventManager->postEvent(SHyprIPCEvent{"workspace", pWorksapce->m_szName});
    EMIT_HOOK_EVENT("workspace", pWorksapce);
}

void OvGridLayout::moveWindowToSourceWorkspace()
{
    CWorkspace *pWorkspace;
    
    hycov_log(LOG,"moveWindowToSourceWorkspace");

    for (auto &nd : m_lOvGridNodesData)
    {
        if (nd.pWindow && (nd.pWindow->m_iWorkspaceID != nd.ovbk_windowWorkspaceId || nd.workspaceName != nd.ovbk_windowWorkspaceName ))
        {
            pWorkspace = g_pCompositor->getWorkspaceByID(nd.ovbk_windowWorkspaceId);
            if (!pWorkspace){
                hycov_log(LOG,"source workspace no exist");
                pWorkspace = g_pCompositor->createNewWorkspace(nd.ovbk_windowWorkspaceId,nd.ovbk_windowMonitorId,nd.ovbk_windowWorkspaceName);
                hycov_log(LOG,"create workspace: id:{} monitor:{} name:{}",nd.ovbk_windowWorkspaceId,nd.pWindow->m_iMonitorID,nd.ovbk_windowWorkspaceName);
            }
            nd.pWindow->m_iMonitorID = nd.ovbk_windowMonitorId;
            nd.workspaceID = nd.pWindow->m_iWorkspaceID = nd.ovbk_windowWorkspaceId;
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
        CWindow *pWindow = w.get();

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