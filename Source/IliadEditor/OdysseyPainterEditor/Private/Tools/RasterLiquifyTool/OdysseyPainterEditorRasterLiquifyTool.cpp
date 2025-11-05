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
    , Radius ( 10 )
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
    mPreviousPointInTexture = mMouseAtDown;

    if (iKey != EKeys::LeftMouseButton)
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(true);
    if (!rasterBlock)
        return false;

    mPaintEngine.RasterBlock(rasterBlock);

    MakeDistortionMap();
    MakeFlowMap();

    mSourcePixels.SetNum( rasterBlock->GetWidth() * rasterBlock->GetHeight() );
    memcpy( &mSourcePixels[0]
          , rasterBlock->GetBlock()->PixelBits(0,0)
          , rasterBlock->GetWidth() * rasterBlock->GetHeight() * sizeof ( uint32 ) );

    return true;
}

bool
UOdysseyPainterEditorRasterLiquifyTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    TSharedPtr<FOdysseyRasterBlock> srcRasterBlock = GetRasterBlockFromEditor(false);
    TSharedPtr<::ULIS::FBlock> srcBlock = srcRasterBlock->GetBlock();
    TSharedPtr<::ULIS::FBlock> dstBlock = mPaintEngine.PaintBlock();

    bIsMouseDown = false;

    if (iKey != EKeys::LeftMouseButton)
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock || rasterBlock != mPaintEngine.GetRasterBlock())
    {
        return false;
    }

    ApplyFlow( srcBlock
             , dstBlock
             , ::ULIS::FRectI::FromXYWH( 0
                                       , 0
                                       , rasterBlock->GetWidth()
                                       , rasterBlock->GetHeight() )
             , false
             , true );

    mPaintEngine.Commit( FOdysseyBlendParameters() );

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
UOdysseyPainterEditorRasterLiquifyTool::MakeFlowMap()
{
    TSharedPtr<FOdysseyRasterBlock> srcRasterBlock = GetRasterBlockFromEditor(false);
    TSharedPtr<::ULIS::FBlock> srcBlock = srcRasterBlock->GetBlock();

    mFlowMap.Empty();
    mFlowMap.SetNum( srcBlock->Width() * srcBlock->Height() );
}

void
UOdysseyPainterEditorRasterLiquifyTool::MakeDistortionMap()
{
    uint32 offset = 0;

    mDistortionMap.SetNum( ((Radius*2)+1) * ((Radius*2)+1) );

    for( int32 j = -Radius, y = 0; j <= Radius; j++, y++ )
    {
        for( int32 i = -Radius, x = 0; i <= Radius; i++, x++ )
        {
            double distanceToCenter = sqrt( ( i * i ) + ( j * j ) );
            FDistortion& distortion = mDistortionMap[offset++];

            if( distanceToCenter <= Radius )
            {
                ::ULIS::FVec2I pointCoords = ::ULIS::FVec2I( i, j );

                distortion.mapped = true;
                distortion.coords = pointCoords;
                distortion.distanceToCenter = distanceToCenter;
                distortion.angle = atan2( j, i );
            }
            else
            {
                distortion.mapped = false;
            }
        }
    }
}

inline void
UOdysseyPainterEditorRasterLiquifyTool::_Liquify_Push( int32 iPrevCenterX
                                                     , int32 iPrevCenterY
                                                     , int32 iCurrCenterX
                                                     , int32 iCurrCenterY
                                                     , FDistortion& distortion
                                                     , ::ULIS::FVec2D& oPrevCoords
                                                     , ::ULIS::FVec2D& oCurrCoords )
{
    ::ULIS::FVec2D prevCenter = ::ULIS::FVec2D( iPrevCenterX, iPrevCenterY );
    ::ULIS::FVec2D currCenter = ::ULIS::FVec2D( iCurrCenterX, iCurrCenterY );
    ::ULIS::FVec2D difCenter = currCenter - prevCenter;

    double distance = distortion.distanceToCenter;
    double factor = 1.0f - ( distance / Radius ); // can be precomputed
    ::ULIS::FVec2D currCoords = currCenter + ::ULIS::FVec2D( distortion.coords.x
                                                           , distortion.coords.y );
    ::ULIS::FVec2D prevCoords = currCoords - ( difCenter * factor );

    oPrevCoords = prevCoords;
    oCurrCoords = distortion.newCoords = currCoords;
}


#define _LIQUIFY_PUSH(iPrevCenterX,iPrevCenterY,iCurrCenterX,iCurrCenterY,distortion,oPrevCoords,oCurrCoords) \
::ULIS::FVec2D _prevCenter = ::ULIS::FVec2D( iPrevCenterX, iPrevCenterY );                                    \
::ULIS::FVec2D _currCenter = ::ULIS::FVec2D( iCurrCenterX, iCurrCenterY );                                    \
::ULIS::FVec2D _difCenter = _currCenter - _prevCenter;                                                        \
                                                                                                              \
