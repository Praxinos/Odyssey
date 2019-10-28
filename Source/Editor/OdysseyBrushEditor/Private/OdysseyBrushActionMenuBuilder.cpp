// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "OdysseyBrushActionMenuBuilder.h"
#include "UObject/UnrealType.h"
#include "Classes/EditorStyleSettings.h"
#include "Engine/Blueprint.h"
#include "Editor/EditorEngine.h"
#include "BlueprintNodeBinder.h"
#include "OdysseyBrushActionMenuItem.h"
#include "BlueprintActionFilter.h"
#include "OdysseyBrushDragDropMenuItem.h"
#include "BlueprintActionDatabase.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintDelegateNodeSpawner.h"
#include "BlueprintVariableNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "ObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushActionMenuBuilder"
DEFINE_LOG_CATEGORY_STATIC(LogOdysseyBrushActionMenuItemFactory, Log, All);

/*******************************************************************************
 * FOdysseyBrushActionMenuItemFactory
 ******************************************************************************/

class FOdysseyBrushActionMenuItemFactory
{
public:
    /**
     * Menu item factory constructor. Sets up the blueprint context, which
     * is utilized when configuring blueprint menu items' names/tooltips/etc.
     *
     * @param  Context    The blueprint context for the menu being built.
     */
    FOdysseyBrushActionMenuItemFactory(FBlueprintActionContext const& Context);

    /** A root category to perpend every menu item with */
    FText RootCategory;
    /** The menu sort order to set every menu item with */
    int32 MenuGrouping;
    /** Cached context for the blueprint menu being built */
    FBlueprintActionContext const& Context;

    /**
     * Spawns a new FOdysseyBrushActionMenuItem with the node-spawner. Constructs
     * the menu item's category, name, tooltip, etc.
     *
     * @param  EditorContext
     * @param  Action            The node-spawner that the new menu item should wrap.
     * @return A newly allocated FOdysseyBrushActionMenuItem (which wraps the supplied action).
     */
    TSharedPtr<FOdysseyBrushActionMenuItem> MakeActionMenuItem(TWeakPtr<FOdysseyBrushEditor> EditorContext, FBlueprintActionInfo const& ActionInfo);

    /**
     * Spawns a new FOdysseyBrushDragDropMenuItem with the node-spawner. Constructs
     * the menu item's category, name, tooltip, etc.
     *
     * @param  SampleAction    One of the (possibly) many node-spawners that this menu item is set to represent.
     * @return A newly allocated FOdysseyBrushActionMenuItem (which wraps the supplied action).
     */
    TSharedPtr<FOdysseyBrushDragDropMenuItem> MakeDragDropMenuItem(UBlueprintNodeSpawner const* SampleAction);

    /**
     *
     *
     * @param  BoundAction
     * @return
     */
    TSharedPtr<FOdysseyBrushActionMenuItem> MakeBoundMenuItem(TWeakPtr<FOdysseyBrushEditor> EditorContext, FBlueprintActionInfo const& ActionInfo);

private:
    /**
     * Utility getter function that retrieves the blueprint context for the menu
     * items being made.
     *
     * @return The first blueprint out of the cached FBlueprintActionContext.
     */
    UBlueprint* GetTarGetBlueprint() const;

    /**
     *
     *
     * @param  EditorContext
     * @return
     */
    UEdGraph* GetTargetGraph(TWeakPtr<FOdysseyBrushEditor> EditorContext) const;

    /**
     *
     *
     * @param  EditorContext
     * @param  ActionInfo
     * @return
     */
    FBlueprintActionUiSpec GetActionUiSignature(TWeakPtr<FOdysseyBrushEditor> EditorContext, FBlueprintActionInfo const& ActionInfo);
};

//------------------------------------------------------------------------------
FOdysseyBrushActionMenuItemFactory::FOdysseyBrushActionMenuItemFactory(FBlueprintActionContext const& ContextIn)
    : RootCategory(FText::GetEmpty())
    , MenuGrouping(0)
    , Context(ContextIn)
{
}

