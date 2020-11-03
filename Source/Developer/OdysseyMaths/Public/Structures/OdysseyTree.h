// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

enum class ePosition : char
{
    kIn,
    kBefore,
    kAfter,
    kInvalid
};

template< typename T >
class FOdysseyNTree
{
public:
    //Events
    // Tree Node Added Event
    // - FOdysseyNTree<T>* is the added node
    DECLARE_MULTICAST_DELEGATE_OneParam(FOdysseyTreeNodeAdded, FOdysseyNTree<T>*);

    // Tree Node Removed Event
    // FOdysseyNTree<T>* is the removed node
    // FOdysseyNTree<T>* is the previous parent node
    // int is the index of the removed node in the previous parent node
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOdysseyTreeNodeRemoved, FOdysseyNTree<T>*, FOdysseyNTree<T>*, int);
    
    // Tree Node Moved Event
    // FOdysseyNTree<T>* is the moved node
    // FOdysseyNTree<T>* is the previous parent node
    // int is the index of the removed node in the previous parent node
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOdysseyTreeNodeMoved, FOdysseyNTree<T>*, FOdysseyNTree<T>*, int);

//Construction Destruction
public:
    FOdysseyNTree( T iNodeContent );
    ~FOdysseyNTree();
    
public:
    FOdysseyNTree<T>* AddNode( T iNodeContent, int iIndexEmplace = -1 );

    void DeleteNode( int iIndex );
    void DeleteNodeIfExist( FOdysseyNTree<T>* iNodeToDelete );
    FOdysseyNTree<T>* MoveNodeTo( FOdysseyNTree<T>* iNewPositionInTree, ePosition iPosition = ePosition::kAfter );
    
    //Find the first node (depthSearch) which content is iToFind
    FOdysseyNTree<T>* FindNode( T iToFind );
    
    FOdysseyNTree<T>* GetParent() const;
    int GetIndexInParent() const;
    T GetNodeContent() const;
    T* GetNodeContentPtr() const;
    
    TArray<FOdysseyNTree<T>*> GetNodes() const;
    
    void DepthFirstSearchTree( TArray<T>* ioContents, bool iIncludeRoot = true ) const;
    void BreadthFirstSearchTree( TArray<T>* ioContents, bool iIncludeRoot = true ) const;
    
    int GetNumberParents() const;
    
    //Check if iParentToSearch is a parent (direct or indirect) of the currentNode
    bool HasForParent( FOdysseyNTree<T>* iParentToSearch ) const;

    FOdysseyTreeNodeAdded& NodeAdded();
    FOdysseyTreeNodeRemoved& NodeRemoved();
    FOdysseyTreeNodeMoved& NodeMoved();

    // Overloads for save in archive
    friend FArchive& operator<<(FArchive &Ar, FOdysseyNTree<IOdysseyLayer*>& ioSaveNTree );

private:
    FOdysseyNTree<T>* AddNode( FOdysseyNTree<T>* iNode, int iIndexEmplace = -1 );

private:
    T mNodeContent;
    TArray<FOdysseyNTree*> mNodes;
    FOdysseyNTree* mParent;
    
    FOdysseyTreeNodeAdded mNodeAdded;
    FOdysseyTreeNodeRemoved mNodeRemoved;
    FOdysseyTreeNodeMoved mNodeMoved;
};

//Implementation
#include "OdysseyTree.inl"
