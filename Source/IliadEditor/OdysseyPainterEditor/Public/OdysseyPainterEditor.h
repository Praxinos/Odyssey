// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "TickableEditorObject.h"
#include "ToolMenuContext.h"

#include "OdysseyHUDElement.h"
#include "OdysseyEditorTab.h"
#include "OdysseyEditorShortcuts.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyVectorEngine.h"
#include "Proxies/OdysseyBrushColor.h"
#include "OdysseyPainterEditorAnimationTimelinePosition.h"
#include "OdysseyPainterEditorColorType.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyRenderingAbility.h"
#include <ULIS>

class IOdysseySurfaceEditable;
class UOdysseyPainterEditorTool;
class FOdysseyBrushContext;
class FOdysseyPainterEditorExtension;
class UOdysseyLayerStack;
class FOdysseyMeshSelector;
class UOdysseyTextureLayerStackUserData;
class UOdysseyPainterEditorRasterSelection;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorSegment;
class FOdysseyVectorPath;
class UOdysseyPalette;
class UOdysseyPaletteSet;
class UOdysseyPaletteEntryColor;
class FOdysseyPainterEditorFlipbookListener;
class UPaperSprite;
class UTexture2D;

class UOdysseyPainterEditorRasterDrawingTool;
class UOdysseyPainterEditorRasterEraserTool;
class UOdysseyPainterEditorRasterSelectionTool;
class UOdysseyPainterEditorRasterTransformTool;
class UOdysseyPainterEditorRasterPrimitiveDrawingTool;
class UOdysseyPainterEditorRasterLiquifyTool;
class UOdysseyPainterEditorRasterPaintBucketTool;
class UOdysseyPainterEditorVectorPrimitiveDrawingTool;
class UOdysseyPainterEditorVectorPathDrawingTool;
class UOdysseyPainterEditorVectorPathEditTool;
class UOdysseyPainterEditorVectorSelectionTool;
class UOdysseyPainterEditorVectorCutTool;
class UOdysseyPainterEditorVectorScenePanTool;
class UOdysseyPainterEditorVectorEraserTool;
class UOdysseyPainterEditorVectorPathPushTool;
class UOdysseyPainterEditorVectorPathSmoothTool;
class UOdysseyPainterEditorVectorPathStitchTool;
class UOdysseyPainterEditorVectorPaintBucketTool;
class UOdysseyPainterEditorColorPickerTool;
class UOdysseyPainterEditorVectorGridTool;
class UOdysseyPainterEditorVectorTransformTool;
class UOdysseyPainterEditorVectorMatchingTool;
class UOdysseyPainterEditorVectorChartTool;
class UOdysseyPainterEditorVectorTrajectoryTool;
class UOdysseyToolCollection;

class UOdysseyAnimation;
class UOdysseyAnimationPlayer;
class UOdysseyPainterEditorAnimationOutOfPegsTool;
class FOdysseyPainterEditorAnimationFlipSystem;

class UToolMenu;

