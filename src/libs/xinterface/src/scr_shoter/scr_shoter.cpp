#include <algorithm>

#include "scr_shoter.h"
#include "../xdefines.h"

#include "core.h"
#include <DxErr.h>

#define SS_TEXTURE_WIDTH 128
#define SS_TEXTURE_HEIGHT 128
#define SS_TEXTURE_FONECOLOR 0xFF000000

uint32_t GetA8R8G8B8_FromFMT(void *p, uint32_t fmt)
{
    uint32_t retVal;

    if (fmt == D3DFMT_R5G6B5)
    {
        retVal = 0xFF000000 | (static_cast<uint32_t>(*static_cast<uint16_t *>(p) & 0xF800) << 8) |
                 (static_cast<uint32_t>(*static_cast<uint16_t *>(p) & 0x7E0) << 5) |
                 (static_cast<uint32_t>(*static_cast<uint16_t *>(p) & 0x1F) << 3);
    }
    else
    {
        retVal = 0xFF000000 | (*static_cast<uint32_t *>(p));
    }

    return retVal;
}

SCRSHOTER::~SCRSHOTER()
{
    if (textureIndex_ != -1 && rs != nullptr)
        rs->TextureRelease(textureIndex_);
    texture_ = nullptr;
    SAVETEXTURES *pst;
    while (m_list)
    {
        pst = m_list;
        m_list = pst->next;
        delete pst->fileName;
        if (pst->textureId != -1 && rs != nullptr)
            rs->TextureRelease(pst->textureId);
        delete pst;
    }
}

bool SCRSHOTER::Init()
{
    // GUARD(SCRSHOTER::Init())
    SetDevice();
    // UNGUARD
    return true;
}

void SCRSHOTER::SetDevice()
{
    // get render service
    rs = static_cast<VDX9RENDER *>(core.GetService("dx9render"));
    if (!rs)
        throw std::runtime_error("No service: dx9render");
}

void SCRSHOTER::Execute(uint32_t Delta_Time)
{
}

void SCRSHOTER::Realize(uint32_t Delta_Time)
{
    if (texture_ == nullptr)
        if (!MakeScreenShot())
        {
            core.Trace("ERROR!!! screen shot create error");
            core.Event("makescrshot");
        }
        else
            core.Event("makescrshot");
}

#define WARN(...) fprintf(stdout, __VA_ARGS__)
#define FIXME WARN
#define TRACE WARN

///////////////////////// Parts from WINE source code for d3dx9 licensed under GPLv2 /////////////////////////

//#define D3DX_FILTER_NONE 0x00000001
//#define D3DX_FILTER_POINT 0x00000002
//#define D3DX_FILTER_LINEAR 0x00000003
//#define D3DX_FILTER_TRIANGLE 0x00000004
//#define D3DX_FILTER_DITHER 0x00080000

//#define D3DXERR_INVALIDDATA 0x88760b59

struct vec4
{
    float x, y, z, w;
};

struct volume
{
    UINT width;
    UINT height;
    UINT depth;
};

enum format_type
{
    FORMAT_ARGB,    /* unsigned */
    FORMAT_ARGBF16, /* float 16 */
    FORMAT_ARGBF,   /* float */
    FORMAT_DXT,
    FORMAT_INDEX,
    FORMAT_UNKNOWN
};

struct pixel_format_desc
{
    D3DFORMAT format;
    BYTE bits[4];
    BYTE shift[4];
    UINT bytes_per_pixel;
    UINT block_width;
    UINT block_height;
    UINT block_byte_count;
    enum format_type type;
    void (*from_rgba)(const struct vec4 *src, struct vec4 *dst);
    void (*to_rgba)(const struct vec4 *src, struct vec4 *dst, const PALETTEENTRY *palette);
};

/************************************************************
 * pixel format table providing info about number of bytes per pixel,
 * number of bits per channel and format type.
 *
 * Call get_format_info to request information about a specific format.
 */
static const struct pixel_format_desc formats[] = {
    /* format              bpc               shifts             bpp blocks   type            from_rgba     to_rgba */
    {D3DFMT_R8G8B8, {0, 8, 8, 8}, {0, 16, 8, 0}, 3, 1, 1, 3, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_A8R8G8B8, {8, 8, 8, 8}, {24, 16, 8, 0}, 4, 1, 1, 4, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_X8R8G8B8, {0, 8, 8, 8}, {0, 16, 8, 0}, 4, 1, 1, 4, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_A8B8G8R8, {8, 8, 8, 8}, {24, 0, 8, 16}, 4, 1, 1, 4, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_X8B8G8R8, {0, 8, 8, 8}, {0, 0, 8, 16}, 4, 1, 1, 4, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_R5G6B5, {0, 5, 6, 5}, {0, 11, 5, 0}, 2, 1, 1, 2, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_X1R5G5B5, {0, 5, 5, 5}, {0, 10, 5, 0}, 2, 1, 1, 2, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_A1R5G5B5, {1, 5, 5, 5}, {15, 10, 5, 0}, 2, 1, 1, 2, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_R3G3B2, {0, 3, 3, 2}, {0, 5, 2, 0}, 1, 1, 1, 1, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_A8R3G3B2, {8, 3, 3, 2}, {8, 5, 2, 0}, 2, 1, 1, 2, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_A4R4G4B4, {4, 4, 4, 4}, {12, 8, 4, 0}, 2, 1, 1, 2, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_X4R4G4B4, {0, 4, 4, 4}, {0, 8, 4, 0}, 2, 1, 1, 2, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_A2R10G10B10, {2, 10, 10, 10}, {30, 20, 10, 0}, 4, 1, 1, 4, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_A2B10G10R10, {2, 10, 10, 10}, {30, 0, 10, 20}, 4, 1, 1, 4, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_A16B16G16R16, {16, 16, 16, 16}, {48, 0, 16, 32}, 8, 1, 1, 8, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_G16R16, {0, 16, 16, 0}, {0, 0, 16, 0}, 4, 1, 1, 4, FORMAT_ARGB, NULL, NULL},
    {D3DFMT_A8, {8, 0, 0, 0}, {0, 0, 0, 0}, 1, 1, 1, 1, FORMAT_ARGB, NULL, NULL},
    //{D3DFMT_A8L8,          { 8,  8,  0,  0}, { 8,  0,  0,  0},  2, 1, 1,  2, FORMAT_ARGB,    la_from_rgba,
    //la_to_rgba}, {D3DFMT_A4L4,          { 4,  4,  0,  0}, { 4,  0,  0,  0},  1, 1, 1,  1, FORMAT_ARGB, la_from_rgba,
    //la_to_rgba}, {D3DFMT_L8,            { 0,  8,  0,  0}, { 0,  0,  0,  0},  1, 1, 1,  1, FORMAT_ARGB, la_from_rgba,
    //la_to_rgba}, {D3DFMT_L16,           { 0, 16,  0,  0}, { 0,  0,  0,  0},  2, 1, 1,  2, FORMAT_ARGB, la_from_rgba,
    //la_to_rgba},
    {D3DFMT_DXT1, {0, 0, 0, 0}, {0, 0, 0, 0}, 1, 4, 4, 8, FORMAT_DXT, NULL, NULL},
    {D3DFMT_DXT2, {0, 0, 0, 0}, {0, 0, 0, 0}, 1, 4, 4, 16, FORMAT_DXT, NULL, NULL},
    {D3DFMT_DXT3, {0, 0, 0, 0}, {0, 0, 0, 0}, 1, 4, 4, 16, FORMAT_DXT, NULL, NULL},
    {D3DFMT_DXT4, {0, 0, 0, 0}, {0, 0, 0, 0}, 1, 4, 4, 16, FORMAT_DXT, NULL, NULL},
    {D3DFMT_DXT5, {0, 0, 0, 0}, {0, 0, 0, 0}, 1, 4, 4, 16, FORMAT_DXT, NULL, NULL},
    {D3DFMT_R16F, {0, 16, 0, 0}, {0, 0, 0, 0}, 2, 1, 1, 2, FORMAT_ARGBF16, NULL, NULL},
    {D3DFMT_G16R16F, {0, 16, 16, 0}, {0, 0, 16, 0}, 4, 1, 1, 4, FORMAT_ARGBF16, NULL, NULL},
    {D3DFMT_A16B16G16R16F, {16, 16, 16, 16}, {48, 0, 16, 32}, 8, 1, 1, 8, FORMAT_ARGBF16, NULL, NULL},
    {D3DFMT_R32F, {0, 32, 0, 0}, {0, 0, 0, 0}, 4, 1, 1, 4, FORMAT_ARGBF, NULL, NULL},
    {D3DFMT_G32R32F, {0, 32, 32, 0}, {0, 0, 32, 0}, 8, 1, 1, 8, FORMAT_ARGBF, NULL, NULL},
    {D3DFMT_A32B32G32R32F, {32, 32, 32, 32}, {96, 0, 32, 64}, 16, 1, 1, 16, FORMAT_ARGBF, NULL, NULL},
    //{D3DFMT_P8,            { 8,  8,  8,  8}, { 0,  0,  0,  0},  1, 1, 1,  1, FORMAT_INDEX,   NULL, index_to_rgba},
    /* marks last element */
    {D3DFMT_UNKNOWN, {0, 0, 0, 0}, {0, 0, 0, 0}, 0, 1, 1, 0, FORMAT_UNKNOWN, NULL, NULL},
};

