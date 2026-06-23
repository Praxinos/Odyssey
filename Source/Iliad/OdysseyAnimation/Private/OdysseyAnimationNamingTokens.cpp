// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationNamingTokens.h"

//#include "Algo/IndexOf.h"
#include "Engine.h"
#include "Misc/FrameRate.h"
//#include "MovieSceneMetaData.h"
#include "NamingTokensEngineSubsystem.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"

//---

#define LOCTEXT_NAMESPACE "OdysseyAnimationNamingTokens"

/*static*/ FString UOdysseyAnimationNamingTokens::TokenNamespace = TEXT( "odysseyAnimation" );

//---

////static
//FText
//UOdysseyAnimationNamingTokens::GetResolvedText( const FString& InStringToEvaluate, UOdysseyAnimation* InAnimation )
//{
//    UOdysseyAnimationNamingTokensContext* NamingTokenContext = NewObject<UOdysseyAnimationNamingTokensContext>();
//    NamingTokenContext->Animation = InAnimation;
//
//    FNamingTokenFilterArgs FilterArgs;
//    FilterArgs.AdditionalNamespacesToInclude.Add( TokenNamespace );
//
//    UNamingTokensEngineSubsystem* NamingTokensSubsystem = GEngine->GetEngineSubsystem<UNamingTokensEngineSubsystem>();
//    FNamingTokenResultData Result = NamingTokensSubsystem->EvaluateTokenString( InStringToEvaluate, FilterArgs, { NamingTokenContext } );
//
//    return Result.EvaluatedText;
//}

UOdysseyAnimationNamingTokens::UOdysseyAnimationNamingTokens()
{
    Namespace = TokenNamespace;
    NamespaceDisplayName = LOCTEXT( "namespace.label", "Odyssey Animation" );
}

//---

void
UOdysseyAnimationNamingTokens::OnCreateDefaultTokens( TArray<FNamingTokenData>& Tokens ) //override
{
    Super::OnCreateDefaultTokens( Tokens );

    //---

    FNamingTokenData NameToken;
    NameToken.TokenKey = TEXT( "name" );
    NameToken.DisplayName = LOCTEXT( "NameToken", "Name" );
    NameToken.TokenProcessorNative.BindLambda( [this]()
                                               {
                                                   if( !ContextAnimation )
                                                       return FText::GetEmpty();

                                                   UOdysseyAnimation* animation = ContextAnimation->Animation.Pin().Get();
                                                   if( !animation )
                                                       return FText::GetEmpty();

                                                   return FText::FromString( animation->GetName() );
                                               } );
    Tokens.Add( NameToken );

    FNamingTokenData WidthToken;
    WidthToken.TokenKey = TEXT( "width" );
    WidthToken.DisplayName = LOCTEXT( "WidthToken", "Width" );
    WidthToken.TokenProcessorNative.BindLambda( [this]()
                                                {
                                                    if( !ContextAnimation )
                                                        return FText::GetEmpty();

                                                    UOdysseyAnimation* animation = ContextAnimation->Animation.Pin().Get();
                                                    if( !animation )
                                                        return FText::GetEmpty();

                                                    return FText::AsNumber( animation->GetWidth() );
                                                } );
    Tokens.Add( WidthToken );

    FNamingTokenData HeightToken;
    HeightToken.TokenKey = TEXT( "height" );
    HeightToken.DisplayName = LOCTEXT( "HeightToken", "Height" );
    HeightToken.TokenProcessorNative.BindLambda( [this]()
                                                 {
                                                     if( !ContextAnimation )
                                                         return FText::GetEmpty();

                                                     UOdysseyAnimation* animation = ContextAnimation->Animation.Pin().Get();
                                                     if( !animation )
                                                         return FText::GetEmpty();

                                                     return FText::AsNumber( animation->GetHeight() );
                                                 } );
    Tokens.Add( HeightToken );

    FNamingTokenData FrameRateToken;
    FrameRateToken.TokenKey = TEXT( "framerate" );
    FrameRateToken.DisplayName = LOCTEXT( "FrameRateToken", "FrameRate" );
    FrameRateToken.TokenProcessorNative.BindLambda( [this]()
                                                    {
                                                        if( !ContextAnimation )
                                                            return FText::GetEmpty();

                                                        UOdysseyAnimation* animation = ContextAnimation->Animation.Pin().Get();
                                                        if( !animation )
                                                            return FText::GetEmpty();

                                                        FFrameRate animationFrameRate( animation->GetFramesPerSecond() * 100, 100 );
                                                        return animationFrameRate.ToPrettyText();
                                                    } );
    Tokens.Add( FrameRateToken );

    FNamingTokenData CurrentLayerNameToken;
    CurrentLayerNameToken.TokenKey = TEXT( "currentLayerName" );
    CurrentLayerNameToken.DisplayName = LOCTEXT( "CurrentLayerNameToken", "Current layer name" );
    CurrentLayerNameToken.TokenProcessorNative.BindLambda( [this]()
                                                           {
                                                               if( !ContextAnimation )
                                                                   return FText::GetEmpty();

                                                               UOdysseyAnimation* animation = ContextAnimation->Animation.Pin().Get();
                                                               UOdysseyLayerStack* layerStack = animation ? animation->GetLayerStack() : nullptr;
                                                               UOdysseyLayer* currentLayer = layerStack ? layerStack->GetCurrentLayer() : nullptr;
                                                               if( !currentLayer )
                                                                   return FText::GetEmpty();

                                                               return currentLayer->GetLayerName();
                                                           } );
    Tokens.Add( CurrentLayerNameToken );

    //---

    FNamingTokenData CurrentFrameToken;
    CurrentFrameToken.TokenKey = TEXT( "currentFrame" );
    CurrentFrameToken.DisplayName = LOCTEXT( "CurrentFrameToken", "Current frame" );
    CurrentFrameToken.TokenProcessorNative.BindLambda( [this]()
                                                       {
                                                           if( !ContextPlayer )
                                                               return FText::GetEmpty();

                                                           UOdysseyAnimationPlayer* player = ContextPlayer->AnimationPlayer.Pin().Get();
                                                           if( !player )
                                                               return FText::GetEmpty();

                                                           // How to (or should we) use UOdysseyPainterEditorAnimationUserSettings::Get(); to get the startframe ?!?!
                                                           // Or make another "editor" naming token context ?
                                                           FFrameTime current_frametime = player->GetCurrentFrame();
                                                           return FText::AsNumber( current_frametime.FloorToFrame().Value );
                                                       } );
    Tokens.Add( CurrentFrameToken );
}

