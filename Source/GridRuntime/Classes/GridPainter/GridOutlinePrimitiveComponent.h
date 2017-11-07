#pragma once

#include "CoreMinimal.h"
#include "PrimitiveSceneProxy.h"
#include "Components/PrimitiveComponent.h"
#include "GridOutlinePrimitiveComponent.generated.h"

class UGridOutlinePainter;
class FGridOutlinePrimitiveSceneProxy;

/**
 * 
 */
UCLASS()
class GRIDRUNTIME_API UGridOutlinePrimitiveComponent : public UPrimitiveComponent
{
	GENERATED_BODY()
	
public:
	UGridOutlinePrimitiveComponent();
	virtual ~UGridOutlinePrimitiveComponent();

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	virtual void UpdateGridInfo();

	UPROPERTY()
	UGridOutlinePainter* GridPainter;
};
