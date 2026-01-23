// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

class UArianeEditorTool;
class FModeToolkit;
class FArianeEditorTab;
class FTabManager;

/**
 * Base class for a Painting Editor
 */
class ARIANEEDITOR_API FArianeEditor
{
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

    protected:
        void AddTool( UArianeEditorTool* iTool );
        void RemoveTool( UArianeEditorTool* iTool );
        void InitTools();
        void AddTab( TSharedPtr<FArianeEditorTab> iTab );
        void RemoveTab( TSharedPtr<FArianeEditorTab> iTab );
        void InitTabs();




    protected:
        FModeToolkit* mToolkit;
        FName mName;
        TArray<UArianeEditorTool*> mTools;
        TArray<TSharedPtr<FArianeEditorTab>> mTabs;
        UArianeEditorTool* mCurrentTool;
};
