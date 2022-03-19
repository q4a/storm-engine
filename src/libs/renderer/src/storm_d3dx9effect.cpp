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
    HANDLE file;
    UINT size;

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
    pathname = static_cast<char *>(malloc((p - parent_name) + strlen(filename) + 1));
    //pathname = HeapAlloc(GetProcessHeap(), 0, (p - parent_name) + strlen(filename) + 1);
    if(!pathname)
        return HRESULT_FROM_WIN32(STATUS_NO_MEMORY);
        //return HRESULT_FROM_WIN32(GetLastError());

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

    //TRACE("Include file found at pathname = %s\n", debugstr_a(pathname));

    //size = GetFileSize(file, NULL);
    //if(size == INVALID_FILE_SIZE)
    //    goto error;

    //buffer = HeapAlloc(GetProcessHeap(), 0, size + sizeof(char *));
    //if(!buffer)
        goto error;
    *buffer = pathname;
    if(!ReadFile(file, buffer + 1, size, bytes, NULL))
        goto error;

    *data = buffer + 1;
    if (!main_file_data)
        main_file_data = *data;

    CloseHandle(file);
    return S_OK;

error:
    free(pathname);
    free(buffer);
    return HRESULT_FROM_WIN32(23);
    //CloseHandle(file);
    //HeapFree(GetProcessHeap(), 0, pathname);
    //HeapFree(GetProcessHeap(), 0, buffer);
    //return HRESULT_FROM_WIN32(GetLastError());
}

static HRESULT WINAPI d3dx_include_from_file_close(const void *data)
{
    free(*((char **)data - 1));
    free((char **)data - 1);
    //HeapFree(GetProcessHeap(), 0, *((char **)data - 1));
    //HeapFree(GetProcessHeap(), 0, (char **)data - 1);
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

HRESULT WINAPI D3DXCreateEffectFromFileExW(struct IDirect3DDevice9 *device, const WCHAR *srcfile,
        const D3DXMACRO *defines, struct ID3DXInclude *include, const char *skipconstants, DWORD flags,
        struct ID3DXEffectPool *pool, struct ID3DXEffect **effect, struct ID3DXBuffer **compilationerrors)
{
    return D3DERR_INVALIDCALL;/*
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

    size = WideCharToMultiByte(CP_ACP, 0, srcfile, -1, NULL, 0, NULL, NULL);
    filename_a = heap_alloc(size);
    if (!filename_a)
        return E_OUTOFMEMORY;
    WideCharToMultiByte(CP_ACP, 0, srcfile, -1, filename_a, size, NULL, NULL);

    EnterCriticalSection(&from_file_mutex);
    ret = ID3DXInclude_Open(include, D3DXINC_LOCAL, filename_a, NULL, &buffer, &size);
    if (FAILED(ret))
    {
        LeaveCriticalSection(&from_file_mutex);
        heap_free(filename_a);
        return D3DXERR_INVALIDDATA;
    }

    ret = D3DXCreateEffectEx(device, buffer, size, defines, include, skipconstants, flags, pool,
            effect, compilationerrors);

    ID3DXInclude_Close(include, buffer);
    LeaveCriticalSection(&from_file_mutex);
    heap_free(filename_a);
    return ret;*/
}

HRESULT WINAPI D3DXCreateEffectFromFileW(struct IDirect3DDevice9 *device, const WCHAR *srcfile,
        const D3DXMACRO *defines, struct ID3DXInclude *include, DWORD flags, struct ID3DXEffectPool *pool,
        struct ID3DXEffect **effect, struct ID3DXBuffer **compilationerrors)
{
    //TRACE("(void): relay\n");
    return D3DXCreateEffectFromFileExW(device, srcfile, defines, include, NULL, flags, pool, effect, compilationerrors);
}

#endif // NOT _WIN32
