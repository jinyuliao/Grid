#include "Components//GridOutlineComponent.h"
#include "GridPainter/GridOutlinePainter.h"
#include "Square/SquareGridManager.h"
#include "Hexagon/HexagonGridManager.h"

const static FName NAME_GridOutlineResourceNameForDebugging(TEXT("GridOutline"));

class FGridOutlineSceneProxy;

struct FSquareEdge
{
	enum EEdgeDirection
	{
		Left = 0,
		Right,
		Top,
		Bottom,
		Max
	};

	EEdgeDirection Direction;

	FIntVector Coord;

	FVector GridCenter;

	FLinearColor Color;
};

struct FHexagonEdge
{
	enum EEdgeDirection
	{
		LeftTop = 0,
		RightTop,
		Right,
		RightBottom,
		LeftBottom,
		Left,
		Max
	};

	EEdgeDirection Direction;

	FIntVector Coord;

	FVector GridCenter;

	FLinearColor Color;
};

struct FGridOutlineInfo
{
	FLinearColor Color;

	FIntVector Coord;

	FVector Center;
};

struct FGridOutlinePrimitiveCompUpdateParams
{
	FGridOutlineSceneProxy* SceneProxy;

	EGridType GridType;

	float GridSize;

	float Thickness;

	float ZOffset;

	TMap<FLinearColor, TArray<FGridOutlineInfo> > Color2Grids;

	TArray<FLinearColor> ColorPriorities;
};

struct FEdgeGroup
{
	FLinearColor Color;

	TArray<FEdge> Edges;

	FEdgeGroup(){}
	FEdgeGroup(const FLinearColor& InColor, const TArray<FEdge>& InEdges): Color(InColor), Edges(InEdges){}
};

class FGridOutlineSceneProxy : public FPrimitiveSceneProxy
{
public:
	FGridOutlineSceneProxy(UGridOutlineComponent* InComponent)
		:FPrimitiveSceneProxy(InComponent, NAME_GridOutlineResourceNameForDebugging)
	{
	}

