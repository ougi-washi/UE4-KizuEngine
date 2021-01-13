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
|                      Image Reference                       |
|![1](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/CharacterData.png)|


|Functions samples|
|-------|
|As an exmaple of some functions, these resources Blueprints offer you the possibility to change the values of the resources. Eventually, you're not obliged to use these unless you want to create some custom system.|
|                      Image Reference                       |
|![2](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/ResourcesBP.png)|

### Spawnable Ability Class

|Base Spawnable Ability|
|-------|
|Create a spawnable ability (Projectiles, Spells, Other) that inherits from KSpawnableAbility and you'll be able to set its data in the default class section. The spawnable ability data contains an array of effects where every effect would affect the resources of the other actors in collision during Spawn or after colliding. The spell can affect the owner, a different faction from the owner's or the owner's faction. Aside from that, every Effect can contain one or multiple status effects(buff/debuff) that will be applied on the target affected.|
|![1](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/Spell.png)|

|Functions samples|
|-------|
|As an exmaple of some functions, these spells Blueprints offer you the ability to create a custom Spell without making use of the Data.|
|![2](https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/SpellsBP.png)|
