// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyAnimationFactory.h"

#include "OdysseyAnimation.h"
#include "Widgets/SOdysseyAnimationConfigureWindow.h"

#include "Editor.h"
#include "EditorStyleSet.h"

#include <ULIS>

/////////////////////////////////////////////////////
// UOdysseyAnimationFactory
UOdysseyAnimationFactory::UOdysseyAnimationFactory( const FObjectInitializer& iObjectInitializer )
    : Super( iObjectInitializer )
{
    // From UFactory
    bCreateNew = true;
    bEditAfterNew = true;
    SupportedClass = UOdysseyAnimation::StaticClass();
}

bool UOdysseyAnimationFactory::ConfigureProperties()
{
    //We go in here before creating the asset: Meaning we can have any modal window here.
    //If return false, we don't create the object, if true, we create it
    TSharedPtr<SOdysseyAnimationConfigureWindow> configurationWindow = SNew( SOdysseyAnimationConfigureWindow );
    GEditor->EditorAddModalWindow( configurationWindow.ToSharedRef() );
    mConfiguration = configurationWindow->GetConfiguration();
    return configurationWindow->GetWindowAnswer();
}

FString
UOdysseyAnimationFactory::GetDefaultNewAssetName() const
{
    return !mConfiguration.Name.ToString().IsEmpty() ? mConfiguration.Name.ToString() : Super::GetDefaultNewAssetName();
}

UObject*
UOdysseyAnimationFactory::FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn )
{
    check(iClass->IsChildOf(UOdysseyAnimation::StaticClass()));

    // Init internal data
    /* ::ULIS::FBlock block( mWidth, mHeight, mFormat );
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(block.Format());
    ctx.Fill(block, mBackgroundColor );
    ctx.Finish(); */
    
    UOdysseyAnimation* animation = NewObject<UOdysseyAnimation>( iParent, iName, iFlags | RF_Transactional );
    animation->Init(mConfiguration);
	//animation->PostEditChange();

    return animation;
}