/**
 * Base class for a Painting Editor
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditor
    : public FGCObject //Allows us to register External UObject in Garbage Collector
    , public FTickableEditorObject //Allows us to react to Tick events
    , public TSharedFromThis<FOdysseyPainterEditor>
{
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnAddEditedObject, UObject*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemoveEditedObject, UObject*);

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditor();
    FOdysseyPainterEditor(TSharedRef<FBaseToolkit> iToolkit);

public:
    const FName& GetId() const;
    UObject* GetEditedObject() const;

    void SetEditedObject(UObject* iObject);
    void OnFlipbookSpriteTextureChanged(UPaperSprite* iSprite, UTexture2D* iOldTexture);

    void AddEditedObject(UObject* iObject);
    void RemoveEditedObject(UObject* iObject);

    void AddTab(TSharedRef<FOdysseyEditorTab> iTab);
    template<class T> void RemoveTab();
    template<class T> TSharedPtr<T> FindTab() const;
    const TArray<TSharedPtr<FOdysseyEditorTab>>& GetTabs() const;
    void CloseAllTabs();
    void OnClose();

    void RegisterTabSpawners( const TSharedRef<FTabManager>& iTabManager );
    void UnregisterTabSpawners( const TSharedRef<FTabManager>& iTabManager );

    FOnAddEditedObject& OnAddEditedObjectDelegate();
    FOnRemoveEditedObject& OnRemoveEditedObjectDelegate();
    FSimpleDelegate& OnRegenerateToolbarAndMenus();

    FOdysseyEditorShortcuts& GetShortcuts();

public:
    // Overridable Methods
    void Initialize();
    TSharedRef<FTabManager::FLayout> CreateLayout(const FName& iLayoutName);
    void BindShortcuts(FBaseToolkit* iToolkit);
    void ExtendMenu( TSharedRef<FExtender> iExtender );
    void ExtendLevelEditorToolbar( UToolMenu* iToolbar );
    void ExtendAssetEditorToolbar( UToolMenu* iToolbar );
    TArray<UObject*> GetAdditionalEditedObjects();
    void InitToolMenuContext(FToolMenuContext& MenuContext);

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

    // FTickableEditorObject implementation
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT( FOdysseyPainterEditor, STATGROUP_Tickables); }

public:
    //Tools
    void ExtendToolbarSaveAssetButton(UToolMenu* iToolMenu);
    void ExtendToolbarToolParameters(UToolMenu* iToolMenu);

    /**
     * @brief Returns the current main tool
     */
    virtual UOdysseyPainterEditorTool* GetCurrentMainTool() const;

    /**
     * @brief Returns the current temporary tool
     */
    virtual UOdysseyPainterEditorTool* GetCurrentTemporaryTool() const;

    /**
     * @brief Returns the current tool (main or temporary)
     */
    virtual UOdysseyPainterEditorTool* GetCurrentTool() const;

    /**
     * @brief Returns the recent tools as a tool collection
     */
    virtual UOdysseyToolCollection* GetRecentTools() const;

    /**
     * @brief Inactivates all tools
     */
    void InactivateAllTools();

    /**
     * @brief Inactivates the current main tool
     * and activates the main tool (if any)
     */
    void InactivateMainTool();

    /**
     * @brief Activates a main tool over the main tool
     */
    void ActivateMainTool( UOdysseyPainterEditorTool* iTool );

    /**
     * @brief Inactivates the current temporary tool
     * and activates the main tool (if any)
     */
    void InactivateTemporaryTool();

    /**
     * @brief Activates a temporary tool over the main tool
     */
    void ActivateTemporaryTool( UOdysseyPainterEditorTool* iTool );

    /**
     * @brief Ensures the current active tool is activable
     * And activates the first activable tool available if needed
     */
    void SanitizeCurrentTool();

    /**
     * @brief Saves iTool to mRecentTools (if not already in it)
     * mRecentTools can't be bigger than 10 tools (most ancient tools are discarded if necessary)
     */
    void SaveMainToolToRecentTools();

    virtual UOdysseyPainterEditorRasterDrawingTool*                  GetRasterDrawingTool() const;
    virtual UOdysseyPainterEditorRasterEraserTool*                   GetRasterEraserTool() const;
    virtual UOdysseyPainterEditorRasterSelectionTool*                GetRasterSelectionTool() const;
    virtual UOdysseyPainterEditorRasterTransformTool*                GetRasterTransformTool() const;
    virtual UOdysseyPainterEditorRasterPrimitiveDrawingTool*         GetRasterPrimitiveDrawingTool() const;
    virtual UOdysseyPainterEditorRasterLiquifyTool*                  GetRasterLiquifyTool() const;
    virtual UOdysseyPainterEditorRasterPaintBucketTool*              GetRasterPaintBucketTool() const;
    virtual UOdysseyPainterEditorVectorPrimitiveDrawingTool*         GetVectorPrimitiveDrawingTool() const;
    virtual UOdysseyPainterEditorVectorPathDrawingTool*              GetVectorPathDrawingTool() const;
    virtual UOdysseyPainterEditorVectorPathEditTool*                 GetVectorPathEditTool() const;
    virtual UOdysseyPainterEditorVectorSelectionTool*                GetVectorSelectionTool() const;
    virtual UOdysseyPainterEditorVectorCutTool*                      GetVectorCutTool() const;
    virtual UOdysseyPainterEditorVectorGridTool*                     GetVectorGridTool() const;
    virtual UOdysseyPainterEditorVectorTransformTool*                GetVectorTransformTool() const;
    virtual UOdysseyPainterEditorVectorMatchingTool*                 GetVectorMatchingTool() const;
    virtual UOdysseyPainterEditorVectorChartTool*                    GetVectorChartTool() const;
    virtual UOdysseyPainterEditorVectorTrajectoryTool*               GetVectorTrajectoryTool() const;
    virtual UOdysseyPainterEditorVectorScenePanTool*                 GetVectorScenePanTool() const;
    virtual UOdysseyPainterEditorVectorEraserTool*                   GetVectorEraserTool() const;
    virtual UOdysseyPainterEditorVectorPathPushTool*                 GetVectorPathPushTool() const;
    virtual UOdysseyPainterEditorVectorPathSmoothTool*               GetVectorPathSmoothTool() const;
    virtual UOdysseyPainterEditorVectorPathStitchTool*               GetVectorPathStitchTool() const;
    virtual UOdysseyPainterEditorVectorPaintBucketTool*              GetVectorPaintBucketTool() const;
    virtual UOdysseyPainterEditorColorPickerTool*                    GetColorPickerTool() const;

    UOdysseyPainterEditorAnimationOutOfPegsTool*    GetOutOfPegsTool() const;
    UOdysseyPainterEditorColorPickerTool*           GetTemporaryColorPickerTool() const;

