#pragma once
#ifndef _WIN32

#define WARN(...) fprintf(stdout, __VA_ARGS__)

#include <d3d9.h>
#include <algorithm>

#define D3DX_DEFAULT ((UINT)-1)

#define _FACDD 0x876
#define MAKE_DDHRESULT(code) MAKE_HRESULT(1, _FACDD, code)

enum _D3DXERR {
    D3DXERR_CANNOTMODIFYINDEXBUFFER = MAKE_DDHRESULT(2900),
    D3DXERR_INVALIDMESH             = MAKE_DDHRESULT(2901),
    D3DXERR_CANNOTATTRSORT          = MAKE_DDHRESULT(2902),
    D3DXERR_SKINNINGNOTSUPPORTED    = MAKE_DDHRESULT(2903),
    D3DXERR_TOOMANYINFLUENCES       = MAKE_DDHRESULT(2904),
    D3DXERR_INVALIDDATA             = MAKE_DDHRESULT(2905),
    D3DXERR_LOADEDMESHASNODATA      = MAKE_DDHRESULT(2906),
    D3DXERR_DUPLICATENAMEDFRAGMENT  = MAKE_DDHRESULT(2907),
    D3DXERR_CANNOTREMOVELASTITEM    = MAKE_DDHRESULT(2908),
};

// Textures Methods

HRESULT D3DXLoadSurfaceFromMemory(IDirect3DSurface9 *dst_surface,
                                         const PALETTEENTRY *dst_palette, const RECT *dst_rect, const void *src_memory,
                                         D3DFORMAT src_format, UINT src_pitch, const PALETTEENTRY *src_palette, const RECT *src_rect,
                                         DWORD filter, D3DCOLOR color_key);

HRESULT D3DXLoadSurfaceFromSurface(IDirect3DSurface9 *dst_surface,
        const PALETTEENTRY *dst_palette, const RECT *dst_rect, IDirect3DSurface9 *src_surface,
        const PALETTEENTRY *src_palette, const RECT *src_rect, DWORD filter, D3DCOLOR color_key);

#endif
