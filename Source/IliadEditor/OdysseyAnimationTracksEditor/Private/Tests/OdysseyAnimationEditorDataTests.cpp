// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Editor.h"
#include "EditorValidatorSubsystem.h"
#include "HAL/FileManager.h"
#include "LevelSequence.h"
#include "Misc/AutomationTest.h"
#include "Misc/PackageName.h"
#include "Misc/Paths.h"
#include "Misc/ScopeExit.h"
#include "MovieScene.h"
#include "ScopedTransaction.h"
#include "UObject/GarbageCollection.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationCellImageRaster.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyAnimationTimelineSectionEditorData.h"
#include "OdysseyAnimationTimelineTrack.h"
#include "OdysseyLayerStack.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationEditorDataTest, "Odyssey.Animation.EditorData.Cuts",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOdysseyAnimationEditorDataTest::RunTest(const FString& Parameters)
{
    TStrongObjectPtr<ULevelSequence> Sequence(NewObject<ULevelSequence>());
    Sequence->Initialize();
    Sequence->GetMovieScene()->SetTickResolutionDirectly(FFrameRate(24, 1));
    TStrongObjectPtr<UOdysseyAnimation> Animation(NewObject<UOdysseyAnimation>());
    Animation->Init(16, 16, EOdysseyAnimationFormat::BGRA8, 24.f);
    UOdysseyAnimationLayer* Layer = CastChecked<UOdysseyAnimationLayer>(Animation->GetLayerStack()->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    UOdysseyAnimationCell* First = Layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
    UOdysseyAnimationCell* Second = Layer->AddCell(UOdysseyAnimationCellImageRaster::StaticClass());
    First->SetExposure(2);
    Second->SetExposure(3);
    TStrongObjectPtr<UOdysseyAnimationTimelineSection> Section(NewObject<UOdysseyAnimationTimelineSection>(Sequence->GetMovieScene()));
    Section->SetRange(TRange<FFrameNumber>(0, 20));
    Section->SetAnimation(Animation.Get());
    TestNull(TEXT("Section starts without editor data"), Section->GetEditorData());
    UOdysseyAnimationTimelineSectionEditorData* Data = UOdysseyAnimationTimelineSectionEditorData::GetOrCreate(*Section);
    TestEqual(TEXT("GetOrCreate reuses the same object"), UOdysseyAnimationTimelineSectionEditorData::GetOrCreate(*Section), Data);
    TestEqual(TEXT("Section retains the editor object"), Section->GetEditorData(), static_cast<UObject*>(Data));
    TestEqual(TEXT("Editor object is outered to its section"), Data->GetOuter(), static_cast<UObject*>(Section.Get()));
    TestTrue(TEXT("Editor object is transient"), Data->HasAnyFlags(RF_Transient));
    TestTrue(TEXT("Editor object is editor only"), Data->IsEditorOnly());
    TestEqual(TEXT("Cut type belongs to the editor module"), FAnimationCut::StaticStruct()->GetOutermost()->GetName(), FString(TEXT("/Script/OdysseyAnimationTracksEditor")));
    TestEqual(TEXT("Channel type belongs to the editor module"), FOdysseyAnimationCutChannel::StaticStruct()->GetOutermost()->GetName(), FString(TEXT("/Script/OdysseyAnimationTracksEditor")));
    TWeakObjectPtr<UOdysseyAnimationTimelineSectionEditorData> WeakData(Data);
    CollectGarbage(RF_NoFlags);
    if (!TestTrue(TEXT("Section property keeps editor data alive through GC"), WeakData.IsValid())) { return false; }
    FOdysseyAnimationCutChannel& Channel = Data->GetAnimationCutChannel();
    if (!TestEqual(TEXT("Editor data builds all cuts"), Channel.GetNumKeys(), 3)) { return false; }
    const TArray<FKeyHandle> Handles = {Channel.GetHandle(1)};
    Channel.SetKeyTimes(Handles, TArray<FFrameNumber>{FFrameNumber(3)});
    Data->UpdateAnimationCutChannel(Handles, EPropertyChangeType::Interactive);
    TestEqual(TEXT("Interactive drag preserves handles"), Channel.GetHandle(1), Handles[0]);
    Data->UpdateAnimationCutChannel(Handles, EPropertyChangeType::ValueSet);
    TestEqual(TEXT("Previous exposure follows the cut"), First->GetExposure(), 3);
    TestEqual(TEXT("Following exposure follows the cut"), Second->GetExposure(), 2);
    Channel.SetKeyTimes(Handles, TArray<FFrameNumber>{FFrameNumber(99)});
    Data->UpdateAnimationCutChannel(Handles, EPropertyChangeType::ValueSet);
    TestEqual(TEXT("Constraints prevent empty cells"), Second->GetExposure(), 1);
    TestEqual(TEXT("Constraints clamp the key time"), Channel.GetTimes()[1], FFrameNumber(4));
    First->SetExposure(5);
    TestEqual(TEXT("Composition changes rebuild cuts"), Channel.GetTimes()[1], FFrameNumber(5));
    Section->SetStartFrameOffset(1);
    TestEqual(TEXT("Section signature changes rebuild cuts"), Channel.GetTimes()[1], FFrameNumber(4));
    Section->SetStartFrameOffset(0);
    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyAnimationEditorDataTests", "EditExposure", "Edit exposure for cut test"));
        First->Modify();
        First->SetExposure(3);
    }
    GEditor->UndoTransaction();
    TestEqual(TEXT("Undo restores exposure"), First->GetExposure(), 5);
    TestEqual(TEXT("Undo rebuilds cuts"), Channel.GetTimes()[1], FFrameNumber(5));
    TStrongObjectPtr<UOdysseyAnimationTimelineSection> Copy(DuplicateObject<UOdysseyAnimationTimelineSection>(Section.Get(), Sequence->GetMovieScene()));
    TestNull(TEXT("Duplication resets the generic editor data reference"), Copy->GetEditorData());
    UOdysseyAnimationTimelineSectionEditorData* CopyData = UOdysseyAnimationTimelineSectionEditorData::GetOrCreate(*Copy);
    TestNotEqual(TEXT("Duplicated section owns separate editor data"), CopyData, Data);
    TestEqual(TEXT("Duplicated editor data has the new owner"), CopyData->GetOuter(), static_cast<UObject*>(Copy.Get()));
    TestEqual(TEXT("Duplicated cuts are reconstructed"), CopyData->GetAnimationCutChannel().GetNumKeys(), 3);
    Copy->SetAnimation(nullptr);
    Section->SetAnimation(nullptr);
    TestEqual(TEXT("Clearing animation clears its cuts"), Channel.GetNumKeys(), 0);
    First->SetExposure(6);
    TestEqual(TEXT("Detached animation does not repopulate cuts"), Channel.GetNumKeys(), 0);
    Data->UpdateAnimationCutChannel({}, EPropertyChangeType::ValueSet);
    const FProperty* Property = FindFProperty<FProperty>(Section->GetClass(), TEXT("EditorData"));
    if (TestNotNull(TEXT("Generic editor data property"), Property))
    {
        TestTrue(TEXT("Property is excluded from cooked data"), Property->HasAnyPropertyFlags(CPF_EditorOnly));
        TestTrue(TEXT("Property is not saved"), Property->HasAnyPropertyFlags(CPF_Transient));
    }
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationEditorDataPersistenceTest, "Odyssey.Animation.EditorData.Persistence",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FOdysseyAnimationEditorDataPersistenceTest::RunTest(const FString& Parameters)
{
    FScopedDisableValidateOnSave DisableValidation;
    const FString Id = FGuid::NewGuid().ToString(EGuidFormats::Digits);
    const FString Mount = TEXT("/OdysseyCutDataTest/");
    const FString Directory = FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() / TEXT("Odyssey-Plugin/Intermediate/OdysseyCutDataTests/"));
    FPackageName::RegisterMountPoint(Mount, Directory + TEXT("/"));
    ON_SCOPE_EXIT { FPackageName::UnRegisterMountPoint(Mount, Directory + TEXT("/")); };
    const FString PackageName = Mount + Id;
    UPackage* Package = CreatePackage(*PackageName);
    TStrongObjectPtr<ULevelSequence> Sequence(NewObject<ULevelSequence>(Package, TEXT("Sequence"), RF_Public | RF_Transactional));
    Sequence->Initialize();
    UOdysseyAnimationTimelineTrack* Track = Sequence->GetMovieScene()->AddTrack<UOdysseyAnimationTimelineTrack>();
    if (!TestNotNull(TEXT("Create sequence track"), Track)) { return false; }
    UOdysseyAnimationTimelineSection* Section = CastChecked<UOdysseyAnimationTimelineSection>(Track->CreateNewSection());
    Section->SetRange(TRange<FFrameNumber>(0, 20));
    Track->AddSection(*Section);
    UOdysseyAnimationTimelineSectionEditorData* Data = UOdysseyAnimationTimelineSectionEditorData::GetOrCreate(*Section);
    TestNotNull(TEXT("Editor data exists before saving"), Data);
    const FString Filename = Directory / (Id + TEXT(".uasset"));
    IFileManager::Get().MakeDirectory(*Directory, true);
    UPackage* LoadedPackage = nullptr;
    ON_SCOPE_EXIT
    {
        if (LoadedPackage) { ResetLoaders(LoadedPackage); }
        ResetLoaders(Package);
        IFileManager::Get().Delete(*Filename);
    };
    FSavePackageArgs Args;
    Args.TopLevelFlags = RF_Public;
    Args.SaveFlags = SAVE_NoError;
    if (!TestTrue(TEXT("Save sequence containing editor data"), UPackage::SavePackage(Package, Sequence.Get(), *Filename, Args))) { return false; }
    Package->Rename(*(PackageName + TEXT("_Original")), nullptr, REN_DontCreateRedirectors | REN_NonTransactional);
    LoadedPackage = LoadPackage(nullptr, *PackageName, LOAD_None);
    if (!TestNotNull(TEXT("Reload sequence package"), LoadedPackage)) { return false; }
    ULevelSequence* LoadedSequence = FindObject<ULevelSequence>(LoadedPackage, TEXT("Sequence"));
    if (!TestNotNull(TEXT("Reload sequence"), LoadedSequence)) { return false; }
    const TArray<UMovieSceneTrack*>& Tracks = LoadedSequence->GetMovieScene()->GetTracks();
    if (!TestEqual(TEXT("Sequence track persists"), Tracks.Num(), 1)) { return false; }
    UOdysseyAnimationTimelineSection* LoadedSection = CastChecked<UOdysseyAnimationTimelineSection>(Tracks[0]->GetAllSections()[0]);
    TestNull(TEXT("Editor data is not persisted in the asset"), LoadedSection->GetEditorData());
    UOdysseyAnimationTimelineSectionEditorData* LoadedData = UOdysseyAnimationTimelineSectionEditorData::GetOrCreate(*LoadedSection);
    TestNotNull(TEXT("Editor data is recreated after loading"), LoadedData);
    TestEqual(TEXT("Recreated data belongs to the loaded section"), LoadedData->GetOuter(), static_cast<UObject*>(LoadedSection));
    return true;
}
#endif
