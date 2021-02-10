<p align="center">
  <img src="https://hiro-ke.github.io/UE4-KizuEngine/wiki/images/Icon128.png">
</p>

Kizu Engine
============

[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

Kizu Engine is an Unreal Engine plug-in that represents a game framework containing the main components for replicated game systems.
It can be used for a huge variety of projects due to its flexibility.

Status
------

Version : v0.3.210210 (pre-release)

Supported engine version : 4.25*

\* *Plugin has been tested and if necessary updated to compile and work with this engine version. As long as possible I will try to maintain backward compatibility of existing features and possibly but not necessarily when adding new features.*

About
-----

The main goal of this plugin is to provide the main components in most of game systems while giving the possibility for replicated usage offering to artists and designers the possibility to make complex online game systems with less effort as Unreal Engine 4 replication system is quite tricky and could be hard for non-programmers.

Most of the functionalities of Kizu Engine are exposed to the unreal engine blueprints. Thus, no C++ skills are required to make use of this plugin.



### Key features
- Resources (Health or any custom resources/stats)
  - Gain
  - Loss
  - Regeneration
- Stats
- Actions (Combo and state dependant)
- Reactions (State and relative source position dependant)
- Cooldowns
- States
- Custom Damage types
- Spawnable abilities (Projectiles, Spells..etc)
- Status effect (Buffs and Debuffs)
- Inventory (Basic)
- Objective system (Basic)

### Extra
- Anim notifies for :
  - States
  - Combo counter
  - Melee tracing
  - Spawnable Ability spawning
- Ai Tasks :
  - Executing Actions
- Ai Services :
  - Updating Blackboard State
- Getters for most of the systems via Custom Arrays or Data Tables

Setting Up The Plugin
-----
- Adding the plugin to your project's "/Plugins/" folder (Binaries can be found here).
- After starting the editor, you'll be able to enable the KizuEngine Plugin.
- It will require you to restart the Unreal Engine Editor. Now, all the framework components are accessible to your project.

How to use
-----
[Documentation link here](https://hiro-ke.github.io/UE4-KizuEngine/) (Still WIP)

Contact
--------
For any issue or other services :
  Jed Fakhfekh
  jedfakhfekh@gmail.com

License
-------
Kizu Engine is licensed under the MIT License, see LICENSE for more information.
