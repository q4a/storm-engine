/*
 * Copyright 2010 Christian Costa
 * Copyright 2011 Rico Schüller
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef _WIN32

#include "storm_d3dx9effect.h"
#include "utf8.h"

#include <fstream>
#include <filesystem>

bool ReadFileS(std::fstream &fileS, char **s, std::streamsize count)
{
    fileS.exceptions(std::fstream::failbit | std::fstream::badbit);
    try
    {
        char *data = *s;
        fileS.read(data, count);
        return true;
    }
    catch (std::fstream::failure e)
    {
        return false;
    }
}

/////////////////////////// d3dx9_36/shader.c //////////////////////////////////

static const void *main_file_data;

/* D3DXInclude private implementation, used to implement
 * D3DXAssembleShaderFromFile() from D3DXAssembleShader(). */
/* To be able to correctly resolve include search paths we have to store the
 * pathname of each include file. We store the pathname pointer right before
 * the file data. */
static HRESULT WINAPI d3dx_include_from_file_open(D3DXINCLUDE_TYPE include_type,
        const char *filename, const void *parent_data, const void **data, UINT *bytes)
{
    const char *p, *parent_name = "";
    char *pathname = NULL, *ptr;
    char **buffer = NULL;
    //HANDLE file;
    //UINT size;
    std::uintmax_t size;

    if (parent_data)
    {
        parent_name = *((const char **)parent_data - 1);
    }
    else
    {
        if (main_file_data)
            parent_name = *((const char **)main_file_data - 1);
    }

    //TRACE("Looking up include file %s, parent %s.\n", debugstr_a(filename), debugstr_a(parent_name));

    if ((p = strrchr(parent_name, '\\')))
        ++p;
    else
        p = parent_name;
    //pathname = HeapAlloc(GetProcessHeap(), 0, (p - parent_name) + strlen(filename) + 1);
    //if(!pathname)
    //    return HRESULT_FROM_WIN32(GetLastError());
    pathname = static_cast<char *>(malloc((p - parent_name) + strlen(filename) + 1));
    if(!pathname)
        return HRESULT_FROM_WIN32(STATUS_NO_MEMORY);

    memcpy(pathname, parent_name, p - parent_name);
    strcpy(pathname + (p - parent_name), filename);
    ptr = pathname + (p - parent_name);
    while (*ptr)
    {
        if (*ptr == '/')
            *ptr = '\\';
        ++ptr;
    }

    //file = CreateFileA(pathname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    //if(file == INVALID_HANDLE_VALUE)
    //    goto error;
    std::fstream fileS(pathname, std::ios::binary | std::ios::in);
    if (!fileS.is_open())
        goto error;

    //TRACE("Include file found at pathname = %s\n", debugstr_a(pathname));

    //size = GetFileSize(file, NULL);
    //if(size == INVALID_FILE_SIZE)
    //    goto error;
    size = std::filesystem::file_size(pathname);

    //buffer = HeapAlloc(GetProcessHeap(), 0, size + sizeof(char *));
    buffer = static_cast<char **>(malloc(size + sizeof(char *)));
    if(!buffer)
        goto error;
    *buffer = pathname;
    //if(!ReadFile(file, buffer + 1, size, bytes, NULL))
    if(!ReadFileS(fileS, buffer + 1, size))
        goto error;

    *data = buffer + 1;
    if (!main_file_data)
        main_file_data = *data;

    //CloseHandle(file);
    fileS.close();
    return S_OK;

error:
    //CloseHandle(file);
    //HeapFree(GetProcessHeap(), 0, pathname);
    //HeapFree(GetProcessHeap(), 0, buffer);
    //return HRESULT_FROM_WIN32(GetLastError());
    fileS.close();
    free(pathname);
    free(buffer);
    return HRESULT_FROM_WIN32(23);
}

static HRESULT WINAPI d3dx_include_from_file_close(const void *data)
{
    //HeapFree(GetProcessHeap(), 0, *((char **)data - 1));
    //HeapFree(GetProcessHeap(), 0, (char **)data - 1);
    free(*((char **)data - 1));
    free((char **)data - 1);
    if (main_file_data == data)
        main_file_data = NULL;
    return S_OK;
}

const struct ID3DXIncludeVtbl d3dx_include_from_file_vtbl =
{
    d3dx_include_from_file_open,
    d3dx_include_from_file_close
};

struct D3DXIncludeImpl {
    ID3DXInclude ID3DXInclude_iface;
};

/////////////////////////// d3dx9_36/effect.c //////////////////////////////////

//struct d3dx_effect
// FIX_ME_D3DX
/*{
    ID3DXEffect ID3DXEffect_iface;
    LONG ref;

    unsigned int parameter_count;
    unsigned int technique_count;
    unsigned int object_count;
    struct d3dx_top_level_parameter *parameters;
    struct d3dx_technique *techniques;
    struct d3dx_object *objects;
    DWORD flags;

    struct wine_rb_tree param_tree;
    char *full_name_tmp;
    unsigned int full_name_tmp_size;

    struct ID3DXEffectStateManager *manager;
    struct IDirect3DDevice9 *device;
    struct d3dx_effect_pool *pool;
    struct d3dx_technique *active_technique;
    struct d3dx_pass *active_pass;
    BOOL started;
    DWORD begin_flags;
    ULONG64 version_counter;

    D3DLIGHT9 current_light[8];
    unsigned int light_updated;
    D3DMATERIAL9 current_material;
    BOOL material_updated;

    struct list parameter_block_list;
    struct d3dx_parameter_block *current_parameter_block;

    char *source;
    SIZE_T source_size;
    char *skip_constants_string;
};*/