	virtual uint32 GetMemoryFootprint(void) const override { return (sizeof(*this) + GetAllocatedSize()); }

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_FGridOutlineSceneProxy_GetDynamicMeshElements);

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];

				FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

				DrawGridOutline(PDI);
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bDynamicRelevance = true;
		Result.bShadowRelevance = false;
		Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
		return Result;
	}

	FVector GetEdgeV0(FSquareEdge::EEdgeDirection Dir, const FVector& Center)
	{
		const float HalfGridSize = UpdateParams.GridSize / 2.f;
		switch (Dir)
		{
		case FSquareEdge::Left:
			return Center + FVector(-HalfGridSize, -HalfGridSize, UpdateParams.ZOffset);
		case FSquareEdge::Right:
			return Center + FVector(-HalfGridSize, HalfGridSize, UpdateParams.ZOffset);
		case FSquareEdge::Top:
			return Center + FVector(HalfGridSize, -HalfGridSize, UpdateParams.ZOffset);
		case FSquareEdge::Bottom:
			return Center + FVector(-HalfGridSize, -HalfGridSize, UpdateParams.ZOffset);
		default:
			LOG_ERROR(TEXT("FCompactGridPrimitiveSceneProxy::GetSquareEdgeV0 unknown direction"));
			return Center;
		}
	}

	FVector GetEdgeV1(FSquareEdge::EEdgeDirection Dir, const FVector& Center)
	{
		const float HalfGridSize = UpdateParams.GridSize / 2.f;
		switch (Dir)
		{
		case FSquareEdge::Left:
			return Center + FVector(HalfGridSize, -HalfGridSize, UpdateParams.ZOffset);
		case FSquareEdge::Right:
			return Center + FVector(HalfGridSize, HalfGridSize, UpdateParams.ZOffset);
		case FSquareEdge::Top:
			return Center + FVector(HalfGridSize, HalfGridSize, UpdateParams.ZOffset);
		case FSquareEdge::Bottom:
			return Center + FVector(-HalfGridSize, HalfGridSize, UpdateParams.ZOffset);
		default:
			LOG_ERROR(TEXT("FCompactGridPrimitiveSceneProxy::GetSquareEdgeV1 unknown direction"));
			return Center;
		}
	}

	FVector GetEdgeV0(FHexagonEdge::EEdgeDirection Dir, const FVector& Center)
	{
		float Radian = PI / 180 * (Dir * 60 + 30);
		return Center + FVector(UpdateParams.GridSize * FMath::Cos(Radian), UpdateParams.GridSize * FMath::Sin(Radian), UpdateParams.ZOffset);
	}

	FVector GetEdgeV1(FHexagonEdge::EEdgeDirection Dir, const FVector& Center)
	{
		float Radian = PI / 180 * (((Dir + 1) % FHexagonEdge::Max) * 60 + 30);
		return Center + FVector(UpdateParams.GridSize * FMath::Cos(Radian), UpdateParams.GridSize * FMath::Sin(Radian), UpdateParams.ZOffset);
	}

	void GetCounterEdge(FSquareEdge::EEdgeDirection InDir, const FIntVector& InCoord, FSquareEdge::EEdgeDirection& CounterDir, FIntVector& CounterCoord)
	{
		const FSquareEdge::EEdgeDirection CounterDirections[] = { FSquareEdge::Right, FSquareEdge::Left, FSquareEdge::Bottom, FSquareEdge::Top };
		const FIntVector CounterCoordDelta[] = { FIntVector(0, -1, 0), FIntVector(0, 1, 0), FIntVector(1, 0, 0), FIntVector(-1, 0, 0) };

		CounterDir = CounterDirections[InDir];
		CounterCoord = InCoord + CounterCoordDelta[InDir];
	}

	void GetCounterEdge(FHexagonEdge::EEdgeDirection InDir, const FIntVector& InCoord, FHexagonEdge::EEdgeDirection& CounterDir, FIntVector& CounterCoord)
	{
		const FHexagonEdge::EEdgeDirection CounterDirections[] = { FHexagonEdge::RightBottom, FHexagonEdge::LeftBottom, FHexagonEdge::Left
																,FHexagonEdge::LeftTop, FHexagonEdge::RightTop, FHexagonEdge::Right };
		const FIntVector CounterCoordDelta[] = { FIntVector(0, -1, 1), FIntVector(-1, 0, 1), FIntVector(-1, 1, 0)
												,FIntVector(0, 1, -1), FIntVector(1, 0, -1), FIntVector(1, -1, 0) };

		CounterDir = CounterDirections[InDir];
		CounterCoord = InCoord + CounterCoordDelta[InDir];
	}

	template <class T>
	void FindOutlineEdges(const TArray<FGridOutlineInfo*>& GridGroup, TArray<T>& OutlineEdges)
	{
		if (GridGroup.Num() == 0)
			return;

		FIntVector Coord, CounterCoord;
		TArray<FIntVector> Dir2Grids[T::Max];
		TMap<FIntVector, FVector> Coord2Center;
		T::EEdgeDirection CounterDir;

		for (int i = 0; i < GridGroup.Num(); ++i)
		{
			Coord = GridGroup[i]->Coord;

			Coord2Center.Add(Coord, GridGroup[i]->Center);

			for (int DirIndex = 0; DirIndex < T::Max; ++DirIndex)
			{
				GetCounterEdge((T::EEdgeDirection)DirIndex, Coord, CounterDir, CounterCoord);

				if (Dir2Grids[CounterDir].Contains(CounterCoord))
				{
					Dir2Grids[CounterDir].Remove(CounterCoord);
				}
				else
				{
					Dir2Grids[DirIndex].Add(Coord);
				}
			}
		}

		T Edge;
		for (int i = 0; i < T::Max; ++i)
		{
			TArray<FIntVector>& Grids = Dir2Grids[i];

			for (int j = 0; j < Grids.Num(); ++j)
			{
				Edge.Coord = Grids[j];
				Edge.Color = GridGroup[0]->Color;;
				Edge.GridCenter = Coord2Center.FindChecked(Edge.Coord);
				Edge.Direction = (T::EEdgeDirection)i;
				OutlineEdges.Add(Edge);
			}
		}
	}

	template <class T>
	void RemoveSharedEdges(TArray<T>& OutlineEdges)
	{
		TArray<T> Result;
		FIntVector CounterCoord;
		T::EEdgeDirection CounterDir;
		for (int i = 0; i < OutlineEdges.Num(); ++i)
		{
			T& Edge = OutlineEdges[i];
			GetCounterEdge(Edge.Direction, Edge.Coord, CounterDir, CounterCoord);

			int j;
			for (j = 0; j < Result.Num(); ++j)
			{
				T& CurrEdge = Result[j];

				if ((int32)(CurrEdge.GridCenter.Z / UpdateParams.GridSize) != (int32)(Edge.GridCenter.Z / UpdateParams.GridSize))
					continue;

				if (CurrEdge.Coord == CounterCoord && CurrEdge.Direction == CounterDir)
					break;

				if (CurrEdge.Coord == Edge.Coord && CurrEdge.Direction == Edge.Direction)
					break;
			}

			if (j >= Result.Num())
				Result.Add(Edge);
		}
		OutlineEdges = Result;
	}

	template <class T>
	void CreateOutlineEdges(const TArray<T>& OutlineEdges)
	{
		if (OutlineEdges.Num() == 0)
			return;

		FEdge Edge;
		for (int i = 0; i < OutlineEdges.Num(); ++i)
		{
			const T& OutlineEdge = OutlineEdges[i];
			Edge = FEdge(GetEdgeV0(OutlineEdge.Direction, OutlineEdge.GridCenter), GetEdgeV1(OutlineEdge.Direction, OutlineEdge.GridCenter));

			int j;
			for (j = 0; j < EdgeGroups.Num(); ++j)
			{
				if (EdgeGroups[j].Color == OutlineEdge.Color)
				{
					EdgeGroups[j].Edges.Add(Edge);
					break;
				}
			}

			if (j >= EdgeGroups.Num())
				EdgeGroups.Add(FEdgeGroup(OutlineEdge.Color, { Edge }));
		}
	}

	template <class T>
	void CollectGridOutline()
	{
		TArray<T> OutlineEdges;
		//for the grids has same color, group by height
		for (auto& Elem : UpdateParams.Color2Grids)
		{
			TMap<int32, TArray<FGridOutlineInfo*> > Height2Grids;
			const FLinearColor& Color = Elem.Key;
			for (int i = 0; i < Elem.Value.Num(); ++i)
			{
				FGridOutlineInfo& GridInfo = Elem.Value[i];

				int32 Height = (int32)(GridInfo.Center.Z / UpdateParams.GridSize);
				if (Height2Grids.Contains(Height))
				{
					Height2Grids.FindChecked(Height).Add(&GridInfo);
				}
				else
				{
					Height2Grids.Add(Height, { &GridInfo });
				}
			}

			for (auto& Height2GridsIter : Height2Grids)
			{
				FindOutlineEdges(Height2GridsIter.Value, OutlineEdges);
			}
		}

		OutlineEdges.Sort([&](const T& L, const T& R)
		{
			int32 LIdx = -1, RIdx = -1;
			bool LSucc = UpdateParams.ColorPriorities.Find(L.Color, LIdx);
			bool RSucc = UpdateParams.ColorPriorities.Find(R.Color, RIdx);
			if (!LSucc || !RSucc)
				LOG_ERROR(TEXT("FGridOutlinePrimitiveSceneProxy::CollectGridOutline Hexagon grid sort failed, can't find grid color in UpdateParams.ColorPriorities"));
			return LIdx < RIdx;
		});

		RemoveSharedEdges(OutlineEdges);
		CreateOutlineEdges(OutlineEdges);
	}

	void DrawGridOutline(FPrimitiveDrawInterface* PDI) const 
	{
		for (int i = 0; i < EdgeGroups.Num(); ++i)
		{
			const FEdgeGroup& EdgeGroup = EdgeGroups[i];
			for (int j = 0; j < EdgeGroup.Edges.Num(); ++j)
			{
				const FEdge& Edge = EdgeGroup.Edges[j];
				PDI->DrawLine(Edge.Vertex[0], Edge.Vertex[1], EdgeGroup.Color, 0, UpdateParams.Thickness);
			}
		}
		//LOG_WARNING(TEXT("DrawSquareGridOutline edge number: %d"), SquareGridOutline.Num());
	}

	void Update(const FGridOutlinePrimitiveCompUpdateParams& NewUpdateParams)
	{
		UpdateParams = NewUpdateParams;

		EdgeGroups.Reset();

		switch (UpdateParams.GridType)
		{
		case EGridType::Square:
			CollectGridOutline<FSquareEdge>();
			break;
		case EGridType::Hexagon:
			CollectGridOutline<FHexagonEdge>();
			break;
		default:
			break;
		}
	}

