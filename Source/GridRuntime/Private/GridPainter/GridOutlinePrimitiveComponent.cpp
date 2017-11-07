#include "GridPainter/GridOutlinePrimitiveComponent.h"
#include "GridPainter/GridOutlinePainter.h"
#include "Square/SquareGridManager.h"
#include "Hexagon/HexagonGridManager.h"

const static FName NAME_GridOutlineResourceNameForDebugging(TEXT("GridOutline"));

class FGridOutlinePrimitiveSceneProxy;

enum EGridType
{
	Square,
	Hexagon
};

namespace SquareNS
{
	enum EEdgeDirection
	{
		Left = 0,
		Right,
		Top,
		Bottom,
		Max
	};
}

namespace HexagonNS
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
}

struct FGridOutlineInfo
{
	FLinearColor Color;

	FIntVector Coord;

	FVector Center;
};

struct FGridOutlinePrimitiveCompUpdateParams
{
	FGridOutlinePrimitiveSceneProxy* SceneProxy;

	EGridType GridType;

	float GridSize;

	float Margin;

	float Thickness;

	float ZDelta;

	TArray<FGridOutlineInfo> GridInfoArray;
};

struct FEdgeGroup
{
	FLinearColor Color;

	TArray<FEdge> Edges;

	FEdgeGroup(){}
	FEdgeGroup(const FLinearColor& InColor, const TArray<FEdge>& InEdges): Color(InColor), Edges(InEdges){}
};

class FGridOutlinePrimitiveSceneProxy : public FPrimitiveSceneProxy
{
public:
	FGridOutlinePrimitiveSceneProxy(UGridOutlinePrimitiveComponent* InComponent)
		:FPrimitiveSceneProxy(InComponent, NAME_GridOutlineResourceNameForDebugging)
	{
	}

	~FGridOutlinePrimitiveSceneProxy() {}

