//============================================================================================
//    Spirenkov Maxim, 2005
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    WdmWindUI
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include "WdmInterfaceObject.h"

class WdmWindUI : public WdmInterfaceObject
{
    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    WdmWindUI();
    ~WdmWindUI() override;

    void SetAttributes(ATTRIBUTES *apnt);
    // Rendering
    void LRender(VDX9RENDER *rs) override;

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:
    int32_t txBack;
    int32_t txSky, txSkyMask;
    int32_t txBar, txBarMask;
    int32_t txWindPointer;
    int32_t txMorale, txMoraleMask, txMoraleBar;
    int32_t dateFont;
    float morale{};
    int32_t food{};
    std::optional<int32_t> rum;
    char month[12][128];
    int32_t txCoord;
    int32_t txNationFlag;
    float resizeRatio;
};
