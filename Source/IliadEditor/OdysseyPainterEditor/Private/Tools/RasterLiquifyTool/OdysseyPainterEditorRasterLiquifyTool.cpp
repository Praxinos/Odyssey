// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyTool.h"
#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyToolHUD.h"
#include "OdysseyMediaRaster.h"
#include "Toolkits/BaseToolkit.h"

#include "ISinglePropertyView.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyPainterEditorCommands.h"
#include "ScopedTransaction.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"

#include "OdysseyHUDElement.h"
#include "SOdysseySinglePropertyView.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

#define M_PI 3.14159265358979323846L

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterLiquifyTool::~UOdysseyPainterEditorRasterLiquifyTool()
{
}

UOdysseyPainterEditorRasterLiquifyTool::UOdysseyPainterEditorRasterLiquifyTool()
    : mLiquifyHUD( MakeShared<FOdysseyPainterEditorRasterLiquifyToolHUD>( this ) )
    , mPaintEngine()
    , Radius ( 50 )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.DrawingTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorRasterLiquifyTool::Activate()
{
    Super::Activate();
}

void
UOdysseyPainterEditorRasterLiquifyTool::Load()
{
    UOdysseyPainterEditorTool::Load();

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();

    rasterSelection->OnChanged().AddUObject(this, &UOdysseyPainterEditorRasterLiquifyTool::OnRasterSelectionChanged);

//    if (!rasterSelection->IsEmpty())
//        mPaintEngine.SetMaskBlock(rasterSelection->GetBlock());

    mHUD->AddElement(rasterSelection->GetHUD());

    mLiquifyHUD->Reset();

    mHUD->AddElement( mLiquifyHUD );

//    mHUD->AddElement(rasterSelection->GetHUD());
//    mHUD->AddElement(mShapeHUD);
}

void
UOdysseyPainterEditorRasterLiquifyTool::Unload()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();

    mPaintEngine.RasterBlock(nullptr);
    //mPaintEngine.SetMaskBlock(nullptr);

    rasterSelection->OnChanged().RemoveAll(this);

    mHUD->RemoveElement( mLiquifyHUD );
    mHUD->RemoveElement( rasterSelection->GetHUD() );

//    mHUD->RemoveElement(rasterSelection->GetHUD());
//    mHUD->RemoveElement(mShapeHUD);

    UOdysseyPainterEditorTool::Unload();
}

bool
UOdysseyPainterEditorRasterLiquifyTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyPainterEditorRasterLiquifyTool::GetRasterBlockFromEditor(bool iCreate) const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return nullptr;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters;
    if (iCreate)
        mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    else
        mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();

    if( mediaRasters.Num() <= 0 )
        return nullptr;

    return mediaRasters[0]->GetRasterBlock();
}



bool
UOdysseyPainterEditorRasterLiquifyTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    bIsMouseDown = true;

    mMouseAtDown = FVector2D( iPointInTexture.x, iPointInTexture.y );

    if (iKey != EKeys::LeftMouseButton)
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(true);
    if (!rasterBlock)
        return false;

    mPaintEngine.RasterBlock(rasterBlock);

    MakeDistortionMap();

    return true;
}

bool
UOdysseyPainterEditorRasterLiquifyTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    bIsMouseDown = false;

    if (iKey != EKeys::LeftMouseButton)
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock || rasterBlock != mPaintEngine.GetRasterBlock())
    {
        return false;
    }

    return true;
}

void
UOdysseyPainterEditorRasterLiquifyTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);

    mLiquifyHUD->SetCursorPositionInTexture( FVector2D( iPointInTexture.x, iPointInTexture.y ) );

    if (!rasterBlock)
        return;
}