//------------------------------------------------------------------------------
TSharedPtr<FOdysseyBrushActionMenuItem> FOdysseyBrushActionMenuItemFactory::MakeActionMenuItem(TWeakPtr<FOdysseyBrushEditor> EditorContext, FBlueprintActionInfo const& ActionInfo)
{
    FBlueprintActionUiSpec UiSignature = GetActionUiSignature(EditorContext, ActionInfo);

    UBlueprintNodeSpawner const* Action = ActionInfo.NodeSpawner;
    FOdysseyBrushActionMenuItem* NewMenuItem = new FOdysseyBrushActionMenuItem(Action, UiSignature, IBlueprintNodeBinder::FBindingSet(), FText::FromString(RootCategory.ToString() + TEXT('|') + UiSignature.Category.ToString()), MenuGrouping);

    return MakeShareable(NewMenuItem);
}

//------------------------------------------------------------------------------
TSharedPtr<FOdysseyBrushDragDropMenuItem> FOdysseyBrushActionMenuItemFactory::MakeDragDropMenuItem(UBlueprintNodeSpawner const* SampleAction)
{
    // FOdysseyBrushDragDropMenuItem takes care of its own menu MenuDescription, etc.
    UProperty const* SampleProperty = nullptr;
    if (UBlueprintDelegateNodeSpawner const* DelegateSpawner = Cast<UBlueprintDelegateNodeSpawner const>(SampleAction))
    {
        SampleProperty = DelegateSpawner->GetDelegateProperty();
    }
    else if (UBlueprintVariableNodeSpawner const* VariableSpawner = Cast<UBlueprintVariableNodeSpawner const>(SampleAction))
    {
        SampleProperty = VariableSpawner->GetVarProperty();
    }

    FText MenuDescription;
    FText TooltipDescription;
    FText Category;
    if (SampleProperty != nullptr)
    {
        bool const bShowFriendlyNames = GetDefault<UEditorStyleSettings>()->bShowFriendlyNames;
        MenuDescription = bShowFriendlyNames ? FText::FromString(UEditorEngine::GetFriendlyName(SampleProperty)) : FText::FromName(SampleProperty->GetFName());

        TooltipDescription = SampleProperty->GetToolTipText();
        Category = FObjectEditorUtils::GetCategoryText(SampleProperty);

        bool const bIsDelegateProperty = SampleProperty->IsA<UMulticastDelegateProperty>();
        if (bIsDelegateProperty && Category.IsEmpty())
        {
            Category = FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Delegates);
        }
        else if (!bIsDelegateProperty)
        {
            check(Context.Blueprints.Num() > 0);
            UBlueprint const* OdysseyBrush = Context.Blueprints[0];
            UClass const*     OdysseyBrushClass = (OdysseyBrush->SkeletonGeneratedClass != nullptr) ? OdysseyBrush->SkeletonGeneratedClass : OdysseyBrush->ParentClass;

            UClass const* PropertyClass = SampleProperty->GetOwnerClass();
            checkSlow(PropertyClass != nullptr);
            bool const bIsMemberProperty = OdysseyBrushClass->IsChildOf(PropertyClass);

            FText TextCategory;
            if (Category.IsEmpty())
            {
                TextCategory = FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Variables);
            }
            else if (bIsMemberProperty)
            {
                TextCategory = FEditorCategoryUtils::BuildCategoryString(FCommonEditorCategory::Variables, Category);
            }

            if (!bIsMemberProperty)
            {
                TextCategory = FText::Format(LOCTEXT("NonMemberVarCategory", "Class|{0}|{1}"), PropertyClass->GetDisplayNameText(), TextCategory);
            }
            Category = TextCategory;
        }
    }
    else
    {
        UE_LOG(LogOdysseyBrushActionMenuItemFactory, Warning, TEXT("Unhandled (or invalid) spawner: '%s'"), *SampleAction->GetName());
    }

    Category = FText::FromString(RootCategory.ToString() + TEXT('|') + Category.ToString());

    FOdysseyBrushDragDropMenuItem* NewMenuItem = new FOdysseyBrushDragDropMenuItem(Context, SampleAction, MenuGrouping, Category, MenuDescription, TooltipDescription);
    return MakeShareable(NewMenuItem);
}