public:
    // Getters
    FSimpleMulticastDelegate& OnSourceChanged();
    FSimpleMulticastDelegate& OnCurrentToolChanged();
    FSimpleMulticastDelegate& OnCurrentMainToolChanged();
    FSimpleMulticastDelegate& OnCurrentTemporaryToolChanged();

    TSharedPtr<FBaseToolkit> GetToolkit() const;
    TSharedPtr<FOdysseyPainterEditorSource>              GetSource() const;
    template<class T> TSharedPtr<T> GetSourceTyped() const;

    virtual TSharedPtr<FOdysseyHUDElement>                              HUDSystem() const;
    TSharedPtr<FOdysseyHUDElement> GetToolsHUD() const;
    TSharedPtr<FOdysseyHUDElement> GetRasterSelectionHUD() const;
    virtual const FOdysseyBrushColor&                        PaintColor() const;

    UOdysseyAnimation*                                       GetAnimation() const;
    UOdysseyTextureLayerStackUserData*                       GetTextureUserData() const;
    UOdysseyAnimationPlayer* GetAnimationPlayer() const;
    TSharedPtr<FOdysseyPainterEditorAnimationFlipSystem>            GetAnimationFlipSystem() const;
    TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition>      GetAnimationTimelinePosition();
    EOdysseyPainterEditorColorType                           GetColorType() const;
    virtual FOdysseyMediaProvider                            GetCurrentMediaProvider();
    virtual UOdysseyLayerStack*                              LayerStack() const;
    virtual TObjectPtr<UOdysseyPainterEditorRasterSelection> RasterSelection();
    int GetCurrentFrame() const;

    const TArray<UOdysseyPaletteSet*> GetPaletteSets() const;
    //const FOdysseyPainterEditorPaletteEntryColor& GetPaletteCurrentColorEntry() const;
    UOdysseyPaletteEntryColor* GetCurrentPaletteColorEntry() const;
    FGuid GetCurrentPaletteSet() const;

    void AddPaletteSet(UOdysseyPalette* iPalette);
    void RemovePaletteSet(UOdysseyPaletteSet* iPaletteSet);

    void SetPaletteSet(FGuid iIndex, UOdysseyPaletteSet* iPaletteSet);
    void SetCurrentPaletteColorEntry(UOdysseyPaletteEntryColor* iEntry, FGuid iSet);

    void SetColorType(EOdysseyPainterEditorColorType iType);

    TSharedPtr<FOdysseyMeshSelector>                        GetMeshSelector() const;

    TArray<FOdysseyBrushContext*>& GetBrushContexts();

    void SetVectorHUDFlags( uint64 iVectorHUDFlags );
    uint64 GetVectorHUDFlags();

    void SetVectorDrawingFlags(uint64 iVectorDrawingFlags);
    uint64 GetVectorDrawingFlags();

    // generic reusable vector methods.
    static void BringForward( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void SendBackward( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void ApplyTransformations( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void MakePaintGroup( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void Ungroup( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void GroupAndAddInbetweenerTag( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void Group( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static FOdysseyVectorGroup* _Group( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void SelectAllPoints( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void SelectAllObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void ResetView( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void LockPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void UnlockPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void UnalignPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void AlignPointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void DeletePointSelectionRecursive( FOdysseyPainterEditor* iEditor
                                             , FOdysseyVectorObject* iVectorObject
                                             , std::vector<FOdysseyVectorVertex*>& oRemovedVertexArray
                                             , std::vector<FOdysseyVectorSegment*>& oRemovedSegmentArray
                                             , std::vector<FOdysseyVectorPath*>& oRemovedPathArray
                                             , std::vector<FOdysseyVectorSegment*>& oAddedSegmentArray );
    static void DeletePointSelection( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void DeleteObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void FlipHorizontal( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void ClearColoring( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void FlipVertical( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void DeleteBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket );
    static void PropagateBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket );
    static void UnpropagateBucket( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket );
    static void CopyObjects( FOdysseyVectorGroupPaint* iScene );
    static void PasteObjects( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void StitchVertices( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene
                              , FOdysseyVectorVertex* iVertexA
                              , FOdysseyVectorVertex* iVertexB );
    static void CopyTransformation( FOdysseyVectorGroupPaint* iScene );
    static void PasteTransformation( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void MergeScenes( FOdysseyVectorGroupPaint* iDestinationScene
                           , const TArray<FOdysseyVectorGroupPaint*>& iSourceSceneArray );
    static void AlterContourWidth( FOdysseyVectorGroupPaint* iScene
                                 , double iValue
                                 , bool   iAbsolute );
    static void AddInbetweenerTag( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void RemoveInbetweenerTag( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void ResetSpacingChart( FOdysseyPainterEditor* iEditor
                                 , FOdysseyVectorGroupPaint* iScene
                                 , bool iResetPositionning
                                 , bool iCurrentBreakdownOnly );
    static void ResetInbetweenerTagSpacingChart( FOdysseyPainterEditor* iEditor, FOdysseyVectorLayer* iSharedEnv );
    static void CommitSelectedInbetweenerTag( FOdysseyPainterEditor* iEditor, FOdysseyVectorLayer* iSharedEnv );
    static void CopySpacingChart( FOdysseyPainterEditor* iEditor
                                , FOdysseyVectorGroupPaint* iScene
                                , bool iCurrentBreakdownOnly );
    static void PasteSpacingChart( FOdysseyPainterEditor* iEditor
                                 , FOdysseyVectorGroupPaint* iScene
                                 , bool iCurrentBreakdownOnly );
    static void ResetInbetweenerGrid( FOdysseyPainterEditor* iEditor
                                    , FOdysseyVectorGroupPaint* iScene
                                    , bool iResetTransformation
                                    , bool iResetDeformation );
    static void RemoveInbetweenerTag( FOdysseyPainterEditor* iEditor
                                    , FOdysseyVectorLayer* iSharedEnv );
    static void Subdivide( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void PasteInbetweenerGrid( FOdysseyPainterEditor* iEditor, FOdysseyVectorGroupPaint* iScene );
    static void CopyInbetweenerGrid( FOdysseyVectorGroupPaint* iScene );

public:
    static const uint64 UI_UPDATE_SCENETREEVIEW = ( 1ULL << ( FOdysseyVectorEngine::NOTIFY_RESERVED_SHIFT + 0 ) );
    static const uint64 UI_UPDATE_TIMELINE      = ( 1ULL << ( FOdysseyVectorEngine::NOTIFY_RESERVED_SHIFT + 1 ) );
    static const uint64 UI_UPDATE_OBJECTDETAILS = ( 1ULL << ( FOdysseyVectorEngine::NOTIFY_RESERVED_SHIFT + 2 ) );

    // Utility functions
    bool HasCopyBlockClipboard(); //Did we copied a selection inside a block ? (Ctrl + C)

    // Populates the Edit Menu everytime it is displayed
    void AddEditMenuEntry( FMenuBuilder& iMenuBuilder );

public:
    // Setters
    void  AddExtension(TSharedPtr<FOdysseyPainterEditorExtension> iExtension);
    void  SetSource(TSharedPtr<FOdysseyPainterEditorSource> iSource);
    void  PaintColor(const FOdysseyBrushColor& iColor, bool iIsCommit);
    template <class T> T* AddMainTool();
    template <class T> T* AddTemporaryTool();

    /** Get the main tool associated to class in parameter */
    UOdysseyPainterEditorTool* GetEditorToolOfClass(UClass* iToolClass);

protected:
    //Callbacks
    virtual void OnApplyOverrides(const TMap<FName, UObject*>& iOverrides);
    void OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack);
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);

private:
    void InitHUD();
    void InitTools();
    void InitTabs();
    void InitShortcuts();

    //TODO: Move these shortcuts in FOdysseyAnimationGlobalShortcuts files
    void SwitchTabletAPI();
    void ClearCurrentLayerOrSelection();
    void ToggleEraserButton();

    bool IsToolsHUDVisible() const;
    bool IsRasterSelectionHUDVisible() const;

    UOdysseyPainterEditorTool* FindDefaultToolForCurrentLayer();

protected:
    TWeakPtr<FBaseToolkit> mToolkit;
    FName mName;
    UObject* mEditedObject = nullptr;
    TArray<TSharedPtr<FOdysseyEditorTab>> mTabs;
    TArray<UObject*> mAdditionalEditedObjects;
    FString mTabsSaveFilename;
    TSharedPtr<FOdysseyPainterEditorFlipbookListener> mFlipbookListener;

    FOnAddEditedObject mOnAddEditedObject;
    FOnRemoveEditedObject mOnRemoveEditedObject;
    FSimpleDelegate mOnRegenerateToolbarAndMenus;

    FOdysseyEditorShortcuts mShortcuts;
    TSharedPtr<FTabManager::FLayout>         mLayout;
    static TSharedPtr<::ULIS::FBlock>        mCopyBlock; // Pixel block in clipboard (ctrl + c, ctrl + v)

    //Tools
    TSharedPtr<FOdysseyPainterEditorSource>  mSource;
    TSharedPtr<FOdysseyMeshSelector>         mMeshSelector;
    TArray<TSharedPtr<FOdysseyPainterEditorExtension>> mExtensions;
    UOdysseyPainterEditorTool*               mCurrentMainTool;
    UOdysseyPainterEditorTool*               mCurrentTemporaryTool;

    TArray<TObjectPtr<UOdysseyPainterEditorTool>> mMainTools;
    TArray<TObjectPtr<UOdysseyPainterEditorTool>> mTemporaryTools;

    uint64                          mVectorHUDFlags;
    uint64                          mVectorDrawingFlags;

    TSharedPtr<FOdysseyHUDElement>  mHUDSystem;
    TSharedPtr<FOdysseyHUDElement>  mToolsHUD;
    TSharedPtr<FOdysseyHUDElement>  mRasterSelectionHUD;
    TObjectPtr<UOdysseyPainterEditorRasterSelection> mRasterSelection;
    TArray<FOdysseyBrushContext*>   mBrushContexts;
    FOdysseyBrushColor              mPaintColor;
    TSharedRef<FOdysseyPainterEditorAnimationTimelinePosition> mAnimationTimelinePosition;
    EOdysseyPainterEditorColorType  mColorType = EOdysseyPainterEditorColorType::Raw;
    FSimpleMulticastDelegate        mOnCurrentToolChanged;
    FSimpleMulticastDelegate        mOnCurrentMainToolChanged;
    FSimpleMulticastDelegate        mOnCurrentTemporaryToolChanged;
    FSimpleMulticastDelegate        mOnSourceChanged;

    TObjectPtr<UOdysseyPainterEditorRasterDrawingTool> mRasterDrawingTool;
    TObjectPtr<UOdysseyPainterEditorRasterEraserTool> mRasterEraserTool;
    TObjectPtr<UOdysseyPainterEditorRasterSelectionTool> mRasterSelectionTool;
    TObjectPtr<UOdysseyPainterEditorRasterTransformTool> mRasterTransformTool;
    TObjectPtr<UOdysseyPainterEditorRasterPrimitiveDrawingTool> mRasterPrimitiveDrawingTool;
    TObjectPtr<UOdysseyPainterEditorRasterLiquifyTool> mRasterLiquifyTool;
    TObjectPtr<UOdysseyPainterEditorRasterPaintBucketTool> mRasterPaintBucketTool;
    TObjectPtr<UOdysseyPainterEditorVectorPrimitiveDrawingTool> mVectorPrimitiveDrawingTool;
    TObjectPtr<UOdysseyPainterEditorVectorPathDrawingTool> mVectorPathDrawingTool;
    TObjectPtr<UOdysseyPainterEditorVectorPathEditTool> mVectorPathEditTool;
    TObjectPtr<UOdysseyPainterEditorVectorSelectionTool> mVectorSelectionTool;
    TObjectPtr<UOdysseyPainterEditorVectorCutTool> mVectorCutTool;
    TObjectPtr<UOdysseyPainterEditorVectorScenePanTool> mVectorScenePanTool;
    TObjectPtr<UOdysseyPainterEditorVectorEraserTool> mVectorEraserTool;
    TObjectPtr<UOdysseyPainterEditorVectorPathPushTool> mVectorPathPushTool;
    TObjectPtr<UOdysseyPainterEditorVectorPathSmoothTool> mVectorPathSmoothTool;
    TObjectPtr<UOdysseyPainterEditorVectorPathStitchTool> mVectorPathStitchTool;
    TObjectPtr<UOdysseyPainterEditorVectorPaintBucketTool> mVectorPaintBucketTool;
    TObjectPtr<UOdysseyPainterEditorColorPickerTool> mColorPickerTool;
    TObjectPtr<UOdysseyPainterEditorVectorGridTool> mVectorGridTool;
    TObjectPtr<UOdysseyPainterEditorVectorTransformTool> mVectorTransformTool;
    TObjectPtr<UOdysseyPainterEditorVectorMatchingTool> mVectorMatchingTool;
    TObjectPtr<UOdysseyPainterEditorVectorChartTool> mVectorChartTool;
    TObjectPtr<UOdysseyPainterEditorVectorTrajectoryTool> mVectorTrajectoryTool;

    TObjectPtr<UOdysseyPainterEditorAnimationOutOfPegsTool> mOutOfPegsTool;
    TObjectPtr<UOdysseyPainterEditorColorPickerTool> mTemporaryColorPickerTool;

    TMap<UClass*, UOdysseyPainterEditorTool*> mCurrentMainToolPerLayerClass;

    UOdysseyToolCollection* mRecentTools;

    FName mToolbarMenuName;

    //Local editor data, for convenience. Palettes and sets are stored in TextureData and Animation. Colors are stored in vector objects.
    TObjectPtr<UOdysseyPaletteEntryColor> mCurrentPaletteEntryColor;
    FGuid mCurrentPaletteSet = FGuid();

    TSharedPtr<FOdysseyPainterEditorAnimationFlipSystem> mAnimationFlipSystem;
    TArray<FGuid> mImageRenderingComposition;
    bool mAnimationTimelineIsScrubbing = false;

};

template <class T>
T* FOdysseyPainterEditor::AddMainTool()
{
    T* tool = NewObject<T>();
    tool->SetEditor(this);
    mMainTools.Add(tool);
    return tool;
}

template <class T>
T* FOdysseyPainterEditor::AddTemporaryTool()
{
    T* tool = NewObject<T>();
    tool->SetEditor(this);
    mTemporaryTools.Add(tool);
    return tool;
}

template<class T>
void
FOdysseyPainterEditor::RemoveTab()
{
    TSharedPtr<FOdysseyEditorTab> tab = FindTab<T>();
    if (!tab)
        return;

    mTabs.RemoveSingle(tab);
}

template<class T>
TSharedPtr<T>
FOdysseyPainterEditor::FindTab() const
{
    const FName& id = T::StaticId();
    for (const TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        if (tab->GetId() == id)
            return StaticCastSharedPtr<T>(tab);
    }
    return nullptr;
}

template<class T>
TSharedPtr<T>
FOdysseyPainterEditor::GetSourceTyped() const
{
    TSharedPtr<FOdysseyPainterEditorSource> source = GetSource();
    if (!source || source->Id() != T::StaticId())
        return nullptr;

    return StaticCastSharedPtr<T>(source);
}
