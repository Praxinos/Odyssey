// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include "Widgets/SWindow.h"


class ODYSSEYTEXTURE_API SOdysseyTextureConfigureWindow
    : public SWindow
{
public:
    enum EBackgroundColor
    {
        kTransparent,
        kWhite,
        kNormal,
    };

public:
    void Construct( const FArguments& iArgs );

    bool GetWindowAnswer();

public:
    int32 GetWidth() const;
    int32 GetHeight() const;
    ETextureSourceFormat GetFormat() const;
    FText GetDefaultName() const;
    FLinearColor GetBackgroundColor() const;

    void OnSetWidth( int32 iNewWidthValue, ETextCommit::Type iCommitInfo );
    void OnChangeWidth( int32 iNewWidthValue );
    void OnSetHeight( int32 iNewHeightValue, ETextCommit::Type iCommitInfo );
    void OnChangeHeight( int32 iNewHeightValue );
    void OnSetName( const FText& iNewWidthName, ETextCommit::Type iCommitInfo );
    void OnChangeName( const FText& iNewWidthName );

    FText               GetFormatText() const;
    FText               GetFormatText( TSharedPtr<ETextureSourceFormat> iFormat ) const;
    TSharedRef<SWidget> GenerateFormatComboBoxItem( TSharedPtr<ETextureSourceFormat> InItem );
    void                HandleOnFormatChanged( TSharedPtr<ETextureSourceFormat> NewSelection, ESelectInfo::Type SelectInfo );

    FText               GetBackgroundColorText( EBackgroundColor iFormat ) const;

    ECheckBoxState      IsBackgroundColorRadioChecked( EBackgroundColor iBackgroundColor ) const;
    void                OnBackgroundColorRadioChanged( ECheckBoxState iCheckType, EBackgroundColor iBackgroundColor );

    //FReply              OnColorClick( const FGeometry& iGeometry, const FPointerEvent& iEvent );

    FReply OnAccept();
    FReply OnCancel();

private:
    int                     mWidth;
    int                     mHeight;
    ETextureSourceFormat    mFormat;
    FText                   mName;
    EBackgroundColor        mBackgroundColor;
    bool                    mWindowAnswer;

    TArray< TSharedPtr<ETextureSourceFormat> >                  mAllFormats;
    TSharedPtr<SComboBox<TSharedPtr<ETextureSourceFormat> > >   mFormatComboBox;
};
