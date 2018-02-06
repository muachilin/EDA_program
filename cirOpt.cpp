/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed

bool sortfunc(CirGate* g1,
              CirGate* g2)
{
    return (g1->getId() < g2->getId());
}
void
CirMgr::examine()
{
    for(int i=0;i<gate_list.size();++i)
    {
        if(gate_list[i] -> getGateType() == AIG_GATE)
        {
            int input1 = gate_list[i]->getInput1Id();
            int input2 = gate_list[i]->getInput2Id();
            if(input_id.count(input1)==true){
                gate_list[input_id[input1]]->setOutput( i ,gate_list[i]->getInput1Invered());
                gate_list[i]->setInput1Pos(input_id[input1]);
            }
            else if(gate_id_map.count(input1)==true)
            {
                gate_list[gate_id_map[input1]]->setOutput( i ,gate_list[i]->getInput1Invered());
                gate_list[i]->setInput1Pos(gate_id_map[input1]);
            }
            
            if(input_id.count(input2)==true){
                gate_list[input_id[input2]]->setOutput( i ,gate_list[i]->getInput2Invered());
                gate_list[i]->setInput2Pos(input_id[input2]);
            }
            else if(gate_id_map.count(input2)==true)
            {
                gate_list[gate_id_map[input2]]->setOutput( i ,gate_list[i]->getInput2Invered());
                gate_list[i]->setInput2Pos(gate_id_map[input2]);
            }
            
        }
        else if(gate_list[i] -> getGateType() == PO_GATE)
        {
            int input1 = gate_list[i]->getInput1Id();
            if(input_id.count(input1)==true){
                gate_list[input_id[input1]]->setOutput( i ,gate_list[i]->getInput1Invered());
                gate_list[i]->setInput1Pos(input_id[input1]);
            }
            else if(gate_id_map.count(input1)==true)
            {
                gate_list[gate_id_map[input1]]->setOutput( i ,gate_list[i]->getInput1Invered());
                gate_list[i]->setInput1Pos(gate_id_map[input1]);
            }
            
        }
        
    }
}
void
CirMgr::sweep()
{
    GateList haha;
    map<int,int>net_map , remain_map;
    gate_id_map.clear();
    for(int i=0;i<net_list.size();++i)
    {
        net_map[net_list[i]]=1;
    }
    int index1=0 , index2=0;
    while(index1<gate_list.size())
    {
        if(net_map.count(index1)==0 && gate_list[index1]->getGateType()!=PI_GATE
           && gate_list[index1]->getGateType()!=PO_GATE
           && gate_list[index1]->getGateType()!=CONST_GATE)
        {
            haha.push_back( gate_list[index1] );
            gate_list.erase(gate_list.begin()+index1);
        }
        else{
            gate_list[index1]->clearOutput();
            remain_map[index2] = index1;
            gate_id_map[gate_list[index1]->getId()] = index1;
            index1++;
            
        }
        index2++;
    }
    sort(haha.begin(), haha.end(),sortfunc);
    for(int i=0;i<haha.size();++i)
    {
        
        if(haha[i] -> getGateType() == UNDEF_GATE)
        {
            cout<<"Sweeping: UNDEF("<<haha[i] -> getId()<<") removed..."<<endl;
        }
        else if(haha[i] -> getGateType() == AIG_GATE){
            gate_number--;
            cout<<"Sweeping: AIG("<<haha[i] -> getId()<<") removed..."<<endl;
        }

    }
    for(int i=0;i<net_list.size();++i)
    {
        if(remain_map.count(net_list[i])==1){net_list[i] = remain_map[net_list[i]];}
    }
    
    examine();
  
    
}
// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
   
    trace.clear();
    for(int i=0;i<gate_list.size();++i)
    {
        if(gate_list[i]->getGateType() == CONST_GATE)
        {
            const_gate_index =gate_list[i]->getId() ;break;
        }
    }
    for(int i=0;i<net_list.size();++i)
    {
        if(gate_list[net_list[i]]->getGateType() == PO_GATE)
        {
            recursive_opt(net_list[i]);
        }
    }
   
  
    int const_gate_ii = gate_id_map[const_gate_index];
    int index1=0 , index2=0;
    gate_id_map.clear();
    while(index1<gate_list.size())
    {
        if(gate_list[index1]->getGateType()==TMP_GATE || (index2!=const_gate_ii && gate_list[index1]->getGateType()==CONST_GATE ))
        {
            gate_list.erase(gate_list.begin()+index1);
            gate_number--;
        }
        else{
            gate_list[index1]->clearOutput();
            gate_id_map[gate_list[index1]->getId()] = index1;
            index1++;
            
        }
        index2++;
    }
    
    net_list.clear();
    trace.clear();
    examine();
    
    for(int i=0;i<gate_list.size();++i)
    {
        if(gate_list[i]->getGateType() == PO_GATE)
        {
            
            cir_dfs(i);
        }
    }
    

    
}

