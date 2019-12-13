// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyTextureDummy/OdysseyTextureDummyFactory.h"

#include "Editor.h"
#include "EditorStyleSet.h"
#include "Engine/Texture2D.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SWindow.h"

#include "OdysseyBlock.h"
#include "OdysseySurface.h"
#include "OdysseyTextureDummy/OdysseyTextureDummy.h"

//---

/////////////////////////////////////////////////////
// Defines
#define LOCTEXT_NAMESPACE "OdysseyTextureDummyFactory"

#define MAX_CANVAS_SIZE 8192
#define MIN_CANVAS_SIZE 1
#define DEFAULT_CANVAS_SIZE 1024

/////////////////////////////////////////////////////
// SOdysseyTextureConfigureWindow
class SOdysseyTextureConfigureWindow 
    : public SWindow
{
public:
    void Construct( const FArguments& iArgs );

    bool GetWindowAnswer();

public:
    TOptional<int32> GetWidth() const;
    TOptional<int32> GetHeight() const;

    void OnSetHeight( int32 iNewHeightValue, ETextCommit::Type iCommitInfo );
    void OnSetWidth( int32 iNewWidthValue, ETextCommit::Type iCommitInfo );
    FReply OnAccept();
    FReply OnCancel();

private:
    int mWidth;
    int mHeight;
    bool mWindowAnswer;
};

//---

void 
SOdysseyTextureConfigureWindow::Construct( const FArguments& iArgs )
{
    mWidth = DEFAULT_CANVAS_SIZE;
    mHeight = DEFAULT_CANVAS_SIZE;
    mWindowAnswer = false;

    SWindow::Construct( SWindow::FArguments()
        .Title( LOCTEXT( "CreateOdysseyTextureAssetOptions", "Create Texture Asset" ) )
        .SizingRule( ESizingRule::Autosized )
        .SupportsMinimize( false )
        .SupportsMaximize( false )
        [
            SNew( SBorder )
            .BorderImage( FEditorStyle::GetBrush( "Menu.Background" ) )
            [
                SNew( SVerticalBox )
                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( 2, 2 )
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    [
                        SNew( STextBlock )
                        .Text( FText::FromString( TEXT( "Texture Width" ) ) )
                    ]
                    +SHorizontalBox::Slot()
                    [
                        SNew( SNumericEntryBox<int32> )
                        .LabelVAlign( VAlign_Center )
                        .Value( this, &SOdysseyTextureConfigureWindow::GetWidth )
                        .MinValue( MIN_CANVAS_SIZE )
                        .MaxValue( MAX_CANVAS_SIZE )
                        .OnValueCommitted( this, &SOdysseyTextureConfigureWindow::OnSetWidth )
                    ]
                ]

                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding( 2, 2 )
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    [
                        SNew( STextBlock )
                        .Text( FText::FromString( TEXT( "Texture Height" ) ) )
                    ]
                    +SHorizontalBox::Slot()
                    [
                        SNew( SNumericEntryBox<int32> )
                        .LabelVAlign( VAlign_Center )
                        .Value( this, &SOdysseyTextureConfigureWindow::GetHeight )
                        .MinValue( MIN_CANVAS_SIZE )
                        .MaxValue( MAX_CANVAS_SIZE )
                        .OnValueCommitted( this, &SOdysseyTextureConfigureWindow::OnSetHeight )
                    ]
                ]

                +SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew( SHorizontalBox )
                    +SHorizontalBox::Slot()
                    [
                        SNew( SButton )
                        .Text( LOCTEXT( "Create Asset", "Create Asset" ) )
                        .HAlign( HAlign_Center )
                        .OnClicked_Raw( this, &SOdysseyTextureConfigureWindow::OnAccept )
                    ]
                    +SHorizontalBox::Slot()
                    [
                        SNew( SButton )
                        .Text( LOCTEXT( "Cancel", "Cancel" ) )
                        .HAlign( HAlign_Center )
                        .OnClicked_Raw( this, &SOdysseyTextureConfigureWindow::OnCancel )
                    ]
                ]
            ]
        ]
    );
}

bool
SOdysseyTextureConfigureWindow::GetWindowAnswer()
{
    return mWindowAnswer;
}

TOptional<int32>
SOdysseyTextureConfigureWindow::GetWidth() const
{
    return mWidth;
}

TOptional<int32>
SOdysseyTextureConfigureWindow::GetHeight() const
{
    return mHeight;
}

void
SOdysseyTextureConfigureWindow::OnSetHeight( int32 iNewHeightValue, ETextCommit::Type iCommitInfo)
{
    mHeight = iNewHeightValue;

    if( mHeight > MAX_CANVAS_SIZE )
        mHeight = MAX_CANVAS_SIZE;
    if( mHeight < MIN_CANVAS_SIZE )
        mHeight = MIN_CANVAS_SIZE;
}

void
SOdysseyTextureConfigureWindow::OnSetWidth( int32 iNewWidthValue, ETextCommit::Type iCommitInfo)
{
    mWidth = iNewWidthValue;

    if( mWidth > MAX_CANVAS_SIZE )
        mWidth = MAX_CANVAS_SIZE;
    if( mWidth < MIN_CANVAS_SIZE )
        mWidth = MIN_CANVAS_SIZE;
}

FReply
SOdysseyTextureConfigureWindow::OnAccept()
{
    mWindowAnswer = true;
    RequestDestroyWindow();

    return FReply::Handled();
}

FReply
SOdysseyTextureConfigureWindow::OnCancel()
{
    mWindowAnswer = false;
    RequestDestroyWindow();

    return FReply::Handled();
}

/////////////////////////////////////////////////////
// UOdysseyTextureDummyFactory
UOdysseyTextureDummyFactory::UOdysseyTextureDummyFactory( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    // From UFactory
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UOdysseyTextureDummy::StaticClass();
}

UObject*
UOdysseyTextureDummyFactory::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    // Make sure we are trying to factory a UOdysseyTextureDummy, then create and init a UTexture2D instead.
    check( iClass->IsChildOf( UOdysseyTextureDummy::StaticClass() ) );

    UTexture2D* object = NewObject<UTexture2D>( iParent, iName, iFlags | RF_Transactional );
    object->Source.Init( mTextureWidth, mTextureHeight, 1, 1, TSF_BGRA8 );
    object->PostEditChange();

    // Init internal data
    FOdysseyBlock block( mTextureWidth, mTextureHeight, ETextureSourceFormat::TSF_BGRA8, nullptr, nullptr, true );
    CopyBlockDataIntoUTexture( &block, object );

    return object;
}

bool UOdysseyTextureDummyFactory::ConfigureProperties()
{
    //We go in here before creating the texture: Meaning we can have any modal window here.
    //If return false, we don't create the object, if true, we create it
    TSharedPtr<SOdysseyTextureConfigureWindow> textureConfigurationWindow = SNew( SOdysseyTextureConfigureWindow );

    GEditor->EditorAddModalWindow( textureConfigurationWindow.ToSharedRef() );
    mTextureWidth = textureConfigurationWindow->GetWidth().GetValue();
    mTextureHeight = textureConfigurationWindow->GetHeight().GetValue();

    return textureConfigurationWindow->GetWindowAnswer();
}

#undef LOCTEXT_NAMESPACE
