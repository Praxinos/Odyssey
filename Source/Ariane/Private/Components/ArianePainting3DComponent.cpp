// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianePainting3DStaticMeshComponent.h"
#include "ArianePath.h"
#include "ArianeCycle.h"
#include "ArianeSegment.h"
#include "ArianeVertex.h"
#include "ArianeGroup.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerFolder.h"
#include "ArianeLayerDrawing.h"
// Unreal headers
#include "Engine/EngineBaseTypes.h"
#include "StaticMeshResources.h"
#include "RenderResource.h"
#include "MeshBatch.h"
#include "Kismet/KismetMathLibrary.h"
#include "RawIndexBuffer.h"
#include "Materials/MaterialRenderProxy.h"
#include "HAL/IConsoleManager.h"
#include "Materials/MaterialInstanceDynamic.h"

// testing
#include "Components/LineBatchComponent.h"

UArianePainting3DComponent::~UArianePainting3DComponent()
{
}

UArianePainting3DComponent::UArianePainting3DComponent()
    : LayerStack ( nullptr )
    , DefaultMaterial ( nullptr )
    , CurrentPaletteColorEntry ( nullptr )
    , EditorInterface ( nullptr )
{
    LayerStack = CreateDefaultSubobject<UArianeLayerStack>(TEXT("LayerStack"));

    LayerStack->SetupAttachment( this );

    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.SetTickFunctionEnable(true);

    bAutoRegister = true;
    bWantsInitializeComponent = true;
    bAutoActivate = true;
    bTickInEditor = true;

    StaticMeshComponent = CreateDefaultSubobject<UArianePainting3DStaticMeshComponent>(TEXT("Dummy"));

    StaticMeshComponent->SetVisibility(false);
    StaticMeshComponent->SetHiddenInGame(true);
    StaticMeshComponent->SetCastShadow(false);
    //StaticMeshComponent->bComponentTickEnabled = false;

    StaticMeshComponent->SetupAttachment(this);

/*
    SelectionOverrideDelegate.BindLambda([](const UPrimitiveComponent*) {
        return false; // don't draw the outline
    });
*/

    //LineBatchComponent = CreateDefaultSubobject<ULineBatchComponent>(TEXT("LineBatcher"));
}

UArianePainting3DStaticMeshComponent*
UArianePainting3DComponent::GetStaticMeshComponent()
{
    return StaticMeshComponent;
}

#if WITH_EDITOR
void
UArianePainting3DComponent::SetEditorInterface( IArianePainting3DComponentEditorInterface* InEditorInterface )
{
    EditorInterface = InEditorInterface;
}
#endif

UMaterialInstanceDynamic*
UArianePainting3DComponent::GetDefaultMaterial()
{
    return DefaultMaterial;
}

void
UArianePainting3DComponent::OnRegister()
{
    UMaterial* BaseMaterial = Cast<UMaterial>( StaticLoadObject( UMaterial::StaticClass()
                                                               , nullptr
                                                               , TEXT("/Odyssey/Materials/ArianeDefaultMaterial.ArianeDefaultMaterial") ) );

    Super::OnRegister();

    DefaultMaterial = UMaterialInstanceDynamic::Create( BaseMaterial, this );
    // disable emission
    //DefaultMaterial->SetVectorParameterValue(FName("EmissiveColor"), FLinearColor::Black);
}

/*
void
UArianePainting3DComponent::GetUsedMaterials( TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials ) const
{
    if( DefaultMaterial )
    {
        OutMaterials.Add( DefaultMaterial );
    }

    LayerStack->GetRootFolder()->Traverse( [ &OutMaterials ] ( UArianeLayer* Layer ) -> UArianeLayerFolder::ETraversalReturnValue
        {
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Layer);

            if( DrawingLayer )
            {
                TArray<UMaterialInterface*> DrawingLayerUsedMaterials;

                DrawingLayer->GetUsedMaterials( DrawingLayerUsedMaterials );

                OutMaterials.Append( DrawingLayerUsedMaterials );
            }

            return UArianeLayerFolder::ETraversalReturnValue::Continue;
        } );
}


int32
UArianePainting3DComponent::GetNumMaterials() const
{
    return UsedMaterials.Num();
}

UMaterialInterface*
UArianePainting3DComponent::GetMaterial(int32 ElementIndex) const
{
    if ( UsedMaterials.IsValidIndex( ElementIndex ) )
    {
        return UsedMaterials[ElementIndex];
    }
    return nullptr;
}

void
UArianePainting3DComponent::SetMaterial( int32 ElementIndex, UMaterialInterface* Material )
{
    if (ElementIndex >= 0 )
    {
        if ( ElementIndex >= UsedMaterials.Num() )
        {
            UsedMaterials.SetNum( ElementIndex + 1 );
        }

        UsedMaterials[ElementIndex] = Material;

        // This call will destroy the Proxy and create a new one with the new data (via CreateSceneProxy)
        MarkRenderStateDirty();
    }
}
*/

