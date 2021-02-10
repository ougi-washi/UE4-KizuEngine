// Copyright (c) 2019 Nexus Division. All rights reserved.


#include "Ai/Services/K_BTService_UpdateState.h"
#include "BehaviorTree/BlackboardComponent.h"

UK_BTService_UpdateState::UK_BTService_UpdateState()
{
	NodeName = "Updates Blackboard ActiveState Key";
}

void UK_BTService_UpdateState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if (OwnerCharacter)
	{
		UpdateState(&OwnerComp);
	}
	else
	{
		SetReferences(&OwnerComp);
		UpdateState(&OwnerComp);
	}
}

void UK_BTService_UpdateState::SetReferences(UBehaviorTreeComponent* OwnerComp)
{
	if (AController* Controller = Cast<AController>(OwnerComp->GetOwner()))
	{
		if (Controller->GetPawn())
		{
			OwnerCharacter = Cast<AKCharacter>(Controller->GetPawn());
		}
	}
}

void UK_BTService_UpdateState::UpdateState(UBehaviorTreeComponent* OwnerComp)
{
	if (OwnerCharacter) {
		OwnerComp->GetBlackboardComponent()->SetValueAsString(ActiveState.SelectedKeyName, OwnerCharacter->ActiveState);
	}
}

FString UK_BTService_UpdateState::GetStaticDescription() const
{
	return "Updates Blackboard [" + ActiveState.SelectedKeyName.ToString() + "] key";
}
