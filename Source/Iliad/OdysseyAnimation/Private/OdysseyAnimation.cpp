// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimation.h"

#include "CanvasTypes.h"
#include "Engine/AssetUserData.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Misc/FeedbackContext.h"
#include "Misc/PackageName.h"
#include "Misc/TransactionObjectEvent.h"
#include "RenderGraphBuilder.h"
#include "ScreenPass.h"
#include "TextureResource.h"
#include "UObject/DevObjectVersion.h"
#include "UObject/ObjectSaveContext.h"
#include "UObject/SavePackage.h"

#if WITH_EDITOR
#include "AssetToolsModule.h"
#include "IAssetTools.h"
#include "TextureCompiler.h"
#endif

#include "OdysseyAnimationRenderingOverride.h"
#include "OdysseyAnimationRenderContext.h"
#include "OdysseyRenderContext.h"

#define LOCTEXT_NAMESPACE "Animation"

int
UOdysseyAnimation::GetWidth() const
{
    return mWidth;
}

int
UOdysseyAnimation::GetHeight() const
{
    return mHeight;
}

int
UOdysseyAnimation::GetWidthFromHeightKeepingRatio( int iHeight )
{
    float ratio = mWidth / (float)mHeight;

    return int( iHeight * ratio );
}
int
UOdysseyAnimation::GetHeightFromWidthKeepingRatio( int iWidth )
{
    float ratio = mWidth / (float)mHeight;

    return int( iWidth / ratio );
}

EOdysseyAnimationFormat
UOdysseyAnimation::GetFormat() const
{
    return Format;
}

FTimespan
UOdysseyAnimation::GetDuration() const
{
    if (GetFrameCount() == 0)
    {
        return FTimespan::FromSeconds(0);
    }
    return FTimespan::FromSeconds(GetFrameCount() / GetFramesPerSecond()) - FTimespan(1);
}

FInt32Range
UOdysseyAnimation::GetFrameRange() const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimation::GetFrameRange);
    #if WITH_EDITOR
    return FInt32Range::Inclusive(GetLeftBoundValue(), GetRightBoundValue());
    #else
    int frameCount = 0;
    for (const FOdysseyAnimationFrame& frame : Frames)
    {
        frameCount += frame.Exposure;
    }
    return FInt32Range::Inclusive(0, frameCount - 1);
    #endif
}

int
UOdysseyAnimation::GetFrameCount() const
{
    FInt32Range frameRange = GetFrameRange();
    return FMath::Max(0, frameRange.GetUpperBoundValue() - frameRange.GetLowerBoundValue() + 1);
}

float
UOdysseyAnimation::GetFramesPerSecond() const
{
    return FramesPerSecond;
}

TRange<FTimespan>
UOdysseyAnimation::GetFrameTimeRange(int iFrameIndex) const
{
    FTimespan start = FTimespan::FromSeconds(iFrameIndex / GetFramesPerSecond());

    //Remove one tick because end timespan is included in the range
    //That way we never have two frame with overlapping timeranges
    FTimespan end = FTimespan::FromSeconds((iFrameIndex + 1) / GetFramesPerSecond()) - FTimespan(1);
    return TRange<FTimespan>(start, end);
}

/* UMediaSource overrides
 *****************************************************************************/

FString UOdysseyAnimation::GetUrl() const
{
    return FString(TEXT("odysseyanimation://")) + GetPathName();
}

bool UOdysseyAnimation::Validate() const
{
    return true;
}

int
UOdysseyAnimation::GetFrameIndexAtFrame(int iFrame) const
{
    int frameIndex = 0;
    int framePos = 0;
    for (const FOdysseyAnimationFrame& frame : Frames)
    {
        if ( iFrame >= framePos && iFrame < framePos + frame.Exposure)
            return frameIndex;

        framePos += frame.Exposure;
        frameIndex++;
    }
    return INDEX_NONE;
}

TArray<FGuid>
UOdysseyAnimation::GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UOdysseyAnimation::GetRenderingComposition);
    TArray<FGuid> idComposition = { GetRenderingId() };

