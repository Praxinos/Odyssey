// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class FOdysseyAnimationEditorExtension;
class FOdysseyAnimationEditorLightTableTab;
class FOdysseyAnimationEditorTimelineTab;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditorGUI
{
public:
    static void ExtendLevelEditorLayout(FLayoutExtender& Extender);
    
public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditorGUI();
    FOdysseyAnimationEditorGUI(FOdysseyAnimationEditorExtension* iExtension);

public:
    void Initialize();
    void Finalize();
	void BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder);
    void OnVectorSceneSignal( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags );
    void OnCurrentFrameChanged( UOdysseyAnimation* iAnimation );
    void ParseVectorSignal( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags );
    void OnSourceChanged();

protected:
    //Init
	void CreateTabs();

public:
    // GettersFName
    TSharedPtr<FOdysseyAnimationEditorTimelineTab>& GetTimelineTab();
    TSharedPtr<FOdysseyAnimationEditorLightTableTab>& GetLightTableTab();

protected:
	//virtual TSharedRef<FTabManager::FSplitter>	CreateBottomSection() override;

private:
    FOdysseyAnimationEditorExtension* mExtension;

    //Tabs
    TSharedPtr<FOdysseyAnimationEditorTimelineTab> mTimelineTab;
    TSharedPtr<FOdysseyAnimationEditorLightTableTab> mLightTableTab;
};