double _distance = distortion.distance;                                                                       \
double _factor = 1.0f - ( _distance / Radius );     /* this can be precomputed */                             \
::ULIS::FVec2D _currCoords = _currCenter + ::ULIS::FVec2D( distortion.coords.x                                \
                                                         , distortion.coords.y );                             \
::ULIS::FVec2D _prevCoords = _currCoords - ( _difCenter * _factor );                                          \
                                                                                                              \
oPrevCoords = _prevCoords;                                                                                    \
oCurrCoords = distortion.newCoords = _currCoords;                                                             \

void
UOdysseyPainterEditorRasterLiquifyTool::Twirl( int32 iSrcCenterX
                                             , int32 iSrcCenterY
                                             , int32 iDstCenterX
                                             , int32 iDstCenterY
                                             , double iAngleInRadians )
{
#ifdef  unused
    Liquify( iSrcCenterX
           , iSrcCenterY
           , iDstCenterX
           , iDstCenterY
           , [ iSrcCenterX
             , iSrcCenterY
             , iDstCenterX
             , iDstCenterY
             , iAngleInRadians ]( FDistortion& distortion
                                , ::ULIS::FVec2D& oSrcCoords
                                , ::ULIS::FVec2D& oDstCoords )
    {
        double distance = distortion.distance;
        double factor = 1.0f/* - ( distance / Radius )*/; // can be precomputed
        double newAngle = distortion.angle - ( iAngleInRadians * factor );
        // TODO : Matrix Operation
        ::ULIS::FVec2D srcCoords = ::ULIS::FVec2D( ( distance * cos( newAngle ) )
                                                 , ( distance * sin( newAngle ) ) );
        ::ULIS::FVec2D dstCoords = ::ULIS::FVec2D( distortion.coords.x
                                                 , distortion.coords.y );

        oSrcCoords = iSrcCenterX + srcCoords;
        oDstCoords = iDstCenterX + dstCoords;
    } );
#endif
}

void
UOdysseyPainterEditorRasterLiquifyTool::Flow( const FVector2D& iPrevCenter
                                            , const FVector2D& iCurrCenter )
{
    TSharedPtr<FOdysseyRasterBlock> srcRasterBlock = GetRasterBlockFromEditor(false);
    TSharedPtr<::ULIS::FBlock> srcBlock = srcRasterBlock->GetBlock();
    FVector2D delta = iCurrCenter - iPrevCenter;

    for( FDistortion& distortion : mDistortionMap )
    {
        if( distortion.mapped )
        {
            ::ULIS::FVec2I absoluteCoords = ::ULIS::FVec2I( iCurrCenter.X + distortion.coords.x
                                                          , iCurrCenter.Y + distortion.coords.y );

            uint32 offset = ( absoluteCoords.y * srcBlock->Width() ) + absoluteCoords.x;
            double factor = 1.0f - ( distortion.distanceToCenter / Radius );

            mFlowMap[offset].delta += ( delta * factor );
        }
    }
}

void UOdysseyPainterEditorRasterLiquifyTool::Tick(float iDeltaTime)
{
    if( bIsMouseDown )
    {
        //Twirl( mMouseAtDown.X, mMouseAtDown.Y, 5.0f * M_PI / 180.0f );
    }
}

