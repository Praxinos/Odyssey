// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyFlipbookTimelineTrack.h"

#include "Fonts/FontMeasure.h"
#include "Framework/Commands/GenericCommands.h"
#include "IAssetTools.h"
#include "ImageUtils.h"
#include "ObjectTools.h"
#include "PaperImporterSettings.h"
#include "PaperSprite.h"
#include "PropertyCustomizationHelpers.h"
#include "RenderGraphUtils.h"
#include "ScreenPass.h"
#include "Widgets/Layout/SScaleBox.h"

#include "SOdysseyFlipbookTimelineFrameList.h"
#include "SOdysseyFlipbookTimelineFrame.h"
#include "OdysseyPainterEditorFlipbookCommands.h"
#include "OdysseyPainterEditorFlipbookListener.h"
#include "OdysseyPainterEditorFlipbookUtils.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyStyle.h"
#include "OdysseyTextureLayerStackUserData.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"

#define THUMBNAIL_SIZE_FLIPBOOK 64

namespace
{
    static TStrongObjectPtr<UTexture2D> sFlipbookCheckerboardTexture;
    static FColor sFlipbookCheckerboardColorOne = FColor( EForceInit::ForceInit );
    static FColor sFlipbookCheckerboardColorTwo = FColor( EForceInit::ForceInit );
};

SOdysseyFlipbookTimelineTrack::~SOdysseyFlipbookTimelineTrack()
{
    UOdysseyPainterEditorSettings* settings = GetMutableDefault<UOdysseyPainterEditorSettings>();
    settings->GetOnCheckerColorChanged().RemoveAll( this );
    settings->GetOnCheckerSizeChanged().RemoveAll( this );

    IOdysseyRenderingAbility::OnRenderingChangedDelegate().RemoveAll( this );
}

void SOdysseyFlipbookTimelineTrack::Construct( const SOdysseyFlipbookTimelineTrack::FArguments& InArgs )
{
    mFlipbook = InArgs._Flipbook;
    mFrameSize = InArgs._FrameSize;
    mOnFlipbookChanged = InArgs._OnFlipbookChanged;
    mOnKeyframeRemoved = InArgs._OnKeyframeRemoved;
    mOnKeyframeAdded = InArgs._OnKeyframeAdded;
    mOnSpriteCreated = InArgs._OnSpriteCreated;
    mOnTextureCreated = InArgs._OnTextureCreated;

    mListener = MakeUnique<FOdysseyPainterEditorFlipbookListener>(mFlipbook);
    mListener->OnSpriteTextureChanged().AddRaw(this, &SOdysseyFlipbookTimelineTrack::OnSpriteTextureChanged);

    mFrameWarningBrush = FOdysseyStyle::GetBrush("FlipbookTimeline.TimelineFrameWarning");

    //---

    IOdysseyRenderingAbility::OnRenderingChangedDelegate().AddSP( this, &SOdysseyFlipbookTimelineTrack::OnRenderingChanged );

    UOdysseyPainterEditorSettings* settings = GetMutableDefault<UOdysseyPainterEditorSettings>();
    settings->GetOnCheckerColorChanged().AddSP( this, &SOdysseyFlipbookTimelineTrack::CreateCheckerboardTexture );
    settings->GetOnCheckerSizeChanged().AddSP( this, &SOdysseyFlipbookTimelineTrack::CreateCheckerboardTexture );

    CreateCheckerboardTexture();

    //---

    ChildSlot
    [
        //Frames
        SAssignNew(mFrameList, SOdysseyFlipbookTimelineFrameList)
        .FrameSize(mFrameSize)
        .OnFramesMoved(this, &SOdysseyFlipbookTimelineTrack::OnFramesMoved)
        .OnFramesLengthChanged(this, &SOdysseyFlipbookTimelineTrack::OnFramesLengthChanged)
        .OnFramesEditStart(this, &SOdysseyFlipbookTimelineTrack::OnFramesEditStart)
        .OnFramesEditStop(this, &SOdysseyFlipbookTimelineTrack::OnFramesEditStop)
        .OnFramesEditCancel(this, &SOdysseyFlipbookTimelineTrack::OnFramesEditCancel)
        .OnGenerateFrameContextMenu(this, &SOdysseyFlipbookTimelineTrack::OnGenerateFrameContextMenu)
    ];

    Rebuild();
}

