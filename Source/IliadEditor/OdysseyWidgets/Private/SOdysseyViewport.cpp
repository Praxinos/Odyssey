// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyViewport.h"

#include "ObjectEditorUtils.h"
#include "Engine/Engine.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/MultiBox/SToolBarButtonBlock.h"
#include "ImageUtils.h"
#include "ISettingsModule.h"
#include "Modules/ModuleManager.h"
#include "NamingTokensEngineSubsystem.h"
#include "SNamingTokensEditableTextBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBar.h"
#include "Widgets/SToolTip.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/SViewport.h"
#include "Widgets/Input/NumericTypeInterface.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SSlider.h"

#include "FOdysseySceneViewport.h"
#include "Math/OdysseyMathUtils.h"
#include "OdysseyStyle.h"
#include "OdysseyViewportCommands.h"

#include <ULIS>



#define MinZoom 0.01
#define MaxZoom 200.0

#define ScrollbarThumbRatio 0.1f
#define ScrollbarSpaceRatio (1.f - ScrollbarThumbRatio)


#define LOCTEXT_NAMESPACE "Widgets"

//---

/*static*/ FString UOdysseyViewportNamingTokens::TokenNamespace = TEXT( "odysseyViewport2d" );

UOdysseyViewportNamingTokens::UOdysseyViewportNamingTokens()
{
    Namespace = TokenNamespace;
    NamespaceDisplayName = LOCTEXT( "namespace.label", "Odyssey Viewport 2D" );
}


TOptional<FIntVector2>
UOdysseyViewportNamingTokens::GetMousePositionInCanvas( TWeakPtr<SOdysseyViewport> iWeakViewportWidget ) const
{
    SOdysseyViewport* viewportWidget = iWeakViewportWidget.Pin().Get();
    if( !viewportWidget )
        return TOptional<FIntVector2>();

    FVector2D cursorPos = FSlateApplication::Get().GetCursorPos();
    if( !viewportWidget->GetViewportWidget()->GetTickSpaceGeometry().IsUnderLocation( cursorPos ) )
        return TOptional<FIntVector2>();

    float scaleDPI = viewportWidget->GetTickSpaceGeometry().GetAccumulatedLayoutTransform().GetScale();
    FVector2D cursorPosInViewport = viewportWidget->GetTickSpaceGeometry().AbsoluteToLocal( cursorPos ) * scaleDPI;
    FVector2D cursorPosInCanvas = viewportWidget->ToLocal( cursorPosInViewport );
    FIntVector2 cursorPosInCanvasInt( FMath::FloorToInt( cursorPosInCanvas.X ), FMath::FloorToInt( cursorPosInCanvas.Y ) );

    cursorPosInCanvasInt += FIntVector2( viewportWidget->GetTexture()->GetSurfaceWidth() / 2, viewportWidget->GetTexture()->GetSurfaceHeight() / 2 );

    return cursorPosInCanvasInt;
}

TOptional<FColor>
UOdysseyViewportNamingTokens::GetColorAtPosition( TWeakPtr<SOdysseyViewport> iWeakViewportWidget ) const
{
    SOdysseyViewport* viewportWidget = iWeakViewportWidget.Pin().Get();
    if( !viewportWidget )
        return TOptional<FColor>();

    UTexture* texture = viewportWidget->GetTexture();
    if( !texture )
        return TOptional<FColor>();

    UTextureRenderTarget2D* renderTarget = Cast<UTextureRenderTarget2D>( texture ); //TODO: should also test UTexture2D but FImageUtils::GetTexture2DImage() does nothing -_-
    if( !renderTarget )
        return TOptional<FColor>();

    TOptional<FIntVector2> mousePosition = GetMousePositionInCanvas( iWeakViewportWidget );
    if( !mousePosition.IsSet() )
        return TOptional<FColor>();

    if( mousePosition->X < 0 || mousePosition->X >= renderTarget->GetSurfaceWidth()
        || mousePosition->Y < 0 || mousePosition->Y >= renderTarget->GetSurfaceHeight() )
        return TOptional<FColor>();

    static FImage image;
    FIntRect rect( mousePosition->X, mousePosition->Y, mousePosition->X + 1, mousePosition->Y + 1 );
    FImageUtils::GetRenderTargetImage( renderTarget, image, rect );
    FLinearColor linearColor = image.GetOnePixelLinear( 0, 0 );
    FColor color = linearColor.ToFColor( true );
    //FColor color = linearColor.ToFColor( false );

    return color;
}

