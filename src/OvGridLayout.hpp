#pragma once

#include <hyprland/src/layout/IHyprLayout.hpp>
#include <hyprland/src/SharedDefs.hpp>

struct SOvGridNodeData
{
  PHLWINDOW pWindow = nullptr;
  int ovbk_windowWorkspaceId = -1;
  std::string ovbk_windowWorkspaceName;
  int ovbk_windowMonitorId = -1;
  std::string workspaceName;
  bool ovbk_windowIsFloating = false;
  bool ovbk_windowIsFullscreen = false;
  eFullscreenMode ovbk_windowFullscreenMode ;
  Vector2D ovbk_position;
  Vector2D ovbk_size;
  Vector2D position;
  Vector2D size;
  bool ovbk_windowIsWithBorder;
  bool ovbk_windowIsWithDecorate;
  bool ovbk_windowIsWithRounding;
  bool ovbk_windowIsWithShadow;
  bool isInOldLayout = false;
  bool isGroupActive = false;

  int workspaceID = -1;

  bool operator==(const SOvGridNodeData &rhs) const
  {
    return pWindow == rhs.pWindow;
  }
};


struct SOldLayoutRecordNodeData
{
  PHLWINDOW pWindow = nullptr;
  bool operator==(const SOldLayoutRecordNodeData &rhs) const
  {
    return pWindow == rhs.pWindow;
  }
};


class OvGridLayout : public IHyprLayout
{
public:
  virtual void onWindowCreatedTiling(PHLWINDOW , eDirection direction = DIRECTION_DEFAULT);
  virtual void onWindowRemovedTiling(PHLWINDOW );
  virtual void onWindowRemoved(PHLWINDOW );
  virtual bool isWindowTiled(PHLWINDOW );
  virtual PHLWINDOW getNextWindowCandidate(PHLWINDOW);
  virtual void recalculateMonitor(const int &);
  virtual void recalculateWindow(PHLWINDOW );
  virtual void resizeActiveWindow(const Vector2D &, eRectCorner corner, PHLWINDOW pWindow = nullptr);
  virtual void fullscreenRequestForWindow(PHLWINDOW , eFullscreenMode, bool);
  virtual std::any layoutMessage(SLayoutMessageHeader, std::string);
  virtual SWindowRenderLayoutHints requestRenderHints(PHLWINDOW );
  virtual void switchWindows(PHLWINDOW , PHLWINDOW );
  virtual void alterSplitRatio(PHLWINDOW , float, bool);
  virtual std::string getLayoutName();
  virtual Vector2D predictSizeForNewWindowTiled();
  virtual void replaceWindowDataWith(PHLWINDOW from, PHLWINDOW to);
  virtual void moveWindowTo(PHLWINDOW, const std::string& direction, bool silent = false);
  virtual void onEnable();
  virtual void onDisable();
  void applyNodeDataToWindow(SOvGridNodeData *);
  void calculateWorkspace(const int &);
  int getNodesNumOnWorkspace(const int &);
  SOvGridNodeData *getNodeFromWindow(PHLWINDOW );
  SOldLayoutRecordNodeData *getOldLayoutRecordNodeFromWindow(PHLWINDOW );
  void resizeNodeSizePos(SOvGridNodeData *, int, int, int, int);
  void moveWindowToWorkspaceSilent(PHLWINDOW , const int &);
  std::list<SOvGridNodeData> m_lOvGridNodesData; 
  std::list<SOldLayoutRecordNodeData> m_lSOldLayoutRecordNodeData; 
  void moveWindowToSourceWorkspace();
  void changeToActivceSourceWorkspace();
  void removeOldLayoutData(PHLWINDOW pWindow);
private:
};