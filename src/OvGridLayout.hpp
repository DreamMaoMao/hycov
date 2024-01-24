#pragma once

#include <hyprland/src/layout/IHyprLayout.hpp>
#include <hyprland/src/SharedDefs.hpp>

struct SOvGridNodeData
{
  CWindow *pWindow = nullptr;
  int ovbk_windowWorkspaceId = -1;
  std::string ovbk_windowWorkspaceName;
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

  int workspaceID = -1;

  bool operator==(const SOvGridNodeData &rhs) const
  {
    return pWindow == rhs.pWindow;
  }
};

class OvGridLayout : public IHyprLayout
{
public:
  virtual void onWindowCreatedTiling(CWindow *, eDirection direction = DIRECTION_DEFAULT);
  virtual void onWindowRemovedTiling(CWindow *);
  virtual bool isWindowTiled(CWindow *);
  virtual CWindow* getNextWindowCandidate(CWindow*);
  virtual void recalculateMonitor(const int &);
  virtual void recalculateWindow(CWindow *);
  virtual void resizeActiveWindow(const Vector2D &, eRectCorner corner, CWindow *pWindow = nullptr);
  virtual void fullscreenRequestForWindow(CWindow *, eFullscreenMode, bool);
  virtual std::any layoutMessage(SLayoutMessageHeader, std::string);
  virtual SWindowRenderLayoutHints requestRenderHints(CWindow *);
  virtual void switchWindows(CWindow *, CWindow *);
  virtual void alterSplitRatio(CWindow *, float, bool);
  virtual std::string getLayoutName();
  virtual void replaceWindowDataWith(CWindow *from, CWindow *to);
  virtual void moveWindowTo(CWindow *, const std::string &dir);
  virtual void onEnable();
  virtual void onDisable();
  void applyNodeDataToWindow(SOvGridNodeData *);
  void calculateWorkspace(const int &);
  int getNodesNumOnWorkspace(const int &);
  SOvGridNodeData *getNodeFromWindow(CWindow *);
  void resizeNodeSizePos(SOvGridNodeData *, int, int, int, int);
  void moveWindowToWorkspaceSilent(CWindow *, const int &);
  std::list<SOvGridNodeData> m_lGridNodesData; 
  void moveWindowToSourceWorkspace();
  void changeToActivceSourceWorkspace();
  void removeOldLayoutData(CWindow *pWindow);

  bool isFromOnEnable = false;
private:
};