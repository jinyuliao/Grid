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

struct FSquareEdge
{
	SquareNS::EEdgeDirection Direction;

	FIntVector Coord;

	FVector GridCenter;

	FLinearColor Color;
};

struct FHexagonEdge
{
	HexagonNS::EEdgeDirection Direction;

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
	FGridOutlinePrimitiveSceneProxy* SceneProxy;

	EGridType GridType;

	float GridSize;

	float Thickness;

	float ZDelta;

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

	FVector GetEdgeV0(SquareNS::EEdgeDirection Dir, const FVector& Center)
	{
		const float HalfGridSize = UpdateParams.GridSize / 2.f;
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

	FVector GetEdgeV1(SquareNS::EEdgeDirection Dir, const FVector& Center)
	{
		const float HalfGridSize = UpdateParams.GridSize / 2.f;
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

	FVector GetEdgeV0(HexagonNS::EEdgeDirection Dir, const FVector& Center)
	{
		float Radian = PI / 180 * (Dir * 60 + 30);
		return Center + FVector(UpdateParams.GridSize * FMath::Cos(Radian), UpdateParams.GridSize * FMath::Sin(Radian), UpdateParams.ZDelta);
	}

	FVector GetEdgeV1(HexagonNS::EEdgeDirection Dir, const FVector& Center)
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

	void GetCounterEdge(SquareNS::EEdgeDirection InDir, const FIntVector& InCoord, SquareNS::EEdgeDirection& CounterDir, FIntVector& CounterCoord)
	{
		const SquareNS::EEdgeDirection CounterDirections[] = { SquareNS::Right, SquareNS::Left, SquareNS::Bottom, SquareNS::Top };
		const FIntVector CounterCoordDelta[] = { FIntVector(0, -1, 0), FIntVector(0, 1, 0), FIntVector(1, 0, 0), FIntVector(-1, 0, 0) };

		CounterDir = CounterDirections[InDir];
		CounterCoord = InCoord + CounterCoordDelta[InDir];
	}

	void GetCounterEdge(HexagonNS::EEdgeDirection InDir, const FIntVector& InCoord, HexagonNS::EEdgeDirection& CounterDir, FIntVector& CounterCoord)
	{
		const HexagonNS::EEdgeDirection CounterDirections[] = { HexagonNS::RightBottom, HexagonNS::LeftBottom, HexagonNS::Left
																,HexagonNS::LeftTop, HexagonNS::RightTop, HexagonNS::Right };
		const FIntVector CounterCoordDelta[] = { FIntVector(0, -1, 1), FIntVector(-1, 0, 1), FIntVector(-1, 1, 0)
												,FIntVector(0, 1, -1), FIntVector(1, 0, -1), FIntVector(1, -1, 0) };

		CounterDir = CounterDirections[InDir];
		CounterCoord = InCoord + CounterCoordDelta[InDir];
	}

	void FindSquareOutlineEdges(const TArray<FGridOutlineInfo*>& GridGroup, TArray<FSquareEdge>& OutlineEdges)
	{
		if (GridGroup.Num() == 0)
			return;

		FIntVector Coord, CounterCoord;
		TArray<FIntVector> Dir2Grids[SquareNS::Max];
		TMap<FIntVector, FVector> Coord2Center;
		SquareNS::EEdgeDirection CounterDir;

		for (int i = 0; i < GridGroup.Num(); ++i)
		{
			Coord = GridGroup[i]->Coord;

			Coord2Center.Add(Coord, GridGroup[i]->Center);

			for (int DirIndex = 0; DirIndex < 4; ++DirIndex)
			{
				GetCounterEdge((SquareNS::EEdgeDirection)DirIndex, Coord, CounterDir, CounterCoord);

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

		FSquareEdge SquareEdge;
		for (int i = 0; i < SquareNS::Max; ++i)
		{
			TArray<FIntVector>& Grids = Dir2Grids[i];

			for (int j = 0; j < Grids.Num(); ++j)
			{
				SquareEdge.Coord = Grids[j];
				SquareEdge.Color = GridGroup[0]->Color;;
				SquareEdge.GridCenter = Coord2Center.FindChecked(SquareEdge.Coord);
				SquareEdge.Direction = (SquareNS::EEdgeDirection)i;
				OutlineEdges.Add(SquareEdge);
			}
		}
	}

	void RemoveRepeatEdges(TArray<FSquareEdge>& OutlineEdges)
	{
		TArray<FSquareEdge> Result;
		FIntVector CounterCoord;
		SquareNS::EEdgeDirection CounterDir;
		for (int i = 0; i < OutlineEdges.Num(); ++i)
		{
			FSquareEdge& Edge = OutlineEdges[i];
			GetCounterEdge(Edge.Direction, Edge.Coord, CounterDir, CounterCoord);

			int j;
			for (j = 0; j < Result.Num(); ++j)
			{
				FSquareEdge& CurrEdge = Result[j];

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

	void CreateSquareEdges(const TArray<FSquareEdge>& SquareEdges)
	{
		if (SquareEdges.Num() == 0)
			return;

		FEdge Edge;
		for (int i = 0; i < SquareEdges.Num(); ++i)
		{
			const FSquareEdge& SquareEdge = SquareEdges[i];
			Edge = FEdge(GetEdgeV0(SquareEdge.Direction, SquareEdge.GridCenter), GetEdgeV1(SquareEdge.Direction, SquareEdge.GridCenter));

			int j;
			for (j = 0; j < EdgeGroups.Num(); ++j)
			{
				if (EdgeGroups[j].Color == SquareEdge.Color)
				{
					EdgeGroups[j].Edges.Add(Edge);
					break;
				}
			}

			if (j >= EdgeGroups.Num())
				EdgeGroups.Add(FEdgeGroup(SquareEdge.Color, { Edge }));
		}
	}

	void FindHexagonOutlineEdges(const TArray<FGridOutlineInfo*>& GridGroup, TArray<FHexagonEdge>& OutlineEdges)
	{
		if (GridGroup.Num() == 0)
			return;

		FIntVector Coord, CounterCoord;
		HexagonNS::EEdgeDirection CounterDir;
		TArray<FIntVector> Dir2Grids[HexagonNS::Max];
		TMap<FIntVector, FVector> Coord2Center;

		for (int i = 0; i < GridGroup.Num(); ++i)
		{
			Coord = GridGroup[i]->Coord;

			Coord2Center.Add(Coord, GridGroup[i]->Center);

			for (int DirIndex = 0; DirIndex < HexagonNS::Max; ++DirIndex)
			{
				GetCounterEdge((HexagonNS::EEdgeDirection)DirIndex, Coord, CounterDir, CounterCoord);

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

		FHexagonEdge HexEdge;
		for (int i = 0; i < HexagonNS::Max; ++i)
		{
			TArray<FIntVector>& Grids = Dir2Grids[i];

			for (int j = 0; j < Grids.Num(); ++j)
			{
				HexEdge.Coord = Grids[j];
				HexEdge.Color = GridGroup[0]->Color;;
				HexEdge.GridCenter = Coord2Center.FindChecked(HexEdge.Coord);
				HexEdge.Direction = (HexagonNS::EEdgeDirection)i;
				OutlineEdges.Add(HexEdge);
			}
		}
	}

	void RemoveRepeatEdges(TArray<FHexagonEdge>& OutlineEdges)
	{
		TArray<FHexagonEdge> Result;
		FIntVector CounterCoord;
		HexagonNS::EEdgeDirection CounterDir;
		for (int i = 0; i < OutlineEdges.Num(); ++i)
		{
			FHexagonEdge& Edge = OutlineEdges[i];
			GetCounterEdge(Edge.Direction, Edge.Coord, CounterDir, CounterCoord);

			int j;
			for (j = 0; j < Result.Num(); ++j)
			{
				FHexagonEdge& CurrEdge = Result[j];

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

	void CreateHexagonEdges(const TArray<FHexagonEdge>& HexagonEdges)
	{
		if (HexagonEdges.Num() == 0)
			return;

		FEdge Edge;
		for (int i = 0; i < HexagonEdges.Num(); ++i)
		{
			const FHexagonEdge& HexEdge = HexagonEdges[i];
			Edge = FEdge(GetEdgeV0(HexEdge.Direction, HexEdge.GridCenter), GetEdgeV1(HexEdge.Direction, HexEdge.GridCenter));

			int j;
			for (j = 0; j < EdgeGroups.Num(); ++j)
			{
				if (EdgeGroups[j].Color == HexEdge.Color)
				{
					EdgeGroups[j].Edges.Add(Edge);
					break;
				}
			}

			if (j >= EdgeGroups.Num())
				EdgeGroups.Add(FEdgeGroup(HexEdge.Color, { Edge }));
		}
	}

	void CollectGridOutline()
	{
		TArray<FHexagonEdge> HexOutlineEdges;
		TArray<FSquareEdge> SquareOutlineEdges;
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
				switch (UpdateParams.GridType)
				{
				case Square:
					FindSquareOutlineEdges(Height2GridsIter.Value, SquareOutlineEdges);
					break;
				case Hexagon:
					FindHexagonOutlineEdges(Height2GridsIter.Value, HexOutlineEdges);
					break;
				default:
					break;
				}
			}
		}

		switch (UpdateParams.GridType)
		{
		case Square:
			SquareOutlineEdges.Sort([&](const FSquareEdge& L, const FSquareEdge& R)
			{
				int32 LIdx = -1, RIdx = -1;
				bool LSucc = UpdateParams.ColorPriorities.Find(L.Color, LIdx);
				bool RSucc = UpdateParams.ColorPriorities.Find(R.Color, RIdx);
				if (!LSucc || !RSucc)
					LOG_ERROR(TEXT("FGridOutlinePrimitiveSceneProxy::CollectGridOutline Hexagon grid sort failed, can't find grid color in UpdateParams.ColorPriorities"));
				return LIdx < RIdx;
			});

			RemoveRepeatEdges(SquareOutlineEdges);
			CreateSquareEdges(SquareOutlineEdges);
			break;
		case Hexagon:
			HexOutlineEdges.Sort([&](const FHexagonEdge& L, const FHexagonEdge& R) 
			{
				int32 LIdx = -1, RIdx = -1;
				bool LSucc = UpdateParams.ColorPriorities.Find(L.Color, LIdx);
				bool RSucc = UpdateParams.ColorPriorities.Find(R.Color, RIdx);
				if (!LSucc || !RSucc)
					LOG_ERROR(TEXT("FGridOutlinePrimitiveSceneProxy::CollectGridOutline Hexagon grid sort failed, can't find grid color in UpdateParams.ColorPriorities"));
				return LIdx < RIdx;
			});

			RemoveRepeatEdges(HexOutlineEdges);
			CreateHexagonEdges(HexOutlineEdges);
			break;
		default:
			break;
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

	if (GridPainter == nullptr)
		return Bounds;

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
	UpdateParams.Thickness = GridPainter->OutlineThickness;
	UpdateParams.ZDelta = GridPainter->ZDelta;
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

	ENQUEUE_RENDER_COMMAND(GridOutlinePrimitiveCompUpdateCommand)(
		[UpdateParams](FRHICommandListImmediate& RHICmdList)
	{
		FScopeCycleCounter Context(UpdateParams.SceneProxy->GetStatId());
		UpdateParams.SceneProxy->Update(UpdateParams);
	});

	UpdateBounds();
	MarkRenderTransformDirty();
}