/************************************************************
 * get_format_info
 *
 * Returns information about the specified format.
 * If the format is unsupported, it's filled with the D3DFMT_UNKNOWN desc.
 *
 * PARAMS
 *   format [I] format whose description is queried
 *
 */
const struct pixel_format_desc *get_format_info(D3DFORMAT format)
{
    unsigned int i = 0;
    while (formats[i].format != format && formats[i].format != D3DFMT_UNKNOWN)
        i++;
    if (formats[i].format == D3DFMT_UNKNOWN)
    {
        WARN("Unknown format %#x.\n", format);
    }
    return &formats[i];
}

static inline BOOL SetRect(RECT *rect, INT left, INT top, INT right, INT bottom)
{
    if (!rect)
        return FALSE;
    rect->left = left;
    rect->right = right;
    rect->top = top;
    rect->bottom = bottom;
    return TRUE;
}

HRESULT lock_surface(IDirect3DSurface9 *surface, const RECT *surface_rect, D3DLOCKED_RECT *lock,
                     IDirect3DSurface9 **temp_surface, bool write)
{
    unsigned int width, height;
    IDirect3DDevice9 *device;
    D3DSURFACE_DESC desc;
    DWORD lock_flag;
    HRESULT hr;

    lock_flag = write ? 0 : D3DLOCK_READONLY;
    *temp_surface = NULL;
    if (FAILED(hr = IDirect3DSurface9_LockRect(surface, lock, surface_rect, lock_flag)))
    {
        IDirect3DSurface9_GetDevice(surface, &device);
        IDirect3DSurface9_GetDesc(surface, &desc);

        if (surface_rect)
        {
            width = surface_rect->right - surface_rect->left;
            height = surface_rect->bottom - surface_rect->top;
        }
        else
        {
            width = desc.Width;
            height = desc.Height;
        }

        hr = write ? IDirect3DDevice9_CreateOffscreenPlainSurface(device, width, height, desc.Format, D3DPOOL_SYSTEMMEM,
                                                                  temp_surface, NULL)
                   : IDirect3DDevice9_CreateRenderTarget(device, width, height, desc.Format, D3DMULTISAMPLE_NONE, 0,
                                                         TRUE, temp_surface, NULL);
        if (FAILED(hr))
        {
            WARN("Failed to create temporary surface, surface %p, format %#x,"
                 " usage %#x, pool %#x, write %#x, width %u, height %u.\n",
                 surface, desc.Format, desc.Usage, desc.Pool, write, width, height);
            IDirect3DDevice9_Release(device);
            return hr;
        }

        if (write || SUCCEEDED(hr = IDirect3DDevice9_StretchRect(device, surface, surface_rect, *temp_surface, NULL,
                                                                 D3DTEXF_NONE)))
            hr = IDirect3DSurface9_LockRect(*temp_surface, lock, NULL, lock_flag);

        IDirect3DDevice9_Release(device);
        if (FAILED(hr))
        {
            WARN("Failed to lock surface %p, write %#x, usage %#x, pool %#x.\n", surface, write, desc.Usage, desc.Pool);
            IDirect3DSurface9_Release(*temp_surface);
            *temp_surface = NULL;
            return hr;
        }
    }
    return hr;
}

HRESULT unlock_surface(IDirect3DSurface9 *surface, const RECT *surface_rect, IDirect3DSurface9 *temp_surface,
                       bool update)
{
    IDirect3DDevice9 *device;
    POINT surface_point;
    HRESULT hr;

    if (!temp_surface)
    {
        hr = IDirect3DSurface9_UnlockRect(surface);
        return hr;
    }

    hr = IDirect3DSurface9_UnlockRect(temp_surface);
    if (update)
    {
        if (surface_rect)
        {
            surface_point.x = surface_rect->left;
            surface_point.y = surface_rect->top;
        }
        else
        {
            surface_point.x = 0;
            surface_point.y = 0;
        }
        IDirect3DSurface9_GetDevice(surface, &device);
        if (FAILED(hr = IDirect3DDevice9_UpdateSurface(device, temp_surface, NULL, surface, &surface_point)))
            WARN("Updating surface failed, hr %#x, surface %p, temp_surface %p.\n", hr, surface, temp_surface);
        IDirect3DDevice9_Release(device);
    }
    IDirect3DSurface9_Release(temp_surface);
    return hr;
}

/************************************************************
 * copy_pixels
 *
 * Copies the source buffer to the destination buffer.
 * Works for any pixel format.
 * The source and the destination must be block-aligned.
 */
void copy_pixels(const BYTE *src, UINT src_row_pitch, UINT src_slice_pitch, BYTE *dst, UINT dst_row_pitch,
                 UINT dst_slice_pitch, const struct volume *size, const struct pixel_format_desc *format)
{
    UINT row, slice;
    BYTE *dst_addr;
    const BYTE *src_addr;
    UINT row_block_count = (size->width + format->block_width - 1) / format->block_width;
    UINT row_count = (size->height + format->block_height - 1) / format->block_height;

    for (slice = 0; slice < size->depth; slice++)
    {
        src_addr = src + slice * src_slice_pitch;
        dst_addr = dst + slice * dst_slice_pitch;

        for (row = 0; row < row_count; row++)
        {
            memcpy(dst_addr, src_addr, row_block_count * format->block_byte_count);
            src_addr += src_row_pitch;
            dst_addr += dst_row_pitch;
        }
    }
}