//------------------------------------------------------------------------------
TSharedPtr<FOdysseyBrushActionMenuItem> FOdysseyBrushActionMenuItemFactory::MakeBoundMenuItem(TWeakPtr<FOdysseyBrushEditor> EditorContext, FBlueprintActionInfo const& ActionInfo)
{
    FBlueprintActionUiSpec UiSignature = GetActionUiSignature(EditorContext, ActionInfo);

    UBlueprintNodeSpawner const* Action = ActionInfo.NodeSpawner;
    FOdysseyBrushActionMenuItem* NewMenuItem = new FOdysseyBrushActionMenuItem(Action, UiSignature, ActionInfo.GetBindings(), FText::FromString(RootCategory.ToString() + TEXT('|') + UiSignature.Category.ToString()), MenuGrouping);

    return MakeShareable(NewMenuItem);
}

//------------------------------------------------------------------------------
UBlueprint* FOdysseyBrushActionMenuItemFactory::GetTarGetBlueprint() const
{
    UBlueprint* TarGetBlueprint = nullptr;
    if (Context.Blueprints.Num() > 0)
    {
        TarGetBlueprint = Context.Blueprints[0];
    }
    return TarGetBlueprint;
}

//------------------------------------------------------------------------------
UEdGraph* FOdysseyBrushActionMenuItemFactory::GetTargetGraph(TWeakPtr<FOdysseyBrushEditor> EditorContext) const
{
    UEdGraph* TargetGraph = nullptr;
    if (Context.Graphs.Num() > 0)
    {
        TargetGraph = Context.Graphs[0];
    }
    else
    {
        UBlueprint* OdysseyBrush = GetTarGetBlueprint();
        check(OdysseyBrush != nullptr);

        if (OdysseyBrush->UbergraphPages.Num() > 0)
        {
            TargetGraph = OdysseyBrush->UbergraphPages[0];
        }
        else if (EditorContext.IsValid())
        {
            TargetGraph = EditorContext.Pin()->GetFocusedGraph();
        }
    }
    return TargetGraph;
}

//------------------------------------------------------------------------------
FBlueprintActionUiSpec FOdysseyBrushActionMenuItemFactory::GetActionUiSignature(TWeakPtr<FOdysseyBrushEditor> EditorContext, FBlueprintActionInfo const& ActionInfo)
{
    UBlueprintNodeSpawner const* Action = ActionInfo.NodeSpawner;

    UEdGraph* TargetGraph = GetTargetGraph(EditorContext);
    Action->PrimeDefaultUiSpec(TargetGraph);

    return Action->GetUiSpec(Context, ActionInfo.GetBindings());
}


/*******************************************************************************
 * Static FOdysseyBrushActionMenuBuilder Helpers
 ******************************************************************************/

namespace FOdysseyBrushActionMenuBuilderImpl
{
    typedef TArray< TSharedPtr<FEdGraphSchemaAction> > MenuItemList;

    /** Defines a sub-section of the overall blueprint menu (filter, heading, etc.) */
    struct FMenuSectionDefinition
    {
    public:
        FMenuSectionDefinition(FBlueprintActionFilter const& SectionFilter, uint32 const Flags);

        /** Series of ESectionFlags, aimed at customizing how we construct this menu section */
        uint32 Flags;
        /** A filter for this section of the menu */
        FBlueprintActionFilter Filter;

        /** Sets the root category for menu items in this section. */
        void SetSectionHeading(FText const& RootCategory);
        /** Gets the root category for menu items in this section. */
        FText const& GetSectionHeading() const;

        /** Sets the grouping for menu items belonging to this section. */
        void SetSectionSortOrder(int32 const MenuGrouping);

        /**
         * Filters the supplied action and if it passes, spawns a new
         * FOdysseyBrushActionMenuItem for the specified menu (does not add the
         * item to the menu-builder itself).
         *
         * @param  EditorContext
         * @param  DatabaseAction    The node-spawner that the new menu item should wrap.
         * @return An empty TSharedPtr if the action was filtered out, otherwise a newly allocated FOdysseyBrushActionMenuItem.
         */
        MenuItemList MakeMenuItems(TWeakPtr<FOdysseyBrushEditor> EditorContext, FBlueprintActionInfo& DatabaseAction);