#if WITH_EDITOR
    const IOdysseyTextureRenderingAbility* renderingOverride = GetRenderingOverride();
    if ( !renderingOverride )
        return idComposition;

    idComposition.Append(renderingOverride->GetRenderingComposition(iRenderType, iFrameIndex));
#else
    int frameIndex = GetFrameIndexAtFrame(iFrameIndex);
    if ( frameIndex == INDEX_NONE )
        return idComposition;

    idComposition.Append(Frames[frameIndex].RenderingComposition);
#endif

    return idComposition;
}

FIntRect
UOdysseyAnimation::GetDefaultRenderRect() const
{
    return FIntRect(0, 0, GetWidth(), GetHeight());
}

bool
UOdysseyAnimation::BuildRenderPipelineInternal(
    IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
    const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
) const
{
    const FOdysseyAnimationRenderContext* AnimationContext = RenderContext ? RenderContext->GetExtension<FOdysseyAnimationRenderContext>() : nullptr;
    const FFrameNumber Frame = AnimationContext ? AnimationContext->Frame.Get(FFrameNumber(0)) : FFrameNumber(0);
#if WITH_EDITOR
    const IOdysseyTextureRenderingAbility* renderingOverride = GetRenderingOverride();
    if ( !renderingOverride )
        return false;

    return renderingOverride->BuildRenderPipeline(oRenderFunction, iParents, RenderContext);
#else
    UTexture2D* srcTexture2D = nullptr;
    int frameIndex = GetFrameIndexAtFrame(Frame.Value);
    if (frameIndex != INDEX_NONE)
    {
        if (Frames[frameIndex].Texture)
            srcTexture2D = Frames[frameIndex].Texture;
    }

    if (!srcTexture2D)
        return false;

    oRenderFunction = [srcTexture2D](
        FRDGBuilder& iGraphBuilder,
        ERHIFeatureLevel::Type iFeatureLevel,
        FRDGTextureRef iDestinationTexture,
        const FIntRect& iSrcRect,
        const FIntRect& iDstRect,
        const FMatrix& iSrcTransform
        )
    {
        AddClearRenderTargetPass(iGraphBuilder, iDestinationTexture, FLinearColor::Transparent, iDstRect);

        FRDGTextureRef sourceTexture = iGraphBuilder.RegisterExternalTexture(CreateRenderTarget(srcTexture2D->GetResource()->TextureRHI, TEXT("UOdysseyAnimation::sourceTexture")));

        AddDrawTexturePass(
            iGraphBuilder,
            FScreenPassViewInfo(),
            sourceTexture,
            iDestinationTexture,
            iSrcRect.Min,
            iSrcRect.Size(),
            iDstRect.Min,
            iDstRect.Size()
        );
    };

    return true;
#endif
}

UTextureRenderTarget2D*
UOdysseyAnimation::CreateRenderingRenderTarget() const
{
    UTextureRenderTarget2D* renderTarget = NewObject<UTextureRenderTarget2D>();
    ETextureRenderTargetFormat renderTargetFormat = RTF_RGBA8;
    switch(Format)
    {
        case EOdysseyAnimationFormat::BGRA8:
        {
            renderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
            renderTarget->bForceLinearGamma = false;
        }
        break;

        case EOdysseyAnimationFormat::RGBAF:
        {
            renderTarget->RenderTargetFormat = RTF_RGBA32f;
            renderTarget->bForceLinearGamma = true;
        }
        break;
        default: checkf(false, TEXT("Non implemented format"))
    }
    renderTarget->SRGB = renderTarget->IsSRGB();
    renderTarget->InitAutoFormat(mWidth, mHeight);

    return renderTarget;
}

#if WITH_EDITOR
EOdysseyAnimationBoundMode
UOdysseyAnimation::GetLeftBoundMode() const
{
    return LeftBoundMode;
}

EOdysseyAnimationBoundMode
UOdysseyAnimation::GetRightBoundMode() const
{
    return RightBoundMode;
}