void
SOdysseyFlipbookTimelineTrack::RefreshTextureRenderTarget( UTexture2D* iTexture, UTextureRenderTarget2D* iRenderTarget )
{
    // It doesn't work and I don't know why because the FObjectThumbnail seems to be filled with the right pixels and they come from the RTResource
    // Until someone found why, fill the render target with ENQUEUE_RENDER_COMMAND()

    //FObjectThumbnail t;
    //FTextureRenderTargetResource* RTResource = iRenderTarget->GameThread_GetRenderTargetResource();
    //ThumbnailTools::RenderThumbnail( iTexture, iRenderTarget->GetSurfaceWidth(), iRenderTarget->GetSurfaceHeight(), ThumbnailTools::EThumbnailTextureFlushMode::NeverFlush, RTResource, &t );

    //OR

    FIntRect iDstRect( 0, 0, iRenderTarget->GetSurfaceWidth(), iRenderTarget->GetSurfaceHeight() );
    FIntRect iSrcRect( 0, 0, iTexture->GetSurfaceWidth(), iTexture->GetSurfaceHeight() );

    ENQUEUE_RENDER_COMMAND( IOdysseyTextureRenderingAbility_RenderRectAtRect )(
        [this, iRenderTarget, iTexture, iSrcRect, iDstRect]( FRHICommandListImmediate& RHICmdList )
        {
            FRDGBuilder graphBuilder( RHICmdList );

            FRDGTextureRef destinationTexture = iRenderTarget->GetRenderTargetResource()->GetRenderTargetTexture( graphBuilder );

            //AddClearRenderTargetPass( graphBuilder, destinationTexture, FLinearColor::Blue, iDstRect );

            FRDGTextureRef sourceTexture = graphBuilder.RegisterExternalTexture( CreateRenderTarget( iTexture->GetResource()->TextureRHI, TEXT( "SOdysseyImportTexturesDialog_Render::sourceTexture" ) ) );

            AddDrawTexturePass(
                graphBuilder,
                FScreenPassViewInfo(),
                sourceTexture,
                destinationTexture,
                iSrcRect.Min,
                iSrcRect.Size(),
                iDstRect.Min,
                iDstRect.Size()
            );

            graphBuilder.Execute();
        }
        );
}

void
SOdysseyFlipbookTimelineTrack::CreateCheckerboardTexture()
{
    const UOdysseyPainterEditorSettings& settings = *GetDefault< UOdysseyPainterEditorSettings >();
    if( !sFlipbookCheckerboardTexture
        || sFlipbookCheckerboardColorOne != settings.GetCheckerColorOne()
        || sFlipbookCheckerboardColorTwo != settings.GetCheckerColorTwo()
        )
    {
        sFlipbookCheckerboardTexture = TStrongObjectPtr<UTexture2D>( FImageUtils::CreateCheckerboardTexture( settings.GetCheckerColorOne(), settings.GetCheckerColorTwo(), 16 ) );
        sFlipbookCheckerboardColorOne = settings.GetCheckerColorOne();
        sFlipbookCheckerboardColorTwo = settings.GetCheckerColorTwo();
    }

    //---

    if( !mCheckerboardBrush )
        mCheckerboardBrush = new FSlateImageBrush( sFlipbookCheckerboardTexture.Get(), FVector2f( sFlipbookCheckerboardTexture->GetSurfaceWidth(), sFlipbookCheckerboardTexture->GetSurfaceHeight() ), FSlateColor( FLinearColor::White ), ESlateBrushTileType::Both );

    mCheckerboardBrush->SetResourceObject( sFlipbookCheckerboardTexture.Get() );
}

