# Functionally Reduced AND-Inverter Graph
Main Functions
  1. Sweep
    (1) Use net_map to record the gates which saved in net_list. 
    (2) Examine entire gate_list. 
        If some gates are not in net_map, and the gates are not input/output or constant gates, remove those gates.
    (3) If the gates in gate_list are in net_map, clear the output of those gates. Save the gates in remain_gate.
        Update gate_id_map to new index since the information in gate_list has changed.
        Use index1 and index2 to save the number of remain_gate and updated number of gates.


project copyright:

  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]