static inline bool is_conversion_from_supported(const struct pixel_format_desc *format)
{
    if (format->type == FORMAT_ARGB || format->type == FORMAT_ARGBF16 || format->type == FORMAT_ARGBF ||
        format->type == FORMAT_DXT)
        return TRUE;
    return !!format->to_rgba;
}

static inline bool is_conversion_to_supported(const struct pixel_format_desc *format)
{
    if (format->type == FORMAT_ARGB || format->type == FORMAT_ARGBF16 || format->type == FORMAT_ARGBF ||
        format->type == FORMAT_DXT)
        return TRUE;
    return !!format->from_rgba;
}

/************************************************************
 * helper functions for D3DXLoadSurfaceFromMemory
 */
struct argb_conversion_info
{
    const struct pixel_format_desc *srcformat;
    const struct pixel_format_desc *destformat;
    DWORD srcshift[4], destshift[4];
    DWORD srcmask[4], destmask[4];
    BOOL process_channel[4];
    DWORD channelmask;
};

#ifndef NOMINMAX
#define NOMINMAX
#undef min
#undef max
#endif

inline int min(int x, int y)
{
    return x < y ? x : y;
}
inline float min(float x, float y)
{
    return x < y ? x : y;
}
inline double min(double x, double y)
{
    return x < y ? x : y;
}

inline int max(int x, int y)
{
    return x > y ? x : y;
}
inline float max(float x, float y)
{
    return x > y ? x : y;
}
inline double max(double x, double y)
{
    return x > y ? x : y;
}

static void init_argb_conversion_info(const struct pixel_format_desc *srcformat,
                                      const struct pixel_format_desc *destformat, struct argb_conversion_info *info)
{
    UINT i;
    // ZeroMemory(info->process_channel, 4 * sizeof(BOOL));
    memset(info->process_channel, 0, 4 * sizeof(bool));
    info->channelmask = 0;

    info->srcformat = srcformat;
    info->destformat = destformat;

    for (i = 0; i < 4; i++)
    {
        /* srcshift is used to extract the _relevant_ components */
        info->srcshift[i] = srcformat->shift[i] + max(srcformat->bits[i] - destformat->bits[i], 0);

        /* destshift is used to move the components to the correct position */
        info->destshift[i] = destformat->shift[i] + max(destformat->bits[i] - srcformat->bits[i], 0);

        info->srcmask[i] = ((1 << srcformat->bits[i]) - 1) << srcformat->shift[i];
        info->destmask[i] = ((1 << destformat->bits[i]) - 1) << destformat->shift[i];

        /* channelmask specifies bits which aren't used in the source format but in the destination one */
        if (destformat->bits[i])
        {
            if (srcformat->bits[i])
                info->process_channel[i] = TRUE;
            else
                info->channelmask |= info->destmask[i];
        }
    }
}

/************************************************************
 * get_relevant_argb_components
 *
 * Extracts the relevant components from the source color and
 * drops the less significant bits if they aren't used by the destination format.
 */
static void get_relevant_argb_components(const struct argb_conversion_info *info, const BYTE *col, DWORD *out)
{
    unsigned int i, j;
    unsigned int component, mask;

    for (i = 0; i < 4; ++i)
    {
        if (!info->process_channel[i])
            continue;

        component = 0;
        mask = info->srcmask[i];
        for (j = 0; j < 4 && mask; ++j)
        {
            if (info->srcshift[i] < j * 8)
                component |= (col[j] & mask) << (j * 8 - info->srcshift[i]);
            else
                component |= (col[j] & mask) >> (info->srcshift[i] - j * 8);
            mask >>= 8;
        }
        out[i] = component;
    }
}

/************************************************************
 * make_argb_color
 *
 * Recombines the output of get_relevant_argb_components and converts
 * it to the destination format.
 */
static DWORD make_argb_color(const struct argb_conversion_info *info, const DWORD *in)
{
    UINT i;
    DWORD val = 0;

    for (i = 0; i < 4; i++)
    {
        if (info->process_channel[i])
        {
            /* necessary to make sure that e.g. an X4R4G4B4 white maps to an R8G8B8 white instead of 0xf0f0f0 */
            signed int shift;
            for (shift = info->destshift[i]; shift > info->destformat->shift[i]; shift -= info->srcformat->bits[i])
                val |= in[i] << shift;
            val |= (in[i] >> (info->destformat->shift[i] - shift)) << info->destformat->shift[i];
        }
    }
    val |= info->channelmask; /* new channels are set to their maximal value */
    return val;
}

/* It doesn't work for components bigger than 32 bits (or somewhat smaller but unaligned). */
static void format_to_vec4(const struct pixel_format_desc *format, const BYTE *src, struct vec4 *dst)
{
    DWORD mask, tmp;
    unsigned int c;

    for (c = 0; c < 4; ++c)
    {
        static const unsigned int component_offsets[4] = {3, 0, 1, 2};
        float *dst_component = (float *)dst + component_offsets[c];

        if (format->bits[c])
        {
            mask = ~0u >> (32 - format->bits[c]);

            memcpy(&tmp, src + format->shift[c] / 8,
                   min(sizeof(DWORD), (format->shift[c] % 8 + format->bits[c] + 7) / 8));

            if (format->type == FORMAT_ARGBF16)
                WARN("Unimplemented for FORMAT_ARGBF16");
            //*dst_component = float_16_to_32(tmp);
            else if (format->type == FORMAT_ARGBF)
                *dst_component = *(float *)&tmp;
            else
                *dst_component = (float)((tmp >> format->shift[c] % 8) & mask) / mask;
        }
        else
            *dst_component = 1.0f;
    }
}

/* It doesn't work for components bigger than 32 bits. */
static void format_from_vec4(const struct pixel_format_desc *format, const struct vec4 *src, BYTE *dst)
{
    DWORD v, mask32;
    unsigned int c, i;

    memset(dst, 0, format->bytes_per_pixel);

    for (c = 0; c < 4; ++c)
    {
        static const unsigned int component_offsets[4] = {3, 0, 1, 2};
        const float src_component = *((const float *)src + component_offsets[c]);

        if (!format->bits[c])
            continue;

        mask32 = ~0u >> (32 - format->bits[c]);

        if (format->type == FORMAT_ARGBF16)
            WARN("Unimplemented for FORMAT_ARGBF16");
        // v = float_32_to_16(src_component);
        else if (format->type == FORMAT_ARGBF)
            v = *(DWORD *)&src_component;
        else
            v = (DWORD)(src_component * ((1 << format->bits[c]) - 1) + 0.5f);

        for (i = format->shift[c] / 8 * 8; i < format->shift[c] + format->bits[c]; i += 8)
        {
            BYTE mask, byte;

            if (format->shift[c] > i)
            {
                mask = mask32 << (format->shift[c] - i);
                byte = (v << (format->shift[c] - i)) & mask;
            }
            else
            {
                mask = mask32 >> (i - format->shift[c]);
                byte = (v >> (i - format->shift[c])) & mask;
            }
            dst[i / 8] |= byte;
        }
    }
}

/************************************************************
 * convert_argb_pixels
 *
 * Copies the source buffer to the destination buffer, performing
 * any necessary format conversion and color keying.
 * Pixels outsize the source rect are blacked out.
 */
