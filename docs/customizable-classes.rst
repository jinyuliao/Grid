Customization
=============

Customizable Classes
--------------------

* UGridInfo: inherit from this class to add game-specific information
* USquarePathFinder and UHexagonPathFinder: customize pathfinding logic
* UGridNavigationAgent: inherit from this class to implement special movement, e.g.: jump, climb and so on
* UGridPainter: customize grid rendering [#f1]_ 

.. [#f1] Default is UGridDecalPainter which using decal component to rendering grid