void
UOdysseyViewportNamingTokens::OnCreateDefaultTokens( TArray<FNamingTokenData>& Tokens ) //override
{
    Super::OnCreateDefaultTokens( Tokens );

    FNamingTokenData CursorXInCanvasToken;
    CursorXInCanvasToken.TokenKey = TEXT( "x" );
    CursorXInCanvasToken.DisplayName = LOCTEXT( "CursorXInCanvasToken", "Cursor X position in canvas" );
    CursorXInCanvasToken.TokenProcessorNative.BindLambda( [this]()
                                                          {
                                                              if( !Context )
                                                                  return FText::FromString( TEXT( "-" ) );
                                                                  //return FText::GetEmpty();

                                                              TOptional<FIntVector2> mousePositionInCanvas = GetMousePositionInCanvas( Context->ViewportWidget );
                                                              if( !mousePositionInCanvas.IsSet() )
                                                                  return FText::FromString( TEXT( "-" ) );
                                                                  //return FText::GetEmpty();

                                                              return FText::AsNumber( mousePositionInCanvas->X );
                                                          } );
    Tokens.Add( CursorXInCanvasToken );

    //-

    FNamingTokenData CursorYInCanvasToken;
    CursorYInCanvasToken.TokenKey = TEXT( "y" );
    CursorYInCanvasToken.DisplayName = LOCTEXT( "CursorYInCanvasToken", "Cursor Y position in canvas" );
    CursorYInCanvasToken.TokenProcessorNative.BindLambda( [this]()
                                                         {
                                                              if( !Context )
                                                                  return FText::FromString( TEXT( "-" ) );

                                                              TOptional<FIntVector2> mousePositionInCanvas = GetMousePositionInCanvas( Context->ViewportWidget );
                                                              if( !mousePositionInCanvas.IsSet() )
                                                                  return FText::FromString( TEXT( "-" ) );

                                                              return FText::AsNumber( mousePositionInCanvas->Y );
                                                         } );
    Tokens.Add( CursorYInCanvasToken );

    //-

    FNamingTokenData RedAtPositionToken;
    RedAtPositionToken.TokenKey = TEXT( "r" );
    RedAtPositionToken.DisplayName = LOCTEXT( "CanvasRedToken", "Canvas RED component at cursor position" );
    RedAtPositionToken.TokenProcessorNative.BindLambda( [this]()
                                                        {
                                                            if( !Context )
                                                                return FText::FromString( TEXT( "-" ) );

                                                            TOptional<FColor> color = GetColorAtPosition( Context->ViewportWidget );
                                                            if( !color.IsSet() )
                                                                return FText::FromString( TEXT( "-" ) );

                                                            return FText::AsNumber( color->R );
                                                            //return FText::FromString( color->ToString() );
                                                            //return FText::Format( LOCTEXT( "viewport.status-bar.pixel-color", "RGBA: {0}, {1}, {2}, {3}" ), FText::AsNumber( color.R ), FText::AsNumber( color.G ), FText::AsNumber( color.B ), FText::AsNumber( color.A ) );
                                                        } );
    Tokens.Add( RedAtPositionToken );

    FNamingTokenData GreenAtPositionToken;
    GreenAtPositionToken.TokenKey = TEXT( "g" );
    GreenAtPositionToken.DisplayName = LOCTEXT( "CanvasGreenToken", "Canvas GREEN component at cursor position" );
    GreenAtPositionToken.TokenProcessorNative.BindLambda( [this]()
                                                          {
                                                              if( !Context )
                                                                  return FText::FromString( TEXT( "-" ) );

                                                              TOptional<FColor> color = GetColorAtPosition( Context->ViewportWidget );
                                                              if( !color.IsSet() )
                                                                  return FText::FromString( TEXT( "-" ) );

                                                              return FText::AsNumber( color->G );
                                                          } );
    Tokens.Add( GreenAtPositionToken );

    FNamingTokenData BlueAtPositionToken;
    BlueAtPositionToken.TokenKey = TEXT( "b" );
    BlueAtPositionToken.DisplayName = LOCTEXT( "CanvasBlueToken", "Canvas BLUE component at cursor position" );
    BlueAtPositionToken.TokenProcessorNative.BindLambda( [this]()
                                                          {
                                                              if( !Context )
                                                                  return FText::FromString( TEXT( "-" ) );

                                                              TOptional<FColor> color = GetColorAtPosition( Context->ViewportWidget );
                                                              if( !color.IsSet() )
                                                                  return FText::FromString( TEXT( "-" ) );

                                                              return FText::AsNumber( color->B );
                                                          } );
    Tokens.Add( BlueAtPositionToken );

    FNamingTokenData AlphaAtPositionToken;
    AlphaAtPositionToken.TokenKey = TEXT( "a" );
    AlphaAtPositionToken.DisplayName = LOCTEXT( "CanvasAlphaToken", "Canvas ALPHA component at cursor position" );
    AlphaAtPositionToken.TokenProcessorNative.BindLambda( [this]()
                                                          {
                                                              if( !Context )
                                                                  return FText::FromString( TEXT( "-" ) );

                                                              TOptional<FColor> color = GetColorAtPosition( Context->ViewportWidget );
                                                              if( !color.IsSet() )
                                                                  return FText::FromString( TEXT( "-" ) );

                                                              return FText::AsNumber( color->A );
                                                          } );
    Tokens.Add( AlphaAtPositionToken );
}

void
UOdysseyViewportNamingTokens::OnPreEvaluate_Implementation( const FNamingTokensEvaluationData& InEvaluationData ) //override
{
    Super::OnPreEvaluate_Implementation( InEvaluationData );

    UOdysseyViewportNamingTokensContext* MatchingContext = nullptr;
    InEvaluationData.Contexts.FindItemByClass<UOdysseyViewportNamingTokensContext>( &MatchingContext );
    Context = MatchingContext;
}

void
UOdysseyViewportNamingTokens::OnPostEvaluate_Implementation() //override
{
    Super::OnPostEvaluate_Implementation();

    Context = nullptr;
}

/////////////////////////////////////////////////////
// SOdysseyViewport
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
void
SOdysseyViewport::Construct( const FArguments& InArgs )
{
    mTexture = InArgs._Texture;

    if( InArgs._RotationStep.IsSet() )
        mRotationStep = InArgs._RotationStep;
    if( InArgs._ZoomStep.IsSet() )
        mZoomStep = InArgs._ZoomStep;

    if( InArgs._StatusbarTemplateString.IsSet() )
        mStatusbarTemplateString = InArgs._StatusbarTemplateString;
    if( InArgs._NamingTokensContexts.IsSet() )
        mNamingTokensContexts = InArgs._NamingTokensContexts;

    mCommandList = MakeShared<FUICommandList>();

#define MAP_ACTION(ACTION, DELEGATE_EXECUTE) mCommandList->MapAction( ACTION, FExecuteAction::CreateSP( this, &SOdysseyViewport::DELEGATE_EXECUTE ), FCanExecuteAction() )

    MAP_ACTION( FOdysseyViewportCommands::Get().ResetViewport1On1, OnResetViewport1On1 );
    MAP_ACTION( FOdysseyViewportCommands::Get().ResetViewportFit, OnResetViewportFit );
    MAP_ACTION( FOdysseyViewportCommands::Get().ResetViewportAll, OnResetViewportAll );
    MAP_ACTION( FOdysseyViewportCommands::Get().ResetViewportPosition, OnResetViewportPosition );
    MAP_ACTION( FOdysseyViewportCommands::Get().ResetViewportRotation, OnResetViewportRotation );
    MAP_ACTION( FOdysseyViewportCommands::Get().ResetViewportZoom, OnResetViewportZoom );

    MAP_ACTION( FOdysseyViewportCommands::Get().RotateViewportLeft, OnRotateViewportLeft );
    MAP_ACTION( FOdysseyViewportCommands::Get().RotateViewportRight, OnRotateViewportRight );

    MAP_ACTION( FOdysseyViewportCommands::Get().ZoomInExponential, OnZoomInExponential );
    MAP_ACTION( FOdysseyViewportCommands::Get().ZoomOutExponential, OnZoomOutExponential );

#undef MAP_ACTION

#define MAP_ACTION_WITH_CHECKED(ACTION, DELEGATE_EXECUTE, DELEGATE_ISCHECKED) mCommandList->MapAction( ACTION, FExecuteAction::CreateSP( this, &SOdysseyViewport::DELEGATE_EXECUTE ), FCanExecuteAction(), FIsActionChecked::CreateSP( this, &SOdysseyViewport::DELEGATE_ISCHECKED ) )

    MAP_ACTION_WITH_CHECKED( FOdysseyViewportCommands::Get().FlipViewportHorizontally, OnFlipViewportHorizontally, IsHorizontallyFlipped );
    MAP_ACTION_WITH_CHECKED( FOdysseyViewportCommands::Get().FlipViewportVertically, OnFlipViewportVertically, IsVerticallyFlipped );

    MAP_ACTION_WITH_CHECKED( FOdysseyViewportCommands::Get().SetZoomFitScreen, OnSetZoomFitScreen, IsZoomMenuFitChecked );

#undef MAP_ACTION_WITH_CHECKED

#define MAP_ACTION_WITH_CHECKED_WITH_PARAMETER(ACTION, DELEGATE_EXECUTE, DELEGATE_ISCHECKED, VALUE) mCommandList->MapAction( ACTION, FExecuteAction::CreateSP( this, &SOdysseyViewport::DELEGATE_EXECUTE, VALUE ), FCanExecuteAction(), FIsActionChecked::CreateSP( this, &SOdysseyViewport::DELEGATE_ISCHECKED, VALUE ) )

    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetRotationMinus135, OnSetRotation, IsRotationEqual, FMath::DegreesToRadians( -135.f ) );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetRotationMinus90, OnSetRotation, IsRotationEqual, FMath::DegreesToRadians( -90.f ) );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetRotationMinus45, OnSetRotation, IsRotationEqual, FMath::DegreesToRadians( -45.f ) );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetRotation0, OnSetRotation, IsRotationEqual, FMath::DegreesToRadians( 0.f ) );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetRotation45, OnSetRotation, IsRotationEqual, FMath::DegreesToRadians( 45.f ) );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetRotation90, OnSetRotation, IsRotationEqual, FMath::DegreesToRadians( 90.f ) );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetRotation135, OnSetRotation, IsRotationEqual, FMath::DegreesToRadians( 135.f ) );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetRotation180, OnSetRotation, IsRotationEqual, FMath::DegreesToRadians( 180.f ) );

    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetZoom10Percent, OnSetZoom, IsZoomEqual, 0.1f );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetZoom25Percent, OnSetZoom, IsZoomEqual, 0.25f );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetZoom50Percent, OnSetZoom, IsZoomEqual, 0.5f );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetZoom75Percent, OnSetZoom, IsZoomEqual, 0.75f );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetZoom100Percent, OnSetZoom, IsZoomEqual, 1.f );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetZoom200Percent, OnSetZoom, IsZoomEqual, 2.f );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetZoom400Percent, OnSetZoom, IsZoomEqual, 4.f );
    MAP_ACTION_WITH_CHECKED_WITH_PARAMETER( FOdysseyViewportCommands::Get().SetZoom800Percent, OnSetZoom, IsZoomEqual, 8.f );

