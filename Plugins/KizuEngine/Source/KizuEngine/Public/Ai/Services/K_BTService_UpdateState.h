// Copyright (c) 2019 Nexus Division. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "Core/KCharacter.h"
#include "K_BTService_UpdateState.generated.h"

/**
 * 
 */

 /** This will update the state of the blackboard from the KCharacter owning this Blackboard. */
UCLASS(Blueprintable, Category = Kizu, meta = (DisplayName = "Update Blackboard Active State"))
class KIZUENGINE_API UK_BTService_UpdateState : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:

	UK_BTService_UpdateState();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Defaults", meta = (ExposeOnSpawn = "true", AllowPrivateAccess = "true"))
	FBlackboardKeySelector ActiveState;

	UPROPERTY(BlueprintReadOnly)
	AKCharacter* OwnerCharacter = nullptr;

	/*Tick only on instance*/
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category = "Ai|Service")
	void SetReferences(UBehaviorTreeComponent* OwnerComp);

	UFUNCTION(BlueprintCallable, Category = "Ai|Service")
	void UpdateState(UBehaviorTreeComponent* OwnerComp);

	virtual FString GetStaticDescription() const override;

};
