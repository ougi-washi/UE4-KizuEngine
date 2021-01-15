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

|Functions samples|
|-------|
|As an exmaple of some functions, these resources Blueprints Functions offer you the possibility to change the values of the resources. Eventually, you're not obliged to use these unless you want to create some custom system.|
|![2](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/ResourcesBP.png)|

### Spawnable Ability Class

|Base Spawnable Ability|
|-------|
|Create a spawnable ability (Projectiles, Spells, Other) that inherits from KSpawnableAbility and you'll be able to set its data in the default class section. The spawnable ability data contains an array of effects where every effect would affect the resources of the other actors in collision during Spawn or after colliding. The Spawnable Ability can affect the owner, a different faction from the owner's or the owner's faction. Aside from that, every Effect can contain one or multiple status effects(buff/debuff) that will be applied on the target affected. 
Reactions :  you can also send a reaction to the affected target (Whether specific reaction that depends on this Spawnable Ability or depends on a data table in the affected actor.
Smooth reaction (Experimental) : is an implementation that makes the reaction happens locally before sending it to the server, the client then will execute it locally and then it will happen on all the other clients besides the reaction source and the reaction target. This functionality is still in testing and may not 100% be reliable.
Ticking/Affect once : The Spawnable Ability can eventually affect the collided actors everytime they collide again with them and can have a ticking effect that can occur depending on the rate.
Destroy actor : The Destroy on hit will start the destruction timer the moment it collides, will disable all collisions and make it invisible. This will allow to have enough time to trigger effects and sounds|
|![1](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/SpawnableAbility.png)|
|Functions samples|
|As an exmaple of some functions, these Ability System Blueprints Functions offer you the ability to customize some of the events that happen such as initiating the movement or such.|
|![2](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/SpawnableAbilityFunc.png)|

### Buff/Debuff Class

|Base Buffs and Debuffs|
|-------|
|Create a Buff/Debuff (Heal over time, Damage over time, resource gain over time.. etc) that inherits from KBuff and you'll be able to set its data in the default class section. The Buff data contains an array of effects where every effect would affect the resources of the other actor by setting its "Ticking Rate", "Duration" and "Ticking Delay". 
Attaching the actor to the target would basically attach to the target character this buff on a specific socket.
Use Time Dilation : This would cause a replicated time dilation during the duration of the Buff/Debuff.|
|![1](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/Buff.png)|
|Functions samples|
|As an exmaple of some functions, these Buffs Blueprints offer you to customize the Buff and make it have a non linear effects or any type of effect depending on the needs.|
|![2](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/BuffFuncBP.png)|

