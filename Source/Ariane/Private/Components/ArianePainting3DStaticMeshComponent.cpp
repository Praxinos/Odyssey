// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "ArianePainting3DStaticMeshComponent.h"
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeCycle.h"
#include "ArianeGroup.h"
#include "ArianeSegment.h"
#include "ArianeDrawing.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerVector.h"
#include "ArianeLayerFolder.h"
// Unreal Headers
#include "MeshDescription.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/Material.h"
#include "Materials/MaterialExpressionScalarParameter.h"
#include "Materials/MaterialExpressionSetMaterialAttributes.h"
#include "Materials/MaterialAttributeDefinitionMap.h"

#define CYCLE_PDO 0.1f
#define PDO_PARAMETER_NAME TEXT("PixelDepthOffset")

#if WITH_EDITOR

void
UArianePainting3DStaticMeshComponent::ConvertCycleToStaticMesh( FMeshDescriptionBuilder& MeshDescriptionBuilder
                                                              , FArianeCycle* Cycle )
{
    FUniqueMaterial* UniqueMaterial = GetUniqueMaterial( Cycle->GetMaterial(), Cycle->GetColor(), CYCLE_PDO );
    const FTransform& CycleTransform = Cycle->GetTransform();
    TArray<int32> MeshVertexIDs;
    const TArray<FDynamicMeshVertex> CycleMeshVertices = Cycle->GetGeometry3D().GetMeshVertices();
    const TArray<uint32>& CycleMeshIndices = Cycle->GetGeometry3D().GetMeshIndices();
    uint32 CycleVertexCount = CycleMeshVertices.Num();
    uint32 CycleIndexCount = CycleMeshIndices.Num();

    // Count the number of vertices for allocation
    MeshVertexIDs.Reserve( CycleVertexCount );

    for ( const FDynamicMeshVertex& DynamicMeshVertex : CycleMeshVertices )
    {
        FVector VertewWorldPosition = CycleTransform.TransformPosition( FVector( DynamicMeshVertex.Position ) );
        int32 MeshVertexID = MeshDescriptionBuilder.AppendVertex( VertewWorldPosition );

        MeshVertexIDs.Add( MeshVertexID );
    }

    // Allocate a polygon group
    FPolygonGroupID PolygonGroup = MeshDescriptionBuilder.AppendPolygonGroup( UniqueMaterial->MaterialInterface->GetFName() );

    for ( int32 i = 0; i < CycleMeshIndices.Num(); i += 3 )
    {
        FVertexInstanceID MeshVertexInstanceID[3];

        for( uint32 j = 0; j < 3; j++ )
        {
            int32 CycleModelVertexID = CycleMeshIndices[i + j];
            const FDynamicMeshVertex& CycleModelVertex = CycleMeshVertices[CycleModelVertexID];
            int32 MeshVertexID = MeshVertexIDs[CycleModelVertexID];
            FVector WorldNormal = CycleTransform.TransformVector( CycleModelVertex.TangentZ.ToFVector() );

            MeshVertexInstanceID[j] = MeshDescriptionBuilder.AppendInstance( MeshVertexID );

            MeshDescriptionBuilder.SetInstanceNormal( MeshVertexInstanceID[j], WorldNormal );
            MeshDescriptionBuilder.SetInstanceUV( MeshVertexInstanceID[j]
                                                , FVector2D( CycleModelVertex.TextureCoordinate[0] )
                                                , 0 );
            MeshDescriptionBuilder.SetInstanceColor( MeshVertexInstanceID[j], FLinearColor( Cycle->GetColor() ) );
        }

        MeshDescriptionBuilder.AppendTriangle( MeshVertexInstanceID[2]
                                             , MeshVertexInstanceID[1]
                                             , MeshVertexInstanceID[0]
                                             , PolygonGroup );
    }
}

