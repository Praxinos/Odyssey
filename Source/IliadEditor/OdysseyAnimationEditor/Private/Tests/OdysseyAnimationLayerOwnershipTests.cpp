// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UObject/CoreRedirects.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationLayerStack.h"
#include "OdysseyLayer.h"
#include "OdysseyLayerStack.h"
#include "OdysseyLighttable.h"
#include "OdysseyAnimationCellImageStagger.h"
#include "OdysseyAnimationLayerStackUserData.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationLayerOwnershipTest, "Odyssey.Animation.LayerOwnership",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** Exercises moved reflection types together with layer editing and cell frame ranges. */
bool FOdysseyAnimationLayerOwnershipTest::RunTest(const FString& Parameters)
{
    TestNull(TEXT("Generic layers have no cells"), UOdysseyLayer::StaticClass()->FindPropertyByName(TEXT("Cells")));
    TestNull(TEXT("Generic layers have no lighttable"), UOdysseyLayer::StaticClass()->FindPropertyByName(TEXT("Lighttable")));
    FProperty* CellsProperty = UOdysseyAnimationLayer::StaticClass()->FindPropertyByName(TEXT("Cells"));
    if (!TestNotNull(TEXT("Animation layers own cells"), CellsProperty))
        return false;
    TestEqual(TEXT("Cells are declared by the animation layer"), CellsProperty->GetOwnerClass(), UOdysseyAnimationLayer::StaticClass());
    TestEqual(TEXT("Cell module is editor only"), UOdysseyAnimationCell::StaticClass()->GetOutermost()->GetName(), FString(TEXT("/Script/OdysseyAnimationEditor")));
    TestEqual(TEXT("Layer stack module is editor only"), UOdysseyLayerStack::StaticClass()->GetOutermost()->GetName(), FString(TEXT("/Script/OdysseyLayerStack")));
    TestTrue(TEXT("Layer stack classes are not cooked"), UOdysseyLayerStack::StaticClass()->GetOutermost()->HasAnyPackageFlags(PKG_EditorOnly));
    TestTrue(TEXT("Animation cell classes are not cooked"), UOdysseyAnimationCell::StaticClass()->GetOutermost()->HasAnyPackageFlags(PKG_EditorOnly));
    TestTrue(TEXT("Animation editor data is not cooked"), UOdysseyAnimationLayerStackUserData::StaticClass()->GetOutermost()->HasAnyPackageFlags(PKG_EditorOnly));
    TestEqual(TEXT("Runtime animation layer stack class is redirected"),
        FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Class, FCoreRedirectObjectName(TEXT("/Script/OdysseyAnimation.OdysseyAnimationLayerStack"))).ToString(),
        UOdysseyAnimationLayerStack::StaticClass()->GetPathName());
    TestEqual(TEXT("Runtime animation raster cell class is redirected"),
        FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Class, FCoreRedirectObjectName(TEXT("/Script/OdysseyAnimation.OdysseyAnimationCellImageRaster"))).ToString(),
        UOdysseyAnimationCellImageRaster::StaticClass()->GetPathName());
    TestEqual(TEXT("Runtime lighttable struct is redirected"),
        FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Struct, FCoreRedirectObjectName(TEXT("/Script/OdysseyAnimation.OdysseyLighttable"))).ToString(),
        FOdysseyLighttable::StaticStruct()->GetPathName());
    TestEqual(TEXT("Runtime post behaviour enum is redirected"),
        FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Enum, FCoreRedirectObjectName(TEXT("/Script/OdysseyAnimation.EOdysseyLayerImagePostBehaviour"))).ToString(),
        StaticEnum<EOdysseyLayerImagePostBehaviour>()->GetPathName());
    const FCoreRedirectObjectName OldCell(TEXT("/Script/OdysseyLayerStack.OdysseyLayerCell"));
    const FCoreRedirectObjectName NewCell = FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Class, OldCell);
    TestEqual(TEXT("Saved cell class references are redirected"), NewCell.ToString(), UOdysseyAnimationCell::StaticClass()->GetPathName());

    TestEqual(TEXT("Animation cell directly owns the former base API"), UOdysseyAnimationCell::StaticClass()->GetSuperClass(), UObject::StaticClass());
    const FCoreRedirectObjectName PreviousCell(TEXT("/Script/OdysseyAnimation.OdysseyLayerCell"));
    TestEqual(TEXT("Previous animation module cell is redirected"),
        FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Class, PreviousCell).ToString(),
        UOdysseyAnimationCell::StaticClass()->GetPathName());
    const FCoreRedirectObjectName PreviousStagger(TEXT("/Script/OdysseyAnimation.OdysseyLayerCellImageStagger"));
    TestEqual(TEXT("Previous stagger class is redirected"),
        FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Class, PreviousStagger).ToString(),
        UOdysseyAnimationCellImageStagger::StaticClass()->GetPathName());
    const FCoreRedirectObjectName PreviousPegs(TEXT("/Script/OdysseyAnimation.OdysseyLayerCellOutOfPegs"));
    TestEqual(TEXT("Out-of-pegs struct is redirected"),
        FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Struct, PreviousPegs).ToString(),
        FOdysseyAnimationCellOutOfPegs::StaticStruct()->GetPathName());
    const FCoreRedirectObjectName PreviousBehaviour(TEXT("/Script/OdysseyAnimation.EOdysseyLayerCellImageStaggerBehaviour"));
    TestEqual(TEXT("Stagger behaviour enum is redirected"),
        FCoreRedirects::GetRedirectedName(ECoreRedirectFlags::Type_Enum, PreviousBehaviour).ToString(),
        StaticEnum<EOdysseyAnimationCellImageStaggerBehaviour>()->GetPathName());

    TStrongObjectPtr<UOdysseyAnimation> Animation(NewObject<UOdysseyAnimation>(GetTransientPackage()));
    Animation->Init(16, 16, EOdysseyAnimationFormat::BGRA8, 24.f);
    UOdysseyAnimationLayer* Layer = CastChecked<UOdysseyAnimationLayer>(UOdysseyAnimationLayerStackUserData::GetLayerStack(Animation.Get())->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    UOdysseyAnimationLayer* Destination = CastChecked<UOdysseyAnimationLayer>(UOdysseyAnimationLayerStackUserData::GetLayerStack(Animation.Get())->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    UOdysseyAnimationCell* First = Layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
    UOdysseyAnimationCell* Second = Layer->AddCell(UOdysseyAnimationCellImageStagger::StaticClass());
    if (!TestNotNull(TEXT("Raster cell"), First) || !TestNotNull(TEXT("Stagger cell"), Second))
        return false;
    TestEqual(TEXT("Raster cell resolves its animation"), First->GetAnimation(), Animation.Get());
    TestEqual(TEXT("Stagger inherits the merged animation cell API"), Second->GetAnimation(), Animation.Get());
    TStrongObjectPtr<UOdysseyAnimationCellImageStagger> Detached(NewObject<UOdysseyAnimationCellImageStagger>(GetTransientPackage()));
    TestNull(TEXT("Detached cell has no animation"), Detached->GetAnimation());
    Layer->SetCellsOffset(5);
    First->SetExposure(2);
    Second->SetExposure(3);
    TestEqual(TEXT("Cell resolves its animation layer"), First->GetLayer(), Layer);
    TestEqual(TEXT("Frame before layer is empty"), Layer->GetCellIndexAtFrame(4), INDEX_NONE);
    TestTrue(TEXT("Exposures contribute to layer range"), Layer->GetFrameRange() == FInt32Range::Inclusive(5, 9));
    TestTrue(TEXT("Second cell follows first exposure"), Second->GetFrameRange() == FInt32Range::Inclusive(7, 9));
    First->SetExposure(4);
    TestTrue(TEXT("Exposure edit invalidates cached cell ranges"), Second->GetFrameRange() == FInt32Range::Inclusive(9, 11));
    TestNull(TEXT("Frame after layer is empty"), Layer->GetCellAtFrame(12));
    TestEqual(TEXT("Hold before animation"), Layer->GetPreBehaviourFrame(EOdysseyLayerImagePostBehaviour::Hold, 4), 5);
    TestEqual(TEXT("Loop after animation"), Layer->GetPostBehaviourFrame(EOdysseyLayerImagePostBehaviour::Loop, 12), 5);

    UOdysseyAnimationCell* Copy = Layer->CopyCellToLayer(First, Destination);
    if (!TestNotNull(TEXT("Copy to another animation layer"), Copy))
        return false;
    TestEqual(TEXT("Copied cell belongs to destination"), Copy->GetLayer(), Destination);
    TestEqual(TEXT("Copied exposure is preserved"), Copy->GetExposure(), 4);
    Destination->SetIsLocked(true);
    TestNull(TEXT("Locked layer rejects cell creation"), Destination->AddCell(UOdysseyAnimationCellImageRaster::StaticClass()));
    Destination->SetIsLocked(false);
    Destination->RemoveCell(Copy);
    TestTrue(TEXT("Removal updates destination cells"), Destination->GetCells().IsEmpty());
    TestTrue(TEXT("Empty destination has an empty frame range"), Destination->GetFrameRange().IsEmpty());
    return true;
}
#endif
