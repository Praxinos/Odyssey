// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

class UArianeEditorTool;
class FModeToolkit;
class FArianeEditorTab;
class FTabManager;
class UWorld;

/**
 * Base class for a Painting Editor
 */
class ARIANEEDITOR_API FArianeEditor
    : public FGCObject //Allows us to register External UObject in Garbage Collector
{
    public:
        DECLARE_MULTICAST_DELEGATE( FOnPreChangeCurrentTool );
        DECLARE_MULTICAST_DELEGATE( FOnPostChangeCurrentTool );

    public:
        ~FArianeEditor();
        FArianeEditor( FModeToolkit* iToolkit );

        FModeToolkit* GetToolkit();
        const TArray<UArianeEditorTool*>& GetTools();
        UArianeEditorTool* GetCurrentTool();
        void SetCurrentTool( UArianeEditorTool* iTool );
        const TArray<TSharedPtr<FArianeEditorTab>>& GetTabs() const;
        void Init();
        const FName& GetId() const;
        void RegisterTabSpawners( const TSharedRef< FTabManager >& iTabManager );
        void UnregisterTabSpawners( const TSharedRef< FTabManager >& iTabManager );
        void CloseAllTabs();
        void AddPainting3DComponent( const TArray<class AActor *> iActors );
        void AddPainting3DActor();
        UWorld* GetWorld();

        // FGCObject overrides
        virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
        virtual FString GetReferencerName() const override;

        FOnPreChangeCurrentTool& OnPreChangeCurrentToolDelegate();
        FOnPostChangeCurrentTool& OnPostChangeCurrentToolDelegate();

    protected:
        void AddTool( UArianeEditorTool* iTool );
        void RemoveTool( UArianeEditorTool* iTool );
        void InitTools();
        void AddTab( TSharedPtr<FArianeEditorTab> iTab );
        void RemoveTab( TSharedPtr<FArianeEditorTab> iTab );
        void InitTabs();




    protected:
        FOnPreChangeCurrentTool OnPreChangeCurrentTool;
        FOnPostChangeCurrentTool OnPostChangeCurrentTool;

        FModeToolkit* mToolkit;
        FName mName;
        TArray<UArianeEditorTool*> mTools;
        TArray<TSharedPtr<FArianeEditorTab>> mTabs;
        UArianeEditorTool* mCurrentTool;
};
