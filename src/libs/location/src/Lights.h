//============================================================================================
//    Spirenkov Maxim aka Sp-Max Shaman, 2001
//--------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------
//    Lights
//--------------------------------------------------------------------------------------------
//
//============================================================================================

#pragma once

#include <set>

#include "collide.h"
#include "dx9render.h"

class Lights : public Entity
{
    // Light source description
    struct LightType
    {
        char *name;
        D3DLIGHT9 dxLight;
        D3DCOLORVALUE color;
        float flicker;
        float flickerSlow;
        float freq;
        float freqSlow;
        float p;
        float pSlow;
        float coronaRange;
        float coronaRange2;
        float invCoronaRange;
        float coronaSize;
        long corona;
    };

    // Source
    struct Light
    {
        D3DCOLORVALUE color; // Current source color
        D3DVECTOR pos;       // Source position
        float time;          // Time since last change of flickering intensity
        float timeSlow;      // Time since the last change in the interpolated intensity
        float itens;         // Shimmering intensity
        float itensSlow;     // Necessary interpolated intensity
        float itensDlt;      // Interpolated intensity difference
        float i;             // Resulting intensity
        float corona;        // Crown transparency

        long type; // Source type index
        uint8_t intensity;
    };

    // Controllable (moving) source
    struct MovingLight
    {
        long id;
        long light;

        MovingLight(long id, long light)
        {
            this->id = id;
            this->light = light;
        }
    };

    struct Vertex
    {
        CVECTOR pos;
        uint32_t color;
        float u, v;
    };

    // To sort by distance
    struct lt_elem
    {
        long idx;
        float dst;
    };

    // --------------------------------------------------------------------------------------------
    // Construction, destruction
    // --------------------------------------------------------------------------------------------
  public:
    Lights();
    ~Lights() override;

    // Initialization
    bool Init() override;
    // Execution
    void Execute(uint32_t delta_time);
    // Drawing crowns
    void Realize(uint32_t delta_time);

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
        case Stage::execute:
            Execute(delta);
            break;
        case Stage::realize:
            Realize(delta);
            break;
            /*case Stage::lost_render:
              LostRender(delta); break;
            case Stage::restore_render:
              RestoreRender(delta); break;*/
        }
    }

    // Find source index
    long FindLight(const char *name);
    // Add source to location
    void AddLight(long index, const CVECTOR &pos);
    // Add lantern model
    bool AddLampModel(entid_t lampModel);
    //
    void DelAllLights();

    // Add portable source
    long AddMovingLight(const char *type, const CVECTOR &pos);
    // Put portable source in new position
    void UpdateMovingLight(long id, const CVECTOR &pos);
    // Remove portable source
    void DelMovingLight(long id);

    // Set lights at pos
    void SetLightsAt(const CVECTOR &pos);
    void UnsetLights();

    // Update source types
    void UpdateLightTypes(long i);

    // --------------------------------------------------------------------------------------------
    // Encapsulation
    // --------------------------------------------------------------------------------------------
  private:

    auto GetLightsAt(const CVECTOR &pos)
    {
        const auto dist = [&pos](const auto &light) {
            const auto dx = (pos.x - light.pos.x);
            const auto dy = (pos.y - light.pos.y);
            const auto dz = (pos.z - light.pos.z);
            return dx * dx + dy * dy + dz * dz + 2.0f;
        };
        const auto cmp = [&](const auto lhs, const auto rhs) { return dist(lights[lhs]) < dist(lights[rhs]); };

        std::set<uint32_t, decltype(cmp)> sorted_lights(cmp);
        for (uint32_t i = 0; i < numLights; i++)
        {
            sorted_lights.insert(i);
        }

        return sorted_lights;
    }

    void PrintDebugInfo();

    constexpr static auto max_d3d_lights = 8U;
    constexpr static auto max_d3d_custom_lights = max_d3d_lights - 1;

    VDX9RENDER *rs;
    COLLIDE *collide;

    // Installed light sources
    struct
    {
        bool set;
        long light;
    } lt[8];

    // Types of light sources
    std::vector<LightType> types;
    long numTypes;
    long maxTypes;
    // Existing lighting sources
    std::vector<Light> lights;
    uint32_t numLights;
    long maxLights;
    long lighter_code;

    // portable light sources
    std::vector<MovingLight> aMovingLight;

    // Lantern models
    entid_t lampModels[16];
    long numLampModels;

    Vertex buf[6 * 1];
};
