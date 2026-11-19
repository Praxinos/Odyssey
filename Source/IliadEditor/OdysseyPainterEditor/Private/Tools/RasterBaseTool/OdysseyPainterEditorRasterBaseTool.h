// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyPainterEditorRasterBaseTool.generated.h"

class FOdysseyMediaRaster;
class FOdysseyRasterBlockEdit;
class ISinglePropertyView;

//Base tool to create options that are common to all raster tools
UCLASS(Abstract, HideDropdown)
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterBaseTool : public UOdysseyPainterEditorTool
{
public:
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorRasterBaseTool();

    //Constructor
    UOdysseyPainterEditorRasterBaseTool();

    static bool DoubleClicked();

    TSharedPtr<SWidget> CreatePropertyWidget( TSharedPtr<class IPropertyHandle> iPropertyHandle
                                            , const TSharedPtr<ISinglePropertyView> iView );
    virtual bool OnKeyDown( const FKey& iKey ) override;
    virtual bool OnKeyUp( const FKey& iKey ) override;
    virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
    virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture ) override;
    virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture ) override;
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
    virtual void ExtendMenu(TSharedRef<FExtender> iExtender) override;
    virtual void ExtendContextMenu( FMenuBuilder& menu, const FOdysseyPoint& iPointInTexture, const FKey& iKey );
    virtual void BindShortcuts( TSharedPtr<FUICommandList> iCommandList ) override;
    virtual bool UsesRasterSelection() const override;

protected:
    /**
     * Returns true when the edit is active and edits the content of the media,
     * e.g. false once the current layer changed or the edit ended by itself.
     */
    static bool IsEditing(const TSharedPtr<FOdysseyRasterBlockEdit>& iEdit, const TSharedPtr<FOdysseyMediaRaster>& iMediaRaster);

    /**
     * Returns the raster content of the current layer or cell.
     * @param iCreate True to create the content when missing, e.g. the cell of the current frame.
     * @return Null when there is no raster content, or it is locked.
     */
    TSharedPtr<FOdysseyMediaRaster> GetMediaRasterFromEditor(bool iCreate) const;

    void SelectAll();
    void ClearCurrentSelection();
    void InvertSelection();

    void CopyCurrentSelection();
    void CutCurrentSelection();
    void PasteCurrentSelection();
    void PasteCurrentSelectionInNewLayer();

    bool CanCopyCurrentSelection();
    bool CanCutCurrentSelection();
    bool CanPasteCurrentSelection();
    bool CanPasteCurrentSelectionInNewLayer();

private:
    void BuildSelectionMenu(FMenuBuilder& iMenu);
    void ExtendMenu(FMenuBuilder& iMenu);

private:
    void PopupContextMenu(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    TSharedPtr<SWidget> CreateContextMenu(const FOdysseyPoint& iPointInTexture, const FKey& iKey);

protected:
    bool mHasContextMenu;
};