void
UOdysseyAnimationNamingTokens::OnPreEvaluate_Implementation( const FNamingTokensEvaluationData& InEvaluationData ) //override
{
    Super::OnPreEvaluate_Implementation( InEvaluationData );

    UOdysseyAnimationNamingTokensContext* MatchingContextAnimation = nullptr;
    InEvaluationData.Contexts.FindItemByClass<UOdysseyAnimationNamingTokensContext>( &MatchingContextAnimation );
    ContextAnimation = MatchingContextAnimation;

    UOdysseyAnimationPlayerNamingTokensContext* MatchingContextPlayer = nullptr;
    InEvaluationData.Contexts.FindItemByClass<UOdysseyAnimationPlayerNamingTokensContext>( &MatchingContextPlayer );
    ContextPlayer = MatchingContextPlayer;
}

void
UOdysseyAnimationNamingTokens::OnPostEvaluate_Implementation() //override
{
    Super::OnPostEvaluate_Implementation();

    ContextAnimation = nullptr;
    ContextPlayer = nullptr;
}
//
//void
//UOdysseyAnimationNamingTokens::AddMetadataToken( const FString& InTokenKey )
//{
//    // If the token already exists, remove it, and replace it with the new one
//    const int32 TokenIndex = Algo::IndexOfBy( CustomTokens, InTokenKey, &FNamingTokenData::TokenKey );
//    if( CustomTokens.IsValidIndex( TokenIndex ) )
//    {
//        CustomTokens.RemoveAt( TokenIndex );
//    }
//
//    auto MetadataTokenFunc = [InTokenKey]( TWeakObjectPtr<UCineAssembly> InAssembly ) -> FText
//        {
//            if( InAssembly.IsValid() )
//            {
//                FString ValueString;
//                if( InAssembly->GetMetadataAsString( InTokenKey, ValueString ) )
//                {
//                    // Test if the value string could be an object path, and if it is, return just the filename instead of the full path
//                    FSoftObjectPath ObjectPath = FSoftObjectPath( ValueString );
//                    if( ObjectPath.IsValid() )
//                    {
//                        ValueString = FPaths::GetBaseFilename( ValueString );
//                    }
//
//                    return FText::FromString( ValueString );
//                }
//            }
//
//            return FText::GetEmpty();
//        };
//
//    FNamingTokenData NewToken;
//    NewToken.TokenKey = InTokenKey;
//    NewToken.DisplayName = FText::Format( LOCTEXT( "MetadataTokenDisplayName", "{0} Metadata" ), FText::FromString( InTokenKey ) );
//    NewToken.TokenProcessorNative.BindLambda( [this, MetadataTokenFunc]()
//                                              {
//                                                  return ExecuteTokenFunc( MetadataTokenFunc );
//                                              } );
//
//    CustomTokens.Add( NewToken );
//}

#undef LOCTEXT_NAMESPACE
