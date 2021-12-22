// ************************************************************************
// *  Author : Vano, 1861; Description : Sea/Ocean/Water/Lake/River/pool  *
// ************************************************************************
#pragma once

#include "Sea_Base.h"
#include "cvector4.h"
#include "dx9render.h"
#include "defines.h"


class SEA : public SEA_BASE
{
  private:
    // uint32_t dwSkyCode = MakeHashValue("sky");
    uint32_t dwIslandCode = MakeHashValue("island");
    uint32_t dwShipCode = MakeHashValue("ship");
    uint32_t dwSailCode = MakeHashValue("sail");

    static IDirect3DVertexDeclaration9 *vertexDecl_;

    struct SeaVertex
    {
        CVECTOR vPos;
        CVECTOR vNormal;
        float tu, tv;
    };

    struct SeaBlock
    {
        long iX1, iX2, iY1, iY2; // result rectangle(in units)
        long iSize0;             //

        long iTX, iTY;
        long iSize;
        long iLOD;
        long iIStart;
        long iIFirst, iILast;

        bool bInProgress, bDone;

        SeaBlock(long iX1, long iX2, long iY1, long iY2, long iSize0, long iTX, long iTY, long iSize, long iLOD,
                 long iIStart, long iIFirst, long iILast, bool bInProgress, bool bDone)
        {
            this->iX1 = iX1;
            this->iX2 = iX2;
            this->iY1 = iY1;
            this->iY2 = iY2;
            this->iSize0 = iSize0;
            this->iTX = iTX;
            this->iTY = iTY;
            this->iSize = iSize;
            this->iLOD = iLOD;
            this->iIStart = iIStart;
            this->iIFirst = iIFirst;
            this->iILast = iILast;
            this->bInProgress = bInProgress;
            this->bDone = bDone;
        }

        static bool QSort(const SeaBlock &b1, const SeaBlock &b2)
        {
            return (b1.iLOD > b2.iLOD);
        }

        inline bool operator<(const SeaBlock &B) const
        {
            return (B.iLOD < iLOD);
        };
    };

    struct SeaTrash
    {
        CVECTOR vPos;
        CVECTOR vSpeed;
        uint32_t dwSubTexture;
        float fTime;
        float fSize;
    };

    struct SeaLight
    {
        CVECTOR vPos;
        CVECTOR vSpeed;
        uint32_t dwSubTexture;
        float fTime;
    };

    VDX9RENDER *rs;
    static SEA *pSea;

    uint32_t dwMaxDim, dwMinDim;
    float fMaxSeaHeight;
    float fGridStep;
    float fLodScale;
    float fBumpScale, fBumpSpeed;
    CVECTOR vMoveSpeed1, vMoveSpeed2;

    CVECTOR vCamPos, vSeaCenterPos, vWorldOffset;

    bool bStarted;
    bool bUnderSea;
    bool bUnderSeaEnable;
    float bUnderSeaStarted;

    long iVSeaBuffer, iISeaBuffer;

    IDirect3DVolumeTexture9 *pVolumeTexture;
    IDirect3DCubeTexture9 *pEnvMap, *pSunRoadMap;
    IDirect3DSurface9 *pZStencil;

    IDirect3DTexture9 *pReflection, *pReflectionSunroad;
    IDirect3DSurface9 *pReflectionSurfaceDepth;

    bool bSimpleSea;

    SeaVertex *pVSea;
    uint16_t *pTriangles;
    uint32_t *pIndices;
    long iVStart, iTStart, iIStart;

    PLANE *pFrustumPlanes;
    uint32_t dwNumFrustumPlanes;

    long iSeaTrashTexture;
    float fLastTrashTime;
    std::vector<SeaTrash> aSeaTrash;
    std::vector<RS_RECT> aTrashRects;

    long iSeaLightTexture;
    std::vector<SeaLight> aSeaLights;
    float fLastLightTime;
    std::vector<RS_RECT> aLightsRects;

