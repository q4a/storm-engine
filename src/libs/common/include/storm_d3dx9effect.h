/*
 * Copyright 2010 Christian Costa
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

#include "storm_d3dx9.h" // for UINT, IUnknown and others

//#define CINTERFACE // Wine written in C, but I placed methods in CPP files

#ifndef _WINNT_
#define _WINNT_
/*
#ifndef DECLSPEC_NOTHROW
# if defined(_MSC_VER) && (_MSC_VER >= 1200) && !defined(MIDL_PASS)
#  define DECLSPEC_NOTHROW __declspec(nothrow)
# elif defined(__GNUC__)
#  define DECLSPEC_NOTHROW __attribute__((nothrow))
# else
#  define DECLSPEC_NOTHROW
# endif
#endif
*/
#ifndef DECLSPEC_HIDDEN
# if defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__) || defined(__sun)
#  define DECLSPEC_HIDDEN
# elif defined(__GNUC__) && ((__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 3)))
#  define DECLSPEC_HIDDEN __attribute__((visibility ("hidden")))
# else
#  define DECLSPEC_HIDDEN
# endif
#endif

#endif  /* _WINNT_ */

////////////////////////////////////////////////////////////////////////////////

/*
#ifndef _BASETYPS_H_
#define _BASETYPS_H_

#define COM_STDMETHOD_CAN_THROW


//#  define DECLARE_INTERFACE_C(i) \
//     typedef interface i { struct i##Vtbl *lpVtbl; } i; \
//     typedef struct i##Vtbl i##Vtbl; \
//     struct i##Vtbl

#ifdef __cplusplus
# define EXTERN_C extern "C"
#else
# define EXTERN_C extern
#endif

#undef STDMETHODCALLTYPE
#define STDMETHODCALLTYPE     WINAPI
#define STDMETHODVCALLTYPE    WINAPIV
#define STDAPICALLTYPE        WINAPI
#define STDAPIVCALLTYPE       WINAPIV
#define STDAPI                EXTERN_C HRESULT STDAPICALLTYPE
#define STDAPI_(type)         EXTERN_C type STDAPICALLTYPE
#define STDMETHODIMP          HRESULT STDMETHODCALLTYPE
#define STDMETHODIMP_(type)   type STDMETHODCALLTYPE
#define STDAPIV               EXTERN_C HRESULT STDAPIVCALLTYPE
#define STDAPIV_(type)        EXTERN_C type STDAPIVCALLTYPE
#define STDMETHODIMPV         HRESULT STDMETHODVCALLTYPE
#define STDMETHODIMPV_(type)  type STDMETHODVCALLTYPE

#undef STDMETHOD
#undef STDMETHOD_
#undef PURE
#undef THIS_
#undef THIS
#undef DECLARE_INTERFACE
#undef DECLARE_INTERFACE_

#if defined(__cplusplus) && !defined(CINTERFACE)

#ifdef COM_STDMETHOD_CAN_THROW
# define COM_DECLSPEC_NOTHROW
#else
# define COM_DECLSPEC_NOTHROW DECLSPEC_NOTHROW
#endif

# define interface struct
# define STDMETHOD(m)    virtual COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE m
# define STDMETHOD_(t,m) virtual COM_DECLSPEC_NOTHROW t STDMETHODCALLTYPE m
# define PURE =0
# define THIS_
# define THIS void
# define DECLARE_INTERFACE(i)    interface i
# define DECLARE_INTERFACE_(i,b) interface i : public b
#else
# define STDMETHOD(m) HRESULT (STDMETHODCALLTYPE *m)
# define STDMETHOD_(t,m) t (STDMETHODCALLTYPE *m)
# define PURE
# define THIS_ INTERFACE *,
# define THIS INTERFACE *
# ifdef CONST_VTABLE
#  define DECLARE_INTERFACE(i) \
     typedef interface i { const struct i##Vtbl *lpVtbl; } i; \
     typedef struct i##Vtbl i##Vtbl; \
     struct i##Vtbl
# else
#  define DECLARE_INTERFACE(i) \
     typedef interface i { struct i##Vtbl *lpVtbl; } i; \
     typedef struct i##Vtbl i##Vtbl; \
     struct i##Vtbl
# endif
# define DECLARE_INTERFACE_(i,b) DECLARE_INTERFACE(i)
#endif

//#include <guiddef.h>

#ifndef _ERROR_STATUS_T_DEFINED
typedef unsigned long error_status_t;
#define _ERROR_STATUS_T_DEFINED
#endif

#ifndef _WCHAR_T_DEFINED
#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif
#define _WCHAR_T_DEFINED
#endif

#endif /* _BASETYPS_H_ */

////////////////////////////////////////////////////////////////////////////////

#ifndef _WINDEF_
#define _WINDEF_

//# ifdef UNICODE
#  define WINELIB_NAME_AW(func) func##W
//# else
//#  define WINELIB_NAME_AW(func) func##A
//# endif

#endif /* _WINDEF_ */

////////////////////////////////////////////////////////////////////////////////

#ifndef __D3DX9SHADER_H__
#define __D3DX9SHADER_H__

#define D3DXSHADER_OPTIMIZATION_LEVEL0            0x4000
#define D3DXSHADER_OPTIMIZATION_LEVEL1            0x0
#define D3DXSHADER_OPTIMIZATION_LEVEL2            0xC000
#define D3DXSHADER_OPTIMIZATION_LEVEL3            0x8000

typedef const char *D3DXHANDLE;

typedef struct _D3DXMACRO
{
    const char *Name;
    const char *Definition;
} D3DXMACRO, *LPD3DXMACRO;