/*
TSharedPtr<::ULIS::FBlock>
UOdysseyPainterEditorRasterLiquifyTool::GetCurrentLayerBlock() const
{
    UOdysseyLayerStack* layerStack = GetEditor()->LayerStack();
    if (!layerStack)
        return nullptr;

    UOdysseyLayer* layer = layerStack->GetCurrentLayer();
    if (!layer)
        return nullptr;

    FFrameNumber currentFrame(0);
    UOdysseyAnimationPlayer* player = GetEditor()->GetAnimationPlayer();
    if (player)
        currentFrame = player->GetCurrentFrame().FrameNumber;

    TStrongObjectPtr<UTextureRenderTarget2D> renderTarget(NewObject<UTextureRenderTarget2D>());
    FIntRect rect = layer->GetDefaultRenderRect();
    renderTarget->InitAutoFormat(rect.Width(), rect.Height());
    layer->Render_GameThread(renderTarget.Get(), currentFrame, EOdysseyRenderingType::Render);

    FImage OutImage;
    if (!FImageUtils::GetRenderTargetImage(renderTarget.Get(), OutImage))
        return nullptr;

    ::ULIS::eFormat format = ULISFormatForRawImageFormat(OutImage.Format);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    TSharedPtr<::ULIS::FBlock> block = MakeShareable(new ::ULIS::FBlock( rect.Width(), rect.Height(), format ));
    CopyImageToBlock(OutImage, block.Get());

    return block;
}
*/

void
UOdysseyPainterEditorRasterLiquifyTool::MakeDistortionMap()
{
    uint32 offset = 0;

    mDistortionMap.SetNum( Radius * Radius * 4 );

    for( int32 j = -Radius, y = 0; j < Radius; j++, y++ )
    {
        for( int32 i = -Radius, x = 0; i < Radius; i++, x++ )
        {
            double distance = sqrt( ( i * i ) + ( j * j ) );
            FDistortion& distortion = mDistortionMap[offset++];

            if( distance < Radius )
            {
                ::ULIS::FVec2I pointCoords = ::ULIS::FVec2I( i, j );

                distortion.mapped = true;
                distortion.coords = pointCoords;
                distortion.distance = distance;
                distortion.angle = atan2( j, i );
            }
            else
            {
                distortion.mapped = false;
            }
        }
    }
}

void
UOdysseyPainterEditorRasterLiquifyTool::Push( int32 iSrcCenterX
                                            , int32 iSrcCenterY
                                            , int32 iDstCenterX
                                            , int32 iDstCenterY )
{
    Liquify( iSrcCenterX
           , iSrcCenterY
           , iDstCenterX
           , iDstCenterY
           , [ this ]( FDistortion& distortion
                     , ::ULIS::FVec2D& oSrcCoordsRelative
                     , ::ULIS::FVec2D& oDstCoordsRelative )
    {
        double distance = distortion.distance;
        double factor = 1.0f - ( distance / Radius ); // can be precomputed
        ::ULIS::FVec2D srcCoordsRelative = ::ULIS::FVec2D( ( distance * cos( distortion.angle ) * factor )
                                                         , ( distance * sin( distortion.angle ) * factor ) );
        ::ULIS::FVec2D dstCoordsRelative = ::ULIS::FVec2D( distortion.coords.x
                                                         , distortion.coords.y );

        oSrcCoordsRelative = srcCoordsRelative;
        oDstCoordsRelative = dstCoordsRelative;
    } );
}

void
UOdysseyPainterEditorRasterLiquifyTool::Twirl( int32 iSrcCenterX
                                             , int32 iSrcCenterY
                                             , int32 iDstCenterX
                                             , int32 iDstCenterY
                                             , double iAngleInRadians )
{
    Liquify( iSrcCenterX
           , iSrcCenterY
           , iDstCenterX
           , iDstCenterY
           , [ iAngleInRadians ]( FDistortion& distortion
                                , ::ULIS::FVec2D& oSrcCoordsRelative
                                , ::ULIS::FVec2D& oDstCoordsRelative )
    {
        double distance = distortion.distance;
        double factor = 1.0f/* - ( distance / Radius )*/; // can be precomputed
        double newAngle = distortion.angle - ( iAngleInRadians * factor );
        ::ULIS::FVec2D srcCoordsRelative = ::ULIS::FVec2D( ( distance * cos( newAngle ) )
                                                         , ( distance * sin( newAngle ) ) );
        ::ULIS::FVec2D dstCoordsRelative = ::ULIS::FVec2D( distortion.coords.x
                                                         , distortion.coords.y );

        oSrcCoordsRelative = srcCoordsRelative;
        oDstCoordsRelative = dstCoordsRelative;
    } );
}