int
UOdysseyAnimation::GetLeftBoundValue() const
{
    switch ( LeftBoundMode )
    {
        case EOdysseyAnimationBoundMode::Automatic:
        {
            const IOdysseyTextureRenderingAbility* source = GetRenderingOverride();
            if ( !source )
                return 0;

            FInt32Range frameRange = source->GetFrameRange();
            return frameRange.GetLowerBoundValue();
        }

        case EOdysseyAnimationBoundMode::Manual:
        {
            return FMath::Max(0, LeftBound);
        }
    }
    return LeftBound;
}

int
UOdysseyAnimation::GetRightBoundValue() const
{
    switch ( RightBoundMode )
    {
        case EOdysseyAnimationBoundMode::Automatic:
        {
            const IOdysseyTextureRenderingAbility* source = GetRenderingOverride();
            if ( !source )
                return 0;

            FInt32Range frameRange = source->GetFrameRange();
            return frameRange.GetUpperBoundValue();
        }

        case EOdysseyAnimationBoundMode::Manual:
        {
            return FMath::Max(0, RightBound);
        }
    }
    return RightBound;
}

void
UOdysseyAnimation::SetLeftBoundMode(EOdysseyAnimationBoundMode iMode)
{
    Modify();
    LeftBoundMode = iMode;

    OnLeftBoundModeChanged();
}

void
UOdysseyAnimation::SetRightBoundMode(EOdysseyAnimationBoundMode iMode)
{
    Modify();
    RightBoundMode = iMode;

    OnRightBoundModeChanged();
}

void
UOdysseyAnimation::SetLeftBoundValue(int iValue)
{
    Modify();
    LeftBound = iValue;

    OnLeftBoundChanged();
}

void
UOdysseyAnimation::SetRightBoundValue(int iValue)
{
    Modify();
    RightBound = iValue;

    OnRightBoundChanged();
}
#endif

#if WITH_EDITOR
void
UOdysseyAnimation::Init(int iWidth, int iHeight, EOdysseyAnimationFormat iFormat, float iFramesPerSecond)
{
    mWidth = iWidth;
    mHeight = iHeight;
    Format = iFormat;
    FramesPerSecond = iFramesPerSecond;
}

void
UOdysseyAnimation::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}

void
UOdysseyAnimation::PostTransacted(const FTransactionObjectEvent& iTransactionEvent)
{
    Super::PostTransacted(iTransactionEvent);

    if ( iTransactionEvent.GetEventType() != ETransactionObjectEventType::UndoRedo )
        return;

    const TArray<FName>& changedPropertyNames = iTransactionEvent.GetChangedProperties();
    for ( const FName& propertyName : changedPropertyNames )
    {
        PropertyChanged(propertyName);
    }
}

void
UOdysseyAnimation::OnLeftBoundModeChanged()
{
    const IOdysseyTextureRenderingAbility* source = GetRenderingOverride();
    if ( !source )
        return;

    FInt32Range frameRange = source->GetFrameRange();
    LeftBound = frameRange.GetLowerBoundValue();
    RightBound = FMath::Max(GetLeftBoundValue(), RightBound);
}

void
UOdysseyAnimation::OnRightBoundModeChanged()
{
    const IOdysseyTextureRenderingAbility* source = GetRenderingOverride();
    if ( !source )
        return;

    FInt32Range frameRange = source->GetFrameRange();
    RightBound = frameRange.GetUpperBoundValue();
    LeftBound = FMath::Min(LeftBound, GetRightBoundValue());
}

void
UOdysseyAnimation::OnLeftBoundChanged()
{
    if (LeftBoundMode == EOdysseyAnimationBoundMode::Automatic)
    {
        const IOdysseyTextureRenderingAbility* source = GetRenderingOverride();
        if ( !source )
            return;

        FInt32Range frameRange = source->GetFrameRange();
        LeftBound = frameRange.GetLowerBoundValue();
    }
    else
    {
        LeftBound = FMath::Clamp(LeftBound, 0, GetRightBoundValue());
    }
}

void
UOdysseyAnimation::OnRightBoundChanged()
{
    if (RightBoundMode == EOdysseyAnimationBoundMode::Automatic)
    {
        const IOdysseyTextureRenderingAbility* source = GetRenderingOverride();
        if ( !source )
            return;

        FInt32Range frameRange = source->GetFrameRange();
        RightBound = frameRange.GetUpperBoundValue();
    }
    else
    {
        RightBound = FMath::Max( RightBound, GetLeftBoundValue() );
    }
}