void convert_argb_pixels(const BYTE *src, UINT src_row_pitch, UINT src_slice_pitch, const struct volume *src_size,
                         const struct pixel_format_desc *src_format, BYTE *dst, UINT dst_row_pitch,
                         UINT dst_slice_pitch, const struct volume *dst_size,
                         const struct pixel_format_desc *dst_format, D3DCOLOR color_key, const PALETTEENTRY *palette)
{
    struct argb_conversion_info conv_info, ck_conv_info;
    const struct pixel_format_desc *ck_format = NULL;
    DWORD channels[4];
    UINT min_width, min_height, min_depth;
    UINT x, y, z;

    memset(channels, 0, sizeof(channels));
    init_argb_conversion_info(src_format, dst_format, &conv_info);

    min_width = std::min(src_size->width, dst_size->width);
    min_height = std::min(src_size->height, dst_size->height);
    min_depth = std::min(src_size->depth, dst_size->depth);

    if (color_key)
    {
        /* Color keys are always represented in D3DFMT_A8R8G8B8 format. */
        ck_format = get_format_info(D3DFMT_A8R8G8B8);
        init_argb_conversion_info(src_format, ck_format, &ck_conv_info);
    }

    for (z = 0; z < min_depth; z++)
    {
        const BYTE *src_slice_ptr = src + z * src_slice_pitch;
        BYTE *dst_slice_ptr = dst + z * dst_slice_pitch;

        for (y = 0; y < min_height; y++)
        {
            const BYTE *src_ptr = src_slice_ptr + y * src_row_pitch;
            BYTE *dst_ptr = dst_slice_ptr + y * dst_row_pitch;

            for (x = 0; x < min_width; x++)
            {
                if (!src_format->to_rgba && !dst_format->from_rgba && src_format->type == dst_format->type &&
                    src_format->bytes_per_pixel <= 4 && dst_format->bytes_per_pixel <= 4)
                {
                    DWORD val;

                    get_relevant_argb_components(&conv_info, src_ptr, channels);
                    val = make_argb_color(&conv_info, channels);

                    if (color_key)
                    {
                        DWORD ck_pixel;

                        get_relevant_argb_components(&ck_conv_info, src_ptr, channels);
                        ck_pixel = make_argb_color(&ck_conv_info, channels);
                        if (ck_pixel == color_key)
                            val &= ~conv_info.destmask[0];
                    }
                    if (dst_format->format == D3DFMT_Q8W8V8U8)
                    {
                        WARN("Unimplemented for D3DFMT_Q8W8V8U8");
                        // val = from_rgba_qwvu(val);
                    }
                    memcpy(dst_ptr, &val, dst_format->bytes_per_pixel);
                }
                else
                {
                    struct vec4 color, tmp;

                    format_to_vec4(src_format, src_ptr, &color);
                    if (src_format->to_rgba)
                        src_format->to_rgba(&color, &tmp, palette);
                    else
                        tmp = color;

                    if (ck_format)
                    {
                        DWORD ck_pixel;

                        format_from_vec4(ck_format, &tmp, (BYTE *)&ck_pixel);
                        if (ck_pixel == color_key)
                            tmp.w = 0.0f;
                    }

                    if (dst_format->from_rgba)
                        dst_format->from_rgba(&tmp, &color);
                    else
                        color = tmp;

                    format_from_vec4(dst_format, &color, dst_ptr);
                }

                src_ptr += src_format->bytes_per_pixel;
                dst_ptr += dst_format->bytes_per_pixel;
            }

            if (src_size->width < dst_size->width) /* black out remaining pixels */
                memset(dst_ptr, 0, dst_format->bytes_per_pixel * (dst_size->width - src_size->width));
        }

        if (src_size->height < dst_size->height) /* black out remaining pixels */
            memset(dst + src_size->height * dst_row_pitch, 0, dst_row_pitch * (dst_size->height - src_size->height));
    }
    if (src_size->depth < dst_size->depth) /* black out remaining pixels */
        memset(dst + src_size->depth * dst_slice_pitch, 0, dst_slice_pitch * (dst_size->depth - src_size->depth));
}

/************************************************************
 * point_filter_argb_pixels
 *
 * Copies the source buffer to the destination buffer, performing
 * any necessary format conversion, color keying and stretching
 * using a point filter.
 */
void point_filter_argb_pixels(const BYTE *src, UINT src_row_pitch, UINT src_slice_pitch, const struct volume *src_size,
                              const struct pixel_format_desc *src_format, BYTE *dst, UINT dst_row_pitch,
                              UINT dst_slice_pitch, const struct volume *dst_size,
                              const struct pixel_format_desc *dst_format, D3DCOLOR color_key,
                              const PALETTEENTRY *palette)
{
    struct argb_conversion_info conv_info, ck_conv_info;
    const struct pixel_format_desc *ck_format = NULL;
    DWORD channels[4];
    UINT x, y, z;

    memset(channels, 0, sizeof(channels));
    init_argb_conversion_info(src_format, dst_format, &conv_info);

    if (color_key)
    {
        /* Color keys are always represented in D3DFMT_A8R8G8B8 format. */
        ck_format = get_format_info(D3DFMT_A8R8G8B8);
        init_argb_conversion_info(src_format, ck_format, &ck_conv_info);
    }

    for (z = 0; z < dst_size->depth; z++)
    {
        BYTE *dst_slice_ptr = dst + z * dst_slice_pitch;
        const BYTE *src_slice_ptr = src + src_slice_pitch * (z * src_size->depth / dst_size->depth);

        for (y = 0; y < dst_size->height; y++)
        {
            BYTE *dst_ptr = dst_slice_ptr + y * dst_row_pitch;
            const BYTE *src_row_ptr = src_slice_ptr + src_row_pitch * (y * src_size->height / dst_size->height);

            for (x = 0; x < dst_size->width; x++)
            {
                const BYTE *src_ptr =
                    src_row_ptr + (x * src_size->width / dst_size->width) * src_format->bytes_per_pixel;

                if (!src_format->to_rgba && !dst_format->from_rgba && src_format->type == dst_format->type &&
                    src_format->bytes_per_pixel <= 4 && dst_format->bytes_per_pixel <= 4)
                {
                    DWORD val;

                    get_relevant_argb_components(&conv_info, src_ptr, channels);
                    val = make_argb_color(&conv_info, channels);

                    if (color_key)
                    {
                        DWORD ck_pixel;

                        get_relevant_argb_components(&ck_conv_info, src_ptr, channels);
                        ck_pixel = make_argb_color(&ck_conv_info, channels);
                        if (ck_pixel == color_key)
                            val &= ~conv_info.destmask[0];
                    }
                    memcpy(dst_ptr, &val, dst_format->bytes_per_pixel);
                }
                else
                {
                    struct vec4 color, tmp;

                    format_to_vec4(src_format, src_ptr, &color);
                    if (src_format->to_rgba)
                        src_format->to_rgba(&color, &tmp, palette);
                    else
                        tmp = color;

                    if (ck_format)
                    {
                        DWORD ck_pixel;

                        format_from_vec4(ck_format, &tmp, (BYTE *)&ck_pixel);
                        if (ck_pixel == color_key)
                            tmp.w = 0.0f;
                    }

                    if (dst_format->from_rgba)
                        dst_format->from_rgba(&tmp, &color);
                    else
                        color = tmp;

                    format_from_vec4(dst_format, &color, dst_ptr);
                }

                dst_ptr += dst_format->bytes_per_pixel;
            }
        }
    }
}