HRESULT WINAPI D3DXCreateEffectEx(struct IDirect3DDevice9 *device, const void *srcdata, UINT srcdatalen,
        const D3DXMACRO *defines, struct ID3DXInclude *include, const char *skip_constants, DWORD flags,
        struct ID3DXEffectPool *pool, struct ID3DXEffect **effect, struct ID3DXBuffer **compilation_errors)
{
    struct d3dx_effect *object;
    HRESULT hr;

    //TRACE("device %p, srcdata %p, srcdatalen %u, defines %p, include %p,"
    //        " skip_constants %p, flags %#x, pool %p, effect %p, compilation_errors %p.\n",
    //        device, srcdata, srcdatalen, defines, include,
    //        skip_constants, flags, pool, effect, compilation_errors);

    if (compilation_errors)
        *compilation_errors = NULL;

    if (!device || !srcdata)
        return D3DERR_INVALIDCALL;

    if (!srcdatalen)
        return E_FAIL;

    /* Native dll allows effect to be null so just return D3D_OK after doing basic checks */
    if (!effect)
        return D3D_OK;

    //object = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(*object));
    /*object = static_cast<d3dx_effect *>(malloc(sizeof(*object)));

    if (!object)
        return E_OUTOFMEMORY;

    hr = d3dx9_effect_init(object, device, srcdata, srcdatalen, (const D3D_SHADER_MACRO *)defines,
            (ID3DInclude *)include, flags, (ID3DBlob **)compilation_errors, pool, skip_constants);
    if (FAILED(hr))
    {
        WARN("Failed to create effect object, hr %#x.\n", hr);
        return hr;
    }

    *effect = &object->ID3DXEffect_iface;*/

    //TRACE("Created ID3DXEffect %p\n", object);

    return D3D_OK;
}

HRESULT WINAPI D3DXCreateEffectFromFileExW(struct IDirect3DDevice9 *device, const WCHAR *srcfile,
        const D3DXMACRO *defines, struct ID3DXInclude *include, const char *skipconstants, DWORD flags,
        struct ID3DXEffectPool *pool, struct ID3DXEffect **effect, struct ID3DXBuffer **compilationerrors)
{
    struct d3dx_include_from_file include_from_file;
    const void *buffer;
    unsigned int size;
    char *filename_a;
    HRESULT ret;

    //TRACE("device %p, srcfile %s, defines %p, include %p, skipconstants %s, "
    //        "flags %#x, pool %p, effect %p, compilationerrors %p.\n",
    //        device, debugstr_w(srcfile), defines, include, debugstr_a(skipconstants),
    //        flags, pool, effect, compilationerrors);

    if (!device || !srcfile)
        return D3DERR_INVALIDCALL;

    if (!include)
    {
        include_from_file.ID3DXInclude_iface.lpVtbl = &d3dx_include_from_file_vtbl;
        include = &include_from_file.ID3DXInclude_iface;
    }

    //size = WideCharToMultiByte(CP_ACP, 0, srcfile, -1, NULL, 0, NULL, NULL);
    //filename_a = heap_alloc(size);
    //if (!filename_a)
    //    return E_OUTOFMEMORY;
    //WideCharToMultiByte(CP_ACP, 0, srcfile, -1, filename_a, size, NULL, NULL);
    //filename_a = utf8::ConvertWideToUtf8(srcfile).c_str();
    // FIX_ME_D3DX

    //EnterCriticalSection(&from_file_mutex);
    ret = ID3DXInclude_Open(include, D3DXINC_LOCAL, filename_a, NULL, &buffer, &size);
    if (FAILED(ret))
    {
        //LeaveCriticalSection(&from_file_mutex);
        //heap_free(filename_a);
        return D3DXERR_INVALIDDATA;
    }

    ret = D3DXCreateEffectEx(device, buffer, size, defines, include, skipconstants, flags, pool,
            effect, compilationerrors);

    ID3DXInclude_Close(include, buffer);
    //LeaveCriticalSection(&from_file_mutex);
    //heap_free(filename_a);
    return ret;
}

HRESULT WINAPI D3DXCreateEffectFromFileW(struct IDirect3DDevice9 *device, const WCHAR *srcfile,
        const D3DXMACRO *defines, struct ID3DXInclude *include, DWORD flags, struct ID3DXEffectPool *pool,
        struct ID3DXEffect **effect, struct ID3DXBuffer **compilationerrors)
{
    //TRACE("(void): relay\n");
    return D3DXCreateEffectFromFileExW(device, srcfile, defines, include, NULL, flags, pool, effect, compilationerrors);
}

#endif // NOT _WIN32