        /**
         *
         *
         * @param  EditorContext
         * @param  DatabaseAction
         * @param  Bindings
         * @return
         */
        void AddBoundMenuItems(TWeakPtr<FOdysseyBrushEditor> EditorContext, FBlueprintActionInfo& DatabaseAction, TArray<UObject*> const& Bindings, MenuItemList& MenuItemsOut);

        /**
         * Clears out any consolidated properties that this may have been
         * tracking (so we can start a new and spawn new consolidated menu items).
         */
        void Empty();

    private:
        /** In charge of spawning menu items for this section (holds category/ordering information)*/
        FOdysseyBrushActionMenuItemFactory ItemFactory;
        /** Tracks the properties that we've already consolidated and passed (when using the ConsolidatePropertyActions flag)*/
        TMap<UProperty const*, TSharedPtr<FOdysseyBrushDragDropMenuItem>> ConsolidatedProperties;
    };

    /**
     *
     *
     * @param  Context
     * @return
     */
    static TArray<UObject*> GetBindingCandidates(FBlueprintActionContext const& Context);
}

//------------------------------------------------------------------------------
FOdysseyBrushActionMenuBuilderImpl::FMenuSectionDefinition::FMenuSectionDefinition(FBlueprintActionFilter const& SectionFilterIn, uint32 const FlagsIn)
    : Flags(FlagsIn)
    , Filter(SectionFilterIn)
    , ItemFactory(Filter.Context)
{
}

//------------------------------------------------------------------------------
void FOdysseyBrushActionMenuBuilderImpl::FMenuSectionDefinition::SetSectionHeading(FText const& RootCategory)
{
    ItemFactory.RootCategory = RootCategory;
}

//------------------------------------------------------------------------------
FText const& FOdysseyBrushActionMenuBuilderImpl::FMenuSectionDefinition::GetSectionHeading() const
{
    return ItemFactory.RootCategory;
}

//------------------------------------------------------------------------------
void FOdysseyBrushActionMenuBuilderImpl::FMenuSectionDefinition::SetSectionSortOrder(int32 const MenuGrouping)
{
    ItemFactory.MenuGrouping = MenuGrouping;
}
//
//------------------------------------------------------------------------------
void FOdysseyBrushActionMenuBuilderImpl::FMenuSectionDefinition::AddBoundMenuItems(TWeakPtr<FOdysseyBrushEditor> EditorContext, FBlueprintActionInfo& DatabaseActionInfo, TArray<UObject*> const& PerspectiveBindings, MenuItemList& MenuItemsOut)
{
    UBlueprintNodeSpawner const* DatabaseAction = DatabaseActionInfo.NodeSpawner;

    TSharedPtr<FOdysseyBrushActionMenuItem> LastMadeMenuItem;
    bool const bConsolidate = (Flags & FOdysseyBrushActionMenuBuilder::ConsolidateBoundActions) != 0;

    IBlueprintNodeBinder::FBindingSet CompatibleBindings;
    // we don't want the blueprint database growing out of control with an entry
    // for every object you could ever possibly bind to, so each
    // UBlueprintNodeSpawner comes with an interface to test/bind through...
    for (auto BindingIt = PerspectiveBindings.CreateConstIterator(); BindingIt;)
    {
        UObject const* BindingObj = *BindingIt;
        ++BindingIt;
        bool const bIsLastBinding = !BindingIt;

        // check to see if this object can be bound to this action
        if (DatabaseAction->IsBindingCompatible(BindingObj))
        {
            // add bindings before filtering (in case tests accept/reject based off of this)
            CompatibleBindings.Add(MakeWeakObjectPtr(const_cast<UObject*>(BindingObj)));
        }

        // if BoundAction is now "full" (meaning it can take any more
        // bindings), or if this is the last binding to test...
        if ((CompatibleBindings.Num() > 0) && (!DatabaseAction->CanBindMultipleObjects() || bIsLastBinding || !bConsolidate))
        {
            // we don't want binding to mutate DatabaseActionInfo, so we clone
            // the action info, and tack on some binding data
            FBlueprintActionInfo BoundActionInfo(DatabaseActionInfo, CompatibleBindings);

            // have to check IsFiltered() for every "fully bound" action (in
            // case there are tests that reject based off of this), we may
            // test this multiple times per action (we have to make sure
            // that every set of bound objects pass before folding them into
            // MenuItem)
            bool const bPassedFilter = !Filter.IsFiltered(BoundActionInfo);
            if (bPassedFilter)
            {
                if (!bConsolidate || !LastMadeMenuItem.IsValid())
                {
                    LastMadeMenuItem = ItemFactory.MakeBoundMenuItem(EditorContext, BoundActionInfo);
                    MenuItemsOut.Add(LastMadeMenuItem);

                    if (Flags & FOdysseyBrushActionMenuBuilder::FlattenCategoryHierarcy)
                    {
                        LastMadeMenuItem->CosmeticUpdateCategory( ItemFactory.RootCategory );
                    }
                }
                else
                {
                    // move these bindings over to the menu item (so we can
                    // test the next set)
                    LastMadeMenuItem->AppendBindings(Filter.Context, CompatibleBindings);
                }
            }
            CompatibleBindings.Empty(); // do before we copy back over cached fields for DatabaseActionInfo

            // copy over any fields that got cached for filtering (with
            // an empty binding set)
            /*DatabaseActionInfo = FBlueprintActionInfo(BoundActionInfo, CompatibleBindings);*/
        }
    }
}

