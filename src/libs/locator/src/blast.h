#pragma once

#include "Matrix.h"
#include "Sd2_h/CannonTrace.h"
#include "blast.h"
#include "dx9render.h"
#include "geometry.h"
#include "vmodule_api.h"

using GEOPARTICLE = struct
{
    CVECTOR pos;
    CVECTOR ang;
    CVECTOR ang_speed;
    CVECTOR dir;
    GEOS *geo;
    float speed;
    bool bDouble;
    bool bEffect;
};

class BLAST : public Entity
{
    VDX9RENDER *rs;
    VGEOMETRY *gs;
    std::vector<GEOPARTICLE> Item;
    CANNON_TRACE_BASE *pSea;
    entid_t sea_eid;
    float AngleDeviation;
    uint32_t ItemsNum;
    CMatrix Center;
    entid_t Splash;

  public:
    ~BLAST() override;
    BLAST();
    bool Init() override;
    uint64_t ProcessMessage(MESSAGE &message) override;
    uint32_t AttributeChanged(ATTRIBUTES *) override;
    void Realize(uint32_t Delta_Time);
    void ProcessTime(uint32_t Delta_Time);
    void SetBlastCenter(CVECTOR pos, CVECTOR ang);
    void AddGeometry(char *name, int32_t num);

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
            // case Stage::execute:
            //    Execute(delta); break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
                LostRender(delta); break;
            case Stage::restore_render:
                RestoreRender(delta); break;*/
        }
    }
};
