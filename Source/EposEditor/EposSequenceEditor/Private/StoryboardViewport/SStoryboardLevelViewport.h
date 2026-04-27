// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Subsystems/PanelExtensionSubsystem.h"

#include "StoryboardViewport/StoryboardVisibleArea.h"
#include "StoryboardViewport/StoryboardViewportZoomController.h"
#include "Tools/EposSequenceTools.h"

class ACineCameraActor;
class FAssetEditorViewportLayout;
class FEposSequenceEditorToolkit;
class FLevelViewportLayout;
class FUICommandList;
class ILevelEditor;
class ISequencer;
class SBox;
class SFilmOverlay;
class SFilmOverlayOptions;
class SStoryboardPreviewViewport;
class SStoryboardTransportRange;
class SEditorViewport;
class SSplitter;
class SLevelViewport;
class SNotesInViewport;
class SNotesAsOverlay;
class UStoryNote;
class SStoryboardViewportSettings;

struct FQualifiedFrameTime;
struct FTypeInterfaceProxy;

struct FStoryboardViewportGeometry
{
    FBox2D CameraBounds = FBox2D(FVector2D::ZeroVector, FVector2D::ZeroVector);
    FVector2D WidgetSize = FVector2D::ZeroVector;
    float WidgetDPIScale = 1.f;
    FIntPoint VirtualSize = FIntPoint::ZeroValue;
};

/** Overridden level viewport client for this viewport */
class FStoryboardLevelViewportClient : public FLevelEditorViewportClient
{
public:
    ~FStoryboardLevelViewportClient();
    FStoryboardLevelViewportClient();

public:
    virtual FSceneView* CalcSceneView(FSceneViewFamily* ViewFamily, const int32 StereoViewIndex) override;
    virtual void DrawCanvas(FViewport& InViewport, FSceneView& View, FCanvas& Canvas) override;

public:
    const FStoryboardViewportGeometry& GetViewportGeometry() const;

    void SetViewportWidget(const TSharedPtr<SEditorViewport>& InViewportWidget) { EditorViewportWidget = InViewportWidget; }
    void SetStoryboardLevelViewport(TSharedPtr<class SStoryboardLevelViewport> iStoryboardViewport) { StoryboardViewportWidget = iStoryboardViewport; };

    virtual FMatrix CalcViewRotationMatrixForControllingActorView(const FRotator& InViewRotation) const override;
    virtual FMatrix CalcViewRotationMatrix(const FRotator& InViewRotation) const override;

    FVector2D GetNormalizedOffset() const;
    void UpdateCameraBounds();
    const FStoryboardVisibleArea& GetZoomedVisibleArea() const;
    float GetDefaultFOV() const;
    float GetFOV() const;

    FStoryboardViewportZoomController& GetZoomController();

    bool IsPerspectiveViewportCameraCutEnabled() const;

private:
    TWeakPtr<class SStoryboardLevelViewport> StoryboardViewportWidget;
    FStoryboardViewportGeometry ViewportGeometry;

    FStoryboardVisibleArea CachedVisibleArea;
    FStoryboardVisibleArea CachedZoomedVisibleArea;
    FStoryboardViewportZoomController mZoomController;
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

    /** The text that represents the selected animations */
    TArray<FText> SelectedAnimations;

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

        SLATE_ARGUMENT( TWeakPtr<SStoryboardPreviewViewport>, PreviewViewport )
    SLATE_END_ARGS()

    /** Access this viewport's viewport client */
    TSharedPtr<FStoryboardLevelViewportClient> GetViewportClient() const { return ViewportClient; }

    TSharedPtr<SLevelViewport> GetLevelViewport() const;

    /** Construct this widget */
    void Construct(const FArguments& InArgs);

    virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime);
    virtual FReply OnPreviewMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& iMouseEvent) override;
    virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& iMouseEvent) override;
    virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& iMouseEvent) override;
    virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent) override;
    virtual bool SupportsKeyboardFocus() const override { return true; }

    const TSharedPtr<SFilmOverlay>& GetOverlayWidget()
    {
        return OverlayWidget;
    }

    TSharedPtr<FUICommandList> GetCommandList()
    {
        return CommandList;
    }
private:

    /** Set up this viewport to operate on its sequencer */
    void Setup();

    /** Clean up this viewport after its sequencer has been closed */
    void CleanUp();

    void OnSequencerChanged();

private:

    friend FStoryboardLevelViewportClient;

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

public:
    int32 GetScaleVisibleWidgetIndex() const;

    EVisibility GetMoveAndScaleActorVisibility() const;

    float GetMoveAndScaleActorDistance() const;
    void SetMoveAndScaleActorDistance( float iDistance );

    template<typename T> void HideAllActors();
    template<typename T> void ShowAllActors();
    void OnPickEditorModeChanged( const FEditorModeID& iMode, bool bIsEntering );

    void OnGetAllowedClassesForActorDistance( TArray<const UClass*>& ioAllowedClasses );
    bool OnShouldFilterActorForActorDistance( const AActor* const iActor );
    void OnActorSelectedForActorDistance( AActor* ioActor );

    TOptional<FConvexVolume> GetCameraFrustum() const;
    TSharedRef<SWidget> OnActorPickerListMenuContent();

    int32 GetScaleActorType() const;
    void OnScaleActorTypeChanged( int32 iScaleActorType, ESelectInfo::Type iSelectType );
    FReply OnFitActorToCameraView();

    EVisibility GetCameraFocalLengthVisibility() const;

    float GetCameraFocalLength() const;
    void SetCameraFocalLength( float iFocalLength );

    void AddViewportRotation( float iDeltaRotation ); // in degrees

    bool IsViewportRotationChecked( float iRotation ); // in degrees
    TSharedRef<SWidget> OnGetViewportRotationMenuContent() const;

    void AddViewportZoom( float iDeltaZoom );
    TSharedRef<SWidget> OnGetViewportZoomMenuContent() const;
    bool IsViewportZoomChecked( float iZoom );

    FReply OnViewportResetTransformButtonClicked();

    FVector2D GetTranslationFromSlidersOffsets(float InScrollOffsetFractionX, float InScrollOffsetFractionY);
    void UpdateScrollBars();
    void OnHorizontalScrollBarScrolled( float InScrollOffsetFraction );
    void OnVerticalScrollBarScrolled(float InScrollOffsetFraction);

private:
    void CreateCommandList();

    TSharedRef<SWidget> NoteSettingsGetMenuContent();

public: // Needed by SStoryboardPreviewViewport
//private:
    FText GetRotationLabel();
    FText GetRotationTooltip();
    TSharedRef<SWidget> CreateRotationWidget();
    FText GetZoomLabel();
    FText GetZoomTooltip();
    TSharedRef<SWidget> CreateZoomWidget();

private:

    /** Widget where the scene viewport is drawn in */
    TSharedPtr<SStoryboardPreviewViewport> ViewportWidget;

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
    TSharedPtr<FStoryboardLevelViewportClient> ViewportClient;

    /** The overlay widget */
    TSharedPtr<SFilmOverlay> OverlayWidget;

    TSharedPtr<SSplitter> mNoteSplitter;

    TWeakObjectPtr<ACineCameraActor>    mCameraToFocalLength;
    TWeakObjectPtr<AActor>              mActorToMove;
    TSharedPtr<SWidget>                 mActorInteractivePickerWidget;
    bool                                mStartStoryboardActorPicking = false;
    TArray<AActor*>                     mActorsTemporaryHidden;
    TSharedPtr<SComboButton>            mActorPickerComboList;
    EScaleActor                         mScaleActorType = EScaleActor::kRelativeScale;
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
};