void
UOdysseyPainterEditorRasterLiquifyTool::Liquify( int32 iSrcCenterX
                                               , int32 iSrcCenterY
                                               , int32 iDstCenterX
                                               , int32 iDstCenterY
                                               , FLiquifyFunction iLiquifyfunc )
{
    TSharedPtr<FOdysseyRasterBlock> srcRasterBlock = GetRasterBlockFromEditor(false);
    TSharedPtr<::ULIS::FBlock> srcBlock = srcRasterBlock->GetBlock();
    const ULIS::uint8 *srcBlockPixels = srcBlock->PixelBits(0,0);
    TSharedPtr<::ULIS::FBlock> dstBlock = mPaintEngine.PaintBlock();
    const ULIS::uint8 *dstBlockPixels = dstBlock->PixelBits(0,0);
    FOdysseyBlendParameters blendParameters = FOdysseyBlendParameters( false
                                                                     , EOdysseyBlendingMode::kNormal
                                                                     , EOdysseyAlphaMode::kNormal
                                                                     , 100.0f );
    ::ULIS::FVec2D srcCenter = ::ULIS::FVec2D( iSrcCenterX, iSrcCenterY );
    ::ULIS::FVec2D dstCenter = ::ULIS::FVec2D( iDstCenterX, iDstCenterY );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_G8);

    ctx.Clear(*dstBlock);
    ctx.Finish();

    for( FDistortion& distortion : mDistortionMap )
    {
        if( distortion.mapped )
        {
#ifdef LIQUIFY_TWIRL
            double distance = distortion.distance;
            double factor = 1.0f/* - ( distance / Radius )*/; // can be precomputed
            double newAngle = distortion.angle - ( iAngleInRadians * factor );
            ::ULIS::FVec2I srcCoords = ::ULIS::FVec2I( iCenterX + ( distance * cos( newAngle ) )
                                                     , iCenterY + ( distance * sin( newAngle ) ) );
            ::ULIS::FVec2I dstCoords = ::ULIS::FVec2I( iCenterX + distortion.coords.x
                                                     , iCenterY + distortion.coords.y );
            uint32 srcOffset = ( srcCoords.y * srcBlock->Width() ) + srcCoords.x;
            uint32 dstOffset = ( dstCoords.y * dstBlock->Width() ) + dstCoords.x;
#endif
#ifdef LIQUIFY_PUSH
            double distance = distortion.distance;
            double factor = 1.0f/* - ( distance / Radius )*/; // can be precomputed
            double newAngle = distortion.angle - ( iAngleInRadians * factor );
            ::ULIS::FVec2I srcCoords = ::ULIS::FVec2I( iCenterX + ( distance * cos( newAngle ) )
                                                     , iCenterY + ( distance * sin( newAngle ) ) );
            ::ULIS::FVec2I dstCoords = ::ULIS::FVec2I( iCenterX + distortion.coords.x
                                                     , iCenterY + distortion.coords.y );
            uint32 srcOffset = ( srcCoords.y * srcBlock->Width() ) + srcCoords.x;
            uint32 dstOffset = ( dstCoords.y * dstBlock->Width() ) + dstCoords.x;
#endif
            ::ULIS::FVec2D srcCoordsRelative;
            ::ULIS::FVec2D dstCoordsRelative;

            iLiquifyfunc( distortion, srcCoordsRelative, dstCoordsRelative );

            ::ULIS::FVec2D srcCoordsTexture = ( srcCoordsRelative + dstCenter );
            ::ULIS::FVec2D dstCoordsTexture = ( dstCoordsRelative + dstCenter );

            uint32 srcOffset = ( ((uint32)srcCoordsTexture.y) * srcBlock->Width() ) +  ((uint32)srcCoordsTexture.x);
            uint32 dstOffset = ( ((uint32)dstCoordsTexture.y) * dstBlock->Width() ) +  ((uint32)dstCoordsTexture.x);

            switch( srcBlock->BytesPerPixel() )
            {
                case 4 :
                {
                    uint8 (*srcBlockPixels32)[4] = (uint8(*)[4]) srcBlockPixels;
                    uint8 (*dstBlockPixels32)[4] = (uint8(*)[4]) dstBlockPixels;

                    dstBlockPixels32[dstOffset][0] = srcBlockPixels32[srcOffset][0];
                    dstBlockPixels32[dstOffset][1] = srcBlockPixels32[srcOffset][1];
                    dstBlockPixels32[dstOffset][2] = srcBlockPixels32[srcOffset][2];
                    dstBlockPixels32[dstOffset][3] = srcBlockPixels32[srcOffset][3];
                }
                break;

                default :
                break;
            }
        }
    }

    dstBlock->Dirty( ::ULIS::FRectD::FromXYWH( dstCenter.x - Radius
                                             , dstCenter.y - Radius
                                             , Radius * 2
                                             , Radius * 2 ) );
    //mPaintEngine.Update( blendParameters );
    mPaintEngine.Commit( blendParameters );
}

