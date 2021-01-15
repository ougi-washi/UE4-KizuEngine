<p align="center">
  <img src="https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/Icon128.png">
</p>

## Introduction

This page represents the documentation of the KizuEngine framework that will make it easy to the developers to make Replicated games. 

## How to use

1. Adding the plugin to your project's "/Plugins/" folder [(Binaries can be found here)](https://github.com/Hiro-KE/UE4-KizuEngine/releases). 
2. After starting the editor, you'll be able to enable the KizuEngine Plugin.
3. It will require you to restart the Unreal Engine Editor. Now, all the framework components are accessible to your project.

### Character Class

|Base resources|
|-------|
|Create a character that inherits from KCharacter and you'll be able to give it all kind of resources Data. This can include health, energy, physical power, magical power, armor, etc. Any kind of Resources owned by the character will be replicated and can be accessed from multiple other functions or events around the game.|
|![1](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/CharacterData.png)|
|As an exmaple of some functions, these resources Blueprints Functions gives you the possibility to change the values of the resources. Eventually, you're not obliged to use these unless you want to create some custom system.|
|![2](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/ResourcesBP.png)|
|Events can as well be called to trigger specific events or functions (Eg. Play Animation when gaining health or when leveling up, etc.)|
|![2](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/CharacterEvents.png)|
|Character States|
|Characters have eventually states that identify what state are they in in order to know whether they can execute a specific Action or Reaction or not. There is a list of all available states and also a single valid state.|
|![3](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/StatesData.png)|
|Eventually setting is as easy as calling the following "ServerSetCurrentstate" function.|
|![4](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/ServerSetCurrentstate.png)|
|Or during an animation by using an AnimNotify called KSetState or "Set State (Kizu)".|
|![5](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/StateNotify.png)|

### Actions

|Actions system|
|-------|
|So now that we have a character class we chose to run animations and make them execute specific actions. This is possible through creating an "ActionData" Data Table and filling it with the required data. So in every single row has : |
|Name : representing the name or ID of the Action Data, it's recommanded to keep it same as row name in case you'd want to store and parse an array of Actions without relying on a data table.|
|Montage data array : this represents all the animations executed by this specific actions. each animation can be executed relying on a set of conditions (direction and valid character states). The system will go through all of the animations, check your Direction compared to the Camera or Actor rotation and check your state in order to determinate what Montage to pick exactly. This also follows a combo counter, wherever it finds multiple results, it moves to the next one if you retrigger the same action wihout calling a "Reset Combo" AnimNotify.|
|Resource Name, Value and cooldown : These are self explanatory where it represents the basic Actions system over relying on a cooldown and what resource to consume. You have to set a resource even if it's 0 as value. (will be fixed soon).|
|![6](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/ActionDT.png)|
|Executing actions is possible through the following functions.|
|![6](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/ActionsBP.png)|

### Reactions

|Reaction system|
|-------|
|Characters can send reactions to each others, whether via a function call, with a melee or a spawnable ability. This is possible through creating an "ReactionData" Data Table and filling it with the required data. So in every single row you have : |
|Name : representing the name or ID of the Reaction Data, it's recommanded to keep it same as row name in case you'd want to store and parse an array of Reactions without relying on a data table.|
|You do have the choice whether you'll use basic or advanced reactions.
Basic reactions will just check the state of the character. However, Advanced reactions check if the state and the relative source position of the reaction causing them to have a specific reaction for each specific side (Front, Back, Left, Right).|
|The filter option is not implemented yet, where you can have custom filters aside from the relative position and state (will be implemented soon).|
|![7](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/ReactionDT.png)|


### Spawnable Ability Class

|Base Spawnable Ability|
|-------|
|Create a spawnable ability (Projectiles, Spells, Other) that inherits from KSpawnableAbility and you'll be able to set its data in the default class section. The spawnable ability data contains an array of effects where every effect would affect the resources of the other actors in collision during Spawn or after colliding. The Spawnable Ability can affect the owner, a different faction from the owner's or the owner's faction. Aside from that, every Effect can contain one or multiple status effects(buff/debuff) that will be applied on the target affected.|
|Reactions :  you can also send a reaction to the affected target (Whether specific reaction that depends on this Spawnable Ability or depends on a data table in the affected actor.|
|Smooth reaction (Experimental) : is an implementation that makes the reaction happens locally before sending it to the server, the client then will execute it locally and then it will happen on all the other clients besides the reaction source and the reaction target. This functionality is still in testing and may not 100% be reliable.|
|Ticking/Affect once : The Spawnable Ability can eventually affect the collided actors everytime they collide again with them and can have a ticking effect that can occur depending on the rate.|
|Destroy actor : The Destroy on hit will start the destruction timer the moment it collides, will disable all collisions and make it invisible. This will allow to have enough time to trigger effects and sounds|
|![8](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/SpawnableAbility.png)|
|As an exmaple of some functions, these Ability System Blueprints Functions gives you the ability to customize some of the events that happen such as initiating the movement or such.|
|![9](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/SpawnableAbilityFunc.png)|

### Buff/Debuff Class

|Base Buffs and Debuffs|
|-------|
|Create a Buff/Debuff (Heal over time, Damage over time, resource gain over time.. etc) that inherits from KBuff and you'll be able to set its data in the default class section. The Buff data contains an array of effects where every effect would affect the resources of the other actor by setting its "Ticking Rate", "Duration" and "Ticking Delay".|
|Attaching the actor to the target would basically attach to the target character this buff on a specific socket.
Use Time Dilation : This would cause a replicated time dilation during the duration of the Buff/Debuff.|
|![10](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/Buff.png)|
|As an exmaple of some functions, these Buffs Blueprints gives you to customize the Buff and make it have a non linear effects or any type of effect depending on the needs.|
|![12](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/BuffFuncBP.png)|

### Melee (Close interaction)

|Melee AnimNotify State|
|-------|
|"KMelee" or "Melee (Kizu)" is an AnimNotifyState that would affect the target in the collision by affecting the health of the target. Setting it up will be by : 
Adding a collision in your KCharacter and giving it a tag that you'll use as a "Collision Component Tag" in the Notify.|
|Setting up what faction will it affect and whether it should affect self or not. |
|Value source can be used to set the damage value. "Static" will be a constant that you'll set in the AnimNotify details panel. "Dynamic" will be via a percentage from the Resources you have in your KCharacter. "Custom" will be by adding custom damage to your character before executing this Notify then using its ID for a specific amount of damage.|
|Reaction Data Sending section determines whether to send reaction or not and set up the data of the reaction. (Reaction Data sending variables explained at Spawnable Ability).|
|![10](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/MeleeNotify.png)|
|Adding a custom damage is possible through calling these functions|
|![12](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/CustomDamageBP.png)|

### Cooldowns

|Cooldown System|
|-------|
|As every Action has a cooldown, you can access and manage the cooldowns easily through these functions. |
|![10](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/CooldownsBP.png)|