HRESULT my_D3DXLoadSurfaceFromMemory(IDirect3DSurface9 *dst_surface, const PALETTEENTRY *dst_palette,
                                         const RECT *dst_rect, const void *src_memory, D3DFORMAT src_format,
                                         UINT src_pitch, const PALETTEENTRY *src_palette, const RECT *src_rect,
                                         DWORD filter, D3DCOLOR color_key)
{
    const struct pixel_format_desc *srcformatdesc, *destformatdesc;
    struct volume src_size, dst_size, dst_size_aligned;
    RECT dst_rect_temp, dst_rect_aligned;
    IDirect3DSurface9 *surface;
    D3DSURFACE_DESC surfdesc;
    D3DLOCKED_RECT lockrect;
    HRESULT hr;

    /*
    TRACE("dst_surface %p, dst_palette %p, dst_rect %s, src_memory %p, src_format %#x, "
            "src_pitch %u, src_palette %p, src_rect %s, filter %#lx, color_key 0x%08lx.\n",
            dst_surface, dst_palette, wine_dbgstr_rect(dst_rect), src_memory, src_format,
            src_pitch, src_palette, wine_dbgstr_rect(src_rect), filter, color_key);
    */

    if (!dst_surface || !src_memory || !src_rect)
    {
        WARN("Invalid argument specified.\n");
        return D3DERR_INVALIDCALL;
    }
    if (src_format == D3DFMT_UNKNOWN || src_rect->left >= src_rect->right || src_rect->top >= src_rect->bottom)
    {
        WARN("Invalid src_format or src_rect.\n");
        return E_FAIL;
    }

    srcformatdesc = get_format_info(src_format);
    if (srcformatdesc->type == FORMAT_UNKNOWN)
    {
        FIXME("Unsupported format %#x.\n", src_format);
        return E_NOTIMPL;
    }

    src_size.width = src_rect->right - src_rect->left;
    src_size.height = src_rect->bottom - src_rect->top;
    src_size.depth = 1;

    IDirect3DSurface9_GetDesc(dst_surface, &surfdesc);
    destformatdesc = get_format_info(surfdesc.Format);
    if (!dst_rect)
    {
        dst_rect = &dst_rect_temp;
        dst_rect_temp.left = 0;
        dst_rect_temp.top = 0;
        dst_rect_temp.right = surfdesc.Width;
        dst_rect_temp.bottom = surfdesc.Height;
    }
    else
    {
        if (dst_rect->left > dst_rect->right || dst_rect->right > surfdesc.Width || dst_rect->top > dst_rect->bottom ||
            dst_rect->bottom > surfdesc.Height || dst_rect->left < 0 || dst_rect->top < 0)
        {
            WARN("Invalid dst_rect specified.\n");
            return D3DERR_INVALIDCALL;
        }
        if (dst_rect->left == dst_rect->right || dst_rect->top == dst_rect->bottom)
        {
            WARN("Empty dst_rect specified.\n");
            return D3D_OK;
        }
    }

    dst_rect_aligned = *dst_rect;
    if (dst_rect_aligned.left & (destformatdesc->block_width - 1))
        dst_rect_aligned.left = dst_rect_aligned.left & ~(destformatdesc->block_width - 1);
    if (dst_rect_aligned.top & (destformatdesc->block_height - 1))
        dst_rect_aligned.top = dst_rect_aligned.top & ~(destformatdesc->block_height - 1);
    if (dst_rect_aligned.right & (destformatdesc->block_width - 1) && dst_rect_aligned.right != surfdesc.Width)
        dst_rect_aligned.right = std::min((UINT)((dst_rect_aligned.right + destformatdesc->block_width - 1) &
                                              ~(destformatdesc->block_width - 1)),
                     surfdesc.Width);
    if (dst_rect_aligned.bottom & (destformatdesc->block_height - 1) && dst_rect_aligned.bottom != surfdesc.Height)
        dst_rect_aligned.bottom =
            std::min((UINT)((dst_rect_aligned.bottom + destformatdesc->block_height - 1) & ~(destformatdesc->block_height - 1)),
                     surfdesc.Height);

    dst_size.width = dst_rect->right - dst_rect->left;
    dst_size.height = dst_rect->bottom - dst_rect->top;
    dst_size.depth = 1;
    dst_size_aligned.width = dst_rect_aligned.right - dst_rect_aligned.left;
    dst_size_aligned.height = dst_rect_aligned.bottom - dst_rect_aligned.top;
    dst_size_aligned.depth = 1;

    if (filter == D3DX_DEFAULT)
        filter = D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER;

    if (FAILED(hr = lock_surface(dst_surface, &dst_rect_aligned, &lockrect, &surface, TRUE)))
        return hr;

    src_memory = (BYTE *)src_memory + src_rect->top / srcformatdesc->block_height * src_pitch +
                 src_rect->left / srcformatdesc->block_width * srcformatdesc->block_byte_count;

    if (src_format == surfdesc.Format && dst_size.width == src_size.width && dst_size.height == src_size.height &&
        color_key == 0 && !(src_rect->left & (srcformatdesc->block_width - 1)) &&
        !(src_rect->top & (srcformatdesc->block_height - 1)) && !(dst_rect->left & (destformatdesc->block_width - 1)) &&
        !(dst_rect->top & (destformatdesc->block_height - 1)))
    {
        TRACE("Simple copy.\n");
        copy_pixels(static_cast<const BYTE *>(src_memory), src_pitch, 0, static_cast<BYTE *>(lockrect.pBits),
                    lockrect.Pitch, 0, &src_size, srcformatdesc);
    }
    else /* Stretching or format conversion. */
    {
        const struct pixel_format_desc *dst_format;
        DWORD *src_uncompressed = NULL;
        BYTE *dst_uncompressed = NULL;
        unsigned int dst_pitch;
        BYTE *dst_mem;

        if (!is_conversion_from_supported(srcformatdesc) || !is_conversion_to_supported(destformatdesc))
        {
            FIXME("Unsupported format conversion %#x -> %#x.\n", src_format, surfdesc.Format);
            unlock_surface(dst_surface, &dst_rect_aligned, surface, FALSE);
            return E_NOTIMPL;
        }

        if (srcformatdesc->type == FORMAT_DXT)
        {
            WARN("Src FORMAT_DXT unsupported\n");
            /*
            void (*fetch_dxt_texel)(int srcRowStride, const BYTE *pixdata,
                    int i, int j, void *texel);
            unsigned int x, y;

            src_pitch = src_pitch * srcformatdesc->block_width / srcformatdesc->block_byte_count;

            src_uncompressed = heap_alloc(src_size.width * src_size.height * sizeof(DWORD));
            if (!src_uncompressed)
            {
                unlock_surface(dst_surface, &dst_rect_aligned, surface, FALSE);
                return E_OUTOFMEMORY;
            }

            switch(src_format)
            {
                case D3DFMT_DXT1:
                    fetch_dxt_texel = fetch_2d_texel_rgba_dxt1;
                    break;
                case D3DFMT_DXT2:
                case D3DFMT_DXT3:
                    fetch_dxt_texel = fetch_2d_texel_rgba_dxt3;
                    break;
                case D3DFMT_DXT4:
                case D3DFMT_DXT5:
                    fetch_dxt_texel = fetch_2d_texel_rgba_dxt5;
                    break;
                default:
                    FIXME("Unexpected compressed texture format %u.\n", src_format);
                    fetch_dxt_texel = NULL;
            }

            TRACE("Uncompressing DXTn surface.\n");
            for (y = 0; y < src_size.height; ++y)
            {
                DWORD *ptr = &src_uncompressed[y * src_size.width];
                for (x = 0; x < src_size.width; ++x)
                {
                    fetch_dxt_texel(src_pitch, src_memory, x + src_rect->left, y + src_rect->top, ptr);
                    ++ptr;
                }
            }
            src_memory = src_uncompressed;
            src_pitch = src_size.width * sizeof(DWORD);
            srcformatdesc = get_format_info(D3DFMT_A8B8G8R8);
            */
        }

        if (destformatdesc->type == FORMAT_DXT)
        {
            WARN("Src FORMAT_DXT unsupported\n");
            /*
            BOOL dst_misaligned = dst_rect->left != dst_rect_aligned.left
                    || dst_rect->top != dst_rect_aligned.top
                    || dst_rect->right != dst_rect_aligned.right
                    || dst_rect->bottom != dst_rect_aligned.bottom;

            dst_uncompressed = HeapAlloc(GetProcessHeap(), dst_misaligned ? HEAP_ZERO_MEMORY : 0,
                    dst_size_aligned.width * dst_size_aligned.height * sizeof(DWORD));
            if (!dst_uncompressed)
            {
                heap_free(src_uncompressed);
                unlock_surface(dst_surface, &dst_rect_aligned, surface, FALSE);
                return E_OUTOFMEMORY;
            }
            dst_pitch = dst_size_aligned.width * sizeof(DWORD);
            dst_format = get_format_info(D3DFMT_A8B8G8R8);
            dst_mem = dst_uncompressed + (dst_rect->top - dst_rect_aligned.top) * dst_pitch
                    + (dst_rect->left - dst_rect_aligned.left) * sizeof(DWORD);
            */
        }
        else
        {
            dst_mem = static_cast<BYTE *>(lockrect.pBits);
            dst_pitch = lockrect.Pitch;
            dst_format = destformatdesc;
        }

        if ((filter & 0xf) == D3DX_FILTER_NONE)
        {
            convert_argb_pixels(static_cast<const BYTE *>(src_memory), src_pitch, 0, &src_size, srcformatdesc, dst_mem,
                                dst_pitch, 0, &dst_size, dst_format, color_key, src_palette);
        }
        else /* if ((filter & 0xf) == D3DX_FILTER_POINT) */
        {
            if ((filter & 0xf) != D3DX_FILTER_POINT)
                FIXME("Unhandled filter %#lx.\n", filter);

            /* Always apply a point filter until D3DX_FILTER_LINEAR,
             * D3DX_FILTER_TRIANGLE and D3DX_FILTER_BOX are implemented. */
            point_filter_argb_pixels(static_cast<const BYTE *>(src_memory), src_pitch, 0, &src_size, srcformatdesc,
                                     dst_mem, dst_pitch, 0, &dst_size, dst_format, color_key, src_palette);
        }

        /*
        heap_free(src_uncompressed);

        if (dst_uncompressed)
        {
            GLenum gl_format = 0;

            TRACE("Compressing DXTn surface.\n");
            switch(surfdesc.Format)
            {
                case D3DFMT_DXT1:
                    gl_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
                    break;
                case D3DFMT_DXT2:
                case D3DFMT_DXT3:
                    gl_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
                    break;
                case D3DFMT_DXT4:
                case D3DFMT_DXT5:
                    gl_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                    break;
                default:
                    ERR("Unexpected destination compressed format %u.\n", surfdesc.Format);
            }
            tx_compress_dxtn(4, dst_size_aligned.width, dst_size_aligned.height,
                    dst_uncompressed, gl_format, lockrect.pBits,
                    lockrect.Pitch * destformatdesc->block_width / destformatdesc->block_byte_count);
            heap_free(dst_uncompressed);
        }
        */
    }

    return unlock_surface(dst_surface, &dst_rect_aligned, surface, TRUE);
}