void UOdysseyPainterEditorRasterLiquifyTool::Tick(float iDeltaTime)
{
    if( bIsMouseDown )
    {
        //Twirl( mMouseAtDown.X, mMouseAtDown.Y, 5.0f * M_PI / 180.0f );
    }
}

void
UOdysseyPainterEditorRasterLiquifyTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);

    mLiquifyHUD->SetCursorPositionInTexture( FVector2D( iPointInTexture.x, iPointInTexture.y ) );

    if (!rasterBlock || rasterBlock != mPaintEngine.GetRasterBlock())
    {
        return;
    }

//    TSharedPtr<::ULIS::FBlock> sourceBlock = GetSourceBlock();
//    if (!sourceBlock)
//        return false;

    TSharedPtr<::ULIS::FBlock> block = rasterBlock->GetBlock();

//    ::ULIS::FPixel sourceColor = sourceBlock->Pixel(iPointInTexture.x, iPointInTexture.y);
//    ::ULIS::FColor dstColor = GetEditor()->PaintColor().GetValue();
//    dstColor.SetAlphaF(BlendParameters.Opacity / 100.f);
//    TSharedPtr<::ULIS::FBlock> sourceMaskBlock = CreateSourceMaskBlock(sourceBlock, sourceColor);
//    TSharedPtr<::ULIS::FBlock> maskBlock = MakeShared<::ULIS::FBlock>(block->Width(), block->Height(), ::ULIS::Format_G8);


//    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
//    TSharedPtr<::ULIS::FBlock> selectionBlock = mPaintEngine.PaintBlock();
    //TSharedPtr<::ULIS::FBlock> selectionBlock = rasterSelection->GetBlock();
//    ULIS::uint8 *selectionBlockPixels = selectionBlock->PixelBits(0,0);


    /*Twirl( iPointInTexture.x - iPointInTexture.deltaPosition.X
         , iPointInTexture.y - iPointInTexture.deltaPosition.Y
         , iPointInTexture.x
         , iPointInTexture.y
         , 5.0f * M_PI / 180.0f );*/

    Push( iPointInTexture.x - iPointInTexture.deltaPosition.X
        , iPointInTexture.y - iPointInTexture.deltaPosition.Y
        , iPointInTexture.x
        , iPointInTexture.y );

    //mPaintEngine.PaintBlock()->Dirty();
    //mPaintEngine.Update( FOdysseyBlendParameters() );