    std::vector<uint32_t *> aNormals;
    std::vector<uint8_t *> aBumps;
    std::vector<SeaBlock> aBlocks;

    CVECTOR4 v4SeaColor, v4SkyColor, v4SeaParameters;

    std::vector<IDirect3DTexture9 *> aBumpMaps;
    IDirect3DTexture9 *pRenderTargetBumpMap;

    float *pSeaFrame1, *pSeaFrame2, *pSeaNormalsFrame1, *pSeaNormalsFrame2;

    CVECTOR vMove1, vMove2;
    float fFrame1, fFrame2;
    float fScale1, fScale2;
    float fAmp1, fAmp2;
    float _fAmp1, _fAmp2;
    float fAnimSpeed1, fAnimSpeed2;
    float fPosShift;
    float fFrenel;

    bool bStop;

    float fFogSeaDensity, fFogStartDistance;
    CVECTOR vFogColor;
    bool bFogEnable;

    bool bTempFullMode;
    float fTempGridStep, fTempLodScale;

    bool bIniFoamEnable, bFoamEnable;
    float fFoamV, fFoamK, fFoamUV, fFoamTextureDisturb;
    long iFoamTexture;

    CMatrix mTexProjection;

    void SSE_WaveXZ(SeaVertex **pArray);
    float WaveXZ(float x, float z, CVECTOR *pNormal = nullptr) override;

    void AddBlock(long iTX, long iTY, long iSize, long iLOD);
    void BuildTree(long iTX, long iTY, long iLev);
    void SetBlock(uint32_t dwBlockIndex);

    bool isVisibleBBox(const CVECTOR &vCenter, const CVECTOR &v1, const CVECTOR &v2);
    void CalculateLOD(const CVECTOR &v1, const CVECTOR &v2, long &iMaxLOD, long &iMinLOD);
    inline float CalcLod(const float &x, const float &y, const float &z);
    [[deprecated]] void WaveXZBlock(SeaBlock &pB);
    void SSE_WaveXZBlock(SeaBlock &pB);
    SeaBlock *GetUndoneBlock();
    void PrepareIndicesForBlock(uint32_t dwBlockIndex);

    long VisCode(const CVECTOR &vP);

    void CalculateHeightMap(float fFrame, float fAmplitude, float *pfOut, std::vector<uint8_t *> &aFrames);
    void CalculateNormalMap(float fFrame, float fAmplitude, float *pfOut, std::vector<uint32_t *> &aFrames);

    bool SunRoad_Render2();
    bool EnvMap_Render2();
    bool SunRoad_Render();
    bool EnvMap_Render();
    void EnvMap_GetSideMatrix(D3DCUBEMAP_FACES Face, CMatrix &mView);

    void SFLB_CreateBuffers();
    void CreateVertexDeclaration();

  public:
    SEA();
    ~SEA() override;

    bool Init() override;
    void BuildVolumeTexture();
    void Realize(uint32_t dwDeltaTime);

    float Trace(const CVECTOR &vSrc, const CVECTOR &vDst) override;
    float Cannon_Trace(long iBallOwner, const CVECTOR &src, const CVECTOR &dst) override;

    bool Clip(const PLANE *planes, long nplanes, const CVECTOR &center, float radius, ADD_POLYGON_FUNC addpoly) override
    {
        return false;
    }

    const char *GetCollideMaterialName() override
    {
        return nullptr;
    };

    bool GetCollideTriangle(TRIANGLE &triangle) override
    {
        return false;
    }

    bool EditMode_Update();

    uint32_t AttributeChanged(ATTRIBUTES *pAttribute) override;

    void ProcessStage(Stage stage, uint32_t delta) override
    {
        switch (stage)
        {
            // case Stage::execute:
            //    Execute(delta); break;
        case Stage::realize:
            Realize(delta);
            break;
        case Stage::lost_render:
            LostRender();
            break;
        case Stage::restore_render:
            RestoreRender();
            break;
        }
    }

    void LostRender();
    void RestoreRender();
};