//------------------------------------------------------------------------------
FOdysseyBrushActionMenuBuilderImpl::MenuItemList FOdysseyBrushActionMenuBuilderImpl::FMenuSectionDefinition::MakeMenuItems(TWeakPtr<FOdysseyBrushEditor> EditorContext, FBlueprintActionInfo& DatabaseAction)
{
    TSharedPtr<FEdGraphSchemaAction> UnBoundMenuEntry;
    bool bPassedFilter = !Filter.IsFiltered(DatabaseAction);

    // if the caller wants to consolidate all property actions, then we have to
    // check and see if this is one of those that needs consolidating (needs
    // a FOdysseyBrushDragDropMenuItem instead of a FOdysseyBrushActionMenuItem)
    if (bPassedFilter && (Flags & FOdysseyBrushActionMenuBuilder::ConsolidatePropertyActions))
    {
        UProperty const* ActionProperty = nullptr;
        if (UBlueprintVariableNodeSpawner const* VariableSpawner = Cast<UBlueprintVariableNodeSpawner>(DatabaseAction.NodeSpawner))
        {
            ActionProperty = VariableSpawner->GetVarProperty();
            bPassedFilter = (ActionProperty != nullptr);
        }
        else if (UBlueprintDelegateNodeSpawner const* DelegateSpawner = Cast<UBlueprintDelegateNodeSpawner>(DatabaseAction.NodeSpawner))
        {
            ActionProperty = DelegateSpawner->GetDelegateProperty();
            bPassedFilter = (ActionProperty != nullptr);
        }

        if (ActionProperty != nullptr)
        {
            if (TSharedPtr<FOdysseyBrushDragDropMenuItem>* ConsolidatedMenuItem = ConsolidatedProperties.Find(ActionProperty))
            {
                (*ConsolidatedMenuItem)->AppendAction(DatabaseAction.NodeSpawner);
                // this menu entry has already been returned, don't need to
                // create/insert a new one
                bPassedFilter = false;
            }
            else
            {
                TSharedPtr<FOdysseyBrushDragDropMenuItem> NewMenuItem = ItemFactory.MakeDragDropMenuItem(DatabaseAction.NodeSpawner);
                ConsolidatedProperties.Add(ActionProperty, NewMenuItem);
                UnBoundMenuEntry = NewMenuItem;
            }
        }
    }

    if (!UnBoundMenuEntry.IsValid() && bPassedFilter)
    {
        UnBoundMenuEntry = ItemFactory.MakeActionMenuItem(EditorContext, DatabaseAction);
        if (Flags & FOdysseyBrushActionMenuBuilder::FlattenCategoryHierarcy)
        {
            UnBoundMenuEntry->CosmeticUpdateCategory( ItemFactory.RootCategory );
        }
    }

    FOdysseyBrushActionMenuBuilderImpl::MenuItemList MenuItems;
    if (UnBoundMenuEntry.IsValid())
    {
        MenuItems.Add(UnBoundMenuEntry);
    }
    AddBoundMenuItems(EditorContext, DatabaseAction, GetBindingCandidates(Filter.Context), MenuItems);

    return MenuItems;
}

