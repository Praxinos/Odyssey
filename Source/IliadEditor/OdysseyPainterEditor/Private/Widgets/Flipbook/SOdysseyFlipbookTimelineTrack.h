// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "PaperFlipbook.h"
#include "Engine/TextureRenderTarget2D.h"

#include "SOdysseyFlipbookTimelineTrackEvents.h"
#include "OdysseyRenderingAbility.h"

class SOdysseyFlipbookTimelineFrameList;
class SOdysseyFlipbookTimelineFrame;
class FOdysseyPainterEditorFlipbookListener;
class UPaperFlipbook;
struct FSlateImageBrush;

class SOdysseyFlipbookTimelineTrack : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyFlipbookTimelineTrack)
        {}
        SLATE_ARGUMENT(UPaperFlipbook*, Flipbook)
        SLATE_ATTRIBUTE(FOdysseyTextureConfiguration, TextureConfiguration)
        SLATE_ATTRIBUTE( float, FrameSize )
        SLATE_EVENT( FOnFlipbookChanged, OnFlipbookChanged)
        SLATE_EVENT( FOnKeyframeRemoved, OnKeyframeRemoved)
        SLATE_EVENT( FOnKeyframeAdded, OnKeyframeAdded)
        SLATE_EVENT( FOnSpriteCreated, OnSpriteCreated)
        SLATE_EVENT( FOnTextureCreated, OnTextureCreated)
    SLATE_END_ARGS()


    ~SOdysseyFlipbookTimelineTrack();

    // Construct the widget
    void Construct( const FArguments& InArgs );

public:
    //Accessors
    float FrameSize() const;

public:
    //Modifiers

    //Adds a new frame containing iTexture for iFrameLength time
    void AddFrame(UTexture2D* iTexture, int32 iFrameLength);

    //Inserts a new frame containing iTexture for iFrameLength time
    void InsertFrame(int32 iIndex, UTexture2D* iTexture, int32 iFrameLength);

    //Sets an existing frame to contain iTexture for iFrameLength time
    void SetFrame(int32 iIndex, UTexture2D* iTexture);

private:
    void Rebuild();
    TSharedPtr<SWidget> CreateFrameContent(UTexture2D* iTexture);
    void DuplicateFrame(int32 iIndex);
    void DeleteFrame(int32 iIndex);
    void ShowKeyFrameSpriteInContentBrowser(int32 iIndex);
    // void EditSpriteForKeyFrame(int32 iIndex);
    void AddNewKeyframe(int32 iIndex);

private:
    //Events
    /* void OnFrameLengthChanged(TSharedPtr<SOdysseyFlipbookTimelineFrame> iFrame);
    FReply OnFrameListDrop(const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent);
    FReply OnFrameDrop(TSharedPtr<SOdysseyFlipbookTimelineFrame> iFrame, const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent, int32 iIndexOffset);
    FReply OnFrameDragDetected(TSharedPtr<SOdysseyFlipbookTimelineFrame> iFrame, const FGeometry& iGeometry, const FPointerEvent& iMouseEvent);
    bool OnFrameAcceptDrop(TSharedPtr<SOdysseyFlipbookTimelineFrame> iFrame, const FGeometry& iGeometry, const FDragDropEvent& iDragDropEvent);
    TSharedRef<SWidget> GenerateFrameContextMenu(TSharedPtr<SOdysseyFlipbookTimelineFrame> iFrame); */


    void OnFramesMoved(TArray<int32> iSrcIndexes, int32 iDstIndexes);
    void OnFramesLengthChanged(TArray<int32> iFrameIndexes);
    void OnFramesEditStart();
    void OnFramesEditStop();
    void OnFramesEditCancel();
    FReply OnGenerateFrameContextMenu(const FGeometry& iGeometry, const FPointerEvent& iMouseEvent, int32 iFrameIndex);

    void OpenSpritePickerMenu(FMenuBuilder& MenuBuilder, int32 iIndex);
    void CloseMenu();
    void OnAssetSelected(const FAssetData& AssetData, int32 iFrameIndex);
    void OnSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture);

    void OnRenderingChanged( const FOdysseyRenderingChangedEvent& iEvent );
    void RefreshTextureRenderTarget( UTexture2D* iTexture, UTextureRenderTarget2D* iRenderTarget );
    void CreateCheckerboardTexture();

private:
    UPaperFlipbook* mFlipbook;
    TUniquePtr<FOdysseyPainterEditorFlipbookListener> mListener;
    TAttribute<float>     mFrameSize;
    TAttribute<FOdysseyTextureConfiguration> mTextureConfiguration;
    TSharedPtr<SOdysseyFlipbookTimelineFrameList> mFrameList;
    TMap<UTexture2D*, TStrongObjectPtr<UTextureRenderTarget2D>> mRenderTargets;
    TMap<UTexture2D*, FSlateImageBrush*> mTextureBrushes;
    FSlateImageBrush* mCheckerboardBrush;
    TSharedPtr<class FUICommandList> mFrameCommandList; //List of commands we can use on a frame
    /* FOnStructureChanged mOnStructureChanged; */
    FOnFlipbookChanged mOnFlipbookChanged;
    FOnKeyframeRemoved mOnKeyframeRemoved;
    FOnKeyframeRemoved mOnKeyframeAdded;
    FOnSpriteCreated mOnSpriteCreated;
    FOnTextureCreated mOnTextureCreated;

    const FSlateBrush* mFrameWarningBrush;
};
