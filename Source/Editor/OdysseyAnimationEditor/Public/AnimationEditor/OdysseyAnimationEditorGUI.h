// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class FOdysseyAnimationEditorExtension;
class FOdysseyAnimationEditorLightTableTab;
class FOdysseyAnimationEditorLayerStackTab;

/**
 * Implements an Editor toolkit for textures.
 */
class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditorGUI
{
public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditorGUI();
    FOdysseyAnimationEditorGUI(FOdysseyAnimationEditorExtension* iExtension);

protected:
    //Init
	void CreateTabs();

public:
    // GettersFName
    TSharedPtr<FOdysseyAnimationEditorLayerStackTab>& GetLayerStackTab();
    TSharedPtr<FOdysseyAnimationEditorLightTableTab>& GetLightTableTab();

protected:
	//virtual TSharedRef<FTabManager::FSplitter>	CreateBottomSection() override;

private:
    FOdysseyAnimationEditorExtension* mExtension;

    //Tabs
    TSharedPtr<FOdysseyAnimationEditorLayerStackTab> mLayerStackTab;
    TSharedPtr<FOdysseyAnimationEditorLightTableTab> mLightTableTab;
};