void
SOdysseyFlipbookTimelineTrack::OnRenderingChanged( const FOdysseyRenderingChangedEvent& iEvent )
{
    if( !mFlipbook )
        return;

    if( iEvent.IsInteractive() )
        return;

    for( int32 index = 0; index < mFlipbook->GetNumKeyFrames(); ++index )
    {
        const FPaperFlipbookKeyFrame& keyframe = mFlipbook->GetKeyFrameChecked( index );
        UTexture2D* texture = OdysseyPainterEditorFlipbookUtils::GetKeyframeTexture( mFlipbook, index );
        if( !texture )
            continue;

        TStrongObjectPtr<UTextureRenderTarget2D>* renderTargetPtr = mRenderTargets.Find( texture );
        if( !renderTargetPtr )
            continue;

        UOdysseyTextureLayerStackUserData* userData = texture->GetAssetUserData<UOdysseyTextureLayerStackUserData>();
        if( !userData )
            continue;

        UOdysseyTextureLayerStack* layerStack = userData->LayerStack;
        if( !layerStack )
            continue;

        TArray<FGuid> composition = layerStack->GetRenderingComposition( EOdysseyRenderingType::Render, 0 );
        if( composition.Contains( iEvent.GetId() ) )
        {
            RefreshTextureRenderTarget( texture, mRenderTargets.FindChecked( texture ).Get() );
        }
    }
}

void
SOdysseyFlipbookTimelineTrack::Rebuild()
{
    if (!mFlipbook)
        return;

    mFrameList->RemoveAllFrames();

    for (int32 index = 0; index < mFlipbook->GetNumKeyFrames(); ++index)
    {
        const FPaperFlipbookKeyFrame& keyframe = mFlipbook->GetKeyFrameChecked(index);
        AddFrame(OdysseyPainterEditorFlipbookUtils::GetKeyframeTexture(mFlipbook, index), keyframe.FrameRun);
    }
}

void
SOdysseyFlipbookTimelineTrack::AddFrame(UTexture2D* iTexture, int32 iFrameLength)
{
    TSharedPtr<SWidget> contentWidget = CreateFrameContent(iTexture);

    int32 index = mFrameList->AddFrame();
    mFrameList->SetFrameLength(index, iFrameLength);
    mFrameList->SetFrameContent(index, contentWidget);
}

void
SOdysseyFlipbookTimelineTrack::InsertFrame(int32 iIndex, UTexture2D* iTexture, int32 iFrameLength)
{
    TSharedPtr<SWidget> contentWidget = CreateFrameContent(iTexture);

    mFrameList->InsertFrame(iIndex);
    mFrameList->SetFrameLength(iIndex, iFrameLength);
    mFrameList->SetFrameContent(iIndex, contentWidget);
}

void
SOdysseyFlipbookTimelineTrack::SetFrame(int32 iIndex, UTexture2D* iTexture)
{
    TSharedPtr<SWidget> contentWidget = CreateFrameContent(iTexture);
    mFrameList->SetFrameContent(iIndex, contentWidget);
}