	virtual uint32 GetMemoryFootprint(void) const override { return (sizeof(*this) + GetAllocatedSize()); }

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_FGridOutlinePrimitiveSceneProxy_GetDynamicMeshElements);

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

	FVector GetSquareEdgeV0(SquareNS::EEdgeDirection Dir, const FVector& Center)
	{
		const float HalfGridSize = UpdateParams.GridSize / 2.f - UpdateParams.Margin;
		switch (Dir)
		{
		case SquareNS::Left:
			return Center + FVector(-HalfGridSize, -HalfGridSize, UpdateParams.ZDelta);
		case SquareNS::Right:
			return Center + FVector(-HalfGridSize, HalfGridSize, UpdateParams.ZDelta);
		case SquareNS::Top:
			return Center + FVector(HalfGridSize, -HalfGridSize, UpdateParams.ZDelta);
		case SquareNS::Bottom:
			return Center + FVector(-HalfGridSize, -HalfGridSize, UpdateParams.ZDelta);
		default:
			LOG_ERROR(TEXT("FCompactGridPrimitiveSceneProxy::GetSquareEdgeV0 unknown direction"));
			return Center;
		}
	}

	FVector GetSquareEdgeV1(SquareNS::EEdgeDirection Dir, const FVector& Center)
	{
		const float HalfGridSize = UpdateParams.GridSize / 2.f - UpdateParams.Margin;
		switch (Dir)
		{
		case SquareNS::Left:
			return Center + FVector(HalfGridSize, -HalfGridSize, UpdateParams.ZDelta);
		case SquareNS::Right:
			return Center + FVector(HalfGridSize, HalfGridSize, UpdateParams.ZDelta);
		case SquareNS::Top:
			return Center + FVector(HalfGridSize, HalfGridSize, UpdateParams.ZDelta);
		case SquareNS::Bottom:
			return Center + FVector(-HalfGridSize, HalfGridSize, UpdateParams.ZDelta);
		default:
			LOG_ERROR(TEXT("FCompactGridPrimitiveSceneProxy::GetSquareEdgeV1 unknown direction"));
			return Center;
		}
	}

	FVector GetHexagonEdgeV0(HexagonNS::EEdgeDirection Dir, const FVector& Center)
	{
		float Radian = PI / 180 * (Dir * 60 + 30);
		return Center + FVector(UpdateParams.GridSize * FMath::Cos(Radian), UpdateParams.GridSize * FMath::Sin(Radian), UpdateParams.ZDelta);
	}

	FVector GetHexagonEdgeV1(HexagonNS::EEdgeDirection Dir, const FVector& Center)
	{
		float Radian = PI / 180 * (((Dir + 1) % HexagonNS::Max) * 60 + 30);
		return Center + FVector(UpdateParams.GridSize * FMath::Cos(Radian), UpdateParams.GridSize * FMath::Sin(Radian), UpdateParams.ZDelta);
	}

	bool CanConnect(SquareNS::EEdgeDirection Dir, const FIntVector& Start, const FIntVector& End)
	{
		switch (Dir)
		{
		case SquareNS::Left:
		case SquareNS::Right:
			return Start.Y == End.Y && FMath::Abs(Start.X - End.X) == 1;
		case SquareNS::Top:
		case SquareNS::Bottom:
			return Start.X == End.X && FMath::Abs(Start.Y - End.Y) == 1;
		default:
			return false;
		}
	}

	void CombineSquareEdges(const TArray<FGridOutlineInfo*>& GridGroup)
	{
		if (GridGroup.Num() == 0)
			return;

		const SquareNS::EEdgeDirection CounterDirections[] = { SquareNS::Right, SquareNS::Left, SquareNS::Bottom, SquareNS::Top };
		const FIntVector CounterCoordDelta[] = { FIntVector(0, -1, 0), FIntVector(0, 1, 0), FIntVector(1, 0, 0), FIntVector(-1, 0, 0) };

		FIntVector Coord, CounterGrid;
		TArray<FIntVector> Dir2Grids[SquareNS::Max];
		FLinearColor Color = GridGroup[0]->Color;
		TMap<FIntVector, FVector> Coord2Center;

		for (int i = 0; i < GridGroup.Num(); ++i)
		{
			Coord = GridGroup[i]->Coord;

			Coord2Center.Add(Coord, GridGroup[i]->Center);

			for (int DirIndex = 0; DirIndex < 4; ++DirIndex)
			{
				auto CounterDir = CounterDirections[DirIndex];
				CounterGrid = Coord + CounterCoordDelta[DirIndex];

				if (Dir2Grids[CounterDir].Contains(CounterGrid))
				{
					Dir2Grids[CounterDir].Remove(CounterGrid);
				}
				else
				{
					Dir2Grids[DirIndex].Add(Coord);
				}
			}
		}

		FIntVector Start;
		TArray<FEdge> Edges;

		for (int i = 0; i < 4; ++i)
		{
			auto Dir = (SquareNS::EEdgeDirection)i;
			TArray<FIntVector>& Grids = Dir2Grids[i];

			Grids.Sort([&](const FIntVector& L, const FIntVector& R)
			{
				switch (Dir)
				{
				case SquareNS::Top:
				case SquareNS::Bottom:
					return L.X == R.X ? L.Y < R.Y : L.X < R.X;
				case SquareNS::Left:
				case SquareNS::Right:
					return L.Y == R.Y ? L.X < R.X : L.Y < R.Y;
				default:
					break;
				}
				return L.X < R.X;
			});

			for (int j = 0; j < Grids.Num(); ++j)
			{
				if (j == 0)
				{
					Start = Grids[j];
				}
				else if (!CanConnect(Dir, Grids[j - 1], Grids[j]))
				{
					Edges.Add(FEdge(GetSquareEdgeV0(Dir, Coord2Center.FindChecked(Start)), GetSquareEdgeV1(Dir, Coord2Center.FindChecked(Grids[j - 1]))));
					Start = Grids[j];
				}
			}

			if (Grids.Num() != 0)
			{
				Edges.Add(FEdge(GetSquareEdgeV0(Dir, Coord2Center.FindChecked(Start)), GetSquareEdgeV1(Dir, Coord2Center.FindChecked(Grids.Last()))));
			}
		}

		bool Succ = false;
		for (int i = 0; i < EdgeGroups.Num(); ++i)
		{
			if (EdgeGroups[0].Color == Color)
			{
				EdgeGroups[0].Edges.Append(Edges);
				Succ = true;
				break;
			}
		}

		if (!Succ)
		{
			EdgeGroups.Add(FEdgeGroup(Color, Edges));
		}
	}

	void CreateHexagonEdges(const TArray<FGridOutlineInfo*>& GridGroup)
	{
		if (GridGroup.Num() == 0)
			return;

		const HexagonNS::EEdgeDirection CounterDirections[] = { HexagonNS::RightBottom, HexagonNS::LeftBottom, HexagonNS::Left
																,HexagonNS::LeftTop, HexagonNS::RightTop, HexagonNS::Right };
		const FIntVector CounterCoordDelta[] = { FIntVector(0, -1, 1), FIntVector(-1, 0, 1), FIntVector(-1, 1, 0)
												,FIntVector(0, 1, -1), FIntVector(1, 0, -1), FIntVector(1, -1, 0)};


		FIntVector Coord, CounterGrid;
		TArray<FIntVector> Dir2Grids[HexagonNS::Max];
		FLinearColor Color = GridGroup[0]->Color;
		TMap<FIntVector, FVector> Coord2Center;

		for (int i = 0; i < GridGroup.Num(); ++i)
		{
			Coord = GridGroup[i]->Coord;

			Coord2Center.Add(Coord, GridGroup[i]->Center);

			for (int DirIndex = 0; DirIndex < 6; ++DirIndex)
			{
				auto CounterDir = CounterDirections[DirIndex];
				CounterGrid = Coord + CounterCoordDelta[DirIndex];

				if (Dir2Grids[CounterDir].Contains(CounterGrid))
				{
					Dir2Grids[CounterDir].Remove(CounterGrid);
				}
				else
				{
					Dir2Grids[DirIndex].Add(Coord);
				}
			}
		}

		TArray<FEdge> Edges;

		for (int i = 0; i < 6; ++i)
		{
			auto Dir = (HexagonNS::EEdgeDirection)i;
			TArray<FIntVector>& Grids = Dir2Grids[i];

			for (int j = 0; j < Grids.Num(); ++j)
			{
				const FIntVector& Grid = Grids[j];
				Edges.Add(FEdge(GetHexagonEdgeV0(Dir, Coord2Center.FindChecked(Grid)), GetHexagonEdgeV1(Dir, Coord2Center.FindChecked(Grid))));
			}
		}

		bool Succ = false;
		for (int i = 0; i < EdgeGroups.Num(); ++i)
		{
			if (EdgeGroups[0].Color == Color)
			{
				EdgeGroups[0].Edges.Append(Edges);
				Succ = true;
				break;
			}
		}

		if (!Succ)
		{
			EdgeGroups.Add(FEdgeGroup(Color, Edges));
		}
	}

	void CollectGridOutline()
	{
		TMap<FLinearColor, TArray<FGridOutlineInfo*> > Color2GridInfoArray;

		// group grids by color
		for (int i = 0; i < UpdateParams.GridInfoArray.Num(); ++i)
		{
			FGridOutlineInfo& GridInfo = UpdateParams.GridInfoArray[i];
			if (Color2GridInfoArray.Contains(GridInfo.Color))
			{
				Color2GridInfoArray.FindChecked(GridInfo.Color).Add(&GridInfo);
			}
			else
			{
				Color2GridInfoArray.Add(GridInfo.Color, { &GridInfo });
			}
		}

		//for the grids has same color, group by height
		TMap<int32, TArray<FGridOutlineInfo*> > Height2GridInfoArray;
		for (auto& Elem : Color2GridInfoArray)
		{
			const FLinearColor& Color = Elem.Key;
			for (int i = 0; i < Elem.Value.Num(); ++i)
			{
				FGridOutlineInfo* GridInfo = Elem.Value[i];

				int32 Height = (int32)(GridInfo->Center.Z / UpdateParams.GridSize);
				if (Height2GridInfoArray.Contains(Height))
				{
					Height2GridInfoArray.FindChecked(Height).Add(GridInfo);
				}
				else
				{
					Height2GridInfoArray.Add(Height, { GridInfo });
				}
			}
		}

		// grid in the same group has same color, similar height, ready for combine
		for (auto& Elem : Height2GridInfoArray)
		{
			switch (UpdateParams.GridType)
			{
			case Square:
				CombineSquareEdges(Elem.Value);
				break;
			case Hexagon:
				// hexagon edges can't combine
				CreateHexagonEdges(Elem.Value);
				break;
			default:
				break;
			}
		}
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

		CollectGridOutline();
	}

