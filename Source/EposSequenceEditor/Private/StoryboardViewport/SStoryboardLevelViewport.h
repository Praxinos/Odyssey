// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Types/SlateStructs.h"
#include "Layout/Visibility.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "LevelEditorViewport.h"
#include "Misc/FrameRate.h"
#include "Framework/Application/IInputProcessor.h"

#include "Tools/EposSequenceTools.h"

class ACineCameraActor;
class APlaneActor;
class FAssetEditorViewportLayout;
class FEposSequenceEditorToolkit;
class FLevelViewportLayout;
class FUICommandList;
class ILevelEditor;
class ISequencer;
class SBox;
class SFilmOverlayOptions;
class SStoryboardPreviewViewport;
class SStoryboardTransportRange;
class SEditorViewport;
class SSplitter;
class SLevelViewport;
class SNotesInViewport;
class SNotesAsOverlay;
class UStoryNote;

struct FQualifiedFrameTime;
struct FTypeInterfaceProxy;

/** Overridden level viewport client for this viewport */
struct FStoryboardViewportClient : FLevelEditorViewportClient
{
    FStoryboardViewportClient();

    void SetViewportWidget(const TSharedPtr<SEditorViewport>& InViewportWidget) { EditorViewportWidget = InViewportWidget; }
};


/** struct containing UI data - populated once per frame */
struct FUIData
{
    /** The name of the current shot */
    FText ShotName;
    /** The name of the current camera */
    FText CameraName;
    /** The name of the current shot's lens */
    FText Lens;
    /** The name of the current shot's filmback */
    FText Filmback;
    /** The text that represents the current playback time relative to the currently evaluated sequence. */
    FText LocalPlaybackTime;
    /** The text that represents the master start frame */
    FText MasterStartText;
    /** The text that represents the master end frame */
    FText MasterEndText;

    /** The text that represents the selected planes */
    TArray<FText> SelectedPlanes;

    /** The tick resolution of the master */
    FFrameRate OuterResolution;
    /** The play rate of the master */
    FFrameRate OuterPlayRate;
};

class FStoryboardLevelViewportInputProcessor
    : public IInputProcessor
{
public:
    const FKey& GetKey();

private:
    //needed to compile against IInputProcessor
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {};
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;

private:
    FKey mKey;
};

class SStoryboardLevelViewport
    : public SCompoundWidget
{
public:
    virtual ~SStoryboardLevelViewport();

public:

    SLATE_BEGIN_ARGS(SStoryboardLevelViewport) {}
        /** The unique name of this viewport inside its parent layout */
        SLATE_ARGUMENT(FName, LayoutName)
        /** Name of the viewport layout we should revert to at the user's request */
        SLATE_ARGUMENT(FName, RevertToLayoutName)
        /** Ptr to this viewport's parent layout */
        SLATE_ARGUMENT(TSharedPtr<FAssetEditorViewportLayout>, ParentLayout)
        /** Ptr to this viewport's parent level editor */
        SLATE_ARGUMENT(TWeakPtr<ILevelEditor>, ParentLevelEditor)
    SLATE_END_ARGS()

    /** Access this viewport's viewport client */
    TSharedPtr<FStoryboardViewportClient> GetViewportClient() const { return ViewportClient; }

    TSharedPtr<SLevelViewport> GetLevelViewport() const;

    /** Construct this widget */
    void Construct(const FArguments& InArgs);

    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& iMouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& iMouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& iMouseEvent) override;
    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
    virtual bool SupportsKeyboardFocus() const override { return true; }

private:

    /** Set up this viewport to operate on its sequencer */
    void Setup();

    /** Clean up this viewport after its sequencer has been closed */
    void CleanUp();

    void OnSequencerChanged();

private:

    /** Get the sequencer ptr from our current toolkit, if set */
    ISequencer* GetSequencer() const;

private:

    /** Cache the desired viewport size based on the filled, allotted geometry */
    void CacheDesiredViewportSize(const FGeometry& AllottedGeometry);

    /** Get the desired width and height of the viewport */
    FOptionalSize GetDesiredViewportWidth() const;
    FOptionalSize GetDesiredViewportHeight() const;

    int32 GetVisibleWidgetIndex() const;
    EVisibility GetControlsVisibility() const;

private:

    TOptional<double> GetMinTime() const;

    TOptional<double> GetMaxTime() const;

    void OnTimeCommitted(double Value, ETextCommit::Type);

    void SetTime(double Value);

    double GetTime() const;

    float GetPlayTimeMinDesiredWidth() const;