HRESULT my_D3DXLoadSurfaceFromSurface(IDirect3DSurface9 *dst_surface, const PALETTEENTRY *dst_palette,
                                      const RECT *dst_rect, IDirect3DSurface9 *src_surface,
                                      const PALETTEENTRY *src_palette, const RECT *src_rect, DWORD filter,
                                      D3DCOLOR color_key)
{
    const struct pixel_format_desc *src_format_desc, *dst_format_desc;
    D3DSURFACE_DESC src_desc, dst_desc;
    struct volume src_size, dst_size;
    IDirect3DSurface9 *temp_surface;
    D3DTEXTUREFILTERTYPE d3d_filter;
    IDirect3DDevice9 *device;
    D3DLOCKED_RECT lock;
    RECT dst_rect_temp;
    HRESULT hr;
    RECT s;

    /*
    TRACE("dst_surface %p, dst_palette %p, dst_rect %s, src_surface %p, "
            "src_palette %p, src_rect %s, filter %#x, color_key 0x%08x.\n",
            dst_surface, dst_palette, wine_dbgstr_rect(dst_rect), src_surface,
            src_palette, wine_dbgstr_rect(src_rect), filter, color_key);
     */

    if (!dst_surface || !src_surface)
        return D3DERR_INVALIDCALL;

    IDirect3DSurface9_GetDesc(src_surface, &src_desc);
    src_format_desc = get_format_info(src_desc.Format);
    if (!src_rect)
    {
        SetRect(&s, 0, 0, src_desc.Width, src_desc.Height);
        src_rect = &s;
    }
    else if (src_rect->left == src_rect->right || src_rect->top == src_rect->bottom)
    {
        WARN("Empty src_rect specified.\n");
        return filter == D3DX_FILTER_NONE ? D3D_OK : E_FAIL;
    }
    else if (src_rect->left > src_rect->right || src_rect->right > src_desc.Width || src_rect->left < 0 ||
             src_rect->left > src_desc.Width || src_rect->top > src_rect->bottom ||
             src_rect->bottom > src_desc.Height || src_rect->top < 0 || src_rect->top > src_desc.Height)
    {
        WARN("Invalid src_rect specified.\n");
        return D3DERR_INVALIDCALL;
    }

    src_size.width = src_rect->right - src_rect->left;
    src_size.height = src_rect->bottom - src_rect->top;
    src_size.depth = 1;

    IDirect3DSurface9_GetDesc(dst_surface, &dst_desc);
    dst_format_desc = get_format_info(dst_desc.Format);
    if (!dst_rect)
    {
        SetRect(&dst_rect_temp, 0, 0, dst_desc.Width, dst_desc.Height);
        dst_rect = &dst_rect_temp;
    }
    else if (dst_rect->left == dst_rect->right || dst_rect->top == dst_rect->bottom)
    {
        WARN("Empty dst_rect specified.\n");
        return filter == D3DX_FILTER_NONE ? D3D_OK : E_FAIL;
    }
    else if (dst_rect->left > dst_rect->right || dst_rect->right > dst_desc.Width || dst_rect->left < 0 ||
             dst_rect->left > dst_desc.Width || dst_rect->top > dst_rect->bottom ||
             dst_rect->bottom > dst_desc.Height || dst_rect->top < 0 || dst_rect->top > dst_desc.Height)
    {
        WARN("Invalid dst_rect specified.\n");
        return D3DERR_INVALIDCALL;
    }

    dst_size.width = dst_rect->right - dst_rect->left;
    dst_size.height = dst_rect->bottom - dst_rect->top;
    dst_size.depth = 1;

    if (!dst_palette && !src_palette && !color_key)
    {
        if (src_desc.Format == dst_desc.Format && dst_size.width == src_size.width &&
            dst_size.height == src_size.height && color_key == 0 &&
            !(src_rect->left & (src_format_desc->block_width - 1)) &&
            !(src_rect->top & (src_format_desc->block_height - 1)) &&
            !(dst_rect->left & (dst_format_desc->block_width - 1)) &&
            !(dst_rect->top & (dst_format_desc->block_height - 1)))
        {
            d3d_filter = D3DTEXF_NONE;
        }
        else
        {
            switch (filter)
            {
            case D3DX_FILTER_NONE:
                d3d_filter = D3DTEXF_NONE;
                break;

            case D3DX_FILTER_POINT:
                d3d_filter = D3DTEXF_POINT;
                break;

            case D3DX_FILTER_LINEAR:
                d3d_filter = D3DTEXF_LINEAR;
                break;

            default:
                d3d_filter = D3DTEXF_FORCE_DWORD;
                break;
            }
        }

        if (d3d_filter != D3DTEXF_FORCE_DWORD)
        {
            IDirect3DSurface9_GetDevice(src_surface, &device);
            hr = IDirect3DDevice9_StretchRect(device, src_surface, src_rect, dst_surface, dst_rect, d3d_filter);
            IDirect3DDevice9_Release(device);
            if (SUCCEEDED(hr))
                return D3D_OK;
        }
    }

    if (FAILED(lock_surface(src_surface, NULL, &lock, &temp_surface, FALSE)))
        return D3DXERR_INVALIDDATA;

    hr = my_D3DXLoadSurfaceFromMemory(dst_surface, dst_palette, dst_rect, lock.pBits, src_desc.Format, lock.Pitch,
                                   src_palette, src_rect, filter, color_key);

    if (FAILED(unlock_surface(src_surface, NULL, temp_surface, FALSE)))
        return D3DXERR_INVALIDDATA;

    return hr;
}