void
UArianePainting3DComponent::Init()
{
    //LayerStack->Init();

    Update( false );
}

void
UArianePainting3DComponent::PostLoad()
{
    Super::PostLoad();

    Init();
}

void
UArianePainting3DComponent::PostEditUndo()
{
    Super::PostEditUndo();

    Update( false );
}

UArianeLayerStack*
UArianePainting3DComponent::GetLayerStack()
{
    return LayerStack;
}

void
UArianePainting3DComponent::BeginPlay()
{
    Super::BeginPlay();
}

void
UArianePainting3DComponent::OnComponentDestroyed( bool bDestroyingHierarchy )
{
    Super::OnComponentDestroyed( bDestroyingHierarchy );

    LayerStack->OnComponentDestroyed( bDestroyingHierarchy );
}

FBoxSphereBounds
UArianePainting3DComponent::CalcBounds(const FTransform& LocalToWorld) const
{
    //FBoxSphereBounds RetBounds = Super::CalcBounds( FTransform::Identity );
    FBoxSphereBounds RetBounds = FBoxSphereBounds(ForceInit);

    RetBounds = /*RetBounds +*/ LayerStack->GetRootFolder()->GetBounds();

    return RetBounds.TransformBy( LocalToWorld );
}

UArianePainting3DComponent::FOnUpdateDelegate&
UArianePainting3DComponent::OnPreUpdateDelegate()
{
    return OnPreUpdate;
}

UArianePainting3DComponent::FOnUpdateDelegate&
UArianePainting3DComponent::OnPostUpdateDelegate()
{
    return OnPostUpdate;
}

void
UArianePainting3DComponent::Update( bool bInteractive )
{
    OnPreUpdate.Broadcast( bInteractive );

    LayerStack->GetRootFolder()->Update( bInteractive );

    // will call CalcBounds (nb: calling UMeshComponent::UpdateBounds() does not work sometimes, especially when then
    // path starts empty but this works.
    UpdateComponentToWorld();

    OnPostUpdate.Broadcast( bInteractive );

//UpdateBounds();
//MarkRenderTransformDirty();
}

void
UArianePainting3DComponent::PostEditChangeProperty( FPropertyChangedEvent& event )
{
    /*
    if( event.GetPropertyName() == GET_MEMBER_NAME_CHECKED( UArianePainting3DComponent, GeometryMode ) )
    {
        for( FInstancedStruct& InstancedObject : InstancedObjects )
        {
            FArianeObject* Object = InstancedObject.GetMutablePtr<FArianeObject>();

            if( Object->GetClass() == FArianePath::StaticClass() )
            {
                FArianePath* Path = static_cast<FArianePath*>(Object);

                Path->InvalidateAllSegments();
            }
        }

        RootObjectID.GetObject()->Update( true );
    }
    */

    Super::PostEditChangeProperty( event );
}

const TArray<UOdysseyPaletteSet*>&
UArianePainting3DComponent::GetPaletteSets() const
{
    return PaletteSets;
}

UOdysseyPaletteEntryColor*
UArianePainting3DComponent::GetCurrentPaletteColorEntry() const
{
    return CurrentPaletteColorEntry;
}

void
UArianePainting3DComponent::SetCurrentPaletteSet( const FGuid& InCurrentPaletteSet )
{
    CurrentPaletteSet = InCurrentPaletteSet;
}

FGuid
UArianePainting3DComponent::GetCurrentPaletteSet() const
{
    return CurrentPaletteSet;
}

void
UArianePainting3DComponent::AddPaletteSet( UOdysseyPalette* iPalette )
{
}

void
UArianePainting3DComponent::RemovePaletteSet( UOdysseyPaletteSet* PaletteSet )
{
}

void
UArianePainting3DComponent::SetPaletteSet( FGuid Index, UOdysseyPaletteSet* PaletteSet )
{
}

void
UArianePainting3DComponent::SetCurrentPaletteColorEntry( UOdysseyPaletteEntryColor* Entry, FGuid Set )
{
}

FColor
UArianePainting3DComponent::GetHUDForegroundColor()
{
    return EditorInterface ? EditorInterface->GetHUDForegroundColor() : FColor::Black;
}

void
UArianePainting3DComponent::ConvertToStaticMesh()
{
    StaticMeshComponent->ConvertToStaticMesh();
}