typedef struct _D3DXSEMANTIC {
    UINT Usage;
    UINT UsageIndex;
} D3DXSEMANTIC, *LPD3DXSEMANTIC;

typedef enum _D3DXINCLUDE_TYPE
{
    D3DXINC_LOCAL,
    D3DXINC_SYSTEM,
    D3DXINC_FORCE_DWORD = 0x7fffffff,
} D3DXINCLUDE_TYPE, *LPD3DXINCLUDE_TYPE;

#define INTERFACE ID3DXInclude

DECLARE_INTERFACE(ID3DXInclude)
{
    struct ID3DXIncludeVtbl *lpVtbl;
//    STDMETHOD(Open)(THIS_ D3DXINCLUDE_TYPE include_type, const char *filename,
//            const void *parent_data, const void **data, UINT *bytes) PURE;
    HRESULT WINAPI Close(const void *data);
};
#undef INTERFACE

#define ID3DXInclude_Open(p,a,b,c,d,e)  (p)->lpVtbl->Open(p,a,b,c,d,e)
#define ID3DXInclude_Close(p,a)         (p)->lpVtbl->Close(p,a)

typedef struct ID3DXInclude *LPD3DXINCLUDE;

HRESULT WINAPI ID3DXInclude::Close(const void *data)
{
/*    HeapFree(GetProcessHeap(), 0, *((char **)data - 1));
    HeapFree(GetProcessHeap(), 0, (char **)data - 1);
    if (main_file_data == data)
        main_file_data = NULL;
*/    return S_OK;
}

#endif /* __D3DX9SHADER_H__ */

////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////

#ifndef __WINE_D3DX9CORE_H
#define __WINE_D3DX9CORE_H

#define INTERFACE ID3DXBuffer
DECLARE_INTERFACE_(ID3DXBuffer, IUnknown)
{
    /*** IUnknown methods ***/
//    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **out) PURE;
//    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
//    STDMETHOD_(ULONG, Release)(THIS) PURE;
    /*** ID3DXBuffer methods ***/
    STDMETHOD_(void *, GetBufferPointer)(THIS) PURE;
//    STDMETHOD_(DWORD, GetBufferSize)(THIS) PURE;
};
#undef INTERFACE

#endif /* __WINE_D3DX9CORE_H */

////////////////////////////////////////////////////////////////////////////////

#ifndef __WINE_D3DX9_PRIVATE_H
#define __WINE_D3DX9_PRIVATE_H

struct d3dx_include_from_file
{
    ID3DXInclude ID3DXInclude_iface;
};

extern const struct ID3DXIncludeVtbl d3dx_include_from_file_vtbl DECLSPEC_HIDDEN;

#endif /* __WINE_D3DX9_PRIVATE_H */

////////////////////////////////////////////////////////////////////////////////

#ifndef __D3DX9EFFECT_H__
#define __D3DX9EFFECT_H__

#define D3DXFX_DONOTSAVESTATE (1 << 0)
#define D3DXFX_DONOTSAVESHADERSTATE (1 << 1)
#define D3DXFX_DONOTSAVESAMPLERSTATE (1 << 2)
#define D3DXFX_NOT_CLONEABLE (1 << 11)
#define D3DXFX_LARGEADDRESSAWARE (1 << 17)

#define D3DX_PARAMETER_SHARED       1
#define D3DX_PARAMETER_LITERAL      2
#define D3DX_PARAMETER_ANNOTATION   4

typedef struct _D3DXEFFECT_DESC
{
    const char *Creator;
    UINT Parameters;
    UINT Techniques;
    UINT Functions;
} D3DXEFFECT_DESC;
/*
typedef struct _D3DXPARAMETER_DESC
{
    const char *Name;
    const char *Semantic;
    D3DXPARAMETER_CLASS Class;
    D3DXPARAMETER_TYPE Type;
    UINT Rows;
    UINT Columns;
    UINT Elements;
    UINT Annotations;
    UINT StructMembers;
    DWORD Flags;
    UINT Bytes;
} D3DXPARAMETER_DESC;
*/
typedef struct _D3DXTECHNIQUE_DESC
{
    const char *Name;
    UINT Passes;
    UINT Annotations;
} D3DXTECHNIQUE_DESC;
/*
typedef struct _D3DXPASS_DESC
{
    const char *Name;
    UINT Annotations;
    const DWORD *pVertexShaderFunction;
    const DWORD *pPixelShaderFunction;
} D3DXPASS_DESC;

typedef struct _D3DXFUNCTION_DESC
{
    const char *Name;
    UINT Annotations;
} D3DXFUNCTION_DESC;

typedef struct ID3DXEffectPool *LPD3DXEFFECTPOOL;

DEFINE_GUID(IID_ID3DXEffectPool, 0x9537ab04, 0x3250, 0x412e, 0x82, 0x13, 0xfc, 0xd2, 0xf8, 0x67, 0x79, 0x33);

#undef INTERFACE
#define INTERFACE ID3DXEffectPool

DECLARE_INTERFACE_(ID3DXEffectPool, IUnknown)
{
    //// IUnknown methods \\\\
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **out) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
};
#undef INTERFACE
*/
typedef struct ID3DXBaseEffect *LPD3DXBASEEFFECT;

DEFINE_GUID(IID_ID3DXBaseEffect, 0x17c18ac, 0x103f, 0x4417, 0x8c, 0x51, 0x6b, 0xf6, 0xef, 0x1e, 0x56, 0xbe);

#define INTERFACE ID3DXBaseEffect