//    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_G8);

/*
    //paintBlock->Dirty();
    Commit();

    FOdysseyPainterEditor* editor = GetEditor();
    TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
    if (source)
        source->RecordCurrentFrameUndo();

    GEditor->EndTransaction();
*/
}

bool
UOdysseyPainterEditorRasterLiquifyTool::OnKeyDown(const FKey& iKey)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
        return false;

    return Super::OnKeyDown( iKey );
}

bool
UOdysseyPainterEditorRasterLiquifyTool::OnKeyUp(const FKey& iKey)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
        return false;

    return Super::OnKeyUp( iKey );
}

void
UOdysseyPainterEditorRasterLiquifyTool::Flush()
{
}

void
UOdysseyPainterEditorRasterLiquifyTool::Commit()
{
/*
    FScopedTransaction transaction(LOCTEXT("raster-liquify-tool.transaction.liquify-area", "Liquify Area"));
    mPaintEngine.Commit(BlendParameters);

    FOdysseyPainterEditor* editor = GetEditor();
    TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
    if (source)
        source->RecordCurrentFrameUndo();
*/
}

void UOdysseyPainterEditorRasterLiquifyTool::BindShortcuts(TSharedPtr<FUICommandList> iCommandList)
{
    Super::BindShortcuts(iCommandList);
/*
    const FOdysseyPainterEditorCommands& painterEditorToolCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) iCommandList->MapAction( action, FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorRasterLiquifyTool::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(painterEditorToolCommands.RefreshBrush, RefreshBrushInstance)
    MAP_ACTION(painterEditorToolCommands.IncreaseBrushSize, AddSize, 1)
    MAP_ACTION(painterEditorToolCommands.DecreaseBrushSize, AddSize, -1)
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeNormal, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Normal )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeErase, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Erase )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeTop, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Top )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeBack, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Back )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeSub, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Sub )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeAdd, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Add )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeMul, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Mul )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeMin, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Min )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeMax, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Max )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeNormal, SetBlendMode, ::ULIS::eBlendMode::Blend_Normal )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeTop, SetBlendMode, ::ULIS::eBlendMode::Blend_Top )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeBack, SetBlendMode, ::ULIS::eBlendMode::Blend_Back )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeBehind, SetBlendMode, ::ULIS::eBlendMode::Blend_Behind )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDissolve, SetBlendMode, ::ULIS::eBlendMode::Blend_Dissolve )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeBayerDither8x8, SetBlendMode, ::ULIS::eBlendMode::Blend_BayerDither8x8 )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDarken, SetBlendMode, ::ULIS::eBlendMode::Blend_Darken )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeMultiply, SetBlendMode, ::ULIS::eBlendMode::Blend_Multiply )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeColorBurn, SetBlendMode, ::ULIS::eBlendMode::Blend_ColorBurn )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLinearBurn, SetBlendMode, ::ULIS::eBlendMode::Blend_LinearBurn )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDarkerColor, SetBlendMode, ::ULIS::eBlendMode::Blend_DarkerColor )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLighten, SetBlendMode, ::ULIS::eBlendMode::Blend_Lighten )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeScreen, SetBlendMode, ::ULIS::eBlendMode::Blend_Screen )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeColorDodge, SetBlendMode, ::ULIS::eBlendMode::Blend_ColorDodge )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLinearDodge, SetBlendMode, ::ULIS::eBlendMode::Blend_LinearDodge )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLighterColor, SetBlendMode, ::ULIS::eBlendMode::Blend_LighterColor )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeOverlay, SetBlendMode, ::ULIS::eBlendMode::Blend_Overlay )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeSoftLight, SetBlendMode, ::ULIS::eBlendMode::Blend_SoftLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeHardLight, SetBlendMode, ::ULIS::eBlendMode::Blend_HardLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeVividLight, SetBlendMode, ::ULIS::eBlendMode::Blend_VividLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLinearLight, SetBlendMode, ::ULIS::eBlendMode::Blend_LinearLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModePinLight, SetBlendMode, ::ULIS::eBlendMode::Blend_PinLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeHardMix, SetBlendMode, ::ULIS::eBlendMode::Blend_HardMix )
    MAP_ACTION(painterEditorToolCommands.SetBlendModePhoenix, SetBlendMode, ::ULIS::eBlendMode::Blend_Phoenix )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeReflect, SetBlendMode, ::ULIS::eBlendMode::Blend_Reflect )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeGlow, SetBlendMode, ::ULIS::eBlendMode::Blend_Glow )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDifference, SetBlendMode, ::ULIS::eBlendMode::Blend_Difference )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeExclusion, SetBlendMode, ::ULIS::eBlendMode::Blend_Exclusion )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeAdd, SetBlendMode, ::ULIS::eBlendMode::Blend_Add )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeSubstract, SetBlendMode, ::ULIS::eBlendMode::Blend_Substract )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDivide, SetBlendMode, ::ULIS::eBlendMode::Blend_Divide )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeAverage, SetBlendMode, ::ULIS::eBlendMode::Blend_Average )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeNegation, SetBlendMode, ::ULIS::eBlendMode::Blend_Negation )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeHue, SetBlendMode, ::ULIS::eBlendMode::Blend_Hue )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeSaturation, SetBlendMode, ::ULIS::eBlendMode::Blend_Saturation )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeColor, SetBlendMode, ::ULIS::eBlendMode::Blend_Color )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLuminosity, SetBlendMode, ::ULIS::eBlendMode::Blend_Luminosity )
    MAP_ACTION(painterEditorToolCommands.SetBlendModePartialDerivative, SetBlendMode, ::ULIS::eBlendMode::Blend_PartialDerivative )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeWhiteOut, SetBlendMode, ::ULIS::eBlendMode::Blend_Whiteout )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeAngleCorrected, SetBlendMode, ::ULIS::eBlendMode::Blend_AngleCorrected )

    #undef MAP_ACTION
*/
}