void
UArianePainting3DStaticMeshComponent::ConvertPathToStaticMesh( FMeshDescriptionBuilder& MeshDescriptionBuilder
                                                             , FArianePath* Path )
{
    FUniqueMaterial* UniqueMaterial = GetUniqueMaterial( Path->GetMaterial(), Path->GetColor(), 0.0f );
    const FTransform& PathTransform = Path->GetTransform();
    TArray<int32> MeshVertexIDs;
    uint32 PathVertexCount = 0;
    uint32 PathIndexCount = 0;

    // Count the number of vertices for allocation
    for ( FArianeSegment* Segment : Path->GetSegments() )
    {
        PathVertexCount += Segment->GetModelVertexCache().Num();
        PathIndexCount += Segment->GetIndexCache().Num();
    }

    // allocate memory IDs;
    MeshVertexIDs.Reserve( PathVertexCount );

    for ( FArianeSegment* Segment : Path->GetSegments() )
    {
        for( const FDynamicMeshVertex& DynamicMeshVertex : Segment->GetModelVertexCache() )
        {
            FVector VertewWorldPosition = PathTransform.TransformPosition( FVector( DynamicMeshVertex.Position ) );
            int32 MeshVertexID = MeshDescriptionBuilder.AppendVertex( VertewWorldPosition );

            MeshVertexIDs.Add( MeshVertexID );
        }
    }

    uint32 VertexIDOffset = 0;

    // Allocate a polygon group
    FPolygonGroupID PolygonGroup = MeshDescriptionBuilder.AppendPolygonGroup( UniqueMaterial->MaterialInterface->GetFName() );

    for ( FArianeSegment* Segment : Path->GetSegments() )
    {
        const TArray<uint32>& SegmentIndexCache = Segment->GetIndexCache();
        const TArray<FDynamicMeshVertex>& SegmentModelVertexCache = Segment->GetModelVertexCache();

        for( int32 i = 0; i < SegmentIndexCache.Num(); i += 3 )
        {
            FVertexInstanceID MeshVertexInstanceID[3];

            for( uint32 j = 0; j < 3; j++ )
            {
                uint32 SegmentModelVertexID = SegmentIndexCache[i + j];
                const FDynamicMeshVertex& SegmentModelVertex = SegmentModelVertexCache[SegmentModelVertexID];
                int32 MeshVertexID = MeshVertexIDs[VertexIDOffset + SegmentModelVertexID];
                FVector WorldNormal = PathTransform.TransformVector( SegmentModelVertex.TangentZ.ToFVector() );

                MeshVertexInstanceID[j] = MeshDescriptionBuilder.AppendInstance( MeshVertexID );

                MeshDescriptionBuilder.SetInstanceNormal( MeshVertexInstanceID[j], WorldNormal );
                MeshDescriptionBuilder.SetInstanceUV( MeshVertexInstanceID[j]
                                                    , FVector2D( SegmentModelVertex.TextureCoordinate[0] )
                                                    , 0 );
                MeshDescriptionBuilder.SetInstanceColor( MeshVertexInstanceID[j], FLinearColor( SegmentModelVertex.Color ) );
            }

            MeshDescriptionBuilder.AppendTriangle( MeshVertexInstanceID[0]
                                                 , MeshVertexInstanceID[1]
                                                 , MeshVertexInstanceID[2]
                                                 , PolygonGroup );
        }

        VertexIDOffset += SegmentModelVertexCache.Num();
    }
}

