Grid Plugin
===========

An UE4 plugin for grid-based game

Feature
-------

* Support square and hexagon grid
* Support multi-level grid
* Grid-based pathfinding and movement
* Blueprint support
* Editor integration
* Customizable classes

Sample
------

Sample_Grid_

QuickStart
----------

* Your pawn should implement IGridPawnInterface(C++) or GridPawnInterface(Blueprint)
* Your pawn's default controller should be AAIController or its subclass(we need it to implement default grid-based movement)
* Add UGridNavigationComponent to your pawn(if you has custom UGridNavigationAgent, add it to Component's 'AgentClasses')
* Spawn GridManager at game startup(if you have custom class, set it properly in spawn parameter)
* Write you game logic, most function you need is in GridManager

Customizable Classes
--------------------

* UGridInfo: inherit from this class to add game-specific information
* USquarePathFinder and UHexagonPathFinder: customize pathfinding logic
* UGridNavigationAgent: inherit from this class to implement special movement, e.g.: jump, climb and so on
* UGridPainter: customize grid rendering(default is UGridDecalPainter which using decal component to rendering grid)

Install
-------

#. clone this project to ${YourProject}/Plugins/
#. generate project file
#. compile


.. _Sample_Grid: https://github.com/jinyuliao/Sample_Grid