TSharedPtr<SWidget>
SOdysseyFlipbookTimelineTrack::CreateFrameContent(UTexture2D* iTexture)
{
    TSharedPtr<SWidget> contentWidget = nullptr;
    if (iTexture)
    {
        float w = iTexture->Source.GetSizeX();
        float h = iTexture->Source.GetSizeY();
        if (w > h)
        {
            float ratio = THUMBNAIL_SIZE_FLIPBOOK / w;
            w = THUMBNAIL_SIZE_FLIPBOOK;
            h *= ratio;
        }
        else
        {
            float ratio = THUMBNAIL_SIZE_FLIPBOOK / h;
            h = THUMBNAIL_SIZE_FLIPBOOK;
            w *= ratio;
        }

        //-

        // Create or get the corresponding render target of the texture
        TStrongObjectPtr<UTextureRenderTarget2D>* renderTargetPtr = mRenderTargets.Find( iTexture );
        if( !renderTargetPtr )
        {
            TStrongObjectPtr<UTextureRenderTarget2D> renderTarget( NewObject<UTextureRenderTarget2D>() );
            renderTarget->RenderTargetFormat = RTF_RGBA8;

            renderTarget->ClearColor = FLinearColor::Red;

            renderTarget->InitAutoFormat( w, h );
            renderTarget->UpdateResourceImmediate();

            mRenderTargets.Add( iTexture, renderTarget );
        }
        else
        {
            TStrongObjectPtr<UTextureRenderTarget2D> renderTarget = *renderTargetPtr;

            renderTarget->ResizeTarget( w, h );
            renderTarget->UpdateResource();
            renderTarget->UpdateResourceImmediate();
        }

        TStrongObjectPtr<UTextureRenderTarget2D> renderTarget = mRenderTargets.FindChecked( iTexture );

        RefreshTextureRenderTarget( iTexture, renderTarget.Get() );

        //-

        // Create or get the corresponding brush of the texture (initialized with the corresponding render target)
        FSlateImageBrush** textureBrushPtr = mTextureBrushes.Find( iTexture );
        if( !textureBrushPtr )
        {
            FSlateImageBrush* textureBrush = new FSlateImageBrush( renderTarget.Get(), FVector2f( renderTarget->GetSurfaceWidth(), renderTarget->GetSurfaceHeight() ) );

            mTextureBrushes.Add( iTexture, textureBrush );
        }
        else
        {
            FSlateImageBrush* textureBrush = *textureBrushPtr;

            textureBrush->SetImageSize( FVector2f( renderTarget->GetSurfaceWidth(), renderTarget->GetSurfaceHeight() ) );
        }

        FSlateImageBrush* textureBrush = mTextureBrushes.FindChecked( iTexture );

        contentWidget = SNew(SScaleBox)
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            .Stretch(EStretch::ScaleToFitY)
            .StretchDirection(EStretchDirection::DownOnly)
            [
                SNew(SBox)
                .WidthOverride(w)
                .HeightOverride(h)
                [
                    SNew( SOverlay )
                    + SOverlay::Slot()
                    [
                        SNew( SImage )
                        .Image( mCheckerboardBrush )
                    ]
                    + SOverlay::Slot()
                    [
                        SNew( SImage )
                        .Image( textureBrush )
                    ]
                ]
            ];
    }
    else
    {
        contentWidget = SNew(SScaleBox)
            .HAlign(HAlign_Left)
            .VAlign(VAlign_Center)
            .Stretch(EStretch::ScaleToFitY)
            .StretchDirection(EStretchDirection::DownOnly)
            [
                SNew(SBox)
                .WidthOverride(mFrameWarningBrush->ImageSize.X)
                .HeightOverride(mFrameWarningBrush->ImageSize.Y)
                [
                    SNew(SImage)
                    .Image(mFrameWarningBrush)
                ]
            ];
    }
    return contentWidget;
}


float
SOdysseyFlipbookTimelineTrack::FrameSize() const
{
    return mFrameSize.Get();
}

