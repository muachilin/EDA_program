# Functionally Reduced AND-Inverter Graph
Main Functions  

  1. Sweep  
  Remove the gates that cannot be reached from POs   
        
  2. Optimization   
  Perform trivial circuit optimizations including constant propagation and redundant gate removal.  

  3. Strash  
  Merge the structurally equivalent gates.   
  After the merger, the fanouts of the merged gate will be re-connected to the gate that merges it.  
 
  4. Simulate  
  Perform circuit simulation to distinguish the functionally different signals and thus collect the FEC pairs/groups.
  
  5. Fraig  
  Based on the identified FEC pairs/groups, perform fraig operations on the circuit. 
  All the SAT-proven equivalent gates should be merged together

project copyright:

  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]