//------------------------------------------------------------------------------
void FOdysseyBrushActionMenuBuilderImpl::FMenuSectionDefinition::Empty()
{
    ConsolidatedProperties.Empty();
}

//------------------------------------------------------------------------------
static TArray<UObject*> FOdysseyBrushActionMenuBuilderImpl::GetBindingCandidates(FBlueprintActionContext const& Context)
{
    return Context.SelectedObjects;
}

/*******************************************************************************
 * FOdysseyBrushActionMenuBuilder
 ******************************************************************************/

//------------------------------------------------------------------------------
FOdysseyBrushActionMenuBuilder::FOdysseyBrushActionMenuBuilder(TWeakPtr<FOdysseyBrushEditor> InOdysseyBrushEditorPtr)
    : OdysseyBrushEditorPtr(InOdysseyBrushEditorPtr)
{
}

//------------------------------------------------------------------------------
void FOdysseyBrushActionMenuBuilder::Empty()
{
    FGraphActionListBuilderBase::Empty();
    MenuSections.Empty();
}

//------------------------------------------------------------------------------
void FOdysseyBrushActionMenuBuilder::AddMenuSection(FBlueprintActionFilter const& Filter, FText const& Heading/* = FText::GetEmpty()*/, int32 MenuOrder/* = 0*/, uint32 const Flags/* = 0*/)
{
    using namespace FOdysseyBrushActionMenuBuilderImpl;

    TSharedRef<FMenuSectionDefinition> SectionDescRef = MakeShareable(new FMenuSectionDefinition(Filter, Flags));
    SectionDescRef->SetSectionHeading(Heading);
    SectionDescRef->SetSectionSortOrder(MenuOrder);

    MenuSections.Add(SectionDescRef);
}

//------------------------------------------------------------------------------
void FOdysseyBrushActionMenuBuilder::RebuildActionList()
{
    using namespace FOdysseyBrushActionMenuBuilderImpl;

    FGraphActionListBuilderBase::Empty();
    for (TSharedRef<FMenuSectionDefinition> MenuSection : MenuSections)
    {
        // clear out intermediate actions that may have been spawned (like
        // consolidated property actions).
        MenuSection->Empty();
    }

    FBlueprintActionDatabase& ActionDatabase = FBlueprintActionDatabase::Get();
    FBlueprintActionDatabase::FActionRegistry const& ActionRegistry = ActionDatabase.GetAllActions();
    for (auto const& ActionEntry : ActionRegistry)
    {
        if (UObject *ActionObject = ActionEntry.Key.ResolveObjectPtr())
        {
            for (UBlueprintNodeSpawner const* NodeSpawner : ActionEntry.Value)
            {
                FBlueprintActionInfo OdysseyBrushAction(ActionObject, NodeSpawner);

                // @TODO: could probably have a super filter that spreads across
                //        all MenuSctions (to pair down on performance?)
                for (TSharedRef<FMenuSectionDefinition> const& MenuSection : MenuSections)
                {
                    for (TSharedPtr<FEdGraphSchemaAction> MenuEntry : MenuSection->MakeMenuItems(OdysseyBrushEditorPtr, OdysseyBrushAction))
                    {
                        AddAction(MenuEntry);
                    }
                }
            }
        }
        else
        {
            // Remove this (invalid) entry on the next tick.
            ActionDatabase.DeferredRemoveEntry(ActionEntry.Key);
        }
    }
}

#undef LOCTEXT_NAMESPACE