void
UOdysseyAnimation::PropertyChanged(const FName& iPropertyName)
{
    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, LeftBoundMode) )
    {
        OnLeftBoundModeChanged();
    }

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, RightBoundMode) )
    {
        OnRightBoundModeChanged();
    }

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, LeftBound) )
    {
        OnLeftBoundChanged();
    }

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, RightBound) )
    {
        OnRightBoundChanged();
    }

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, bLinearCompositing) )
    {
        RenderingCompositionChanged(false);
    }
}

void
UOdysseyAnimation::GetAssetRegistryTags( FAssetRegistryTagsContext ioContext ) const
{
    Super::GetAssetRegistryTags( ioContext );

    ioContext.AddTag( { "Image Size", FString::Printf(TEXT("%dx%d"), mWidth, mHeight), FAssetRegistryTag::TT_Alphabetical } );

    // don't display the decimal part if none
    if( ((int)FramesPerSecond) == FramesPerSecond )
        ioContext.AddTag( { "Frame Rate", FString::Printf(TEXT("%d fps"), (int)FramesPerSecond), FAssetRegistryTag::TT_Alphabetical } );
    else
        ioContext.AddTag( { "Frame Rate", FString::Printf(TEXT("%.2f fps"), FramesPerSecond), FAssetRegistryTag::TT_Alphabetical } );
}

void
UOdysseyAnimation::GetAssetRegistryTagMetadata( TMap<FName, FAssetRegistryTagMetadata>& OutMetadata ) const
{
    Super::GetAssetRegistryTagMetadata( OutMetadata );

    OutMetadata.Add(
        "Image Size",
        FAssetRegistryTagMetadata()
        .SetDisplayName( NSLOCTEXT( "OdysseyAnimation", "ImageSize_Label", "Image Size" ) )
        .SetTooltip( NSLOCTEXT( "OdysseyAnimation", "ImageSize_Tooltip", "Image size" ) )
    );

    OutMetadata.Add(
        "Frame Rate",
        FAssetRegistryTagMetadata()
        .SetDisplayName( NSLOCTEXT( "OdysseyAnimation", "FrameRate_Label", "Frame Rate" ) )
        .SetTooltip( NSLOCTEXT( "OdysseyAnimation", "FrameRate_Tooltip", "Frame Rate" ) )
    );
}

void
UOdysseyAnimation::PreSave(FObjectPreSaveContext SaveContext)
{
    Super::PreSave(SaveContext);

    if ( SaveContext.IsCooking() )
        return;

    if( !FApp::CanEverRender() ) //Returns false while packaging a Game, in which case any rendering is forbidden and Render_GameThread would fail
        return;

    Frames.Empty();

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(NewObject<UTextureRenderTarget2D>());
    renderTarget->RenderTargetFormat = RTF_RGBA8_SRGB;
    renderTarget->bForceLinearGamma = false;
    renderTarget->InitAutoFormat(GetWidth(), GetHeight());
    renderTarget->UpdateResourceImmediate();

    FInt32Range range = GetFrameRange();
    TArray<FGuid> lastRenderingComposition;
    for ( int i = range.GetLowerBoundValue(); i <= range.GetUpperBoundValue(); i++ )
    {
        TArray<FGuid> renderingComposition = GetRenderingComposition(EOdysseyRenderingType::Render, i);
        if ( renderingComposition == lastRenderingComposition )
        {
            Frames.Last().Exposure++;
            continue;
        }

        lastRenderingComposition = renderingComposition;

        FOdysseyRenderContext RenderContext = FOdysseyAnimationRenderContext::MakeForFrame(FFrameNumber(i));
        Render_GameThread(renderTarget.Get(), EOdysseyRenderingType::Render, &RenderContext);


        FString Name = FString::Printf(TEXT("OdysseyAnimationFrameTexture%d"), i);

        UTexture2D* texture = Cast<UTexture2D>(renderTarget->ConstructTexture(this, Name, RF_Public, CTF_Default));
        texture->PreSave(SaveContext);
        // Through the compiling manager, which also replaces the stand-in resource of the build: BlockOnAnyAsyncBuild()
        // only finishes the platform data, and leaves the texture reporting a compilation nothing can finish.
        UTexture* const BuiltTexture = texture;
        FTextureCompilingManager::Get().FinishCompilation({ BuiltTexture });

        FOdysseyAnimationFrame frame;
        frame.Texture = texture;
        frame.Exposure = 1;
        frame.RenderingComposition = renderingComposition;
        Frames.Add(frame);
    }
}