#undef MAP_ACTION_WITH_CHECKED_WITH_PARAMETER

    //---

    mFlipStateUV = FVector2D(0.f, 0.f);
    mPan = FVector2D( 0.f, 0.f );
    mRotation = 0;
    mZoom = 1;

    UpdateTransform();

    mAlignWithViewport = true;

    //---

    FSlimHorizontalToolBarBuilder toolbarBuilder( mCommandList, FMultiBoxCustomization::None, nullptr /* extender */, true );
    toolbarBuilder.SetLabelVisibility( EVisibility::Collapsed );
    toolbarBuilder.SetStyle( &FOdysseyStyle::Get(), "PainterEditor.ViewportToolbar" );

    // Rotation
    FButtonArgs args;
    args.Command = FOdysseyViewportCommands::Get().RotateViewportLeft;
    toolbarBuilder.AddToolBarButton( args );

    args.Command = FOdysseyViewportCommands::Get().RotateViewportRight;
    toolbarBuilder.AddToolBarButton( args );

    toolbarBuilder.AddToolBarWidget(
        SNew( SSpinBox< int > )
        .Value( this, &SOdysseyViewport::GetGuiRotationValue )
        .OnValueChanged( this, &SOdysseyViewport::HandleRotationChanged )
        .LinearDeltaSensitivity( 10 )  // If we're an unbounded spinbox, what value do we divide mouse movement by before multiplying by Delta. Requires Delta to be set.
        .Delta( 1 )
        .TypeInterface( MakeShared<TNumericUnitTypeInterface< int >>( EUnit::Degrees ) )
        .MinDesiredWidth( 48.0f ),
        LOCTEXT( "viewport.rotation.label", "Rotation" )
        );

    toolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateLambda( [this, iExtender=InArgs._OptionExtender]() -> TSharedRef<SWidget>
                                     {
                                         FMenuBuilder rotationMenu( true, mCommandList, iExtender );

                                         rotationMenu.BeginSection( "RotationOptionsSection" );

                                         // Can't be used because FMenuEntryParams has its own constructor -_-
                                         //FMenuEntryParams params = { .Action=FOdysseyViewportCommands::Get().ResetViewportRotation };
                                         // It works, but yerk ... -_-
                                         //FMenuEntryParams params;
                                         //*( const_cast<TSharedPtr< const FUICommandInfo >*>( &params.Action ) ) = FOdysseyViewportCommands::Get().ResetViewportRotation;
                                         //ioZoomMenuBuilder.AddMenuEntry( params );
                                         // So, use the old way
                                         rotationMenu.AddMenuEntry( FOdysseyViewportCommands::Get().ResetViewportRotation, NAME_None, LOCTEXT( "viewport.rotation.reset.label", "Reset" ) );

                                         rotationMenu.AddSeparator();

                                         rotationMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetRotationMinus135, NAME_None, LOCTEXT( "viewport.rotation.preset-Minus135.label", "-135°" ) );
                                         rotationMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetRotationMinus90, NAME_None, LOCTEXT( "viewport.rotation.preset-Minus90.label", "-90°" ) );
                                         rotationMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetRotationMinus45, NAME_None, LOCTEXT( "viewport.rotation.preset-Minus45.label", "-45°" ) );
                                         rotationMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetRotation0, NAME_None, LOCTEXT( "viewport.rotation.preset-0.label", "0°" ) );
                                         rotationMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetRotation45, NAME_None, LOCTEXT( "viewport.rotation.preset-45.label", "45°" ) );
                                         rotationMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetRotation90, NAME_None, LOCTEXT( "viewport.rotation.preset-90.label", "90°" ) );
                                         rotationMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetRotation135, NAME_None, LOCTEXT( "viewport.rotation.preset-135.label", "135°" ) );
                                         rotationMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetRotation180, NAME_None, LOCTEXT( "viewport.rotation.preset-180.label", "180°" ) );

                                         rotationMenu.EndSection();

                                         return rotationMenu.MakeWidget();
                                     } ),
        LOCTEXT( "viewport.rotate.options.label", "Rotation Options" ),
        LOCTEXT( "viewport.rotate.options.tooltip", "Rotation options" ),
        TAttribute<FSlateIcon>(),
        false
    );

    toolbarBuilder.AddSeparator();

    // Zoom
    args.Command = FOdysseyViewportCommands::Get().ZoomInExponential;
    toolbarBuilder.AddToolBarButton( args );

    args.Command = FOdysseyViewportCommands::Get().ZoomOutExponential;
    toolbarBuilder.AddToolBarButton( args );

    toolbarBuilder.AddToolBarWidget(
        SNew( SSpinBox< float > )
        .Value( this, &SOdysseyViewport::GetGuiZoomValue )
        .OnValueChanged( this, &SOdysseyViewport::HandleZoomSliderChanged )
        //.LinearDeltaSensitivity(20)  // If we're an unbounded spinbox, what value do we divide mouse movement by before multiplying by Delta. Requires Delta to be set. But we choosed not to use this option here.
        .Delta( 1 )
        .TypeInterface( MakeShared<TNumericUnitTypeInterface<float>>( EUnit::Percentage ) )
        .ShiftMultiplier( 15.f )
        .SliderExponent( 1.f ) // Can't work properly if the following options are in use :  .LinearDeltaSensitivity .MinValue .MaxValue
        .MinFractionalDigits( 2 )
        .MaxFractionalDigits( 2 )
        .MinDesiredWidth( 70.0f ),
        LOCTEXT( "viewport.zoom.label", "Zoom" )
    );

    //toolbarBuilder.AddToolBarWidget(
    //    SNew( SSpinBox< float > )
    //    .Value( this, &SOdysseyViewport::GetGuiZoomValue )
    //    .OnValueChanged( this, &SOdysseyViewport::HandleZoomSliderChanged )
    //    .MinValue( 1.f )
    //    .MaxValue( 20000.f )
    //    .MinSliderValue( 1.f )
    //    .MaxSliderValue( 20000.f )
    //    //.LinearDeltaSensitivity(20)  // If we're an unbounded spinbox, what value do we divide mouse movement by before multiplying by Delta. Requires Delta to be set. But we choosed not to use this option here.
    //    //.Delta( 1 )
    //    .TypeInterface( MakeShared<TNumericUnitTypeInterface<float>>( EUnit::Percentage ) )
    //    .ShiftMultiplier( 15.f )
    //    .SliderExponent( 4.f ) // Can't work properly if the following options are in use :  .LinearDeltaSensitivity .MinValue .MaxValue
    //    .SliderExponentNeutralValue( 100.f )
    //    .MinFractionalDigits( 2 )
    //    .MaxFractionalDigits( 2 )
    //    .MinDesiredWidth( 70.0f ),
    //    LOCTEXT( "viewport.zoom.label", "Zoom" )
    //);

    toolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateLambda( [this, iExtender = InArgs._OptionExtender]() -> TSharedRef<SWidget>
                                     {
                                         FMenuBuilder zoomMenu( true, mCommandList, iExtender );

                                         zoomMenu.BeginSection( "ZoomOptionsSection" );

                                         zoomMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetZoomFitScreen, NAME_None, LOCTEXT( "viewport.zoom.fit.label", "Scale To Fit" ) );
                                         zoomMenu.AddMenuEntry( FOdysseyViewportCommands::Get().ResetViewportZoom, NAME_None, LOCTEXT( "viewport.zoom.reset.label", "Reset" ) );

                                         zoomMenu.AddSeparator();

                                         zoomMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetZoom10Percent, NAME_None, LOCTEXT( "viewport.zoom.preset-10.label", "10%" ) );
                                         zoomMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetZoom25Percent, NAME_None, LOCTEXT( "viewport.zoom.preset-25.label", "25%" ) );
                                         zoomMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetZoom50Percent, NAME_None, LOCTEXT( "viewport.zoom.preset-50.label", "50%" ) );
                                         zoomMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetZoom75Percent, NAME_None, LOCTEXT( "viewport.zoom.preset-75.label", "75%" ) );
                                         zoomMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetZoom100Percent, NAME_None, LOCTEXT( "viewport.zoom.preset-100.label", "100%" ) );
                                         zoomMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetZoom200Percent, NAME_None, LOCTEXT( "viewport.zoom.preset-200.label", "200%" ) );
                                         zoomMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetZoom400Percent, NAME_None, LOCTEXT( "viewport.zoom.preset-400.label", "400%" ) );
                                         zoomMenu.AddMenuEntry( FOdysseyViewportCommands::Get().SetZoom800Percent, NAME_None, LOCTEXT( "viewport.zoom.preset-800.label", "800%" ) );

                                         zoomMenu.EndSection();

                                         return zoomMenu.MakeWidget();
                                     } ),
        LOCTEXT( "viewport.zoom.options.label", "Zoom Options" ),
        LOCTEXT( "viewport.zoom.options.tooltip", "Zoom options" ),
        TAttribute<FSlateIcon>(),
        false
    );

    toolbarBuilder.AddSeparator();

    // Reset
    args.Command = FOdysseyViewportCommands::Get().ResetViewport1On1;
    toolbarBuilder.AddToolBarButton( args );

    args.Command = FOdysseyViewportCommands::Get().ResetViewportFit;
    toolbarBuilder.AddToolBarButton( args );

    toolbarBuilder.AddSeparator();

    // Flip
    args.Command = FOdysseyViewportCommands::Get().FlipViewportHorizontally;
    toolbarBuilder.AddToolBarButton( args );

    args.Command = FOdysseyViewportCommands::Get().FlipViewportVertically;
    toolbarBuilder.AddToolBarButton( args );

    toolbarBuilder.AddSeparator();

    // Settings
    toolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateLambda( [this, iExtender = InArgs._OptionExtender]() -> TSharedRef<SWidget>
                                     {
                                         FMenuBuilder settingsMenu( true, mCommandList, iExtender );

                                         //---

                                         settingsMenu.BeginSection( "SettingsMiscSection", LOCTEXT( "viewport.settings-section.advanced.label", "Misc" ) );

                                         settingsMenu.AddMenuEntry( FOdysseyViewportCommands::Get().ResetViewportAll );

                                         settingsMenu.AddSeparator( "SettingsMiscSeparator" );

                                         {
                                             FMenuEntryParams params;
                                             params.LabelOverride = LOCTEXT( "viewport.settings.entry.preserve-canvas-when-flipping.label", "Preserve canvas position when flipping" );
                                             params.ToolTipOverride = LOCTEXT( "viewport.settings.entry.preserve-canvas-when-flipping.tooltip", "Keep the center of the viewport at the same position when flipping it when on" );
                                             params.DirectActions = FUIAction(
                                                 FExecuteAction::CreateSP( this, &SOdysseyViewport::HandleAlignWithViewportClicked ),
                                                 FCanExecuteAction(),
                                                 FIsActionChecked::CreateSP( this, &SOdysseyViewport::IsAlignWithViewportChecked )
                                             );
                                             params.UserInterfaceActionType = EUserInterfaceActionType::ToggleButton;
                                             settingsMenu.AddMenuEntry( params );
                                         }

                                         settingsMenu.EndSection();

                                         //---

                                         settingsMenu.BeginSection( "SettingsAdvancedSection", LOCTEXT( "viewport.settings-section.advanced.label", "Advanced Settings" ) );

                                         {
                                             FMenuEntryParams params;
                                             params.LabelOverride = LOCTEXT( "viewport.settings.entry.settings.label", "Open Viewport Settings..." );
                                             params.ToolTipOverride = LOCTEXT( "viewport.settings.entry.settings.tooltip", "Open the viewport settings panel" );
                                             params.DirectActions = FUIAction(
                                                 FExecuteAction::CreateLambda( []()
                                                                               {
                                                                                   FModuleManager::LoadModuleChecked<ISettingsModule>( "Settings" ).ShowViewer( "Editor", "Plugins", "OdysseyPainterEditor" );
                                                                               } )
                                             );
                                             settingsMenu.AddMenuEntry( params );
                                         }

                                         settingsMenu.EndSection();

                                         //---

                                         return settingsMenu.MakeWidget();
                                     } ),
        LOCTEXT( "viewport.settings.label", "Settings" ),
        LOCTEXT( "viewport.settings.tooltip", "Settings" ),
        FSlateIcon( FOdysseyStyle::GetStyleSetName(), "PainterEditor.Settings" ),
        false
    );

    //---

    this->ChildSlot
    [
        SNew(SVerticalBox)

        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            .FillWidth(1.0f)
            [
                SNew(SVerticalBox)

                + SVerticalBox::Slot()
                .FillHeight(1)
                [
                    SNew(SOverlay)

                    // viewport canvas
                    + SOverlay::Slot()
                    [
                        SAssignNew(mViewportWidget, SViewport)
                        .EnableGammaCorrection( false )
                        .IsEnabled( FSlateApplication::Get().GetNormalExecutionAttribute() )
                        .ShowEffectWhenDisabled( false )
                        .EnableBlending( true )
                    ]
                ]
            ]

            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                // vertical scroll bar
                SAssignNew(mVerticalScrollBar, SScrollBar)
                .AlwaysShowScrollbar( true )
                .Thickness( FVector2D( 10.f, 10.f ) )
                .OnUserScrolled( this, &SOdysseyViewport::HandleVerticalScrollBarScrolled )
            ]
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            // horizontal scrollbar
            SAssignNew(mHorizontalScrollBar, SScrollBar)
            .Orientation( Orient_Horizontal )
            .AlwaysShowScrollbar( true )
            .Thickness( FVector2D( 10.f, 10.f ) )
            .OnUserScrolled( this, &SOdysseyViewport::HandleHorizontalScrollBarScrolled )
        ]

        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding(2.0f, 2.0f, 2.0f, 0.0f)
        [
            SNew(SHorizontalBox)

            //+ SHorizontalBox::Slot()
            //.AutoWidth()
            //.VAlign( VAlign_Center )
            //[
            //    SNew( STextBlock )
            //    .Text( this, &SOdysseyViewport::GetTextureInfosValue )
            //]

            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign( VAlign_Center )
            [
                SNew( STextBlock )
                .Text( this, &SOdysseyViewport::GetStatusbarText )
            ]

            //+ SHorizontalBox::Slot()
            //.AutoWidth()
            //.VAlign( VAlign_Center )
            //[
            //    SNew( SNamingTokensEditableTextBox )
            //    .IsReadOnly( true )
            //    .Text_Lambda( [this]() -> FText
            //                  {
            //                      return FText::FromString( mStatusbarTemplateString.Get().Template );
            //                  } )
            //    .ResolvedText_Lambda( [this]() -> FText
            //                          {
            //                              //FNamingTokenFilterArgs FilterArgs;
            //                              //FilterArgs.AdditionalNamespacesToInclude.Add( TokenNamespace );

            //                              UNamingTokensEngineSubsystem* NamingTokensSubsystem = GEngine->GetEngineSubsystem<UNamingTokensEngineSubsystem>();
            //                              FNamingTokenResultData Result = NamingTokensSubsystem->EvaluateTokenString( mStatusbarTemplateString.Get().Template );

            //                              return Result.EvaluatedText;
            //                          } )
            //]

            + SHorizontalBox::Slot()
            .FillWidth( 1.f )
            .HAlign( HAlign_Right )
            .VAlign( VAlign_Center )
            [
                toolbarBuilder.MakeWidget()
            ]
        ]
    ];
}