private:
	FGridOutlinePrimitiveCompUpdateParams UpdateParams;

	TArray<FEdgeGroup> EdgeGroups;
};
//////////////////////////////////////////////////////////////////////////
UGridOutlineComponent::UGridOutlineComponent()
{
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECR_Ignore);
}

UGridOutlineComponent::~UGridOutlineComponent()
{
}

FPrimitiveSceneProxy* UGridOutlineComponent::CreateSceneProxy()
{
	return new FGridOutlineSceneProxy(this);
}

FBoxSphereBounds UGridOutlineComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBoxSphereBounds Bounds(ForceInitToZero);

	UGridOutlinePainter* GridPainter = Cast<UGridOutlinePainter>(GetOuter());

	if (GridPainter == nullptr)
		return Bounds;

	for (int i = 0; i < GridPainter->VisibleGrids.Num(); ++i)
	{
		UGrid* Grid = GridPainter->VisibleGrids[i];

		Bounds = Bounds + Grid->Bounds;
	}
	return Bounds;
}

void UGridOutlineComponent::UpdateGridInfo()
{
	UGridOutlinePainter* GridPainter = Cast<UGridOutlinePainter>(GetOuter());
	AGridManager* GridManager = GridPainter->GridManager;

	FGridOutlinePrimitiveCompUpdateParams UpdateParams;
	UpdateParams.SceneProxy = (FGridOutlineSceneProxy*)SceneProxy;
	UpdateParams.GridType = Cast<ASquareGridManager>(GridManager) == nullptr ? EGridType::Hexagon : EGridType::Square;
	UpdateParams.GridSize = GridManager->GetGridSize();
	UpdateParams.Thickness = GridPainter->OutlineThickness;
	UpdateParams.ZOffset = GridPainter->ZOffset;
	GridPainter->GetColorPriority(UpdateParams.ColorPriorities);

	FGridOutlineInfo GridInfo;
	TArray<FLinearColor> Colors;
	for (int i = 0; i < GridPainter->VisibleGrids.Num(); ++i)
	{
		UGrid* Grid = GridPainter->VisibleGrids[i];

		GridInfo.Coord = Grid->GetCoord();
		GridInfo.Center = Grid->GetCenter();

		GridPainter->GetColors(Grid, Colors);
		for (int j = 0; j < Colors.Num(); ++j)
		{
			GridInfo.Color = Colors[j];

			if (UpdateParams.Color2Grids.Contains(GridInfo.Color))
			{
				UpdateParams.Color2Grids.FindChecked(GridInfo.Color).Add(GridInfo);
			}
			else
			{
				UpdateParams.Color2Grids.Add(GridInfo.Color, { GridInfo });
			}
		}
	}

	check(UpdateParams.SceneProxy != nullptr);

	ENQUEUE_RENDER_COMMAND(GridOutlineCompUpdateCommand)(
		[UpdateParams](FRHICommandListImmediate& RHICmdList)
	{
		FScopeCycleCounter Context(UpdateParams.SceneProxy->GetStatId());
		UpdateParams.SceneProxy->Update(UpdateParams);
	});

	UpdateBounds();
	MarkRenderTransformDirty();
}
