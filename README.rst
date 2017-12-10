Grid Plugin
===========

An UE4 plugin for grid-based game

* SampleProject_
* A more complicated project: SRPGTemplate_

.. image:: https://github.com/jinyuliao/Grid/blob/master/docs/images/grid.png?raw=true

Features
--------

* Support square and hexagon grid
* Support multi-level grid
* Grid-based pathfinding and movement
* Multiple grid rendering method: Decal and Outline(XCom2-like)
* Blueprint support
* Editor integration
* Customizable classes
* PathGuide for rendering path

QuickStart
----------

* Implement IGridPawnInterface(C++) or GridPawnInterface(Blueprint) in your pawn class
* Set pawn's default controller to AAIController or its subclass [#f1]_ 
* Add UGridNavigationComponent to your pawn [#f2]_
* Spawn GridManager at game startup [#f3]_
* Write game logic, most function you need is in GridManager

Customizable Classes
--------------------

* UGridInfo: inherit from this class to add game-specific information
* USquarePathFinder and UHexagonPathFinder: customize pathfinding logic
* UGridNavigationAgent: inherit from this class to implement special movement, e.g.: jump, climb and so on
* UGridPainter: customize grid rendering [#f4]_ 

Install
-------

#. Clone this project to ${YourProject}/Plugins/
#. Generate project file
#. Compile


.. image:: docs/images/donate.png
    :target: https://jinyuliao.github.io/blog/html/pages/donate.html

.. _SampleProject: https://github.com/jinyuliao/Sample_Grid
.. _SRPGTemplate: https://github.com/jinyuliao/SRPGTemplate

.. [#f1] we need it to implement default grid-based movement
.. [#f2] if you have custom UGridNavigationAgent, add it to Component's 'AgentClasses'
.. [#f3] if you have custom class, set it properly in spawn parameter
.. [#f4] Default is UGridDecalPainter which using decal component to rendering grid