DECLARE_INTERFACE_(ID3DXBaseEffect, IUnknown)
{
    /*** IUnknown methods ***/
/*    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **out) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    //// ID3DXBaseEffect methods \\\\
    STDMETHOD(GetDesc)(THIS_ D3DXEFFECT_DESC* desc) PURE;
    STDMETHOD(GetParameterDesc)(THIS_ D3DXHANDLE parameter, D3DXPARAMETER_DESC* desc) PURE;
*/    STDMETHOD(GetTechniqueDesc)(THIS_ D3DXHANDLE technique, D3DXTECHNIQUE_DESC* desc) PURE;
/*    STDMETHOD(GetPassDesc)(THIS_ D3DXHANDLE pass, D3DXPASS_DESC* desc) PURE;
    STDMETHOD(GetFunctionDesc)(THIS_ D3DXHANDLE shader, D3DXFUNCTION_DESC* desc) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameter)(THIS_ D3DXHANDLE parameter, UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameterByName)(THIS_ D3DXHANDLE parameter, const char *name) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameterBySemantic)(THIS_ D3DXHANDLE parameter, const char *semantic) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameterElement)(THIS_ D3DXHANDLE parameter, UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetTechnique)(THIS_ UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetTechniqueByName)(THIS_ const char *name) PURE;
    STDMETHOD_(D3DXHANDLE, GetPass)(THIS_ D3DXHANDLE technique, UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetPassByName)(THIS_ D3DXHANDLE technique, const char *name) PURE;
    STDMETHOD_(D3DXHANDLE, GetFunction)(THIS_ UINT index);
    STDMETHOD_(D3DXHANDLE, GetFunctionByName)(THIS_ const char *name);
    STDMETHOD_(D3DXHANDLE, GetAnnotation)(THIS_ D3DXHANDLE object, UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetAnnotationByName)(THIS_ D3DXHANDLE object, const char *name) PURE;
*/    STDMETHOD(SetValue)(THIS_ D3DXHANDLE parameter, const void *data, UINT bytes) PURE;
/*    STDMETHOD(GetValue)(THIS_ D3DXHANDLE parameter, void *data, UINT bytes) PURE;
    STDMETHOD(SetBool)(THIS_ D3DXHANDLE parameter, BOOL b) PURE;
    STDMETHOD(GetBool)(THIS_ D3DXHANDLE parameter, BOOL* b) PURE;
    STDMETHOD(SetBoolArray)(THIS_ D3DXHANDLE parameter, const BOOL *b, UINT count) PURE;
    STDMETHOD(GetBoolArray)(THIS_ D3DXHANDLE parameter, BOOL* b, UINT count) PURE;
    STDMETHOD(SetInt)(THIS_ D3DXHANDLE parameter, INT n) PURE;
    STDMETHOD(GetInt)(THIS_ D3DXHANDLE parameter, INT* n) PURE;
    STDMETHOD(SetIntArray)(THIS_ D3DXHANDLE parameter, const INT *n, UINT count) PURE;
    STDMETHOD(GetIntArray)(THIS_ D3DXHANDLE parameter, INT* n, UINT count) PURE;
*/    STDMETHOD(SetFloat)(THIS_ D3DXHANDLE parameter, FLOAT f) PURE;
/*    STDMETHOD(GetFloat)(THIS_ D3DXHANDLE parameter, FLOAT* f) PURE;
    STDMETHOD(SetFloatArray)(THIS_ D3DXHANDLE parameter, const FLOAT *f, UINT count) PURE;
    STDMETHOD(GetFloatArray)(THIS_ D3DXHANDLE parameter, FLOAT* f, UINT count) PURE;
    STDMETHOD(SetVector)(THIS_ D3DXHANDLE parameter, const D3DXVECTOR4 *vector) PURE;
    STDMETHOD(GetVector)(THIS_ D3DXHANDLE parameter, D3DXVECTOR4* vector) PURE;
    STDMETHOD(SetVectorArray)(THIS_ D3DXHANDLE parameter, const D3DXVECTOR4 *vector, UINT count) PURE;
    STDMETHOD(GetVectorArray)(THIS_ D3DXHANDLE parameter, D3DXVECTOR4* vector, UINT count) PURE;
    STDMETHOD(SetMatrix)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX *matrix) PURE;
    STDMETHOD(GetMatrix)(THIS_ D3DXHANDLE parameter, D3DXMATRIX* matrix) PURE;
    STDMETHOD(SetMatrixArray)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX *matrix, UINT count) PURE;
    STDMETHOD(GetMatrixArray)(THIS_ D3DXHANDLE parameter, D3DXMATRIX* matrix, UINT count) PURE;
    STDMETHOD(SetMatrixPointerArray)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX **matrix, UINT count) PURE;
    STDMETHOD(GetMatrixPointerArray)(THIS_ D3DXHANDLE parameter, D3DXMATRIX** matrix, UINT count) PURE;
    STDMETHOD(SetMatrixTranspose)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX *matrix) PURE;
    STDMETHOD(GetMatrixTranspose)(THIS_ D3DXHANDLE parameter, D3DXMATRIX* matrix) PURE;
    STDMETHOD(SetMatrixTransposeArray)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX *matrix, UINT count) PURE;
    STDMETHOD(GetMatrixTransposeArray)(THIS_ D3DXHANDLE parameter, D3DXMATRIX* matrix, UINT count) PURE;
    STDMETHOD(SetMatrixTransposePointerArray)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX **matrix, UINT count) PURE;
    STDMETHOD(GetMatrixTransposePointerArray)(THIS_ D3DXHANDLE parameter, D3DXMATRIX** matrix, UINT count) PURE;
    STDMETHOD(SetString)(THIS_ D3DXHANDLE parameter, const char *string) PURE;
    STDMETHOD(GetString)(THIS_ D3DXHANDLE parameter, const char **string) PURE;
    STDMETHOD(SetTexture)(THIS_ D3DXHANDLE parameter, struct IDirect3DBaseTexture9 *texture) PURE;
    STDMETHOD(GetTexture)(THIS_ D3DXHANDLE parameter, struct IDirect3DBaseTexture9 **texture) PURE;
    STDMETHOD(GetPixelShader)(THIS_ D3DXHANDLE parameter, struct IDirect3DPixelShader9 **shader) PURE;
    STDMETHOD(GetVertexShader)(THIS_ D3DXHANDLE parameter, struct IDirect3DVertexShader9 **shader) PURE;
    STDMETHOD(SetArrayRange)(THIS_ D3DXHANDLE parameter, UINT start, UINT end) PURE;
*/};
#undef INTERFACE
/*
typedef struct ID3DXEffectStateManager *LPD3DXEFFECTSTATEMANAGER;

DEFINE_GUID(IID_ID3DXEffectStateManager, 0x79aab587, 0x6dbc, 0x4fa7, 0x82, 0xde, 0x37, 0xfa, 0x17, 0x81, 0xc5, 0xce);

#define INTERFACE ID3DXEffectStateManager

DECLARE_INTERFACE_(ID3DXEffectStateManager, IUnknown)
{
    //// IUnknown methods \\\\
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **out) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    //// ID3DXEffectStateManager methods \\\\
    STDMETHOD(SetTransform)(THIS_ D3DTRANSFORMSTATETYPE state, const D3DMATRIX *matrix) PURE;
    STDMETHOD(SetMaterial)(THIS_ const D3DMATERIAL9 *material) PURE;
    STDMETHOD(SetLight)(THIS_ DWORD index, const D3DLIGHT9 *light) PURE;
    STDMETHOD(LightEnable)(THIS_ DWORD index, BOOL enable) PURE;
    STDMETHOD(SetRenderState)(THIS_ D3DRENDERSTATETYPE state, DWORD value) PURE;
    STDMETHOD(SetTexture)(THIS_ DWORD stage, struct IDirect3DBaseTexture9 *texture) PURE;
    STDMETHOD(SetTextureStageState)(THIS_ DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value) PURE;
    STDMETHOD(SetSamplerState)(THIS_ DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value) PURE;
    STDMETHOD(SetNPatchMode)(THIS_ FLOAT num_segments) PURE;
    STDMETHOD(SetFVF)(THIS_ DWORD format) PURE;
    STDMETHOD(SetVertexShader)(THIS_ struct IDirect3DVertexShader9 *shader) PURE;
    STDMETHOD(SetVertexShaderConstantF)(THIS_ UINT register_index, const FLOAT *constant_data, UINT register_count) PURE;
    STDMETHOD(SetVertexShaderConstantI)(THIS_ UINT register_index, const INT *constant_data, UINT register_count) PURE;
    STDMETHOD(SetVertexShaderConstantB)(THIS_ UINT register_index, const BOOL *constant_data, UINT register_count) PURE;
    STDMETHOD(SetPixelShader)(THIS_ struct IDirect3DPixelShader9 *shader) PURE;
    STDMETHOD(SetPixelShaderConstantF)(THIS_ UINT register_index, const FLOAT *constant_data, UINT register_count) PURE;
    STDMETHOD(SetPixelShaderConstantI)(THIS_ UINT register_index, const INT *constant_data, UINT register_count) PURE;
    STDMETHOD(SetPixelShaderConstantB)(THIS_ UINT register_index, const BOOL *constant_data, UINT register_count) PURE;
};
#undef INTERFACE
*/
typedef struct ID3DXEffect *LPD3DXEFFECT;

