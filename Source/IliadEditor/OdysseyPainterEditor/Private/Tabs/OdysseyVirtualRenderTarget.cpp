// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVirtualRenderTarget.h"

#include "EngineModule.h"
#include "HAL/LowLevelMemStats.h"
#include "ProfilingDebugging/AssetMetadataTrace.h"
#include "RendererInterface.h"
#include "RHI.h"
#include "RHIResources.h"
#include "VirtualTextureEnum.h"
#include "VT/VirtualTextureScalability.h"

#include "OdysseyVirtualRenderTargetData.h"
#include "OdysseyVirtualRenderTargetProducer.h"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorTiledViewportClient

FOdysseyVirtualRenderTargetResource::FOdysseyVirtualRenderTargetResource(TSharedPtr<FOdysseyVirtualRenderTargetData> InVTData)
    : WeakVTData(InVTData)
{
    bSRGB = true;
    bGreyScaleFormat = false;
    //TextureReferenceRHI = nullptr;

    //TODO: Retrieve infos from owner (see FVirtualTexture2DResource)
    TextureName = TEXT("FOdysseyVirtualRenderTargetResource");
    PackageName = TEXT("FOdysseyVirtualRenderTargetResource");
    FullNameHash = 1234;

    TexCreateFlags = bSRGB ? ETextureCreateFlags::SRGB : ETextureCreateFlags::None;
    TexCreateFlags |= ETextureCreateFlags::RenderTargetable;
    TexCreateFlags |= ETextureCreateFlags::ShaderResource;
    TexCreateFlags |= ETextureCreateFlags::NoTiling;

    bSinglePhysicalSpace = false;
    bRequiresSinglePhysicalPool = false;

    VirtualTextureStreamingPriority = EVTProducerPriority::Normal;

    // Initialize this resource FeatureLevel, so it gets re-created on FeatureLevel changes
    SetFeatureLevel(GMaxRHIFeatureLevel);
}

void
FOdysseyVirtualRenderTargetResource::InitRHI(FRHICommandListBase& RHICmdList)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyVirtualRenderTargetResource::InitRHI);
    LLM_SCOPE_DYNAMIC_STAT_OBJECTPATH_FNAME(PackageName, ELLMTagSet::Assets);
    LLM_SCOPE_DYNAMIC_STAT_OBJECTPATH_FNAME(FRHITextureCreateDesc().GetTraceClassName(), ELLMTagSet::AssetClasses);
    UE_TRACE_METADATA_SCOPE_ASSET_FNAME(NAME_None, FRHITextureCreateDesc().GetTraceClassName(), PackageName);

    TSharedPtr<FOdysseyVirtualRenderTargetData> PinnedVTData = WeakVTData.Pin();
    if (!PinnedVTData)
        return;

    uint32 MaxAnisotropy = 0;
    if (VirtualTextureScalability::IsAnisotropicFilteringEnabled())
    {
        // Limit HW MaxAnisotropy to avoid sampling outside VT borders
        MaxAnisotropy = FMath::Min<int32>(VirtualTextureScalability::GetMaxAnisotropy(), PinnedVTData->TileBorderSize);
    }

    // We always create a sampler state if we're attached to a texture. This is used to sample the cache texture during actual rendering and the miptails editor resource.
    // If we're not attached to a texture it likely means we're light maps which have sampling handled differently.
    FSamplerStateInitializerRHI SamplerStateInitializer
    (
        // This will ensure nearest/linear/trilinear which does matter when sampling both the cache and the miptail
        Filter,

        // This doesn't really matter when sampling the cache texture but it does when sampling the miptail texture
        AddressU,
        AddressV,
        AM_Wrap,

        // This doesn't really matter when sampling the cache texture (as it only has a level 0, so whatever the bias that is sampled) but it does when we sample miptail texture
        0, // VT currently ignores global mip bias ensure the miptail works the same -> UTexture2D::GetGlobalMipMapLODBias()
        MaxAnisotropy
    );

    if (MaxAnisotropy == 0u)
    {
        if (SamplerStateInitializer.Filter == SF_AnisotropicLinear || SamplerStateInitializer.Filter == SF_AnisotropicPoint)
        {
            SamplerStateInitializer.Filter = SF_Bilinear;
        }
    }

    SamplerStateRHI = GetOrCreateSamplerState(SamplerStateInitializer);



    const int32 MaxLevel = PinnedVTData->GetNumMips() - FirstMipToUse - 1;
    check(MaxLevel >= 0);

    FVTProducerDescription ProducerDesc;
    ProducerDesc.Name = TextureName;
    ProducerDesc.FullNameHash = FullNameHash;
    ProducerDesc.Dimensions = 2;
    ProducerDesc.TileSize = PinnedVTData->TileSize;
    ProducerDesc.TileBorderSize = PinnedVTData->TileBorderSize;
    ProducerDesc.BlockWidthInTiles = FMath::DivideAndRoundUp<uint32>(GetNumTilesX(), PinnedVTData->WidthInBlocks);
    ProducerDesc.BlockHeightInTiles = FMath::DivideAndRoundUp<uint32>(GetNumTilesY(), PinnedVTData->HeightInBlocks);
    ProducerDesc.WidthInBlocks = PinnedVTData->WidthInBlocks;
    ProducerDesc.HeightInBlocks = PinnedVTData->HeightInBlocks;
    ProducerDesc.DepthInTiles = 1u;
    ProducerDesc.MaxLevel = MaxLevel;
    ProducerDesc.NumTextureLayers = PinnedVTData->GetNumLayers();
    ProducerDesc.NumPhysicalGroups = bSinglePhysicalSpace ? 1 : PinnedVTData->GetNumLayers();
    for (uint32 LayerIndex = 0u; LayerIndex < PinnedVTData->GetNumLayers(); ++LayerIndex)
    {
        ProducerDesc.LayerFormat[LayerIndex] = PinnedVTData->LayerTypes[LayerIndex];
        ProducerDesc.LayerFallbackColor[LayerIndex] = PinnedVTData->LayerFallbackColors[LayerIndex];
        ProducerDesc.PhysicalGroupIndex[LayerIndex] = bSinglePhysicalSpace ? 0 : LayerIndex;
        ProducerDesc.bIsLayerSRGB[LayerIndex] = bSRGB;
    }
    ProducerDesc.bRequiresSinglePhysicalPool = bRequiresSinglePhysicalPool;
    ProducerDesc.Priority = VirtualTextureStreamingPriority;

    //FUploadingVirtualTexture* VirtualTexture = new FUploadingVirtualTexture(ProducerDesc.Name, PinnedVTData, FirstMipToUse);
    FOdysseyVirtualRenderTargetProducer* Producer = new FOdysseyVirtualRenderTargetProducer(ProducerDesc.Name, PinnedVTData, FirstMipToUse, ProducerDesc);
    ProducerHandle = GetRendererModule().RegisterVirtualTextureProducer(RHICmdList, ProducerDesc, Producer);

    // Only create the miptails mini-texture in-editor.