UTexture2D*
UOdysseyAnimation::CreateExportTexture( const FString& iAssetName, const FString& iPackagePath, UClass* iAssetClass, UFactory* iFactory ) //override
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>( "AssetTools" ).Get();
    UObject* object = AssetTools.CreateAsset(
        iAssetName,
        iPackagePath,
        iAssetClass,
        iFactory
    );

    UTexture2D* texture = Cast<UTexture2D>( object );
    if( !ensure( texture ) )
        return nullptr;

    switch( Format )
    {
        case EOdysseyAnimationFormat::BGRA8:
        {
            texture->PreEditChange( nullptr );
            texture->Source.Init( mWidth, mHeight, 1, 1, TSF_BGRA8 );
            texture->SRGB = true;
            texture->PostEditChange();
        }
        break;

        case EOdysseyAnimationFormat::RGBAF:
        {
            texture->PreEditChange( nullptr );
            texture->Source.Init( mWidth, mHeight, 1, 1, TSF_RGBA32F );
            texture->SRGB = false;
            texture->PostEditChange();
        }
        break;
        default: checkf( false, TEXT( "Non implemented format" ) )
    }

    return texture;
}

void
UOdysseyAnimation::OnRefreshReferencedPalette(UOdysseyPalette* iPalette)
{
    //CanEverRender() returns false while packaging a Game, in which case any rendering is forbidden
    //Refreshing cells palette, leads to rendering, so we don't want that
    //Also if this function is called during Palette's PreSave
    //And we ABSOLUTELY don't want SavePackage() to be called at this moment
    //because it leads to assets file handles to be locked on windows, preventing any Game Packaging to succeed
    if( !FApp::CanEverRender() )
        return;

    for (UAssetUserData* userData : AssetUserData)
    {
        IOdysseyPaletteReferencer* referencer = Cast<IOdysseyPaletteReferencer>(userData);
        if (referencer)
            referencer->OnRefreshReferencedPalette(iPalette);
    }
}

UOdysseyAnimation::FOnLegacyEditorDataLoaded&
UOdysseyAnimation::OnLegacyEditorDataLoaded()
{
    static FOnLegacyEditorDataLoaded delegate;
    return delegate;
}

#endif

#if WITH_EDITOR
const IOdysseyTextureRenderingAbility*
UOdysseyAnimation::GetRenderingOverride() const
{
    for (UAssetUserData* userData : AssetUserData)
    {
        const IOdysseyAnimationRenderingOverride* renderingOverride = Cast<IOdysseyAnimationRenderingOverride>(userData);
        const IOdysseyTextureRenderingAbility* overridingAbility = renderingOverride ? renderingOverride->GetAnimationRenderingOverride() : nullptr;
        if (overridingAbility)
            return overridingAbility;
    }

    return nullptr;
}
#endif

#if WITH_EDITORONLY_DATA
bool
UOdysseyAnimation::HasLegacyEditorData() const
{
    return HasLegacyLayerStack() || HasLegacyPalettes() || HasLegacyToolCollections();
}

bool
UOdysseyAnimation::HasLegacyLayerStack() const
{
    return mLayerStack_DEPRECATED != nullptr;
}

bool
UOdysseyAnimation::HasLegacyPalettes() const
{
    return !Palettes_DEPRECATED.IsEmpty();
}

bool
UOdysseyAnimation::HasLegacyToolCollections() const
{
    return !ToolCollections_DEPRECATED.IsEmpty();
}

UObject*
UOdysseyAnimation::TakeLegacyLayerStack()
{
    UObject* layerStack = mLayerStack_DEPRECATED;
    mLayerStack_DEPRECATED = nullptr;
    return layerStack;
}

