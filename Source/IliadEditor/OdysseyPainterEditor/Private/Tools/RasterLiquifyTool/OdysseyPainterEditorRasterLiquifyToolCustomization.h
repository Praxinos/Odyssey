// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "IDetailCustomization.h"
#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyTool.h"

class UOdysseyPainterEditorRasterLiquifyTool;

class FOdysseyPainterEditorRasterLiquifyToolCustomization : public IDetailCustomization
{
public:
    /** IDetailCustomization interface */
    virtual void CustomizeDetails( IDetailLayoutBuilder& DetailBuilder ) override;

private:
    //FOdysseyLiquifyMode* GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const;

    //EOdysseyLiquifyMode GetSelectedMode() const;
    //void OnModeSelected(EOdysseyLiquifyMode iShape, ECheckBoxState iState);
    //const FSlateBrush* GetModeIcon(EOdysseyLiquifyMode iMode) const;
    UOdysseyPainterEditorRasterLiquifyTool* GetTool( const IDetailLayoutBuilder& iDetailBuilder );
    TSharedRef<SWidget> CreateModeWidget();
    TSharedPtr<IPropertyHandle> GetPropertyHandle( const FName& iPropertyName );
    EOdysseyLiquifyMode GetSelectedMode() const;
    void OnModeSelected( EOdysseyLiquifyMode iMode, ECheckBoxState iState );
    const FSlateBrush* GetModeIcon( EOdysseyLiquifyMode iMode ) const;
    EVisibility GetPropertyVisibility( TSharedPtr<IPropertyHandle> iPropertyHandle ) const;

private:
    TSharedPtr<IPropertyHandle> mModePropertyHandle;
    //TSharedPtr<IPropertyHandle> mEnumValuePropertyHandle;
    UOdysseyPainterEditorRasterLiquifyTool* mLiquifyTool;
    TArray<TSharedPtr<IPropertyHandle>> mPropertyHandleArray;
};