void
SOdysseyFlipbookTimelineTrack::DeleteFrame(int32 iIndex)
{
    //Remove from GUI
    mFrameList->RemoveFrameAt(iIndex);

    //Remove from Data
    FPaperFlipbookKeyFrame keyframe = mFlipbook->GetKeyFrameChecked(iIndex);

    OdysseyPainterEditorFlipbookUtils::RemoveKeyFrame(mFlipbook, iIndex);

    mOnKeyframeRemoved.ExecuteIfBound(keyframe);
    mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::DuplicateFrame(int32 iIndex)
{
    //Remove from Data
    UTexture2D* createdTexture = NULL;
    UPaperSprite* createdSprite = NULL;
    if (!OdysseyPainterEditorFlipbookUtils::DuplicateKeyFrame(mFlipbook, iIndex, &createdTexture, &createdSprite))
        return;

    FPaperFlipbookKeyFrame keyframe = mFlipbook->GetKeyFrameChecked(iIndex + 1);
    InsertFrame(iIndex + 1, createdTexture, keyframe.FrameRun);

    mOnKeyframeAdded.ExecuteIfBound(keyframe);
    mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::ShowKeyFrameSpriteInContentBrowser(int32 iIndex)
{
    OdysseyPainterEditorFlipbookUtils::ShowKeyFrameSpriteInContentBrowser(mFlipbook, iIndex);
}

void
SOdysseyFlipbookTimelineTrack::OnFramesMoved(TArray<int32> iSrcIndexes, int32 iDstIndex)
{
    //Move in Data
    OdysseyPainterEditorFlipbookUtils::MoveKeyFrames(mFlipbook, iSrcIndexes, iDstIndex);

    mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::OnFramesLengthChanged(TArray<int32> iFrameIndexes)
{
    //Change in Data
    {
        FScopedFlipbookMutator mutator(mFlipbook);
        for( int i = 0; i < iFrameIndexes.Num(); i++)
        {
            OdysseyPainterEditorFlipbookUtils::SetKeyFrameLength(mFlipbook, iFrameIndexes[i], mFrameList->GetFrameLength(iFrameIndexes[i]));
        }
    }

    mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::OnFramesEditStart()
{
    //Nothing to do here
}

void
SOdysseyFlipbookTimelineTrack::OnFramesEditStop()
{
    mOnFlipbookChanged.ExecuteIfBound();
}

void
SOdysseyFlipbookTimelineTrack::OnFramesEditCancel()
{
    //Nothing to do here
}

FReply
SOdysseyFlipbookTimelineTrack::OnGenerateFrameContextMenu(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent, int32 iFrameIndex)
{
    const FOdysseyPainterEditorFlipbookCommands& flipbookCommands = FOdysseyPainterEditorFlipbookCommands::Get();
    TSharedPtr<FUICommandList> frameCommandList = MakeShareable(new FUICommandList());
    frameCommandList->MapAction(FGenericCommands::Get().Duplicate, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::DuplicateFrame, iFrameIndex));
    frameCommandList->MapAction(FGenericCommands::Get().Delete, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::DeleteFrame, iFrameIndex));
    frameCommandList->MapAction(flipbookCommands.ShowSpriteInContentBrowser, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::ShowKeyFrameSpriteInContentBrowser, iFrameIndex));
    // frameCommandList->MapAction(flipbookCommands.EditSpriteForKeyFrame, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::EditSpriteForKeyFrame, iFrameIndex));
    frameCommandList->MapAction(flipbookCommands.AddNewKeyFrame, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::AddNewKeyframe, mFlipbook->GetNumKeyFrames()), FCanExecuteAction());
    frameCommandList->MapAction(flipbookCommands.AddNewKeyFrameBefore, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::AddNewKeyframe, iFrameIndex), FCanExecuteAction());
    frameCommandList->MapAction(flipbookCommands.AddNewKeyFrameAfter, FExecuteAction::CreateSP(this, &SOdysseyFlipbookTimelineTrack::AddNewKeyframe, iFrameIndex + 1), FCanExecuteAction());

    FMenuBuilder MenuBuilder(true, frameCommandList);
    {
        const FText KeyframeSectionTitle = LOCTEXT("timeline.context-menu.keyframe-section.name", "Keyframe Actions");
        MenuBuilder.BeginSection("KeyframeActions", KeyframeSectionTitle);
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Duplicate);
        MenuBuilder.AddMenuEntry(FGenericCommands::Get().Delete);

        MenuBuilder.AddMenuSeparator();

        MenuBuilder.AddMenuEntry(flipbookCommands.AddNewKeyFrameBefore);
        MenuBuilder.AddMenuEntry(flipbookCommands.AddNewKeyFrameAfter);

        MenuBuilder.EndSection();

        const FText SpriteSectionTitle = LOCTEXT("timeline.context-menu.sprite-section.name", "Sprite Actions");
        MenuBuilder.BeginSection("SpriteActions", SpriteSectionTitle);
        MenuBuilder.AddMenuEntry(flipbookCommands.ShowSpriteInContentBrowser);
        //MenuBuilder.AddMenuEntry(flipbookCommands.EditSpriteForKeyFrame);
        /* MenuBuilder.AddSubMenu(
            flipbookCommands.PickNewSpriteFrame->GetLabel(),
            flipbookCommands.PickNewSpriteFrame->GetDescription(),
            FNewMenuDelegate::CreateSP(this, &SOdysseyFlipbookTimelineTrack::OpenSpritePickerMenu, iFrameIndex)); */
        MenuBuilder.EndSection();
    }

    TSharedRef<SWidget> MenuContents = MenuBuilder.MakeWidget();
    FWidgetPath WidgetPath = iMouseEvent.GetEventPath() != nullptr ? *iMouseEvent.GetEventPath() : FWidgetPath();
    FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuContents, iMouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

    return FReply::Handled();
}

