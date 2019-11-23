# Functionally Reduced AND-Inverter Graph
  Project Copyright
  ------------

  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
  This project is written by Chung-Yang (Ric) Huang. Parts of the programs are designed by myself.
  
  Main Functions
  ----------

  1. Sweep  
     - Remove the gates that cannot be reached from POs   
        
  2. Optimization   
     - Perform trivial circuit optimizations including constant propagation and redundant gate removal.  

  3. Strash  
     - Merge the structurally equivalent gates. After the merger, the fanouts of the merged gate will be re-connected to the gate that merges it.  
 
  4. Simulate  
     - Perform circuit simulation to distinguish the functionally different signals. And thus collect the Functionally Equivalent Candidates (FEC) pairs/groups.
  
  5. Fraig  
     - Based on the identified FEC pairs/groups, perform fraig operations on the circuit. All the SAT-proven equivalent gates should be merged together
  
  Problems and Solutions
  -----------------

  1. Strash
      - Processing the inverted information of inputs needs to use additional bool variable.
        In order to save memory usage, I cast the variable of input gate into size_t.
  
  2. Simulate  
     - Use "~" to inverse bit patterns can cause ambiguities. Therefore, I use bitset to perform inverse.  
     - Too many times of simulations will test unnecessary gates and encumber efficiency. But too few times of simulations will cause the following functions inefficient. So I set a stopping criteria according to overall circuit size to control when to stop simulation. Record the number of continuous times of simulations that gates do not split new groups of FEC. If the number equals to the stopping criteria, then stop the simulation right away.