private:
    int32 GetScaleVisibleWidgetIndex() const;

    EVisibility GetMoveAndScalePlaneVisibility() const;

    float GetMoveAndScalePlaneDistance() const;
    void SetMoveAndScalePlaneDistance( float iDistance );

    int32 GetScalePlaneType() const;
    void OnScalePlaneTypeChanged( int32 iScalePlaneType, ESelectInfo::Type iSelectType );

    EVisibility GetCameraFocalLengthVisibility() const;

    float GetCameraFocalLength() const;
    void SetCameraFocalLength( float iFocalLength );

    float GetViewportRotation() const; // in degrees
    void SetViewportRotation( float iRotation ); // in degrees

    void AddViewportRotation( float iDeltaRotation ); // in degrees

    bool IsViewportRotationChecked( float iRotation ); // in degrees
    TSharedRef<SWidget> OnGetViewportRotationMenuContent() const;

    float GetViewportZoom() const;
    void SetViewportZoom( float iZoom, FVector2D iZoomPosition = FVector2D(0.0f, 0.0f) );
    void AddViewportZoom( float iDeltaZoom );
    void FitToScreen();
    TSharedRef<SWidget> OnGetViewportZoomMenuContent() const;
    bool IsViewportZoomChecked( float iZoom );

    FVector2D GetViewportPan() const;
    void SetViewportPan( FVector2D iPan );

    FSlateRenderTransform GetViewportTransform() const;
    void UpdateViewportWidgetTransform();
    FReply OnViewportResetTransformButtonClicked();
    void ResetViewportTransform();

    FVector2D GetTranslationFromSlidersOffsets(float InScrollOffsetFractionX, float InScrollOffsetFractionY);
    void UpdateScrollBars();
    void OnHorizontalScrollBarScrolled( float InScrollOffsetFraction );
    void OnVerticalScrollBarScrolled(float InScrollOffsetFraction);

    int GetToolbarIntegrationMode() const;

private:
    void CreateCommandList();
    void RegisterToolBarExtender();
    void UnregisterToolBarExtender();
    void ExtendToolBar(FToolBarBuilder& iBuilder);
    TSharedRef<SWidget> NoteSettingsGetMenuContent();

private:

    /** Widget where the scene viewport is drawn in */
    TSharedPtr<SStoryboardPreviewViewport> ViewportWidget;
    /** Widget where the grid option are and which contains the widget where grid is drawn in */
    TSharedPtr<SFilmOverlayOptions> FilmOverlayOptions;

    /** The sequencer we're currently editing */
    TWeakPtr<ISequencer> mCurrentSquencer;

    /** Commandlist used in the viewport (Maps commands to viewport specific actions) */
    TSharedPtr<FUICommandList> CommandList;

    /** Slot for transport controls */
    TSharedPtr<SBox> TransportControlsContainer, TimeRangeContainer;

    /** Decorated transport controls */
    TSharedPtr<SWidget> DecoratedTransportControls;

    /** Transport range widget */
    TSharedPtr<SStoryboardTransportRange> TransportRange;

    /** Cached UI data */
    FUIData UIData;

    /** Cached desired size of the viewport */
    FVector2D DesiredViewportSize;

    /** Viewport controls widget that sits just below, and matches the width, of the viewport */
    TSharedPtr<SWidget> ViewportControls;

    /** Weak ptr to our parent layout */
    TWeakPtr<FAssetEditorViewportLayout> ParentLayout;

    /** Name of the viewport in the parent layout */
    FName LayoutName;
    /** Name of the viewport layout we should revert to at the user's request */
    FName RevertToLayoutName;

    /** The time spin box */
    TSharedPtr<FTypeInterfaceProxy> TypeInterfaceProxy;

    /** The level editor viewport client for this viewport */
    TSharedPtr<FStoryboardViewportClient> ViewportClient;

    float mViewportRotation { 0.f };
    FVector2D mViewportPan { 0.f, 0.f };
    float mViewportZoom { 1.f };

    TSharedPtr<SSplitter> mNoteSplitter;

    TWeakObjectPtr<ACineCameraActor>    mCameraToFocalLength;
    TWeakObjectPtr<APlaneActor>         mPlaneToMove;
    EScalePlane                         mScalePlaneType { EScalePlane::kFitToCamera };
    TArray<TWeakObjectPtr<UStoryNote>>  mNotes;
    TSharedPtr<SNotesInViewport>        mWidgetNotesInViewport;
    TSharedPtr<SNotesAsOverlay>         mWidgetNotesAsOverlay;

    TSharedPtr<FStoryboardLevelViewportInputProcessor> mInputProcessor;

    //Rotation ---
    bool mIsRotating = false;
    FVector2D mRotateMouseInitialPosition;
    FVector2D mRotateCenter;
    float mRotateInitialMouseAngle;
    float mRotateInitialRotation;

    //Paning ---
    bool mIsPaning = false;
    FVector2D mPanMouseInitialPosition;
    FVector2D mPanInitialPan;

    //Zooming ---
    bool mIsZooming = false;
    FVector2D mZoomMouseInitialPosition;
    float mZoomInitialZoom;

    TSharedPtr<SScrollBar> mVerticalScrollBar;
    TSharedPtr<SScrollBar> mHorizontalScrollBar;
    bool mNeedUpdateScrollbars = true;

    TSharedPtr<SWidget> mViewportTransformBox;

    TSharedPtr<FExtender> mToolBarExtender;
};
