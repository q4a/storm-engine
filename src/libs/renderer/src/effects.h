#pragma once

#include <cstdint>
#include <d3d9.h>
#ifdef _WIN32 // FIX_LINUX ID3DXEffect
#include <d3dx9.h>
#endif
#include <unordered_map>
#include <vector>

class Effects final
{
  private:
    struct Technique
    {
#ifdef _WIN32 // FIX_LINUX ID3DXEffect
        Technique(ID3DXEffect *fx, D3DXHANDLE handle, D3DXTECHNIQUE_DESC desc) : fx(fx), handle(handle), desc(desc)
        {
        }

        ID3DXEffect *fx;
        D3DXHANDLE handle;
        D3DXTECHNIQUE_DESC desc;
#endif
    };

    IDirect3DDevice9 *device_;

#ifdef _WIN32 // FIX_LINUX ID3DXEffect
    std::vector<ID3DXEffect *> effects_;
#endif
    std::unordered_map<std::string, Technique> techniques_;

    const Technique *currentTechnique_;
    uint32_t currentPass_;

    std::string_view debugMsg_;

    inline bool ErrorHandler(HRESULT hr, const char *file, unsigned line, const char *func, const char *expr) const;

  public:
    Effects(Effects &) = delete;
    Effects(Effects &&) = delete;
    Effects &operator=(Effects &) = delete;
    Effects &operator=(Effects &&) = delete;

    explicit Effects(IDirect3DDevice9 *d3dDevice = nullptr);
    ~Effects();
    // Set device
    void setDevice(IDirect3DDevice9 *device);
    // Compile effect by path
    void compile(const char *fxPath);
    // Release all effects
    void release();
    // Begin technique
    bool begin(const std::string &techniqueName);
    // Execute next technique
    bool next();
#ifdef _WIN32 // FIX_LINUX ID3DXEffect
    // Get effect pointer by technique name
    ID3DXEffect *getEffectPointer(const std::string &techniqueName);
#endif
};
