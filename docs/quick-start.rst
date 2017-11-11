Quick Start
===========

* Implement IGridPawnInterface(C++) or GridPawnInterface(Blueprint) in your pawn class
* Set your pawn's default controller to AAIController or its subclass [#f1]_ 
* Add UGridNavigationComponent to your pawn [#f2]_
* Spawn GridManager at game startup [#f3]_
* Write you game logic, most function you need is in GridManager
  
.. [#f1] we need it to implement default grid-based movement
.. [#f2] if you have custom UGridNavigationAgent, add it to Component's 'AgentClasses'
.. [#f3] if you have custom class, set it properly in spawn parameter