bool SCRSHOTER::MakeScreenShot()
{
    int n;
    D3DLOCKED_RECT inRect, outRect;

    auto hr = D3D_OK;

    // make the video card draw all unfinished tasks
    hr = rs->EndScene();
    if (hr != D3D_OK)
    {
        core.Trace("ERROR!!! Can`t EndScene");
        return false;
    }
    hr = rs->BeginScene();
    if (hr != D3D_OK)
    {
        core.Trace("ERROR!!! Can`t BeginScene");
        return false;
    }

    // delete the old screen shot
    if (textureIndex_ != -1 && rs != nullptr)
    {
        rs->TextureRelease(textureIndex_);
    }
    texture_ = nullptr;
    textureIndex_ = -1;

    // get data of the old render surface
    D3DSURFACE_DESC desc;
    IDirect3DSurface9 *pOldRenderTarg = nullptr;
    hr = rs->GetRenderTarget(&pOldRenderTarg);
    if (hr == D3D_OK)
        hr = pOldRenderTarg->GetDesc(&desc);

    // get a copy of the render buffer
    IDirect3DSurface9 *pRenderTarg = nullptr;
    if (hr == D3D_OK)
        hr = rs->CreateOffscreenPlainSurface(desc.Width, desc.Height, desc.Format, &pRenderTarg);
    if (hr == D3D_OK)
        hr = rs->GetRenderTargetData(pOldRenderTarg, pRenderTarg);
    pOldRenderTarg->Release();

    // create a new screen shot
    if (hr == D3D_OK)
    {
        textureIndex_ =
            rs->TextureCreate(SS_TEXTURE_WIDTH, SS_TEXTURE_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
        texture_ = static_cast<IDirect3DTexture9 *>(rs->GetTextureFromID(textureIndex_));
    }

    // get a buffer for a copy of the rendering surface
    void *pIn = nullptr;
    if (hr == D3D_OK)
        hr = pRenderTarg->LockRect(&inRect, nullptr, 0);
    if (hr == D3D_OK)
        pIn = inRect.pBits;
    // get a buffer for the texture
    void *pOut = nullptr;
    if (hr == D3D_OK)
        hr = texture_->LockRect(0, &outRect, nullptr, 0);
    if (hr == D3D_OK)
        pOut = outRect.pBits;

    // fill this texture from a copy of our render buffer
    if (hr == D3D_OK)
    {
        for (int vi = 0; vi < SS_TEXTURE_HEIGHT; vi++)
        {
            const size_t vertical_offset = vi * desc.Height / SS_TEXTURE_HEIGHT;
            auto *const pInPxl = static_cast<uint8_t *>(pIn) + inRect.Pitch * vertical_offset;
            auto *pOutPxl = (uint32_t *)(static_cast<uint8_t *>(pOut) + outRect.Pitch * vi);
            for (int hi = 0; hi < SS_TEXTURE_WIDTH; hi++)
            {
                const size_t horizontal_offset = (hi * desc.Width / SS_TEXTURE_WIDTH) * (inRect.Pitch / desc.Width);
                pOutPxl[hi] = GetA8R8G8B8_FromFMT(&pInPxl[horizontal_offset], desc.Format);
            }
        }
    }

    // close opened buffers
    if (pIn != nullptr)
        pRenderTarg->UnlockRect();

    // close opened buffers
    if (pOut != nullptr)
        texture_->UnlockRect(0);

    // Delete unnecessary screen copy
    pRenderTarg->Release();

    // Add a texture with a frame to the shot
    const int nTextureID = rs->TextureCreate("interfaces\\EmptyBorder.tga");
    if (nTextureID >= 0)
    {
        IDirect3DTexture9 *pScrShotTex = nullptr;
        if (D3D_OK == rs->CreateTexture(SS_TEXTURE_WIDTH, SS_TEXTURE_HEIGHT, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
                                        D3DPOOL_DEFAULT, &pScrShotTex))
        {
            const uint32_t BI_SCRSHOTER_VERTEX_FORMAT = (D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_TEXTUREFORMAT2);
            struct BI_SCRSHOTER_VERTEX
            {
                CVECTOR pos;
                float w;
                float tu, tv;
            } vert[4];
            for (auto i = 0; i < 4; i++)
            {
                vert[i].pos.z = 1.f;
                vert[i].w = 0.5f;
            }
            vert[1].pos.x = vert[0].pos.x = 0;
            vert[2].pos.y = vert[0].pos.y = 0;
            vert[3].pos.x = vert[2].pos.x = SS_TEXTURE_WIDTH;
            vert[3].pos.y = vert[1].pos.y = SS_TEXTURE_HEIGHT;
            vert[1].tu = vert[0].tu = 0;
            vert[2].tv = vert[0].tv = 0;
            vert[3].tu = vert[2].tu = 1.f;
            vert[3].tv = vert[1].tv = 1.f;

            pOldRenderTarg = nullptr;
            pRenderTarg = nullptr;
            if (rs->GetRenderTarget(&pOldRenderTarg) == S_OK)
            {
                IDirect3DSurface9 *pStencil = nullptr;
                rs->GetDepthStencilSurface(&pStencil);
                pScrShotTex->GetSurfaceLevel(0, &pRenderTarg);
                if (rs->SetRenderTarget(pRenderTarg, nullptr) == S_OK)
                {
                    rs->TextureSet(0, textureIndex_);
                    rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, BI_SCRSHOTER_VERTEX_FORMAT, 2, vert,
                                        sizeof(BI_SCRSHOTER_VERTEX), "battle_icons");
                    rs->TextureSet(0, nTextureID);
                    rs->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, BI_SCRSHOTER_VERTEX_FORMAT, 2, vert,
                                        sizeof(BI_SCRSHOTER_VERTEX), "battle_icons");
                    rs->SetRenderTarget(pOldRenderTarg, pStencil);
                }
                if (pRenderTarg)
                    pRenderTarg->Release();
                if (pStencil)
                    pStencil->Release();
                if (pOldRenderTarg)
                    pOldRenderTarg->Release();
            }
            IDirect3DSurface9 *pSurf1 = nullptr, *pSurf2 = nullptr;
            rs->GetSurfaceLevel(texture_, 0, &pSurf1);
            rs->GetSurfaceLevel(pScrShotTex, 0, &pSurf2);
            // rs->UpdateSurface(pSurf2,null,0,pSurf1,null);
            //hr = my_D3DXLoadSurfaceFromSurface(pSurf1, nullptr, nullptr, pSurf2, nullptr, nullptr, D3DX_DEFAULT, 0);
            if (pSurf1)
                rs->Release(pSurf1);
            if (pSurf2)
                rs->Release(pSurf2);
            rs->Release(pScrShotTex);
        }

        rs->TextureRelease(nTextureID);
    }

    return hr == D3D_OK;
}