private:
	FGridOutlinePrimitiveCompUpdateParams UpdateParams;

	TArray<FEdgeGroup> EdgeGroups;
};
//////////////////////////////////////////////////////////////////////////
UGridOutlinePrimitiveComponent::UGridOutlinePrimitiveComponent()
{
}

UGridOutlinePrimitiveComponent::~UGridOutlinePrimitiveComponent()
{
}

FPrimitiveSceneProxy* UGridOutlinePrimitiveComponent::CreateSceneProxy()
{
	return new FGridOutlinePrimitiveSceneProxy(this);
}

FBoxSphereBounds UGridOutlinePrimitiveComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	FBoxSphereBounds Bounds(ForceInitToZero);

	UGridOutlinePainter* GridPainter = Cast<UGridOutlinePainter>(GetOuter());

	for (int i = 0; i < GridPainter->VisibleGrids.Num(); ++i)
	{
		UGrid* Grid = GridPainter->VisibleGrids[i];

		Bounds = Bounds + Grid->Bounds;
	}
	return Bounds;
}

void UGridOutlinePrimitiveComponent::UpdateGridInfo()
{
	UGridOutlinePainter* GridPainter = Cast<UGridOutlinePainter>(GetOuter());
	AGridManager* GridManager = GridPainter->GridManager;

	FGridOutlinePrimitiveCompUpdateParams UpdateParams;
	UpdateParams.SceneProxy = (FGridOutlinePrimitiveSceneProxy*)SceneProxy;
	UpdateParams.GridType = Cast<ASquareGridManager>(GridManager) == nullptr ? EGridType::Hexagon : EGridType::Square;
	UpdateParams.GridSize = GridManager->GetGridSize();
	UpdateParams.Thickness = GridPainter->Thickness;
	UpdateParams.Margin = GridPainter->Margin;
	UpdateParams.ZDelta = GridPainter->ZDelta;

	FGridOutlineInfo GridInfo;
	for (int i = 0; i < GridPainter->VisibleGrids.Num(); ++i)
	{
		UGrid* Grid = GridPainter->VisibleGrids[i];

		GridInfo.Coord = Grid->GetCoord();
		GridInfo.Center = Grid->GetCenter();
		GridInfo.Color = GridPainter->GetColor(Grid);

		UpdateParams.GridInfoArray.Add(GridInfo);
	}

	check(UpdateParams.SceneProxy != nullptr);

	ENQUEUE_RENDER_COMMAND(GridOutlinePrimitiveCompUpdateCommand)(
		[UpdateParams](FRHICommandListImmediate& RHICmdList)
	{
		FScopeCycleCounter Context(UpdateParams.SceneProxy->GetStatId());
		UpdateParams.SceneProxy->Update(UpdateParams);
	});

	UpdateBounds();
	MarkRenderTransformDirty();
}
