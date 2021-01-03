

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_KExecuteAction.generated.h"

class UDataTable;

/**
 * 
 */
UCLASS()
class KIZUENGINE_API UBTTask_KExecuteAction : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UPROPERTY(Category = Node, EditAnywhere)
	UDataTable* ActionDataTable;

	UPROPERTY(Category = Node, EditAnywhere)
	FName ActionRowName;

	UPROPERTY(Category = Node, EditAnywhere)
	uint8 bUseCooldown : 1;

	UBTTask_KExecuteAction();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;


};