#if WITH_EDITOR
    //InitializeEditorResources(RHICmdList, VirtualTexture);
#endif

    /*if (TextureRHI.IsValid())
    {
        TextureRHI->SetOwnerName(GetOwnerName());
    }*/
}

void
FOdysseyVirtualRenderTargetResource::ReleaseRHI()
{
    ReleaseAllocatedVT();

    GetRendererModule().ReleaseVirtualTextureProducer(ProducerHandle);
    ProducerHandle = FVirtualTextureProducerHandle();
}

uint32
FOdysseyVirtualRenderTargetResource::GetSizeX() const
{
    TSharedPtr<FOdysseyVirtualRenderTargetData> PinnedVTData = WeakVTData.Pin();
    if (!PinnedVTData)
        return 0;

    return FMath::Max(PinnedVTData->Width >> FirstMipToUse, 1u);
}

uint32
FOdysseyVirtualRenderTargetResource::GetSizeY() const
{
    TSharedPtr<FOdysseyVirtualRenderTargetData> PinnedVTData = WeakVTData.Pin();
    if (!PinnedVTData)
        return 0;

    return FMath::Max(PinnedVTData->Height >> FirstMipToUse, 1u);
}

EPixelFormat
FOdysseyVirtualRenderTargetResource::GetFormat(uint32 LayerIndex) const
{
    TSharedPtr<FOdysseyVirtualRenderTargetData> PinnedVTData = WeakVTData.Pin();
    if (!PinnedVTData)
        return PF_Unknown;

    return PinnedVTData->LayerTypes[LayerIndex];
}

int
FOdysseyVirtualRenderTargetResource::GetNumBlocks() const
{
    FIntPoint sizeInBlocks = GetSizeInBlocks();
    return sizeInBlocks.X * sizeInBlocks.Y;
}

FIntPoint
FOdysseyVirtualRenderTargetResource::GetSizeInBlocks() const
{
    TSharedPtr<FOdysseyVirtualRenderTargetData> PinnedVTData = WeakVTData.Pin();
    if (!PinnedVTData)
        return FIntPoint(0, 0);

    return FIntPoint(PinnedVTData->WidthInBlocks, PinnedVTData->HeightInBlocks);
}

uint32
FOdysseyVirtualRenderTargetResource::GetNumTilesX() const
{
    TSharedPtr<FOdysseyVirtualRenderTargetData> PinnedVTData = WeakVTData.Pin();
    if (!PinnedVTData)
        return 0;

    return FMath::Max(PinnedVTData->GetWidthInTiles() >> FirstMipToUse, 1u);
}

uint32
FOdysseyVirtualRenderTargetResource::GetNumTilesY() const
{
    TSharedPtr<FOdysseyVirtualRenderTargetData> PinnedVTData = WeakVTData.Pin();
    if (!PinnedVTData)
        return 0;

    return FMath::Max(PinnedVTData->GetHeightInTiles() >> FirstMipToUse, 1u);
}

uint32
FOdysseyVirtualRenderTargetResource::GetNumMips() const
{
    TSharedPtr<FOdysseyVirtualRenderTargetData> PinnedVTData = WeakVTData.Pin();
    if (!PinnedVTData)
        return 0;

    ensure((int32)PinnedVTData->GetNumMips() > FirstMipToUse);
    return PinnedVTData->GetNumMips() - FirstMipToUse;
}

uint32
FOdysseyVirtualRenderTargetResource::GetNumLayers() const
{
    TSharedPtr<FOdysseyVirtualRenderTargetData> PinnedVTData = WeakVTData.Pin();
    if (!PinnedVTData)
        return 0;

    return PinnedVTData->GetNumLayers();
}

uint32
FOdysseyVirtualRenderTargetResource::GetTileSize() const
{
    TSharedPtr<FOdysseyVirtualRenderTargetData> PinnedVTData = WeakVTData.Pin();
    if (!PinnedVTData)
        return 0;

    return PinnedVTData->TileSize;
}

uint32
FOdysseyVirtualRenderTargetResource::GetBorderSize() const
{
    TSharedPtr<FOdysseyVirtualRenderTargetData> PinnedVTData = WeakVTData.Pin();
    if (!PinnedVTData)
        return 0;

    return PinnedVTData->TileBorderSize;
}