void
CirMgr::recursive_opt(int index)
{
    if(trace.count(index)==0 ){
        trace[index]=0;
        
        if(gate_list[index] -> getGateType() == PO_GATE)
        {
            recursive_opt(gate_list[index]->getInput1Pos());
        }
        else if(gate_list[index] -> getGateType() == AIG_GATE){
    
            if(gate_list[index]->getInput1Pos()>=0) {recursive_opt(gate_list[index]->getInput1Pos());}
            if(gate_list[index]->getInput2Pos()>=0) {recursive_opt(gate_list[index]->getInput2Pos());}
            
            vector<pair<unsigned,bool>> out_vec = gate_list[index]->getOutput();
            
            if(gate_list[ gate_list[index]->getInput1Pos() ]->getGateType()==CONST_GATE)
            {
                
                    if(gate_list[ gate_list[index]->getInput2Pos() ]->getGateType()!=CONST_GATE)
                    {
                        
                        if(gate_list[index]->getInput1Invered()==true){//1 AIG
                            
                            for(int i=0;i<out_vec.size();++i)
                            {
                                if(gate_list[out_vec[i].first]->getInput1Id() == gate_list[index]->getId())
                                {
                                    gate_list[out_vec[i].first]->setInput1(gate_list[index]->getInput2Id());
                                    gate_list[out_vec[i].first]->setInput1Pos(gate_id_map[gate_list[index]->getInput2Id()]);
                                    gate_list[out_vec[i].first]->setInput1Invered(gate_list[index]->getInput2Invered()
                                                                                  ^gate_list[out_vec[i].first]->getInput1Invered());
                                }
                        
                                else if(gate_list[out_vec[i].first]->getInput2Id() == gate_list[index]->getId())
                                {
                                    gate_list[out_vec[i].first]->setInput2(gate_list[index]->getInput2Id());
                                    gate_list[out_vec[i].first]->setInput2Pos(gate_id_map[gate_list[index]->getInput2Id()]);
                                    gate_list[out_vec[i].first]->setInput2Invered(gate_list[index]->getInput2Invered()
                                                                                   ^gate_list[out_vec[i].first]->getInput2Invered());
                                }
                            }
                            
                            cout<<"Simplifying: "<<gate_list[index]->getInput2Id()<<" merging ";
                            if(gate_list[index]->getInput2Invered()){cout<<"!";}
                            cout<<gate_list[index]->getId()<<"..."<<endl;
                            gate_list[index] -> setGateType(TMP_GATE);
                           
                        }
                        else{
                            //0 AIG
                            for(int i=0;i<out_vec.size();++i)
                            {
                                if(gate_list[out_vec[i].first]->getInput1Id() == gate_list[index]->getId())
                                {
                                   
                                    gate_list[out_vec[i].first]->setInput1(const_gate_index);
                                    gate_list[out_vec[i].first]->setInput1Pos(gate_id_map[const_gate_index]);
                                    gate_list[out_vec[i].first]->setInput1Invered(false^gate_list[out_vec[i].first]->getInput1Invered());
                                }
                                
                                else if(gate_list[out_vec[i].first]->getInput2Id() == gate_list[index]->getId())
                                {
                                    
                                    gate_list[out_vec[i].first]->setInput2(const_gate_index);
                                    gate_list[out_vec[i].first]->setInput2Pos(gate_id_map[const_gate_index]);
                                    gate_list[out_vec[i].first]->setInput2Invered(false ^gate_list[out_vec[i].first]->getInput2Invered());
                                }
                            }
                            cout<<"Simplifying: "<<"0"<<" merging ";
                            cout<<gate_list[index]->getId()<<"..."<<endl;
                            gate_list[index] -> setGateType(CONST_GATE);
                        }
                        
                    }else{
                        //1 1 , 0 1 ,1 0 , 0  0
                            for(int i=0;i<out_vec.size();++i)
                            {
                                if(gate_list[out_vec[i].first]->getInput1Id() == gate_list[index]->getId())
                                {
                                    gate_list[out_vec[i].first]->setInput1(const_gate_index);
                                    gate_list[out_vec[i].first]->setInput1Pos(gate_id_map[const_gate_index]);
                                    
                                    if(gate_list[index]->getInput1Invered() ==false ||   gate_list[index]->getInput2Invered()==false)
                                    {
                                        
                                        gate_list[out_vec[i].first]->setInput1Invered(false^gate_list[out_vec[i].first]->getInput1Invered());
                                    }
                                    else{
                                        gate_list[out_vec[i].first]->setInput1Invered(true^gate_list[out_vec[i].first]->getInput1Invered());
                                        
                                    }
                                    
                                }
                                
                                else if(gate_list[out_vec[i].first]->getInput2Id() == gate_list[index]->getId())
                                {
                                    gate_list[out_vec[i].first]->setInput2(const_gate_index);
                                    gate_list[out_vec[i].first]->setInput2Pos(gate_id_map[const_gate_index]);
                                   
                                    if(gate_list[index]->getInput1Invered()==false || gate_list[index]->getInput2Invered()==false)
                                    {
                                        gate_list[out_vec[i].first]->setInput2Invered(false^gate_list[out_vec[i].first]->getInput2Invered());
                                        
                                    }
                                    else{
                                        
                                          gate_list[out_vec[i].first]->setInput2Invered(true^gate_list[out_vec[i].first]->getInput2Invered());
                                    }
                                }
                            }
                        cout<<"Simplifying: "<<"0"<<" merging ";
                        if(gate_list[index]->getInput1Invered() ==true &&   gate_list[index]->getInput2Invered()==true)
                        {
                            cout<<"!";
                        }
                        cout<<gate_list[index]->getId()<<"..."<<endl;
                        gate_list[index] -> setGateType(CONST_GATE);
                    }
            }
            
            else{
                //// t t ->f
                //// f f ->f
                //// t f ->t
                //// f t ->t
                if(gate_list[ gate_list[index]->getInput2Pos() ]->getGateType()==CONST_GATE)
                {
                    ////AIG 1
                    if(gate_list[index]->getInput2Invered()==true){
                        
                        for(int i=0;i<out_vec.size();++i)
                        {
                            if(gate_list[out_vec[i].first]->getInput1Id() == gate_list[index]->getId())
                            {
                                gate_list[out_vec[i].first]->setInput1(gate_list[index]->getInput1Id());
                                gate_list[out_vec[i].first]->setInput1Pos(gate_id_map[gate_list[index]->getInput1Id()]);
                                gate_list[out_vec[i].first]->setInput1Invered(gate_list[index]->getInput1Invered()
                                                                              ^gate_list[out_vec[i].first]->getInput1Invered());
                            }
                            
                            else if(gate_list[out_vec[i].first]->getInput2Id() == gate_list[index]->getId())
                            {
                                gate_list[out_vec[i].first]->setInput2(gate_list[index]->getInput1Id());
                                gate_list[out_vec[i].first]->setInput2Pos(gate_id_map[gate_list[index]->getInput1Id()]);
                                gate_list[out_vec[i].first]->setInput2Invered(gate_list[index]->getInput1Invered()
                                                                               ^gate_list[out_vec[i].first]->getInput2Invered());
                            }
                        }
                        
                        cout<<"Simplifying: "<<gate_list[index]->getInput1Id()<<" merging ";
                        if(gate_list[index]->getInput1Invered()){cout<<"!";}
                        cout<<gate_list[index]->getId()<<"..."<<endl;
                        gate_list[index] -> setGateType(TMP_GATE);
                    }
                    else{
                        ////AIG 0
                        for(int i=0;i<out_vec.size();++i)
                        {
                            if(gate_list[out_vec[i].first]->getInput1Id() == gate_list[index]->getId())
                            {
                                
                                gate_list[out_vec[i].first]->setInput1(const_gate_index);
                                gate_list[out_vec[i].first]->setInput1Pos(gate_id_map[const_gate_index]);
                                gate_list[out_vec[i].first]->setInput1Invered(false^gate_list[out_vec[i].first]->getInput1Invered());
                            }
                            
                            else if(gate_list[out_vec[i].first]->getInput2Id() == gate_list[index]->getId())
                            {
                               
                                gate_list[out_vec[i].first]->setInput2(const_gate_index);
                                gate_list[out_vec[i].first]->setInput2Pos(gate_id_map[const_gate_index]);
                                gate_list[out_vec[i].first]->setInput2Invered(false ^gate_list[out_vec[i].first]->getInput2Invered());
                            }
                        }
                        cout<<"Simplifying: "<<"0"<<" merging ";
                        cout<<gate_list[index]->getId()<<"..."<<endl;
                        gate_list[index] -> setGateType(CONST_GATE);
                    }
                   
                }
                else{   
                    
                    if(gate_list[index] -> getInput1Id() == gate_list[index] -> getInput2Id())
                    {
                        
                        
                        if(gate_list[index] -> getInput1Invered() != gate_list[index] -> getInput2Invered())
                        {
                            for(int i=0;i<out_vec.size();++i)
                            {
                                if(gate_list[out_vec[i].first]->getInput1Id() == gate_list[index]->getId())
                                {
                                    gate_list[out_vec[i].first]->setInput1(const_gate_index);
                                    gate_list[out_vec[i].first]->setInput1Pos(gate_id_map[const_gate_index]);
                                    gate_list[out_vec[i].first]->setInput1Invered(false ^ gate_list[out_vec[i].first]->getInput1Invered());
                                }
                                
                                else if(gate_list[out_vec[i].first]->getInput2Id() == gate_list[index]->getId())
                                {
                                    gate_list[out_vec[i].first]->setInput2(const_gate_index);
                                    gate_list[out_vec[i].first]->setInput2Pos(gate_id_map[const_gate_index]);
                                    gate_list[out_vec[i].first]->setInput2Invered(false^ gate_list[out_vec[i].first]->getInput2Invered());
                                }
                            }
                            cout<<"Simplifying: "<<"0"<<" merging "<<gate_list[index]->getId()<<"..."<<endl;
                            gate_list[index] -> setGateType(CONST_GATE);
                          
                        }
                        
                        else{
                
                            for(int i=0;i<out_vec.size();++i)
                            {
                                if(gate_list[out_vec[i].first]->getInput1Id() == gate_list[index]->getId())
                                {
                                    gate_list[out_vec[i].first]->setInput1(gate_list[index] -> getInput1Id());
                                    gate_list[out_vec[i].first]->setInput1Pos(gate_id_map[gate_list[index] -> getInput1Id()]);
                                    gate_list[out_vec[i].first]->setInput1Invered(gate_list[index] -> getInput1Invered()
                                                                                   ^gate_list[out_vec[i].first]->getInput1Invered());
                                }
                                
                                else if(gate_list[out_vec[i].first]->getInput2Id() == gate_list[index]->getId())
                                {
                                    gate_list[out_vec[i].first]->setInput2(gate_list[index] -> getInput1Id());
                                    gate_list[out_vec[i].first]->setInput2Pos(gate_id_map[gate_list[index] -> getInput1Id()]);
                                    gate_list[out_vec[i].first]->setInput2Invered(gate_list[index] -> getInput1Invered()
                                                                                   ^gate_list[out_vec[i].first]->getInput2Invered());
                                }
                            }
                            cout<<"Simplifying: "<<gate_list[index]->getInput1Id()<<" merging ";
                            if(gate_list[index]->getInput1Invered()){cout<<"!";}
                            cout<<gate_list[index]->getId()<<"..."<<endl;
                            gate_list[index] -> setGateType(TMP_GATE);
                        }
                        
                        
                        
                    }
                    else{
                       //normal
                        
                        
                    }
                    
                    
                    
                    
                    
                }
                
                
                
                
            }
            
        }
    }
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/