void
UOdysseyPainterEditorRasterLiquifyTool::ApplyFlow( TSharedPtr<::ULIS::FBlock> iSrcBlock
                                                 , TSharedPtr<::ULIS::FBlock> iDstBlock
                                                 , const ::ULIS::FRectI& iRegionOfInterest
                                                 , bool iReferToROIOnly
                                                 , bool iBilinearFiltered )
{
    const ULIS::uint8 *srcBlockPixels = iSrcBlock->PixelBits(0,0);
    const ULIS::uint8 *dstBlockPixels = iDstBlock->PixelBits(0,0);
    ::ULIS::FRectI roi = iRegionOfInterest;

    for( int32 y = iRegionOfInterest.y; y < ( roi.y + roi.h ); y++ )
    {
        for( int32 x = roi.x; x < ( roi.x + roi.w ); x++ )
        {
            uint32 dstOffset = ( y * iSrcBlock->Width() ) + x;
            FFlow& flow = mFlowMap[dstOffset];

            if( flow.delta.IsZero() == false )
            {
                //::ULIS::FVec2D relativeCoords = GetRelativeCoords( flow );
                ::ULIS::FVec2D srcCoords = ::ULIS::FVec2D( x - flow.delta.X
                                                         , y - flow.delta.Y );
                double deltaX = srcCoords.x - ((uint32)srcCoords.x);
                double deltaY = srcCoords.y - ((uint32)srcCoords.y);

                srcCoords.x = ((uint32)srcCoords.x) % iSrcBlock->Width();
                srcCoords.y = ((uint32)srcCoords.y) % iSrcBlock->Height();

                uint32 srcOffset = ( ((uint32)srcCoords.y) * iSrcBlock->Width() ) + ((uint32)srcCoords.x);

                switch( iSrcBlock->BytesPerPixel() )
                {
                    case 4 :
                    {
                        uint8 (*srcBlockPixels32)[4] = (uint8(*)[4]) &mSourcePixels[0];
                        uint8 (*dstBlockPixels32)[4] = (uint8(*)[4]) dstBlockPixels;

                        // Do the bilinear interpolation thing for every component of the pixel
                        if( iBilinearFiltered )
                        {
                            uint32 SRCOFFSETTOPLEFT     = srcOffset
                                 , SRCOFFSETTOPRIGHT    = SRCOFFSETTOPLEFT + 1
                                 , SRCOFFSETBOTTOMRIGHT = SRCOFFSETTOPLEFT + 1 + (iSrcBlock->Width())
                                 , SRCOFFSETBOTTOMLEFT  = SRCOFFSETTOPLEFT + (iSrcBlock->Width());

                            for ( uint32 k = 0; k < 4; ++k )
                            {
                                uint8 C0 = (srcBlockPixels32[SRCOFFSETTOPRIGHT][k]    - srcBlockPixels32[SRCOFFSETTOPLEFT][k])    * deltaX + srcBlockPixels32[SRCOFFSETTOPLEFT][k];
                                uint8 C1 = (srcBlockPixels32[SRCOFFSETBOTTOMRIGHT][k] - srcBlockPixels32[SRCOFFSETBOTTOMLEFT][k]) * deltaX + srcBlockPixels32[SRCOFFSETBOTTOMLEFT][k];
                                uint8 CF = (C1 - C0) * deltaY + C0;

                                dstBlockPixels32[dstOffset][k] = CF;
                            }
                        }
                        else
                        {
                            uint8 R = srcBlockPixels32[srcOffset][0];
                            uint8 G = srcBlockPixels32[srcOffset][1];
                            uint8 B = srcBlockPixels32[srcOffset][2];
                            uint8 A = srcBlockPixels32[srcOffset][3];

                            dstBlockPixels32[dstOffset][0] = R;
                            dstBlockPixels32[dstOffset][1] = G;
                            dstBlockPixels32[dstOffset][2] = B;
                            dstBlockPixels32[dstOffset][3] = A;
                        }
                    }
                    break;

                    default :
                    break;
                }
            }
        }
    }

    iDstBlock->Dirty( roi );
}

void
UOdysseyPainterEditorRasterLiquifyTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    FVector2D deltaPoint = FVector2D( iPointInTexture.x - mPreviousPointInTexture.X
                                    , iPointInTexture.y - mPreviousPointInTexture.Y );
    double deltaPointDistance = deltaPoint.Length();

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

    // iPointInTexture.deltaPosition can not be trusted, I had strange result
    // with it so I had to use mPreviousPointInTexture
/*
    Liquify( mPreviousPointInTexture.X
           , mPreviousPointInTexture.Y
           , iPointInTexture.x
           , iPointInTexture.y );
*/

    if( deltaPointDistance >= 1.0f )
    {
        FVector2D stampPointInTexture = mPreviousPointInTexture;
        double stepX = deltaPoint.X  / (uint32) deltaPointDistance;
        double stepY = deltaPoint.Y  / (uint32) deltaPointDistance;

        for( uint32 i = 0; i < (uint32) deltaPointDistance; i++ )
        {
            FVector2D stampNextPointInTexture = FVector2D( stampPointInTexture.X + stepX
                                                         , stampPointInTexture.Y + stepY );

            Flow( stampPointInTexture, stampNextPointInTexture );

            stampPointInTexture = stampNextPointInTexture;
        }

        TSharedPtr<FOdysseyRasterBlock> srcRasterBlock = GetRasterBlockFromEditor(false);
        TSharedPtr<::ULIS::FBlock> srcBlock = srcRasterBlock->GetBlock();
        const ULIS::uint8 *srcBlockPixels = srcBlock->PixelBits(0,0);
        TSharedPtr<::ULIS::FBlock> dstBlock = mPaintEngine.PaintBlock();
        const ULIS::uint8 *dstBlockPixels = dstBlock->PixelBits(0,0);
        ::ULIS::FRectI roi = ::ULIS::FRectI::FromXYWH( mPreviousPointInTexture.X - Radius
                                                     , mPreviousPointInTexture.Y - Radius
                                                     , (Radius*2) + 1
                                                     , (Radius*2) + 1 )
                           | ::ULIS::FRectI::FromXYWH( iPointInTexture.x - Radius
                                                     , iPointInTexture.y - Radius
                                                     , (Radius*2) + 1
                                                     , (Radius*2) + 1 );

        ApplyFlow( srcBlock, dstBlock, roi, false, false );

        mPaintEngine.Update( FOdysseyBlendParameters() );

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
        mPreviousPointInTexture = FVector2D( iPointInTexture.x, iPointInTexture.y );
    }
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