TArray<UOdysseyPaletteSet*>
UOdysseyAnimation::TakeLegacyPalettes()
{
    TArray<UOdysseyPaletteSet*> palettes = MoveTemp(Palettes_DEPRECATED);
    Palettes_DEPRECATED.Reset();
    return palettes;
}

TArray<FSoftObjectPath>
UOdysseyAnimation::TakeLegacyToolCollections()
{
    TArray<FSoftObjectPath> toolCollections = MoveTemp(ToolCollections_DEPRECATED);
    ToolCollections_DEPRECATED.Reset();
    return toolCollections;
}
#endif

namespace
{
/** Custom version of UOdysseyAnimation. */
struct FOdysseyAnimationObjectVersion
{
    enum Type
    {
        // Before any version changes were made: layers were blended with linear colors
        BeforeCustomVersionWasAdded,

        // Layers are blended with sRGB encoded colors unless bLinearCompositing is set
        EncodedCompositingByDefault,

        // -----<new versions can be added above this line>-------------------------------------------------
        VersionPlusOne,
        LatestVersion = VersionPlusOne - 1
    };

    // The GUID for this custom version number
    const static FGuid GUID;
};

const FGuid FOdysseyAnimationObjectVersion::GUID(0x6A2F43C1, 0x8B0E4D27, 0x9C5A1E73, 0x40D8B612);
FDevVersionRegistration GRegisterOdysseyAnimationObjectVersion(FOdysseyAnimationObjectVersion::GUID, FOdysseyAnimationObjectVersion::LatestVersion, TEXT("OdysseyAnimation"));
}

#if WITH_EDITOR
bool
UOdysseyAnimation::UsesLinearCompositing() const
{
    return bLinearCompositing;
}

void
UOdysseyAnimation::SetLinearCompositing(bool bValue)
{
    Modify();
    bLinearCompositing = bValue;
    PropertyChanged(GET_MEMBER_NAME_CHECKED(UOdysseyAnimation, bLinearCompositing));
}
#endif

void
UOdysseyAnimation::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);
    Ar.UsingCustomVersion(FOdysseyAnimationObjectVersion::GUID);
}

void
UOdysseyAnimation::PostLoad()
{
    Super::PostLoad();

#if WITH_EDITORONLY_DATA
    // Keep the colors of animations created when layers were blended with linear colors.
    if (GetLinkerCustomVersion(FOdysseyAnimationObjectVersion::GUID) < FOdysseyAnimationObjectVersion::EncodedCompositingByDefault)
    {
        bLinearCompositing = true;
    }
#endif

#if WITH_EDITOR
    if (HasLegacyEditorData())
        OnLegacyEditorDataLoaded().Broadcast(this);
#endif
}

void
UOdysseyAnimation::AddAssetUserData(UAssetUserData* InUserData)
{
    if (!InUserData)
        return;

    UAssetUserData* existingData = GetAssetUserDataOfClass(InUserData->GetClass());
    if (existingData)
        AssetUserData.Remove(existingData);

    AssetUserData.Add(InUserData);
}

void
UOdysseyAnimation::RemoveUserDataOfClass(TSubclassOf<UAssetUserData> InUserDataClass)
{
    for (int32 dataIndex = AssetUserData.Num() - 1; dataIndex >= 0; dataIndex--)
    {
        UAssetUserData* userData = AssetUserData[dataIndex];
        if (userData && userData->IsA(InUserDataClass))
            AssetUserData.RemoveAt(dataIndex);
    }
}

UAssetUserData*
UOdysseyAnimation::GetAssetUserDataOfClass(TSubclassOf<UAssetUserData> InUserDataClass)
{
    for (UAssetUserData* userData : AssetUserData)
    {
        if (userData && userData->IsA(InUserDataClass))
            return userData;
    }

    return nullptr;
}

const TArray<UAssetUserData*>*
UOdysseyAnimation::GetAssetUserDataArray() const
{
    return &ToRawPtrTArrayUnsafe(AssetUserData);
}


#undef LOCTEXT_NAMESPACE
