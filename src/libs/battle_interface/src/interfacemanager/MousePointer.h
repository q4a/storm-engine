#pragma once

#include "BaseManager.h"
#include <string>

#define BI_CURSORS_QUANTITY 10
#define BI_CURSOR_COMMON 0

class IBIImage;

class MousePointer : public BI_MousePointerBase
{
  public:
    MousePointer(BI_ManagerBase *pManager, ATTRIBUTES *pARoot);
    ~MousePointer() override;

    void Update() override;

  protected:
    BI_ManagerBase *m_pManager;
    ATTRIBUTES *m_pARoot;

    IBIImage *m_pIcon;
    FPOINT m_mousepos;
    FPOINT m_mousesensivity;
    RECT m_cursorzone;

    struct MouseCursorInfo
    {
        LPOINT offset;
        std::string texture;
        FRECT uv;
    };

    MouseCursorInfo m_aCursors[BI_CURSORS_QUANTITY];
    long m_nCurrentCursor;
    LPOINT m_cursorsize;

    void InitMouseCursors();
    void MoveCursor();
    void SetCurrentCursor();
    RECT GetCurrentCursorIconPos() const;
};