void
SOdysseyFlipbookTimelineTrack::OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture)
{
    UTexture2D* texture = iSprite->GetSourceTexture();
    for (int i = 0; i < mFlipbook->GetNumKeyFrames(); i++)
    {
        UPaperSprite* sprite = OdysseyPainterEditorFlipbookUtils::GetKeyframeSprite(mFlipbook, i);
        if (sprite == iSprite)
        {
            SetFrame(i, texture);
        }
    }
}

void
SOdysseyFlipbookTimelineTrack::OpenSpritePickerMenu(FMenuBuilder& MenuBuilder, int32 iIndex)
{
    UPaperSprite* sprite = OdysseyPainterEditorFlipbookUtils::GetKeyframeSprite(mFlipbook, iIndex);
    if (!sprite)
        return;

    FAssetData CurrentAssetData(sprite);

    const bool bAllowClear = true;
    TArray<const UClass*> AllowedClasses;
    AllowedClasses.Add(UPaperSprite::StaticClass());

    TSharedRef<SWidget> AssetPickerWidget = PropertyCustomizationHelpers::MakeAssetPickerWithMenu(CurrentAssetData,
        bAllowClear,
        AllowedClasses,
        PropertyCustomizationHelpers::GetNewAssetFactoriesForClasses(AllowedClasses),
        FOnShouldFilterAsset(),
        FOnAssetSelected::CreateSP(this, &SOdysseyFlipbookTimelineTrack::OnAssetSelected, iIndex),
        FSimpleDelegate::CreateSP(this, &SOdysseyFlipbookTimelineTrack::CloseMenu));

    MenuBuilder.AddWidget(AssetPickerWidget, FText::GetEmpty(), /*bNoIndent=*/ true);
}

void
SOdysseyFlipbookTimelineTrack::CloseMenu()
{
    FSlateApplication::Get().DismissAllMenus();
}

void
SOdysseyFlipbookTimelineTrack::OnAssetSelected(const FAssetData& AssetData, int32 iFrameIndex)
{
    UPaperSprite* sprite = Cast<UPaperSprite>(AssetData.GetAsset());
    OdysseyPainterEditorFlipbookUtils::SetKeyframeSprite(mFlipbook, iFrameIndex, sprite);
}

void
SOdysseyFlipbookTimelineTrack::AddNewKeyframe(int32 iIndex)
{
    UPaperSprite* sprite = NULL;
    UTexture2D* texture = NULL;
    FOdysseyTextureConfiguration textureConfiguration = mTextureConfiguration.Get();
    if (!OdysseyPainterEditorFlipbookUtils::CreateKeyFrame(mFlipbook, iIndex, &texture, &sprite, textureConfiguration))
        return;

    InsertFrame(iIndex, texture, mFlipbook->GetKeyFrameChecked(iIndex).FrameRun);
    mOnSpriteCreated.ExecuteIfBound(sprite);
    mOnTextureCreated.ExecuteIfBound(texture, textureConfiguration);
}

#undef LOCTEXT_NAMESPACE