#if D3DX_SDK_VERSION <= 25
DEFINE_GUID(IID_ID3DXEffect, 0xd165ccb1, 0x62b0, 0x4a33, 0xb3, 0xfa, 0xa9, 0x23, 0x00, 0x30, 0x5a, 0x11);
#elif D3DX_SDK_VERSION == 26
DEFINE_GUID(IID_ID3DXEffect, 0xc7b17651, 0x5420, 0x490e, 0x8a, 0x7f, 0x92, 0x36, 0x75, 0xa2, 0xd6, 0x87);
#else
DEFINE_GUID(IID_ID3DXEffect, 0xf6ceb4b3, 0x4e4c, 0x40dd, 0xb8, 0x83, 0x8d, 0x8d, 0xe5, 0xea, 0x0c, 0xd5);
#endif

#define INTERFACE ID3DXEffect

DECLARE_INTERFACE_(ID3DXEffect, ID3DXBaseEffect)
{
    /*** IUnknown methods ***/
/*    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **out) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    //// ID3DXBaseEffect methods \\\\
    STDMETHOD(GetDesc)(THIS_ D3DXEFFECT_DESC* desc) PURE;
    STDMETHOD(GetParameterDesc)(THIS_ D3DXHANDLE parameter, D3DXPARAMETER_DESC* desc) PURE;
*/    STDMETHOD(GetTechniqueDesc)(THIS_ D3DXHANDLE technique, D3DXTECHNIQUE_DESC* desc) PURE;
/*    STDMETHOD(GetPassDesc)(THIS_ D3DXHANDLE pass, D3DXPASS_DESC* desc) PURE;
    STDMETHOD(GetFunctionDesc)(THIS_ D3DXHANDLE shader, D3DXFUNCTION_DESC* desc) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameter)(THIS_ D3DXHANDLE parameter, UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameterByName)(THIS_ D3DXHANDLE parameter, const char *name) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameterBySemantic)(THIS_ D3DXHANDLE parameter, const char *semantic) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameterElement)(THIS_ D3DXHANDLE parameter, UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetTechnique)(THIS_ UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetTechniqueByName)(THIS_ const char *name) PURE;
    STDMETHOD_(D3DXHANDLE, GetPass)(THIS_ D3DXHANDLE technique, UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetPassByName)(THIS_ D3DXHANDLE technique, const char *name) PURE;
    STDMETHOD_(D3DXHANDLE, GetFunction)(THIS_ UINT index);
    STDMETHOD_(D3DXHANDLE, GetFunctionByName)(THIS_ const char *name);
    STDMETHOD_(D3DXHANDLE, GetAnnotation)(THIS_ D3DXHANDLE object, UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetAnnotationByName)(THIS_ D3DXHANDLE object, const char *name) PURE;
*/    STDMETHOD(SetValue)(THIS_ D3DXHANDLE parameter, const void *data, UINT bytes) PURE;
/*    STDMETHOD(GetValue)(THIS_ D3DXHANDLE parameter, void *data, UINT bytes) PURE;
    STDMETHOD(SetBool)(THIS_ D3DXHANDLE parameter, BOOL b) PURE;
    STDMETHOD(GetBool)(THIS_ D3DXHANDLE parameter, BOOL* b) PURE;
    STDMETHOD(SetBoolArray)(THIS_ D3DXHANDLE parameter, const BOOL *b, UINT count) PURE;
    STDMETHOD(GetBoolArray)(THIS_ D3DXHANDLE parameter, BOOL* b, UINT count) PURE;
    STDMETHOD(SetInt)(THIS_ D3DXHANDLE parameter, INT n) PURE;
    STDMETHOD(GetInt)(THIS_ D3DXHANDLE parameter, INT* n) PURE;
    STDMETHOD(SetIntArray)(THIS_ D3DXHANDLE parameter, const INT *n, UINT count) PURE;
    STDMETHOD(GetIntArray)(THIS_ D3DXHANDLE parameter, INT* n, UINT count) PURE;
    STDMETHOD(SetFloat)(THIS_ D3DXHANDLE parameter, FLOAT f) PURE;
    STDMETHOD(GetFloat)(THIS_ D3DXHANDLE parameter, FLOAT* f) PURE;
    STDMETHOD(SetFloatArray)(THIS_ D3DXHANDLE parameter, const FLOAT *f, UINT count) PURE;
    STDMETHOD(GetFloatArray)(THIS_ D3DXHANDLE parameter, FLOAT* f, UINT count) PURE;
    STDMETHOD(SetVector)(THIS_ D3DXHANDLE parameter, const D3DXVECTOR4 *vector) PURE;
    STDMETHOD(GetVector)(THIS_ D3DXHANDLE parameter, D3DXVECTOR4* vector) PURE;
    STDMETHOD(SetVectorArray)(THIS_ D3DXHANDLE parameter, const D3DXVECTOR4 *vector, UINT count) PURE;
    STDMETHOD(GetVectorArray)(THIS_ D3DXHANDLE parameter, D3DXVECTOR4* vector, UINT count) PURE;
    STDMETHOD(SetMatrix)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX *matrix) PURE;
    STDMETHOD(GetMatrix)(THIS_ D3DXHANDLE parameter, D3DXMATRIX* matrix) PURE;
    STDMETHOD(SetMatrixArray)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX *matrix, UINT count) PURE;
    STDMETHOD(GetMatrixArray)(THIS_ D3DXHANDLE parameter, D3DXMATRIX* matrix, UINT count) PURE;
    STDMETHOD(SetMatrixPointerArray)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX **matrix, UINT count) PURE;
    STDMETHOD(GetMatrixPointerArray)(THIS_ D3DXHANDLE parameter, D3DXMATRIX** matrix, UINT count) PURE;
    STDMETHOD(SetMatrixTranspose)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX *matrix) PURE;
    STDMETHOD(GetMatrixTranspose)(THIS_ D3DXHANDLE parameter, D3DXMATRIX* matrix) PURE;
    STDMETHOD(SetMatrixTransposeArray)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX *matrix, UINT count) PURE;
    STDMETHOD(GetMatrixTransposeArray)(THIS_ D3DXHANDLE parameter, D3DXMATRIX* matrix, UINT count) PURE;
    STDMETHOD(SetMatrixTransposePointerArray)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX **matrix, UINT count) PURE;
    STDMETHOD(GetMatrixTransposePointerArray)(THIS_ D3DXHANDLE parameter, D3DXMATRIX** matrix, UINT count) PURE;
    STDMETHOD(SetString)(THIS_ D3DXHANDLE parameter, const char *string) PURE;
    STDMETHOD(GetString)(THIS_ D3DXHANDLE parameter, const char **string) PURE;
    STDMETHOD(SetTexture)(THIS_ D3DXHANDLE parameter, struct IDirect3DBaseTexture9 *texture) PURE;
    STDMETHOD(GetTexture)(THIS_ D3DXHANDLE parameter, struct IDirect3DBaseTexture9 **texture) PURE;
    STDMETHOD(GetPixelShader)(THIS_ D3DXHANDLE parameter, struct IDirect3DPixelShader9 **shader) PURE;
    STDMETHOD(GetVertexShader)(THIS_ D3DXHANDLE parameter, struct IDirect3DVertexShader9 **shader) PURE;
    STDMETHOD(SetArrayRange)(THIS_ D3DXHANDLE parameter, UINT start, UINT end) PURE;
    //// ID3DXEffect methods \\\\
    STDMETHOD(GetPool)(THIS_ ID3DXEffectPool **pool) PURE;
*/    STDMETHOD(SetTechnique)(THIS_ D3DXHANDLE technique) PURE;
//    STDMETHOD_(D3DXHANDLE, GetCurrentTechnique)(THIS) PURE;
//    STDMETHOD(ValidateTechnique)(THIS_ D3DXHANDLE technique) PURE;
    STDMETHOD(FindNextValidTechnique)(THIS_ D3DXHANDLE technique, D3DXHANDLE* next_technique) PURE;
//    STDMETHOD_(BOOL, IsParameterUsed)(THIS_ D3DXHANDLE parameter, D3DXHANDLE technique) PURE;
    STDMETHOD(Begin)(THIS_ UINT *passes, DWORD flags) PURE;
    STDMETHOD(BeginPass)(THIS_ UINT pass) PURE;
//    STDMETHOD(CommitChanges)(THIS) PURE;
    STDMETHOD(EndPass)(THIS) PURE;
    STDMETHOD(End)(THIS) PURE;
/*    STDMETHOD(GetDevice)(THIS_ struct IDirect3DDevice9 **device) PURE;
    STDMETHOD(OnLostDevice)(THIS) PURE;
    STDMETHOD(OnResetDevice)(THIS) PURE;
    STDMETHOD(SetStateManager)(THIS_ ID3DXEffectStateManager *manager) PURE;
    STDMETHOD(GetStateManager)(THIS_ ID3DXEffectStateManager **manager) PURE;
    STDMETHOD(BeginParameterBlock)(THIS) PURE;
    STDMETHOD_(D3DXHANDLE, EndParameterBlock)(THIS) PURE;
    STDMETHOD(ApplyParameterBlock)(THIS_ D3DXHANDLE parameter_block) PURE;
#if D3DX_SDK_VERSION >= 26
    STDMETHOD(DeleteParameterBlock)(THIS_ D3DXHANDLE parameter_block) PURE;
#endif
    STDMETHOD(CloneEffect)(THIS_ struct IDirect3DDevice9 *device, struct ID3DXEffect **effect) PURE;
#if D3DX_SDK_VERSION >= 27
    STDMETHOD(SetRawValue)(THIS_ D3DXHANDLE parameter, const void *data, UINT byte_offset, UINT bytes) PURE;
#endif
*/};