TSharedPtr<FUICommandList>
SOdysseyViewport::GetCommandList()
{
    return mCommandList;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Shortcuts


void
SOdysseyViewport::OnResetViewport1On1()
{
    ResetPan();
    SetRotation( 0.f );
    SetZoom( 1.f );
}

void
SOdysseyViewport::OnResetViewportFit()
{
    FitToViewport();
}

void
SOdysseyViewport::OnResetViewportAll()
{
    mFlipStateUV = FVector2D( 0.f, 0.f );
    SetRotation( 0.f );
    FitToViewport();
}

void
SOdysseyViewport::OnResetViewportPosition()
{
    SetRotation( 0 );
    ResetPan();
}

void
SOdysseyViewport::OnResetViewportRotation()
{
    SetRotation( 0 );
}

void
SOdysseyViewport::OnResetViewportZoom()
{
    SetZoom( 1.f );
}

void
SOdysseyViewport::OnRotateViewportLeft()
{
    RotateLeft();
}

void
SOdysseyViewport::OnRotateViewportRight()
{
    RotateRight();
}

void SOdysseyViewport::OnFlipViewportHorizontally()
{
    FlipHorizontal();
}

void SOdysseyViewport::OnFlipViewportVertically()
{
    FlipVertical();
}

bool
SOdysseyViewport::IsHorizontallyFlipped()
{
    return mFlipStateUV.X == 1;
}

bool
SOdysseyViewport::IsVerticallyFlipped()
{
    return mFlipStateUV.Y == 1;
}

void
SOdysseyViewport::OnSetRotation( float iAngleRadians )
{
    SetRotation( iAngleRadians );
}

bool
SOdysseyViewport::IsRotationEqual( float iAngleRadians )
{
    return FMath::IsNearlyEqual( FMath::UnwindRadians( mRotation ), FMath::UnwindRadians( iAngleRadians ) );
}

void
SOdysseyViewport::OnSetZoom( float iZoomValue )
{
    SetZoom( iZoomValue );
    // Old way ?
    //SetZoom( iZoomValue, GetViewportCenter() );
}

bool
SOdysseyViewport::IsZoomEqual( float iZoomValue )
{
    return FMath::IsNearlyEqual( mZoom, iZoomValue );
}

void
SOdysseyViewport::OnSetZoomFitScreen()
{
    ToggleFitToViewport();
}

void
SOdysseyViewport::OnZoomInExponential()
{
    ZoomExponential( GetZoom(), mZoomStep.Get() );
}

void
SOdysseyViewport::OnZoomOutExponential()
{
    ZoomExponential( GetZoom(), -mZoomStep.Get() );
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API


TSharedPtr< FOdysseySceneViewport >
SOdysseyViewport::GetViewport() const
{
    return  mViewport;
}


TSharedPtr< SViewport >
SOdysseyViewport::GetViewportWidget( ) const
{
    return  mViewportWidget;
}


TSharedPtr< SScrollBar >
SOdysseyViewport::GetVerticalScrollBar( ) const
{
    return  mVerticalScrollBar;
}


TSharedPtr< SScrollBar >
SOdysseyViewport::GetHorizontalScrollBar( ) const
{
    return  mHorizontalScrollBar;
}


UTexture*
SOdysseyViewport::GetTexture() const
{
    return  mTexture.Get();
}


void
SOdysseyViewport::UpdateScrollBars()
{
    UTexture* texture = GetTexture();
    if (!texture)
        return;

    float width = texture->GetSurfaceWidth();
    float height = texture->GetSurfaceHeight();

    //Get the BoundingBox

    //All calculations are done in the Transform Coordinate system
    //So (0,0) is bottom left
    TArray<FVector2D> points;
    points.Add(mTransform.TransformPoint(FVector2D(width / 2.f, height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(-width / 2.f, height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(width / 2.f, -height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(-width / 2.f, -height / 2.f)));

    FBox2D bbox(points);

    FVector2D minPos( -bbox.GetSize().X / 2.f, -bbox.GetSize().Y / 2.f);
    FVector2D maxPos( mViewport->GetSizeXY().X + bbox.GetSize().X / 2.f, mViewport->GetSizeXY().Y + bbox.GetSize().Y / 2.f);

    FVector2D dist = maxPos - minPos;
    FVector2D center = bbox.GetCenter() + GetViewportCenter();

    FVector2D pos = (center - minPos) / dist;
    pos = pos.ClampAxes(0.f, 1.f) ;

    mHorizontalScrollBar->SetState((1.0 - pos.X) * ScrollbarSpaceRatio, ScrollbarThumbRatio);
    mVerticalScrollBar->SetState((1.0 - pos.Y) * ScrollbarSpaceRatio, ScrollbarThumbRatio);
}

void SOdysseyViewport::UpdateTransform()
{
    mTransform = FTransform2D( GetFlipMatrix() );
    mTransform = mTransform.Concatenate( FTransform2D(FQuat2D(mRotation)));
    mTransform = mTransform.Concatenate( FTransform2D( mZoom, mPan ) );
}

const FMatrix2x2 SOdysseyViewport::GetFlipMatrix() const
{
    int flipX = mFlipStateUV.X == 0 ? 1 : -1;
    int flipY = mFlipStateUV.Y == 0 ? 1 : -1;

    const FMatrix2x2 flipState = FMatrix2x2(flipX, 0.f,
                                            0.f, flipY);

    return flipState;
}


void SOdysseyViewport::SetViewportClient(TSharedPtr<class FViewportClient> InViewportClient)
{
    if (!InViewportClient.IsValid())
        return;

    if( mViewportClient.IsValid() )
    {
        mViewportClient.Reset();
        mViewport.Reset();
        mViewportWidget.Reset();
    }

    mViewportClient  = InViewportClient;
    mViewport        = MakeShareable(new FOdysseySceneViewport(mViewportClient.Pin(), mViewportWidget));
    mViewportWidget->SetViewportInterface(mViewport.ToSharedRef());

    SetFitToViewport(true);
    UpdateScrollBars();
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides
void SOdysseyViewport::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    if (mIsFitToViewport) //If someone wants to, find somewhere else than the tick method to fit the viewport continuously
        FitToViewport();

    mViewport->Invalidate();
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Private Callbacks

float
SOdysseyViewport::GetGuiZoomValue() const
{
    return GetZoom() * 100;
}

FVector2D
SOdysseyViewport::GetTranslationFromSlidersOffsets( float InScrollOffsetFractionX, float InScrollOffsetFractionY )
{
    UTexture* texture = GetTexture();
    if (!texture)
        return FVector2D(0.f, 0.f);

    float width = texture->GetSurfaceWidth();
    float height = texture->GetSurfaceHeight();

    //Get the BoundingBox

    //All calculations are done in the Transform Coordinate system
    //So (0,0) is bottom left
    TArray<FVector2D> points;
    points.Add(mTransform.TransformPoint(FVector2D(width / 2.f, height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(-width / 2.f, height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(width / 2.f, -height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(-width / 2.f, -height / 2.f)));

    FBox2D bbox(points);

    FVector2D minPos(-bbox.GetSize().X / 2.f, -bbox.GetSize().Y / 2.f);
    FVector2D maxPos(mViewport->GetSizeXY().X + bbox.GetSize().X / 2.f, mViewport->GetSizeXY().Y + bbox.GetSize().Y / 2.f);

    FVector2D dist = maxPos - minPos;

    FVector2D pos = FVector2D( 1.f - (InScrollOffsetFractionX / (ScrollbarSpaceRatio)), 1.f - (InScrollOffsetFractionY / (ScrollbarSpaceRatio)));
    pos *= dist;
    pos += minPos;
    pos -= GetViewportCenter();

    return pos;
}



void
SOdysseyViewport::HandleHorizontalScrollBarScrolled(float InScrollOffsetFraction)
{
    FVector2D translation = GetTranslationFromSlidersOffsets(InScrollOffsetFraction, mVerticalScrollBar->DistanceFromTop());
    mPan = FVector2D(translation.X, mPan.Y);
    UpdateTransform();
    mHorizontalScrollBar->SetState(FMath::Clamp(InScrollOffsetFraction, 0.f, ScrollbarSpaceRatio), ScrollbarThumbRatio);
    SetFitToViewport(false);
}


void
SOdysseyViewport::HandleVerticalScrollBarScrolled( float InScrollOffsetFraction )
{
    FVector2D translation = GetTranslationFromSlidersOffsets(mHorizontalScrollBar->DistanceFromTop(), InScrollOffsetFraction);
    mPan = FVector2D(mPan.X, translation.Y);
    UpdateTransform();
    mVerticalScrollBar->SetState(FMath::Clamp(InScrollOffsetFraction, 0.f, ScrollbarSpaceRatio), ScrollbarThumbRatio);
    SetFitToViewport(false);
}


void
SOdysseyViewport::HandleAlignWithViewportClicked()
{
    ToggleAlignWithViewport();
}

//void
//SOdysseyViewport::HandleViewportReset()
//{
//    mTransform = FTransform2D(1.0f, FVector2D(0.0f, 0.0f));
//    mFlipStateUV = FVector2D(0.f, 0.f);
//    mPan = FVector2D( 0.f, 0.f );
//    mRotation = 0;
//    mZoom = 1.f;
//    SetFitToViewport(false);
//    UpdateScrollBars();
//}


bool
SOdysseyViewport::IsZoomMenuFitChecked() const
{
    return GetFitToViewport();
}

bool
SOdysseyViewport::IsAlignWithViewportChecked() const
{
    return mAlignWithViewport;
}

void
SOdysseyViewport::HandleRotationChanged( int newRotation )
{
    SetRotation( FMath::DegreesToRadians(newRotation) );
}

int
SOdysseyViewport::GetGuiRotationValue() const
{
    float angle = FMath::RadiansToDegrees(GetRotation());
    //if (angle < 0)
    //    angle += 360.0;

    //FMath::Fmod( angle, 360.0 );
    return FMath::RoundHalfFromZero(angle);
}

FText
SOdysseyViewport::GetStatusbarText() const
{
    //TODO: For the moment, display nothing when in texture editor mode:
    // 1- problem with UOdysseyViewportNamingTokens::GetColorAtPosition() to get (in an optimize way) the color at position of a texture
    // 2- multiple solutions
    //     - must create new naming tokens which will store a texture as context instead of an animation
    //       which implies another namespace which implies (maybe) 2 patterns in settings but in this case, how to know which one to display ?!
    //     - OR use another context to store the texture like the animation does (or add the texture in the existing context near the animation ?)
    //       but in this case, the name of the namespace is no more really coherent (?)
    if( Cast<UTexture2D>( GetTexture() ) )
        return FText::GetEmpty();

    FNamingTokenFilterArgs FilterArgs;
    //FilterArgs.AdditionalNamespacesToInclude.Add( TokenNamespace );

    //TArray<UObject*> contexts;

    UNamingTokensEngineSubsystem* NamingTokensSubsystem = GEngine->GetEngineSubsystem<UNamingTokensEngineSubsystem>();
    FNamingTokenResultData Result = NamingTokensSubsystem->EvaluateTokenString( mStatusbarTemplateString.Get().Template, FilterArgs, mNamingTokensContexts.Get() );
    //FNamingTokenResultData Result = NamingTokensSubsystem->EvaluateTokenString( mStatusbarTemplateString.Get().Template, FilterArgs, contexts );

    return Result.EvaluatedText;
}

FText
SOdysseyViewport::GetTextureInfosValue() const
{
    UTexture* texture       = GetTexture();
    if (!texture)
        return LOCTEXT("viewport.status-bar.no-texture-provided", "No Texture Provided");

    FText textureSize = FText::Format( LOCTEXT( "viewport.status-bar.texture-size", "{0}x{1} px" ), FText::AsNumber( texture->GetSurfaceWidth() ), FText::AsNumber( texture->GetSurfaceHeight() ) );

    FVector2D cursorPos = FSlateApplication::Get().GetCursorPos();
    FVector2D cursorPosInViewport = mViewportWidget->GetTickSpaceGeometry().AbsoluteToLocal( cursorPos );
    FVector2D cursorPosInCanvas = ToLocal( cursorPosInViewport );
    FIntVector2 cursorPosInCanvasInt( FMath::FloorToInt( cursorPosInCanvas.X ), FMath::FloorToInt( cursorPosInCanvas.Y ) );

    cursorPosInCanvasInt += FIntVector2( texture->GetSurfaceWidth() / 2, texture->GetSurfaceHeight() / 2 );

    FText cursorText = FText::Format( LOCTEXT( "viewport.status-bar.cursor", "XY: {0}, {1} px" ), FText::AsNumber( cursorPosInCanvasInt.X ), FText::AsNumber( cursorPosInCanvasInt.Y ) );

    FText colorText;
    UTextureRenderTarget2D* renderTarget = Cast<UTextureRenderTarget2D>( texture ); //TODO: should also test UTexture2D but FImageUtils::GetTexture2DImage() does nothing -_-
    if( !renderTarget )
    {
        colorText = LOCTEXT( "viewport.status-bar.pixel-color.no-pixel", "RGBA: No pixel" );
    }
    else
    {
        if( cursorPosInCanvasInt.X < 0 || cursorPosInCanvasInt.X >= renderTarget->GetSurfaceWidth()
            || cursorPosInCanvasInt.Y < 0 || cursorPosInCanvasInt.Y >= renderTarget->GetSurfaceHeight() )
        {
            colorText = LOCTEXT( "viewport.status-bar.pixel-color.outside-texture", "RGBA: Outside" );
        }
        else
        {
            static FImage image;
            FIntRect rect( cursorPosInCanvasInt.X, cursorPosInCanvasInt.Y, cursorPosInCanvasInt.X + 1, cursorPosInCanvasInt.Y + 1 );
            FImageUtils::GetRenderTargetImage( renderTarget, image, rect );
            FLinearColor linearColor = image.GetOnePixelLinear( 0, 0 );
            FColor color = linearColor.ToFColor( true );
            //FColor color = linearColor.ToFColor( false );

            colorText = FText::Format( LOCTEXT( "viewport.status-bar.pixel-color", "RGBA: {0}, {1}, {2}, {3}" ), FText::AsNumber( color.R ), FText::AsNumber( color.G ), FText::AsNumber( color.B ), FText::AsNumber( color.A ) );
        }
    }

    return FText::Format( LOCTEXT("viewport.status-bar.text","{0} | {1} | {2}"), textureSize, cursorText, colorText );
}


void
SOdysseyViewport::HandleZoomSliderChanged( float NewValue )
{
    SetZoom( NewValue / 100.f);
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Navigation API
double
SOdysseyViewport::GetZoom() const
{
    return mZoom;
}


void
SOdysseyViewport::SetZoom( double ZoomValue, const FVector2D& iZoomPosition)
{
    Zoom(ZoomValue, iZoomPosition);
    SetFitToViewport(false);
    UpdateScrollBars();
}

void
SOdysseyViewport::Zoom(double ZoomValue, const FVector2D& iZoomPosition)
{
    ZoomValue = FMath::Clamp( ZoomValue, MinZoom, MaxZoom );

    FTransform2D zoomTransform = FTransform2D(ZoomValue / GetZoom(), FVector2D(0, 0));

    mZoom = ZoomValue;

    FVector2D translation = iZoomPosition;

    FTransform2D translationDiff = mTransform.Concatenate(FTransform2D(-translation));
    translationDiff = FTransform2D(Concatenate(translationDiff, zoomTransform.GetMatrix()));
    translationDiff = translationDiff.Concatenate(FTransform2D(translation));

    mPan = translationDiff.GetTranslation();

    UpdateTransform();
}

void
SOdysseyViewport::ZoomExponential(float iBaseZoom, float iSliderOffsetToAdd, const FVector2D& iZoomPosition)
{
    double sliderPos = FMath::Loge(iBaseZoom);
    sliderPos += iSliderOffsetToAdd;
    double newZoom = FMath::Clamp(double(FMath::Exp(sliderPos)), MinZoom, MaxZoom);
    SetZoom( newZoom, iZoomPosition);
}

bool
SOdysseyViewport::GetFitToViewport() const
{
    return mIsFitToViewport;
}


void
SOdysseyViewport::SetFitToViewport( bool bFitToViewport )
{
    if (!bFitToViewport)
    {
        mIsFitToViewport = bFitToViewport;
        return;
    }

    mIsFitToViewport = bFitToViewport;
}

void
SOdysseyViewport::FitToViewport()
{
    UTexture* texture = GetTexture();
    if (!texture)
        return;

    if (mViewport->GetSizeXY().X == 0 || mViewport->GetSizeXY().Y == 0)
        return;


    float width = texture->GetSurfaceWidth();
    float height = texture->GetSurfaceHeight();

    //Get the BoundingBox

    //All calculations are done in the Transform Coodinate system
    //So (0,0) is bottom left
    TArray<FVector2D> points;
    points.Add(mTransform.TransformPoint(FVector2D(width / 2.f, height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(-width / 2.f, height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(width / 2.f, -height / 2.f)));
    points.Add(mTransform.TransformPoint(FVector2D(-width / 2.f, -height / 2.f)));

    FBox2D bbox(points);

    float viewportRatio = (float)mViewport->GetSizeXY().X / (float)mViewport->GetSizeXY().Y;
    float textureRatio = bbox.GetSize().X / bbox.GetSize().Y;

    float zoom = GetZoom();
    if (textureRatio > viewportRatio)
    {
        //width
        zoom = GetZoom() * (mViewport->GetSizeXY().X / bbox.GetSize().X);
    }
    else
    {
        //height
        zoom = GetZoom() * (mViewport->GetSizeXY().Y / bbox.GetSize().Y);
    }

    Zoom(zoom);
    Pan(FVector2D(0.0f, 0.0f));
    UpdateScrollBars();
}


void
SOdysseyViewport::ToggleFitToViewport()
{
    SetFitToViewport(!mIsFitToViewport);
}

void
SOdysseyViewport::ToggleAlignWithViewport()
{
    mAlignWithViewport = !mAlignWithViewport;
}


double SOdysseyViewport::GetRotation() const
{
    return mRotation;
}

void SOdysseyViewport::SetRotation(double RotationValue)
{
    FTransform2D rotationTransform = FTransform2D(FQuat2D(RotationValue - mRotation));

    FTransform2D translationDiff = mTransform.Concatenate(rotationTransform);

    mPan = translationDiff.GetTranslation();
    mRotation = FMath::UnwindRadians( RotationValue );
    //mRotation = fmod( 2 * PI + fmod(RotationValue, 2 * PI), 2 * PI); //Positive modulo;

    UpdateTransform();
    UpdateScrollBars();
}

void SOdysseyViewport::Rotate(double RotationValue)
{
    SetRotation(mRotation + RotationValue);
}

FVector2D SOdysseyViewport::GetPan() const
{
    return mPan;
}

FVector2D SOdysseyViewport::GetFlip() const
{
    return mFlipStateUV;
}

FVector2D SOdysseyViewport::GetViewportCenter() const
{
    return FVector2D(mViewport->GetSizeXY().X / 2.0f, mViewport->GetSizeXY().Y / 2.0f);
}

void SOdysseyViewport::AddPan( FVector2D iPanValue )
{
    mPan = mPan + iPanValue;
    UpdateTransform();
    UpdateScrollBars();
    SetFitToViewport(false);
}

void
SOdysseyViewport::SOdysseyViewport::Pan(FVector2D iPanValue)
{
    mPan = iPanValue;
    UpdateTransform();
}

void SOdysseyViewport::ResetPan()
{
    mPan = FVector2D(0.0f, 0.0f);
    UpdateTransform();
    UpdateScrollBars();
    SetFitToViewport(false);
}

void SOdysseyViewport::RotateLeft()
{
    Rotate(- FMath::DegreesToRadians(mRotationStep.Get()));
}

void SOdysseyViewport::RotateRight()
{
    Rotate(FMath::DegreesToRadians(mRotationStep.Get()));
}

void SOdysseyViewport::FlipHorizontal()
{
    mFlipStateUV.X = int(mFlipStateUV.X + 1) % 2;

    if( mAlignWithViewport )
    {
        HandleHorizontalScrollBarScrolled(ScrollbarSpaceRatio - mHorizontalScrollBar->DistanceFromTop());
        SetRotation(-mRotation);
    }

    UpdateTransform();
}

void SOdysseyViewport::FlipVertical()
{
    mFlipStateUV.Y = int(mFlipStateUV.Y + 1) % 2;

    if (mAlignWithViewport)
    {
        HandleVerticalScrollBarScrolled(ScrollbarSpaceRatio - mVerticalScrollBar->DistanceFromTop());
    }

    UpdateTransform();
}

void SOdysseyViewport::ComputeTextureDisplayDimensions( uint32& Width, uint32& Height ) const
{
    Width = 0;
    Height = 0;

    UTexture* texture = GetTexture();
    if (!texture)
        return;

    Width = texture->GetSurfaceWidth() * GetZoom();
    Height = texture->GetSurfaceHeight() * GetZoom();
}

FTransform2D
SOdysseyViewport::GetTransformToDisplayedTexture()
{
    UTexture* texture = GetTexture();
    if (!texture)
        return FTransform2D();


    uint32 width = texture->GetSurfaceWidth();
    uint32 height = texture->GetSurfaceHeight();

    FVector2D translation = (FVector2D(width, height) / 2.0f);
    FTransform2D transform(-translation);

    transform = transform.Concatenate(mTransform);

    translation = GetViewportCenter();
    transform = transform.Concatenate(FTransform2D(translation));

    return transform;
}

FTransform2D
SOdysseyViewport::GetTransformToSourceTexture()
{
    UTexture* texture = GetTexture();
    if (!texture)
        return FTransform2D();

    //Convert the position from the displayed texture size to the the position in the texture source size
    uint32 textureFullWidth = texture->Source.IsValid() ? texture->Source.GetSizeX() : texture->GetSurfaceWidth();
    uint32 textureFullHeight = texture->Source.IsValid() ? texture->Source.GetSizeY() : texture->GetSurfaceHeight();

    switch (texture->PowerOfTwoMode)
    {
    case ETexturePowerOfTwoSetting::None:
        break;

    case ETexturePowerOfTwoSetting::PadToPowerOfTwo:
        textureFullWidth = FMath::RoundUpToPowerOfTwo(textureFullWidth);
        textureFullHeight = FMath::RoundUpToPowerOfTwo(textureFullHeight);
        break;

    case ETexturePowerOfTwoSetting::PadToSquarePowerOfTwo:
        textureFullWidth = textureFullHeight = FMath::Max(FMath::RoundUpToPowerOfTwo(textureFullWidth), FMath::RoundUpToPowerOfTwo(textureFullHeight));
        break;

    default:
        checkf(false, TEXT("Unknown entry in ETexturePowerOfTwoSetting::Type"));
        break;
    }

    uint32 width = texture->GetSurfaceWidth();
    uint32 height = texture->GetSurfaceHeight();

    FVector2D translation = (FVector2D(width, height) / 2.0f);

    FTransform2D transform(FScale2D(width / textureFullWidth, height / textureFullHeight));

    transform = transform.Concatenate(FTransform2D(-translation));
    transform = transform.Concatenate(mTransform);

    translation = GetViewportCenter();
    transform = transform.Concatenate(FTransform2D(translation));

    return transform;
}

FVector2D
SOdysseyViewport::ToLocal(const FVector2D& iPoint) const
{
    FVector2D center = GetViewportCenter();
    FVector2D pos = iPoint - center;

    FVector2D tpos = mTransform.Inverse().TransformPoint(pos);

    return tpos;
}

FVector2D
SOdysseyViewport::ToWorld(const FVector2D& iPoint) const
{
    FVector2D center = GetViewportCenter();
    FVector2D tpos = mTransform.TransformPoint(iPoint) + center;

    return tpos;
}

#undef LOCTEXT_NAMESPACE
