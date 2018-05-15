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
* Grid sensing component [#f1]_
* Blueprint support
* Editor integration
* Customizable classes
* PathGuide for rendering path

QuickStart
----------

* Set pawn's default controller to AAIController or its subclass [#f2]_ 
* Add UGridNavigationComponent to your pawn [#f3]_
* Add UGridSensingComponent to your pawn (optional)
* Spawn GridManager at game startup [#f4]_
* Write game logic

Customizable Classes
--------------------

* UGridInfo: inherit from this class to add game-specific information
* USquarePathFinder and UHexagonPathFinder: customize pathfinding logic
* UGridNavigationAgent: inherit from this class to implement special movement, e.g.: jump, climb and so on
* UGridPainter: customize grid rendering [#f5]_ 

Install
-------

#. Clone this project to ${YourProject}/Plugins/
#. Generate project file
#. Compile

Donate
------

.. image:: https://jinyuliao.github.io/blog/html/_images/donate.png
    :target: https://jinyuliao.github.io/blog/html/pages/donate.html
    :alt: Donate

.. _SampleProject: https://github.com/jinyuliao/Sample_Grid
.. _SRPGTemplate: https://github.com/jinyuliao/SRPGTemplate

.. [#f1] plus a component visualizer for debug and level design
.. [#f2] we need it to implement default grid-based movement
.. [#f3] if you have custom UGridNavigationAgent, add it to Component's 'AgentClasses'
.. [#f4] if you have custom class, set it properly in spawn parameter
.. [#f5] Default is UGridDecalPainter which using decal component to rendering grid