uint64_t SCRSHOTER::ProcessMessage(MESSAGE &message)
{
    switch (message.Long())
    {
    case MSG_SCRSHOT_MAKE:
        return textureIndex_;
        break;
    case MSG_SCRSHOT_READ: {
        VDATA *pvdat;

        const std::string &param = message.String();
        const std::string &param2 = message.String();
        pvdat = message.ScriptVariablePointer();

        int32_t pRetTex = AddSaveTexture(param.c_str(), param2.c_str());
        auto *const strDat = FindSaveData(param2.c_str());
        if (pvdat)
            if (!strDat)
            {
                pvdat->Set("\0");
            }
            else
            {
                pvdat->Set(strDat);
            }
        return pRetTex;
    }
    break;
    case MSG_SCRSHOT_RELEASE: {
        const std::string &param = message.String();
        DelSaveTexture(param.c_str());
    }
    break;
    }
    return 0;
}

int32_t SCRSHOTER::FindSaveTexture(const char *fileName) const
{
    if (!fileName)
        return -1;
    auto *ps = m_list;
    while (ps)
    {
        if (ps->fileName && storm::iEquals(fileName, ps->fileName))
            return ps->textureId;
        ps = ps->next;
    }
    return -1;
}

char *SCRSHOTER::FindSaveData(const char *fileName) const
{
    if (!fileName)
        return nullptr;
    SAVETEXTURES *ps = m_list;
    while (ps)
    {
        if (ps->fileName && storm::iEquals(fileName, ps->fileName))
            return ps->dataString;
        ps = ps->next;
    }
    return nullptr;
}

int32_t SCRSHOTER::AddSaveTexture(const char *dirName, const char *fileName)
{
    if (fileName == nullptr)
        return -1;
    int32_t rval = FindSaveTexture(fileName);
    if (rval != -1)
        return rval;
    if (storm::iEquals(fileName, "newsave"))
        return textureIndex_;
    auto *ps = new SAVETEXTURES;
    if (ps == nullptr)
    {
        throw std::runtime_error("Allocate memory error");
    }
    ps->dataString = nullptr;
    ps->next = m_list;
    m_list = ps;
    const auto len = strlen(fileName) + 1;
    m_list->fileName = new char[len];
    if (m_list->fileName == nullptr)
    {
        throw std::runtime_error("Allocate memory error");
    }
    memcpy(m_list->fileName, fileName, len);
    char param[1024];
    if (dirName == nullptr || dirName[0] == 0)
        sprintf_s(param, "%s", fileName);
    else
        sprintf_s(param, "%s\\%s", dirName, fileName);
    m_list->textureId = GetTexFromSave(param, &ps->dataString);
    return m_list->textureId;
}

void SCRSHOTER::DelSaveTexture(const char *fileName)
{
    if (!fileName)
        return;
    SAVETEXTURES *oldps = nullptr;
    SAVETEXTURES *ps = m_list;
    while (ps)
    {
        if (ps->fileName && storm::iEquals(fileName, ps->fileName))
        {
            if (oldps)
                oldps->next = ps->next;
            else
                m_list = ps->next;
            delete ps->fileName;
            delete ps->dataString;
            if (ps->textureId != -1 && rs != nullptr)
                rs->TextureRelease(ps->textureId);
            delete ps;
            return;
        }
        oldps = ps;
        ps = ps->next;
    }
}

int32_t SCRSHOTER::GetTexFromSave(char *fileName, char **pDatStr) const
{
    HRESULT hr = D3D_OK;
    D3DLOCKED_RECT outRect;
    int32_t textureId = -1;
    *pDatStr = nullptr; //~!~

    int32_t datSize = 0;
    char *pdat = nullptr;
    pdat = static_cast<char *>(core.GetSaveData(fileName, datSize));
    int32_t startIdx = 0;
    int32_t texSize = 0;
    if (pdat != nullptr && datSize > sizeof(SAVE_DATA_HANDLE))
    {
        startIdx = ((SAVE_DATA_HANDLE *)pdat)->StringDataSize + sizeof(SAVE_DATA_HANDLE);
        texSize = ((SAVE_DATA_HANDLE *)pdat)->SurfaceDataSize;
    }
    if (datSize > sizeof(SAVE_DATA_HANDLE) && datSize == startIdx + texSize &&
        texSize == 4 * SS_TEXTURE_WIDTH * SS_TEXTURE_HEIGHT)
    {
        textureId = rs->TextureCreate(SS_TEXTURE_WIDTH, SS_TEXTURE_HEIGHT, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED);
        auto *pt = static_cast<IDirect3DTexture9 *>(rs->GetTextureFromID(textureId));
        hr = pt->LockRect(0, &outRect, nullptr, 0);
        if (hr == D3D_OK)
        {
            memcpy(outRect.pBits, &pdat[startIdx], texSize);
            pt->UnlockRect(0);
        }
        char *stringData = &pdat[sizeof(SAVE_DATA_HANDLE)];
        if (!utf8::IsValidUtf8(stringData))
        {
            utf8::FixInvalidUtf8(stringData);
        }
        const int strLen = ((SAVE_DATA_HANDLE *)pdat)->StringDataSize;
        *pDatStr = new char[strLen + 1];
        if (!*pDatStr)
        {
            throw std::runtime_error("allocate memory error");
        }
        strncpy_s(*pDatStr, strLen + 1, stringData, strLen);
        (*pDatStr)[strLen] = 0;
    }
    /*    else
      {
        int idx=0;
        for(int i=0; i<SS_TEXTURE_HEIGHT; i++)
          for(int j=0; j<SS_TEXTURE_WIDTH; j++)
          {
            ((DWORD*)outRect.pBits)[idx] = 0xFF000000;
            idx++;
          }
      }*/
    delete pdat;

    if (hr == D3D_OK)
    {
        return textureId;
    }
    if (*pDatStr)
        delete (*pDatStr);
    *pDatStr = nullptr;
    if (textureId && rs)
        rs->TextureRelease(textureId);
    return -1;
}