void
UArianePainting3DStaticMeshComponent::ConvertDrawingToStaticMesh( FMeshDescriptionBuilder& MeshDescriptionBuilder
                                                              , UArianeLayerVector* VectorLayer )
{
    FArianeGroup* RootGroup = VectorLayer->GetDrawing()->GetRootGroup();

    FArianeObject::Traverse( RootGroup
                           , FArianeObject::ETraversalPolicy::AllChildren
                           , [ this
                             , &MeshDescriptionBuilder ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
        {
            // convert path and path-like primitives
            if( Object->HasBaseClass( FArianePath::StaticClass() ) )
            {
                FArianePath* Path = static_cast<FArianePath*>(Object);

                ConvertPathToStaticMesh( MeshDescriptionBuilder, Path );
            }

            // convert path and path-like primitives
            if( Object->HasBaseClass( FArianeCycle::StaticClass() ) )
            {
                FArianeCycle* Cycle = static_cast<FArianeCycle*>(Object);

                ConvertCycleToStaticMesh( MeshDescriptionBuilder, Cycle );
            }

            return FArianeObject::ETraversalReturnValue::Continue;
        } );
}

UArianePainting3DStaticMeshComponent::FUniqueMaterial*
UArianePainting3DStaticMeshComponent::GetUniqueMaterial( UMaterialInterface* OriginialMaterialInterface
                                                       , const FColor& RequestedColor
                                                       , float RequestedPDO )
{
    FUniqueMaterial* UniqueMaterial = UniqueMaterials.FindByPredicate( [ OriginialMaterialInterface
                                                                       , RequestedColor
                                                                       , RequestedPDO ]( const FUniqueMaterial& UniqueMaterial )
        {
            return ( UniqueMaterial.OriginalMaterialInterface == OriginialMaterialInterface )
                && ( UniqueMaterial.Color == RequestedColor )
                && ( UniqueMaterial.PixelDepthOffset == RequestedPDO );
        } );

    return UniqueMaterial;
}

UMaterialInterface*
UArianePainting3DStaticMeshComponent::GetOrCreatePDOMaterial( UMaterialInterface* OriginalMaterialInterface
                                                            , TArray<UMaterialInterface*>& OutDuplicatedPDOMaterials )
{
    // Look for a copy already created for this material
    for( const FPDOMaterial& PDOMaterial : DuplicatedPDOMaterials )
    {
        if( PDOMaterial.OriginalMaterialInterface == OriginalMaterialInterface )
        {
            return PDOMaterial.DuplicatedMaterialInterface;
        }
    }

    UMaterial* BaseMaterial = OriginalMaterialInterface->GetMaterial();

    if( !BaseMaterial )
    {
        return OriginalMaterialInterface;
    }

    // Duplicate the base material in the transient package
    UMaterial* Material = DuplicateObject<UMaterial>( BaseMaterial
                                                    , GetTransientPackage()
                                                    , MakeUniqueObjectName( GetTransientPackage()
                                                                          , UMaterial::StaticClass()
                                                                          , *( BaseMaterial->GetName() + TEXT("_PDO") ) ) );

    // Add a "PixelDepthOffset" scalar parameter and plug it into the PDO input
    UMaterialExpressionScalarParameter* PDOParameter = NewObject<UMaterialExpressionScalarParameter>( Material );
    PDOParameter->Material      = Material;
    PDOParameter->ParameterName = PDO_PARAMETER_NAME;
    PDOParameter->DefaultValue  = 0.0f;
    PDOParameter->MaterialExpressionEditorX = -300;
    PDOParameter->MaterialExpressionEditorY = 600;
    PDOParameter->UpdateParameterGuid( true, true );

    Material->GetExpressionCollection().AddExpression( PDOParameter );

    UMaterialEditorOnlyData* EditorOnlyData = Material->GetEditorOnlyData();

    if( Material->bUseMaterialAttributes )
    {
        // When "Use Material Attributes" is checked, the material output node only has a single
        // "Material Attributes" input: the compiler reads every attribute (BaseColor, Roughness,
        // PixelDepthOffset...) from the expression plugged into EditorOnlyData->MaterialAttributes
        // and ignores the individual inputs such as EditorOnlyData->PixelDepthOffset.
        // Plugging the parameter into EditorOnlyData->PixelDepthOffset would therefore have no effect.
        //
        // Instead, we inject the PDO into the attributes flow by inserting a SetMaterialAttributes node
        // right before the output:
        //
        //     [current attributes] --> SetMaterialAttributes --> Output (Material Attributes)
        //                                     ^
        //     PixelDepthOffset parameter -----+ (PixelDepthOffset pin)
        //
        // This works whatever produces the attributes (MakeMaterialAttributes, material function,
        // layer blend...) without modifying the rest of the graph.
        UMaterialExpressionSetMaterialAttributes* SetAttributes = NewObject<UMaterialExpressionSetMaterialAttributes>( Material );
        SetAttributes->Material = Material;
        SetAttributes->MaterialExpressionEditorX = -200;
        SetAttributes->MaterialExpressionEditorY = 0;

        // Inputs[0] is created by the constructor and receives the incoming attributes
        SetAttributes->Inputs[0].Expression  = EditorOnlyData->MaterialAttributes.Expression;
        SetAttributes->Inputs[0].OutputIndex = EditorOnlyData->MaterialAttributes.OutputIndex;

        // Each additional pin Inputs[i] overrides the attribute AttributeSetTypes[i - 1]
        const FGuid PDOAttributeID = FMaterialAttributeDefinitionMap::GetID( MP_PixelDepthOffset );
        SetAttributes->AttributeSetTypes.Add( PDOAttributeID );

        FExpressionInput& PDOInput = SetAttributes->Inputs.AddDefaulted_GetRef();
        PDOInput.InputName  = FName( *FMaterialAttributeDefinitionMap::GetDisplayNameForMaterial( PDOAttributeID, Material ).ToString() );
        PDOInput.Expression = PDOParameter;

        Material->GetExpressionCollection().AddExpression( SetAttributes );

        EditorOnlyData->MaterialAttributes.Expression  = SetAttributes;
        EditorOnlyData->MaterialAttributes.OutputIndex = 0;
    }
    else
    {
        EditorOnlyData->PixelDepthOffset.Expression = PDOParameter;
    }

    Material->PreEditChange( nullptr );
    Material->PostEditChange();

    DuplicatedPDOMaterials.Add( FPDOMaterial( OriginalMaterialInterface, Material ) );
    OutDuplicatedPDOMaterials.Add( Material );

    return Material;
}

UArianePainting3DStaticMeshComponent::FUniqueMaterial*
UArianePainting3DStaticMeshComponent::CreateUniqueMaterial( UArianePainting3DComponent* Painting3DComponent
                                                          , UMaterialInterface* OriginialMaterialInterface
                                                          , const FColor& RequestedColor
                                                          , float RequestedPDO
                                                          , TArray<UMaterialInterface*>& OutMaterialInstanceConstants
                                                          , TArray<UMaterialInterface*>& OutDuplicatedPDOMaterials )
{
    FLinearColor LinearColor = FLinearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    float PixelDepthOffset = 0.0f;
    FUniqueMaterial* UniqueMaterial = GetUniqueMaterial( OriginialMaterialInterface, RequestedColor, RequestedPDO );

    // if no material with the requested color was found, create one only if the requested color differs from the default one
    // or the material does not need any vector parameter of name "color". Otherwise, just use the original material as is.
    if( UniqueMaterial == nullptr  )
    {
        // 1. Make sure the parent material has a PixelDepthOffset parameter, duplicate it otherwise
        UMaterialInterface* ParentMaterial = OriginialMaterialInterface;

        if( OriginialMaterialInterface->GetScalarParameterValue( FMaterialParameterInfo( PDO_PARAMETER_NAME ), PixelDepthOffset ) == false )
        {
            ParentMaterial = GetOrCreatePDOMaterial( OriginialMaterialInterface, OutDuplicatedPDOMaterials );
            ParentMaterial->GetScalarParameterValue( FMaterialParameterInfo( PDO_PARAMETER_NAME ), PixelDepthOffset );
        }

        const bool bHasColorParameter = ParentMaterial->GetVectorParameterValue( FMaterialParameterInfo( TEXT("Color") ), LinearColor );

        // A material without "Color" parameter can't be tinted: ignore the color in that case
        const bool bColorMatches = ( bHasColorParameter == false ) || ( LinearColor == FLinearColor( RequestedColor ) );

        // 2. Use the parent as is if it already matches, otherwise create an instance with the requested
        // color/PDO. An instance is also needed to make a one-sided parent two-sided.
        UMaterialInterface* MaterialInstance = ParentMaterial;

        if ( ( bColorMatches == false )
          || ( PixelDepthOffset != RequestedPDO )
          || ( ParentMaterial->IsTwoSided() == false ) )
        {
            // Name the instance after its parent, color and PDO. The static mesh name is added when saving
            const FString MICBaseName = FString::Printf( TEXT("%s_%08X_%s")
                                                       , *ParentMaterial->GetName()
                                                       , RequestedColor.DWColor()
                                                       , RequestedPDO ? TEXT("PDO") : TEXT("NOPDO") );
            UMaterialInstanceConstant* MIC = NewObject<UMaterialInstanceConstant>( GetTransientPackage()
                                                                                 , MakeUniqueObjectName( GetTransientPackage()
                                                                                                       , UMaterialInstanceConstant::StaticClass()
                                                                                                       , *MICBaseName ) );

            MIC->SetParentEditorOnly( ParentMaterial );

            if( bHasColorParameter )
            {
                MIC->SetVectorParameterValueEditorOnly( FMaterialParameterInfo( TEXT("Color") ), FLinearColor( RequestedColor ) );
            }

            MIC->SetScalarParameterValueEditorOnly( FMaterialParameterInfo( PDO_PARAMETER_NAME ), RequestedPDO );

            // Ariane objects are flat and can be drawn from any point of view, so they have no meaningful
            // front side and must be two-sided. This has to be done on the material: it is compiled with a
            // permutation that flips back face normals for lighting. Doubling the geometry instead would
            // z-fight with materials that are already two-sided.
            MIC->BasePropertyOverrides.bOverride_TwoSided = true;
            MIC->BasePropertyOverrides.TwoSided = true;
            MIC->UpdateOverridableBaseProperties();
            MIC->PostEditChange();

            OutMaterialInstanceConstants.Add( MIC );
            MaterialInstance = MIC;
        }

        UniqueMaterial = &UniqueMaterials.Add_GetRef( FUniqueMaterial( OriginialMaterialInterface
                                                                     , MaterialInstance
                                                                     , RequestedColor
                                                                     , RequestedPDO ) );

    }

    return UniqueMaterial;
}

void
UArianePainting3DStaticMeshComponent::ConvertMaterials( UStaticMesh* ConvertedStaticMesh
                                                      , TArray<UMaterialInterface*>& OutMaterialInstanceConstants
                                                      , TArray<UMaterialInterface*>& OutDuplicatedPDOMaterials )
{
    UArianePainting3DComponent* Painting3DComponent = Cast<UArianePainting3DComponent>(GetOuter());

    UniqueMaterials.Empty();
    DuplicatedPDOMaterials.Empty();

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();

        // Traverse each vector layer
        UArianeLayerFolder::Traverse( LayerStack->GetRootFolder()
                                    , [ this
                                      , Painting3DComponent
                                      , &OutMaterialInstanceConstants
                                      , &OutDuplicatedPDOMaterials ]( UArianeLayer* Layer ) -> UArianeLayerFolder::ETraversalReturnValue
            {
                UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(Layer);

                if( VectorLayer )
                {
                    UArianeDrawing* Drawing = VectorLayer->GetDrawing();

                    // TODO: Put that part in a separate function
                    // Retrieve all materials and the color they need to create new materials
                    FArianeObject::Traverse( Drawing->GetRootGroup()
                                           , FArianeObject::ETraversalPolicy::AllChildren
                                           , [ this
                                             , Painting3DComponent
                                             , &OutMaterialInstanceConstants
                                             , &OutDuplicatedPDOMaterials ] ( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
                        {
                            if ( Object->HasBaseClass( FArianePath::StaticClass() ) )
                            {
                                FArianePath* Path = static_cast<FArianePath*>(Object);

                                CreateUniqueMaterial( Painting3DComponent
                                                    , Path->GetMaterial()
                                                    , Path->GetColor()
                                                    , 0.0f
                                                    , OutMaterialInstanceConstants
                                                    , OutDuplicatedPDOMaterials );
                            }

                            if ( Object->GetClass() == FArianeCycle::StaticClass() )
                            {
                                FArianeCycle* Cycle = static_cast<FArianeCycle*>(Object);

                                CreateUniqueMaterial( Painting3DComponent
                                                    , Cycle->GetMaterial()
                                                    , Cycle->GetColor()
                                                    // Pixel Depth Offset for cycles only, it will prevent ZFighting
                                                    // when using the default Ariane Material
                                                    , CYCLE_PDO
                                                    , OutMaterialInstanceConstants
                                                    , OutDuplicatedPDOMaterials );
                            }

                            return FArianeObject::ETraversalReturnValue::Continue;
                        } );
                }

                return UArianeLayerFolder::ETraversalReturnValue::Continue;
            } );
    }

    for( FUniqueMaterial& UniqueMaterial : UniqueMaterials )
    {
        ConvertedStaticMesh->GetStaticMaterials().Add( FStaticMaterial( UniqueMaterial.MaterialInterface
                                                                      , UniqueMaterial.MaterialInterface->GetFName() ) );
    }
}

void
UArianePainting3DStaticMeshComponent::ConvertToStaticMesh( TArray<UMaterialInterface*>& OutMaterialInstanceConstants
                                                         , TArray<UMaterialInterface*>& OutDuplicatedPDOMaterials )
{
    FName UniqueMeshName = MakeUniqueObjectName( GetTransientPackage()
                                               , UStaticMesh::StaticClass()
                                               , TEXT("TransientPaintingMesh_"));
    UStaticMesh* ConvertedStaticMesh = NewObject<UStaticMesh>(GetTransientPackage(), UniqueMeshName, RF_Transient);
    FMeshDescription MeshDescription;
    FStaticMeshAttributes Attributes(MeshDescription);

    UArianePainting3DComponent* Painting3DComponent = Cast<UArianePainting3DComponent>(GetOuter());

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();

        ConvertedStaticMesh->InitResources();
        ConvertedStaticMesh->SetLightingGuid();

        Attributes.Register();

        FMeshDescriptionBuilder MeshDescriptionBuilder;

        MeshDescriptionBuilder.SetMeshDescription( &MeshDescription );
        MeshDescriptionBuilder.EnablePolyGroups();
        MeshDescriptionBuilder.SetNumUVLayers(1);

        // retrieve all materials and create them related to the vector object's color if needed
        ConvertMaterials( ConvertedStaticMesh, OutMaterialInstanceConstants, OutDuplicatedPDOMaterials );

        // convert vector objects
        UArianeLayerFolder::Traverse( LayerStack->GetRootFolder()
                                    , [ this
                                      , ConvertedStaticMesh
                                      , &MeshDescriptionBuilder ]( UArianeLayer* Layer ) -> UArianeLayerFolder::ETraversalReturnValue
            {
                UArianeLayerVector* VectorLayer = Cast<UArianeLayerVector>(Layer);

                if( VectorLayer )
                {
                    UArianeDrawing* Drawing = VectorLayer->GetDrawing();

                    ConvertDrawingToStaticMesh( MeshDescriptionBuilder, VectorLayer );
                }

                return UArianeLayerFolder::ETraversalReturnValue::Continue;
            } );

        FStaticMeshSourceModel& StaticMeshSourceModel = ConvertedStaticMesh->AddSourceModel();
        StaticMeshSourceModel.BuildSettings.bRecomputeNormals = false;
        StaticMeshSourceModel.BuildSettings.bRecomputeTangents = true;
        StaticMeshSourceModel.BuildSettings.bRemoveDegenerates = false;

        ConvertedStaticMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));
        ConvertedStaticMesh->CommitMeshDescription(0);

        // <begin_explanation>
        // I after some debugging and troubles with materials, I found that I had to fill the Mesh's SectionInfoMap
        // before converting the mesh with Build() in order to have Material correctly attached to the sections (polygongroups).
        // I don't get the point of doing :
        // FPolygonGroupID PolygonGroup = MeshDescriptionBuilder.AppendPolygonGroup( Path->GetMaterial()->GetFName() );
        // in ConvertPathToStaticMesh then if the mapping is not automatically done. Anyways...
        const TPolygonGroupAttributesRef<FName> PolygonGroupNames = Attributes.GetPolygonGroupMaterialSlotNames();

        for ( const FPolygonGroupID PolygonGroupID : MeshDescription.PolygonGroups().GetElementIDs() )
        {
            int32 SectionIndex = PolygonGroupID.GetValue();
            FName MaterialSlotName = *PolygonGroupNames[PolygonGroupID].ToString();
            int32 MaterialIndex = ConvertedStaticMesh->GetMaterialIndexFromImportedMaterialSlotName( MaterialSlotName );

            ConvertedStaticMesh->GetSectionInfoMap().Set( 0
                                                        , SectionIndex
                                                        , FMeshSectionInfo(MaterialIndex) );
        }
        // <end_explanation>

        ConvertedStaticMesh->SetImportVersion( EImportStaticMeshVersion::LastVersion );
        ConvertedStaticMesh->InitResources();
        ConvertedStaticMesh->SetLightingGuid();
        ConvertedStaticMesh->Build( false );
        ConvertedStaticMesh->PostEditChange();

        SetStaticMesh(ConvertedStaticMesh);

        MarkRenderStateDirty();
    }
}

#endif // WITH_EDITOR