void
UOdysseyPainterEditorRasterLiquifyTool::OnRasterSelectionChanged()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    if (rasterSelection->IsEmpty())
    {
        //mPaintEngine.SetMaskBlock(nullptr);
    }
    else
    {
        //mPaintEngine.SetMaskBlock(rasterSelection->GetBlock());
    }
}

void
UOdysseyPainterEditorRasterLiquifyTool::ExtendMenu( TSharedRef<FExtender> iExtender )
{
    Super::ExtendMenu(iExtender);
}

void
UOdysseyPainterEditorRasterLiquifyTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);
}

void UOdysseyPainterEditorRasterLiquifyTool::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
    Super::PropertyChanged(iPropertyName);

    mLiquifyHUD->Reset();
}

void
UOdysseyPainterEditorRasterLiquifyTool::PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive)
{
    Super::PostPropertyChanged(iPropertyName, iIsInteractive);

}

EMouseCursor::Type
UOdysseyPainterEditorRasterLiquifyTool::GetMouseCursor() const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return EMouseCursor::SlashedCircle;

    return UOdysseyPainterEditorTool::GetMouseCursor();
}

FText
UOdysseyPainterEditorRasterLiquifyTool::GetTooltip() const
{
    return LOCTEXT("raster-liquify-tool.tooltip", "Liquify Tool");
}

/*
void
UOdysseyPainterEditorRasterLiquifyTool::OnRasterSelectionChanged()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    if (rasterSelection->IsEmpty())
    {
        mPaintEngine.SetMaskBlock(nullptr);
    }
    else
    {
        mPaintEngine.SetMaskBlock(rasterSelection->GetBlock());
    }
}
*/

#undef LOCTEXT_NAMESPACE
