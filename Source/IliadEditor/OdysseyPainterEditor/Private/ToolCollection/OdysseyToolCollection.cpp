// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyToolCollection.h"

#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "OdysseyPainterEditorToolConfigurationUtils.h"

/////////////////////////////////////////////////////
// FToolPropertySnapshot

bool FToolPropertySnapshot::operator==(const FToolPropertySnapshot& iOther) const
{
    if (Values.Num() != iOther.Values.Num())
    {
        return false;
    }

    TArray<uint8> bufferA;
    TArray<uint8> bufferB;

    FMemoryWriter memWriterA(bufferA, true);
    FMemoryWriter memWriterB(bufferB, true);

    FObjectAndNameAsStringProxyArchive arA(memWriterA, false);
    FObjectAndNameAsStringProxyArchive arB(memWriterB, false);

    arA.SetIsSaving(true);
    arB.SetIsSaving(true);

    FToolPropertySnapshot::StaticStruct()->SerializeItem(arA, (void*)this, nullptr);
    FToolPropertySnapshot::StaticStruct()->SerializeItem(arB, (void*)&iOther, nullptr);

    return bufferA == bufferB;
}

/////////////////////////////////////////////////////
// OdysseyToolCollection

UOdysseyToolCollection::UOdysseyToolCollection(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

bool UOdysseyToolCollection::IsCollectionTransient() const
{
    if (HasAnyFlags(RF_Transient) || GetOutermost() == GetTransientPackage())
    {
        return true;
    }

    return false;
}

UOdysseyPainterEditorToolConfiguration* UOdysseyToolCollection::AddToolConfiguration(UClass* iToolClass, TObjectPtr<UOdysseyPainterEditorTool> iTool, FSlateBrush& iIcon, int32 iIndex)
{
    if( !iToolClass || !iTool )
        return nullptr;

    if( !mToolsConfig.IsValidIndex( iIndex ) )
        iIndex = INDEX_NONE;

    UOdysseyPainterEditorToolConfiguration* toolConfig = NewObject<UOdysseyPainterEditorToolConfiguration>(this);
    toolConfig->mToolClass = iToolClass;
    toolConfig->mTool = DuplicateObject<UOdysseyPainterEditorTool>(iTool,this);
    toolConfig->mIcon = iIcon;

    Modify();

    if( iIndex == INDEX_NONE )
        mToolsConfig.Add( toolConfig );
    else
        mToolsConfig.Insert( toolConfig, iIndex );

    OnCollectionChanged.Broadcast();

    return toolConfig;
}

void UOdysseyToolCollection::RemoveToolConfigurationAtIndex(int iIndex)
{
    if( iIndex >= mToolsConfig.Num() )
        return;

    Modify();

    mToolsConfig.RemoveAt( iIndex );
    OnCollectionChanged.Broadcast();
}

void UOdysseyToolCollection::RemoveToolConfiguration(UOdysseyPainterEditorToolConfiguration* iToolConfig)
{
    if( !mToolsConfig.Contains(iToolConfig))
        return;

    Modify();

    mToolsConfig.Remove(iToolConfig);
    OnCollectionChanged.Broadcast();
}

void UOdysseyToolCollection::MoveToolConfiguration(int32 iFromIndex, int32 iToIndex)
{
    if (mToolsConfig.IsValidIndex(iFromIndex) && (mToolsConfig.IsValidIndex(iToIndex) || iToIndex == mToolsConfig.Num()) )
    {
        Modify();

        UOdysseyPainterEditorToolConfiguration* toolConfig = mToolsConfig[iFromIndex];
        mToolsConfig.RemoveAt(iFromIndex);

        // adjust index if removal shifted the array
        if (iFromIndex < iToIndex)
        {
            iToIndex--;
        }

        mToolsConfig.Insert(toolConfig, iToIndex);

        OnCollectionChanged.Broadcast();
    }
}

int32 UOdysseyToolCollection::GetIndexOfToolConfiguration(UOdysseyPainterEditorToolConfiguration* iToolConfig)
{
    int32 index = INDEX_NONE;
    mToolsConfig.Find(iToolConfig, index);
    return index;
}

bool UOdysseyToolCollection::ContainsSimilarToolConfiguration(UClass* iToolClass, UOdysseyPainterEditorTool* iTool)
{
    check( iToolClass );

    FToolPropertySnapshot toolToCheckSnapshot;
    UOdysseyToolCollection::ConvertToolToPropertySnapshot( iTool, toolToCheckSnapshot );

    for (UOdysseyPainterEditorToolConfiguration* toolConfig : mToolsConfig)
    {
        FToolPropertySnapshot toolConfigSnapshot;
        UOdysseyToolCollection::ConvertToolToPropertySnapshot( toolConfig->mTool, toolConfigSnapshot );

        if( toolToCheckSnapshot == toolConfigSnapshot )
            return true;
    }

    return false;
}

const TArray<UOdysseyPainterEditorToolConfiguration*> UOdysseyToolCollection::GetToolConfigurations() const
{
    return mToolsConfig;
}

void UOdysseyToolCollection::ConvertToolToPropertySnapshot(UOdysseyPainterEditorTool* iTool, FToolPropertySnapshot& oSnapshot)
{
    oSnapshot.Values.Reset();

    for (TFieldIterator<FProperty> it(iTool->GetClass()); it; ++it)
    {
        FProperty* property = *it;

        if (property->HasMetaData(TEXT("IgnoreToolConfiguration")))
        {
            continue;
        }

        const void* valuePtr = property->ContainerPtrToValuePtr<void>(iTool);

        // Enum
        if (FEnumProperty* enumProperty = CastField<FEnumProperty>(property))
        {
            FToolEnumValue data;
            data.Value = enumProperty->GetUnderlyingProperty()->GetUnsignedIntPropertyValue(valuePtr);

            oSnapshot.Values.Add(property->GetFName(), FInstancedStruct::Make(data));
        }
        // Bool
        else if (FBoolProperty* boolProperty = CastField<FBoolProperty>(property))
        {
            FToolBoolValue data;
            data.Value = boolProperty->GetPropertyValue(valuePtr);

            oSnapshot.Values.Add(property->GetFName(), FInstancedStruct::Make(data));
        }
        // Int
        else if (FIntProperty* intProperty = CastField<FIntProperty>(property))
        {
            FToolIntValue data;
            data.Value = intProperty->GetPropertyValue(valuePtr);

            oSnapshot.Values.Add(property->GetFName(), FInstancedStruct::Make(data));
        }
        // Float
        else if (FFloatProperty* floatProperty = CastField<FFloatProperty>(property))
        {
            FToolFloatValue data;
            data.Value = floatProperty->GetPropertyValue(valuePtr);

            oSnapshot.Values.Add(property->GetFName(), FInstancedStruct::Make(data));
        }
        // Double
        else if (FDoubleProperty* doubleProperty = CastField<FDoubleProperty>(property))
        {
            FToolDoubleValue data;
            data.Value = doubleProperty->GetPropertyValue(valuePtr);

            oSnapshot.Values.Add(property->GetFName(), FInstancedStruct::Make(data));
        }
        // Serialized UObject
        else if (FObjectProperty* objectProperty = CastField<FObjectProperty>(property))
        {
            FToolObjectValue data;
            data.Value = objectProperty->GetPropertyValue(valuePtr);

            oSnapshot.Values.Add(property->GetFName(), FInstancedStruct::Make(data));
        }
        // UStruct
        else if (FStructProperty* structProperty = CastField<FStructProperty>(property))
        {
            FInstancedStruct data;
            data.InitializeAs(structProperty->Struct);

            void* dest = data.GetMutableMemory();
            const void* src = structProperty->ContainerPtrToValuePtr<void>(iTool);

            structProperty->Struct->CopyScriptStruct(dest, src);

            oSnapshot.Values.Add(property->GetFName(), MoveTemp(data));
        }
    }
}