#undef INTERFACE
/*
typedef struct ID3DXEffectCompiler *LPD3DXEFFECTCOMPILER;

DEFINE_GUID(IID_ID3DXEffectCompiler, 0x51b8a949, 0x1a31, 0x47e6, 0xbe, 0xa0, 0x4b, 0x30, 0xdb, 0x53, 0xf1, 0xe0);

#define INTERFACE ID3DXEffectCompiler

DECLARE_INTERFACE_(ID3DXEffectCompiler, ID3DXBaseEffect)
{
    //// IUnknown methods \\\\
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **out) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    //// ID3DXBaseEffect methods \\\\
    STDMETHOD(GetDesc)(THIS_ D3DXEFFECT_DESC* desc) PURE;
    STDMETHOD(GetParameterDesc)(THIS_ D3DXHANDLE parameter, D3DXPARAMETER_DESC* desc) PURE;
    STDMETHOD(GetTechniqueDesc)(THIS_ D3DXHANDLE technique, D3DXTECHNIQUE_DESC* desc) PURE;
    STDMETHOD(GetPassDesc)(THIS_ D3DXHANDLE pass, D3DXPASS_DESC* desc) PURE;
    STDMETHOD(GetFunctionDesc)(THIS_ D3DXHANDLE shader, D3DXFUNCTION_DESC* desc) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameter)(THIS_ D3DXHANDLE parameter, UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameterByName)(THIS_ D3DXHANDLE parameter, const char *name) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameterBySemantic)(THIS_ D3DXHANDLE parameter, const char *semantic) PURE;
    STDMETHOD_(D3DXHANDLE, GetParameterElement)(THIS_ D3DXHANDLE parameter, UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetTechnique)(THIS_ UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetTechniqueByName)(THIS_ const char *name) PURE;
    STDMETHOD_(D3DXHANDLE, GetPass)(THIS_ D3DXHANDLE technique, UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetPassByName)(THIS_ D3DXHANDLE technique, const char *name) PURE;
    STDMETHOD_(D3DXHANDLE, GetFunction)(THIS_ UINT index);
    STDMETHOD_(D3DXHANDLE, GetFunctionByName)(THIS_ const char *name);
    STDMETHOD_(D3DXHANDLE, GetAnnotation)(THIS_ D3DXHANDLE object, UINT index) PURE;
    STDMETHOD_(D3DXHANDLE, GetAnnotationByName)(THIS_ D3DXHANDLE object, const char *name) PURE;
    STDMETHOD(SetValue)(THIS_ D3DXHANDLE parameter, const void *data, UINT bytes) PURE;
    STDMETHOD(GetValue)(THIS_ D3DXHANDLE parameter, void *data, UINT bytes) PURE;
    STDMETHOD(SetBool)(THIS_ D3DXHANDLE parameter, BOOL b) PURE;
    STDMETHOD(GetBool)(THIS_ D3DXHANDLE parameter, BOOL* b) PURE;
    STDMETHOD(SetBoolArray)(THIS_ D3DXHANDLE parameter, const BOOL *b, UINT count) PURE;
    STDMETHOD(GetBoolArray)(THIS_ D3DXHANDLE parameter, BOOL* b, UINT count) PURE;
    STDMETHOD(SetInt)(THIS_ D3DXHANDLE parameter, INT n) PURE;
    STDMETHOD(GetInt)(THIS_ D3DXHANDLE parameter, INT* n) PURE;
    STDMETHOD(SetIntArray)(THIS_ D3DXHANDLE parameter, const INT *n, UINT count) PURE;
    STDMETHOD(GetIntArray)(THIS_ D3DXHANDLE parameter, INT* n, UINT count) PURE;
    STDMETHOD(SetFloat)(THIS_ D3DXHANDLE parameter, FLOAT f) PURE;
    STDMETHOD(GetFloat)(THIS_ D3DXHANDLE parameter, FLOAT* f) PURE;
    STDMETHOD(SetFloatArray)(THIS_ D3DXHANDLE parameter, const FLOAT *f, UINT count) PURE;
    STDMETHOD(GetFloatArray)(THIS_ D3DXHANDLE parameter, FLOAT* f, UINT count) PURE;
    STDMETHOD(SetVector)(THIS_ D3DXHANDLE parameter, const D3DXVECTOR4 *vector) PURE;
    STDMETHOD(GetVector)(THIS_ D3DXHANDLE parameter, D3DXVECTOR4* vector) PURE;
    STDMETHOD(SetVectorArray)(THIS_ D3DXHANDLE parameter, const D3DXVECTOR4 *vector, UINT count) PURE;
    STDMETHOD(GetVectorArray)(THIS_ D3DXHANDLE parameter, D3DXVECTOR4* vector, UINT count) PURE;
    STDMETHOD(SetMatrix)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX *matrix) PURE;
    STDMETHOD(GetMatrix)(THIS_ D3DXHANDLE parameter, D3DXMATRIX* matrix) PURE;
    STDMETHOD(SetMatrixArray)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX *matrix, UINT count) PURE;
    STDMETHOD(GetMatrixArray)(THIS_ D3DXHANDLE parameter, D3DXMATRIX* matrix, UINT count) PURE;
    STDMETHOD(SetMatrixPointerArray)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX **matrix, UINT count) PURE;
    STDMETHOD(GetMatrixPointerArray)(THIS_ D3DXHANDLE parameter, D3DXMATRIX** matrix, UINT count) PURE;
    STDMETHOD(SetMatrixTranspose)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX *matrix) PURE;
    STDMETHOD(GetMatrixTranspose)(THIS_ D3DXHANDLE parameter, D3DXMATRIX* matrix) PURE;
    STDMETHOD(SetMatrixTransposeArray)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX *matrix, UINT count) PURE;
    STDMETHOD(GetMatrixTransposeArray)(THIS_ D3DXHANDLE parameter, D3DXMATRIX* matrix, UINT count) PURE;
    STDMETHOD(SetMatrixTransposePointerArray)(THIS_ D3DXHANDLE parameter, const D3DXMATRIX **matrix, UINT count) PURE;
    STDMETHOD(GetMatrixTransposePointerArray)(THIS_ D3DXHANDLE parameter, D3DXMATRIX** matrix, UINT count) PURE;
    STDMETHOD(SetString)(THIS_ D3DXHANDLE parameter, const char *string) PURE;
    STDMETHOD(GetString)(THIS_ D3DXHANDLE parameter, const char **string) PURE;
    STDMETHOD(SetTexture)(THIS_ D3DXHANDLE parameter, struct IDirect3DBaseTexture9 *texture) PURE;
    STDMETHOD(GetTexture)(THIS_ D3DXHANDLE parameter, struct IDirect3DBaseTexture9 **texture) PURE;
    STDMETHOD(GetPixelShader)(THIS_ D3DXHANDLE parameter, struct IDirect3DPixelShader9 **shader) PURE;
    STDMETHOD(GetVertexShader)(THIS_ D3DXHANDLE parameter, struct IDirect3DVertexShader9 **shader) PURE;
    STDMETHOD(SetArrayRange)(THIS_ D3DXHANDLE parameter, UINT start, UINT end) PURE;
    //// ID3DXEffectCompiler methods \\\\
    STDMETHOD(SetLiteral)(THIS_ D3DXHANDLE parameter, BOOL literal) PURE;
    STDMETHOD(GetLiteral)(THIS_ D3DXHANDLE parameter, BOOL* literal) PURE;
    STDMETHOD(CompileEffect)(THIS_ DWORD flags, ID3DXBuffer **effect, ID3DXBuffer **error_msgs) PURE;
    STDMETHOD(CompileShader)(THIS_ D3DXHANDLE function, const char *target, DWORD flags,
            ID3DXBuffer **shader, ID3DXBuffer **error_msgs, ID3DXConstantTable **constant_table) PURE;
};
#undef INTERFACE
*/
#ifdef __cplusplus
extern "C" {
#endif
/*
HRESULT WINAPI D3DXCreateEffectPool(ID3DXEffectPool **pool);
HRESULT WINAPI D3DXCreateEffect(struct IDirect3DDevice9 *device, const void *srcdata, UINT srcdatalen,
        const D3DXMACRO *defines, struct ID3DXInclude *include, DWORD flags,
        struct ID3DXEffectPool *pool, struct ID3DXEffect **effect, struct ID3DXBuffer **compilation_errors);
HRESULT WINAPI D3DXCreateEffectEx(struct IDirect3DDevice9 *device, const void *srcdata, UINT srcdatalen,
        const D3DXMACRO *defines, struct ID3DXInclude *include, const char *skip_constants, DWORD flags,
        struct ID3DXEffectPool *pool, struct ID3DXEffect **effect, struct ID3DXBuffer **compilation_errors);
HRESULT WINAPI D3DXCreateEffectCompiler(const char *srcdata, UINT srcdatalen, const D3DXMACRO *defines,
        ID3DXInclude *include, DWORD flags, ID3DXEffectCompiler **compiler, ID3DXBuffer **parse_errors);
HRESULT WINAPI D3DXCreateEffectFromFileExA(struct IDirect3DDevice9 *device, const char *srcfile,
        const D3DXMACRO *defines, struct ID3DXInclude *include, const char *skip_constants, DWORD flags,
        struct ID3DXEffectPool *pool, struct ID3DXEffect **effect, struct ID3DXBuffer **compilation_errors);
*/HRESULT WINAPI D3DXCreateEffectFromFileExW(struct IDirect3DDevice9 *device, const WCHAR *srcfile,
        const D3DXMACRO *defines, struct ID3DXInclude *include, const char *skip_constants, DWORD flags,
        struct ID3DXEffectPool *pool, struct ID3DXEffect **effect, struct ID3DXBuffer **compilation_errors);
/*#define D3DXCreateEffectFromFileEx WINELIB_NAME_AW(D3DXCreateEffectFromFileEx)

HRESULT WINAPI D3DXCreateEffectFromFileA(struct IDirect3DDevice9 *device, const char *srcfile,
        const D3DXMACRO *defines, struct ID3DXInclude *include, DWORD flags,
        struct ID3DXEffectPool *pool, struct ID3DXEffect **effect, struct ID3DXBuffer **compilation_errors);
*/HRESULT WINAPI D3DXCreateEffectFromFileW(struct IDirect3DDevice9 *device, const WCHAR *srcfile,
        const D3DXMACRO *defines, struct ID3DXInclude *include, DWORD flags,
        struct ID3DXEffectPool *pool, struct ID3DXEffect **effect, struct ID3DXBuffer **compilation_errors);
#define D3DXCreateEffectFromFile WINELIB_NAME_AW(D3DXCreateEffectFromFile)
/*
HRESULT WINAPI D3DXCreateEffectFromResourceExA(struct IDirect3DDevice9 *device, HMODULE srcmodule,
        const char *srcresource, const D3DXMACRO *defines, struct ID3DXInclude *include,
        const char *skip_constants, DWORD flags, struct ID3DXEffectPool *pool,
        struct ID3DXEffect **effect, struct ID3DXBuffer **compilation_errors);
HRESULT WINAPI D3DXCreateEffectFromResourceExW(struct IDirect3DDevice9 *device, HMODULE srcmodule,
        const WCHAR *srcresource, const D3DXMACRO *defines, struct ID3DXInclude *include,
        const char *skip_constants, DWORD flags, struct ID3DXEffectPool *pool,
        struct ID3DXEffect **effect, struct ID3DXBuffer **compilation_errors);
#define D3DXCreateEffectFromResourceEx WINELIB_NAME_AW(D3DXCreateEffectFromResourceEx)

HRESULT WINAPI D3DXCreateEffectFromResourceA(struct IDirect3DDevice9 *device, HMODULE srcmodule,
        const char *srcresource, const D3DXMACRO *defines, struct ID3DXInclude *include, DWORD flags,
        struct ID3DXEffectPool *pool, struct ID3DXEffect **effect, struct ID3DXBuffer **compilation_errors);
HRESULT WINAPI D3DXCreateEffectFromResourceW(struct IDirect3DDevice9 *device, HMODULE srcmodule,
        const WCHAR *srcresource, const D3DXMACRO *defines, struct ID3DXInclude *include, DWORD flags,
        struct ID3DXEffectPool *pool, struct ID3DXEffect **effect, struct ID3DXBuffer **compilation_errors);
#define D3DXCreateEffectFromResource WINELIB_NAME_AW(D3DXCreateEffectFromResource)

HRESULT WINAPI D3DXCreateEffectCompilerFromFileA(const char *srcfile, const D3DXMACRO *defines,
        ID3DXInclude *include, DWORD flags, ID3DXEffectCompiler **effectcompiler, ID3DXBuffer **parseerrors);
HRESULT WINAPI D3DXCreateEffectCompilerFromFileW(const WCHAR *srcfile, const D3DXMACRO *defines,
        ID3DXInclude *include, DWORD flags, ID3DXEffectCompiler **effectcompiler, ID3DXBuffer **parseerrors);
#define D3DXCreateEffectCompilerFromFile WINELIB_NAME_AW(D3DXCreateEffectCompilerFromFile)

HRESULT WINAPI D3DXCreateEffectCompilerFromResourceA(HMODULE srcmodule, const char *srcresource,
        const D3DXMACRO *defines, ID3DXInclude *include, DWORD flags,
        ID3DXEffectCompiler **effectcompiler, ID3DXBuffer **parseerrors);
HRESULT WINAPI D3DXCreateEffectCompilerFromResourceW(HMODULE srcmodule, const WCHAR *srcresource,
        const D3DXMACRO *defines, ID3DXInclude *include, DWORD flags,
        ID3DXEffectCompiler **effectcompiler, ID3DXBuffer **parseerrors);
#define D3DXCreateEffectCompilerFromResource WINELIB_NAME_AW(D3DXCreateEffectCompilerFromResource)

HRESULT WINAPI D3DXDisassembleEffect(ID3DXEffect *effect, BOOL enable_color_code, ID3DXBuffer **disassembly);
*/
#ifdef __cplusplus
}
#endif

#endif /* __D3DX9EFFECT_H__ */