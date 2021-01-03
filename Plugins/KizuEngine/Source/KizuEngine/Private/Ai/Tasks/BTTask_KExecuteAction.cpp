


#include "Ai/Tasks/BTTask_KExecuteAction.h"
#include "AIController.h"
#include "Core/KCharacter.h"
#include "Engine/DataTable.h"
#include "FunctionLibrary/KActionFunctionLibrary.h"

UBTTask_KExecuteAction::UBTTask_KExecuteAction() : Super()
{
	NodeName = "Execute Action (Kizu)";
}

EBTNodeResult::Type UBTTask_KExecuteAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	if (AAIController* AiController = Cast<AAIController> (OwnerComp.GetOwner()))
		if (AKCharacter* KCharacter = Cast<AKCharacter>(AiController->GetPawn())) {
			if (UKActionFunctionLibrary::ExecuteActionFromDT(KCharacter, ActionDataTable, ActionRowName, bUseCooldown))
				return EBTNodeResult::Succeeded;
		}
	return EBTNodeResult::Failed;
}

