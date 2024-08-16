// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyPainterEditorRasterBaseTool.generated.h"

UCLASS(Abstract)
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

        virtual TSharedRef<SWidget> CreateTopTabWidget() override;
        TSharedPtr<SWidget> CreatePropertyWidget( TSharedPtr<class IPropertyHandle> iPropertyHandle
                                                , const TSharedPtr<ISinglePropertyView> iView );

        virtual bool OnKeyDown( const FKey& iKey ) override;
        virtual bool OnKeyUp( const FKey& iKey ) override;
        virtual bool OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey ) override;
        virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
        virtual void OnMouseHover( const FOdysseyPoint& iPointInTexture );
        virtual void OnMouseDrag( const FOdysseyPoint& iPointInTexture );
        virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
        virtual void ExtendContextMenu( FMenuBuilder& menu );
        virtual void BindShortcuts( FBaseToolkit* iToolkit );

    protected:
        void PopupContextMenu();
        TSharedPtr<SWidget> CreateContextMenu();

    protected:
        void SelectAll();
        void CopySelection();
        void CutSelection();
        void PasteSelection();
        void PasteSelectionInNewLayer();
        void ClearSelection();
        void InvertSelection();

protected:
        bool mHasContextMenu;
};
