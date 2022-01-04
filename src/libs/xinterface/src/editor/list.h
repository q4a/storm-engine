#pragma once

#include "../inode.h"
#include "defines.h"
#include "editor_defines.h"
#include "font.h"

class GIEditor;
class CXI_IMAGE;

class GIEditorList : public GIEditorObject
{
  public:
    GIEditorList(GIEditor *pEditor);
    ~GIEditorList() override;

    void Release();
    void Create();

    void Render();

    void SetPosition(float fLeft, float fTop, float fRight, float fBottom);

    void AddString(std::string &sNewStr);
    void RemoveString(const std::string &sStr);
    void RemoveString(int32_t nIndex);
    void RemoveAllStrings();
    int32_t FindString(const std::string &sStr);

    int32_t GetStringQuantity() const
    {
        return m_aStrings.size();
    }

    std::string &GetString(int32_t nIndex);

    int32_t GetSelectIndex() const
    {
        return m_nSelectIndex;
    }

    void SetSelectIndex(int32_t nIndex);

    std::string &GetSelectString()
    {
        return GetString(m_nSelectIndex);
    }

    bool CheckMouseInside(float fX, float fY) const;
    void MakeMouseClick(float fX, float fY);
    void DoKeyChecking();

    void IncrementSelectedLine(bool bIncr);

  protected:
    void UpdateSelectPosition();

    GIEditor *m_pEditor;

    CXI_IMAGE *m_pBackImage;
    CXI_IMAGE *m_pSelectImage;

    bool m_bShowSelect;
    FXYRECT m_frBackRect;
    FXYRECT m_frSelectRect;

    std::string m_sEmptyString;
    GIFont *m_pFont;

    FXYRECT m_frStrOffset;
    float m_fStrLineStep;
    std::vector<std::string> m_aStrings;
    int32_t m_nSelectIndex;
    int32_t m_nTopIndex;
    int32_t m_nLineQuantity;

    float m_fUpPressTime;
    float m_fDownPressTime;
    float m_fKeyRepeatDelay;
    float m_fKeyRepeatInterval;

  public:
    uint32_t m_dwStatus;
    GIEditorEventHandler *m_pChangeSelected;
};
