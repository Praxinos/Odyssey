// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

class FOdysseyPainterEditor;

/**
 * Implements an Editor toolkit for the Painter Editor.
 * The toolkit is the main entry point for the Painter Editor
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorToolkit
    : public FAssetEditorToolkit
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorToolkit();
    FOdysseyPainterEditorToolkit(const FName& iAppIdentifier);

public:
    void Init(TSharedPtr<FOdysseyPainterEditor> iEditor, UObject* iEditedObject);

public:
    virtual void AddEditingObject(UObject* Object);
    virtual void RemoveEditingObject(UObject* Object);

protected:
    // FAssetEditorToolkit interface
    virtual void SaveAssetAs_Execute() override;
    virtual bool OnRequestClose() override;
    virtual FText GetToolkitName() const override;
    virtual FText GetToolkitToolTipText() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;

	/** Called to check to see if there's an asset capable of being reimported */
	virtual bool CanReimport() const;
	virtual bool CanReimport(UObject* EditingObject) const;

    virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager) override;
    virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager) override;

protected:
	virtual void OpenAsset(UObject* iObject) = 0;
    virtual TArray<UObject*> GetAllEditedObjects();

private:
	void InitMenu();


private:
    TSharedPtr<FOdysseyPainterEditor> mEditor;

    FName mAppIdentifier;

protected:
    UObject* mEditedObject;
};
