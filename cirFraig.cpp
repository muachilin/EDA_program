/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <algorithm>
#include <hash_map>
#include <unordered_map>
#include <cmath>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "util.h"

using namespace std;
using namespace __gnu_cxx;
// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions
#define NEG 0x1
/*******************************/
/*   Global variable and enum  */
/*******************************/
bool myfunc(pair<int,int>const&a,pair<int,int>const&b)
{
    return (cirMgr->gate_list[ a.first]->getId() < cirMgr->gate_list[ b.first]->getId());
}

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed

size_t
CirMgr::hash_to_key(size_t&a,size_t& b)
{
    size_t ans = (((a+b+1)*(a+b))+b)%((size_t)(pow(2,64))-1);
    return ans;
}

void
CirMgr::strash()
{
    
    hash_num = gate_list.size();
    hash_map<int,int>remove_gate;
 
   
    map<pair<size_t,size_t>,int>gate_hash;
    
   
    hash_vec.clear();
 
    bool has_remove=true;
    while(has_remove==true){
       
        has_remove = false;
    for(int i=0;i<net_list.size();++i)
    {
        if(gate_list[net_list[i]]->getGateType()==AIG_GATE){
           
            size_t p1 = 0;
            size_t p2 = 0;
            if(gate_list[net_list[i]]->getInput1Invered()==true){
                 p1 = (size_t)((size_t)(      gate_list[  gate_list[net_list[i]]->getInput1Pos()  ]   ))+1;
            }
            else{
                 p1 = (size_t)((size_t)(      gate_list[  gate_list[net_list[i]]->getInput1Pos()  ]   ));
            }
            if(gate_list[net_list[i]]->getInput2Invered()==true){
                 p2 = (size_t)((size_t)(      gate_list[  gate_list[net_list[i]]->getInput2Pos()  ]   ))+1;
            }
            else{
                p2 = (size_t)((size_t)(      gate_list[  gate_list[net_list[i]]->getInput2Pos()  ]   ));
            }
            if( remove_gate.count( gate_list[net_list[i]]->getInput1Pos() )==true)
            {
              
                if(gate_list[net_list[i]]->getInput1Invered()==true){
                    p1 = (size_t)((size_t)(      gate_list [ remove_gate[gate_list[net_list[i]]->getInput1Pos()]]   ))+1;
                }
                else{
                    p1 = (size_t)((size_t)(     gate_list [ remove_gate[gate_list[net_list[i]]->getInput1Pos()]]     ));
                }
                
            }
            if( remove_gate.count( gate_list[net_list[i]]->getInput2Pos() )==true)
            {
                if(gate_list[net_list[i]]->getInput2Invered()==true){
                    p2 = (size_t)((size_t)(      gate_list [ remove_gate[gate_list[net_list[i]]->getInput2Pos()]]   ))+1;
                }
                else{
                    p2 = (size_t)((size_t)(     gate_list [ remove_gate[gate_list[net_list[i]]->getInput2Pos()]]     ));
                }
                
            }
            if(gate_hash.count(make_pair(p1,p2))==true )
            {
                if(gate_hash[make_pair(p1,p2)]!=net_list[i]){
                    if(remove_gate.count(net_list[i])==false)
                    {
                       
                        remove_gate[ net_list[i] ] = gate_hash[make_pair(p1,p2)];
                        has_remove = true;
                    }
                }
            }
            else if(gate_hash.count(make_pair(p2,p1))==true )
            {
                if(gate_hash[make_pair(p2,p1)]!=net_list[i]){
                    if(remove_gate.count(net_list[i])==false)
                    {
                        
                        remove_gate[ net_list[i] ] = gate_hash[make_pair(p2,p1)];
                        has_remove = true;
                    }
                }
            }
            else{
                gate_hash[make_pair(p1,p2)]=net_list[i];
            }
        }
    }
        
    }
    
    auto cmp = [this](std::size_t const & a, std::size_t const & b)
    {
        return (gate_list[ a]->getId() < gate_list[ b]->getId());
    };
    
    auto cmp1 = [this](std::pair<int,int> const & a, std::pair<int,int> const & b)
    {
        return gate_list[ a.second]->getId() != gate_list[ b.second]->getId() ? (gate_list[ a.second]->getId() < gate_list[ b.second]->getId()) : (gate_list[ a.first]->getId() < gate_list[ b.first]->getId());
    };
    
    
    vector<pair<int,int>>vec;
    for(auto&i:remove_gate)
    {
       
        vec.push_back(make_pair(i.second,i.first));
       
    }
    sort(vec.begin(),vec.end(),cmp1);
   
    for(auto&i:vec)
    {
        cout<<gate_list[ i.first]->getId()<<" : "<<gate_list[ i.second]->getId()<<endl;
    }
    
    
    
    
    int index =0;
    while (index<net_list.size())
    {
        if(remove_gate.count( net_list[index])==true)
        {
            gate_list[net_list[index]]->setGateType(TMP_GATE);
        }
        index++;
    }
    gate_id_map.clear();
    
    for(int i=0;i<net_list.size();++i)
    {
        if(gate_list[net_list[i]]->getGateType()==AIG_GATE){
            if(remove_gate.count( gate_list[net_list[i]]-> getInput1Pos())==true)
            {
                int ss  = remove_gate[gate_list[net_list[i]]-> getInput1Pos()];
                gate_list[net_list[i]]-> setInput1Pos( ss  );
                gate_list[net_list[i]]-> setInput1(gate_list[  ss ]->getId());
               
            }
            if(remove_gate.count( gate_list[net_list[i]]-> getInput2Pos())==true)
            {
                int ss = remove_gate[gate_list[net_list[i]]-> getInput2Pos()] ;
                gate_list[net_list[i]]-> setInput2Pos( ss);
                gate_list[net_list[i]]-> setInput2(gate_list[  ss  ]->getId());
            }
        }
        else if (gate_list[net_list[i]]->getGateType()==PO_GATE){
            if(remove_gate.count( gate_list[net_list[i]]-> getInput1Pos())==true)
            {
                int ss  = remove_gate[gate_list[net_list[i]]-> getInput1Pos()];
                gate_list[net_list[i]]-> setInput1Pos( ss );
                gate_list[net_list[i]]-> setInput1(gate_list[ ss ]->getId());
            }

        }
        
    }
    index=0 ;
    while(index<gate_list.size())
    {
        if(gate_list[index]->getGateType()==TMP_GATE)
        {
            gate_list.erase(gate_list.begin()+index);
            gate_number--;
        }
        else{
            gate_list[index]->clearOutput();
            gate_id_map[gate_list[index]->getId()] = index;
            index++;
        }
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
CirMgr::createSatSolver(SatSolver& s,Var czero)
{
    
    for(int i=0;i<gate_list.size();++i)
    {
        if(gate_list[i]->getGateType()!=PO_GATE)
        {
            Var v = s.newVar();
            gate_list[i]->setVar(v);
        }
        
    }
    
    for(int i=0;i<gate_list.size();++i)
    {
        if(gate_list[i]->getGateType()==AIG_GATE)
        {
            Var x , y;
            if(gate_list[i]->getInput1Pos()==gate_id_map[0])
            {
                x = czero;
            }
            else{
                x = gate_list[gate_list[i]->getInput1Pos()]->getVar();
            }
            if(gate_list[i]->getInput2Pos()==gate_id_map[0])
            {
                y = czero;
            }
            else{
                y = gate_list[gate_list[i]->getInput2Pos()]->getVar();
            }
            s.addAigCNF(gate_list[i]->getVar(), x,
                        gate_list[i]->getInput1Invered(), y, gate_list[i]->getInput2Invered());
        }
        
    }
    

}
void
CirMgr::reportResult(const SatSolver& solver, bool result)
{
    solver.printStats();
    cout << (result? "SAT" : "UNSAT") << endl;
    if (result) {
        for (size_t i = 0, n = gate_list.size(); i < n; ++i){
            if(gate_list[i]->getGateType()!=PO_GATE)
            {
                cout << solver.getValue(gate_list[i]->getVar()) << endl;
            }
        }
    }
}


void
CirMgr::fraig()
{
    SatSolver solver;
    solver.initialize();
    Var czero = solver.newVar();
    createSatSolver(solver,czero);
    hash_map<int,int>remove_gate;
    remove_gate.clear();
    
    for(int i=0;i<fecs.size();++i)
    {
        for(int j=1;j<(fecs[i]).size();++j)
        {
            bool result;
            int x,y;
            
            if(fecs[i][0]<0){ x = fecs[i][0]+gate_list.size();}
            else{x = fecs[i][0] ;  }
            
            if(fecs[i][j]<0){ y = fecs[i][j]+gate_list.size();}
            else{y = fecs[i][j] ;  }
            
            Var newV = solver.newVar();
            
            if(x==gate_id_map[0])
            {
                solver.addXorCNF(newV,czero,false,gate_list[y]->getVar(), (fecs[i][j]<0) );
            }
            else{
                solver.addXorCNF(newV, gate_list[x]->getVar(),  (fecs[i][0]<0),
                                 gate_list[y]->getVar(), (fecs[i][j]<0) );
            }
            
            solver.assumeRelease();
            solver.assumeProperty(newV, true);
            solver.assumeProperty(czero, false);
            result = solver.assumpSolve();

            if(result==0){
                
                remove_gate[fecs[i][j]] = fecs[i][0];
            
            }
            
          if(result == 0){
            cout<<i<<" : "<<(fecs[i][0]<0)<<" "<<gate_list[x]->getId()<<" , "<<(fecs[i][j]<0)<<" "<<gate_list[y]->getId();
            cout<<" , result : "<<result<<endl;
        }
           
       }
        
    }
    
    
    fraig_remove_gate.clear();
    for(auto&i:remove_gate)
    {
        if(i.first<0)
        {
            if(i.second<0)
            {
               
                pair<int,bool> p2 =make_pair(gate_list[gate_list.size()+i.second]->getId(),false);
                fraig_remove_gate[gate_list[gate_list.size()+i.first]->getId() ]= p2;
            }
            else
            {
                 pair<int,bool> p2 =make_pair(gate_list[i.second]->getId(),true);
                fraig_remove_gate[gate_list[gate_list.size()+i.first]->getId()] = p2;
            }
            
        }
        else{
            if(i.second<0)
            {
               
                pair<int,bool> p2 =make_pair(gate_list[gate_list.size()+i.second]->getId(),true);
               
                fraig_remove_gate[gate_list[i.first]->getId()] = p2;
               
            }
            else
            {
                 pair<int,bool> p2 =make_pair(gate_list[i.second]->getId(),false);
                fraig_remove_gate[gate_list[i.first]->getId()] = p2;
        
            }
            
            
        }
        
        
    }
    

    
    int index = 0;
    while (index<net_list.size())
    {
        if(remove_gate.count( net_list[index])==true)
        {
            gate_list[net_list[index]]->setGateType(TMP_GATE);
            
            cout<<"remove... "<<gate_list[net_list[index]]->getId()<<endl;
        }
        else if(remove_gate.count( net_list[index]-gate_list.size())==true)
        {
            gate_list[net_list[index]]->setGateType(TMP_GATE);
            
            cout<<"remove... "<<gate_list[net_list[index]]->getId()<<endl;
        }
        index++;
    }
    
    gate_id_map.clear();
    for(int i=0;i<net_list.size();++i)
    {
        if(gate_list[net_list[i]]->getGateType()==AIG_GATE){
            if(remove_gate.count( gate_list[net_list[i]]-> getInput1Pos())==true)
            {
                int ss  = remove_gate[gate_list[net_list[i]]-> getInput1Pos()];
                if(ss<0)
                {
                    gate_list[net_list[i]]-> setInput1Pos( ss+gate_list.size()  );
                    gate_list[net_list[i]]-> setInput1(gate_list[  ss+gate_list.size()  ]->getId());
                    gate_list[net_list[i]]-> setInput1Invered( (gate_list[net_list[i]]-> getInput1Invered())^true    );
                }
                else{
                    gate_list[net_list[i]]-> setInput1Pos( ss);
                    gate_list[net_list[i]]-> setInput1(gate_list[  ss  ]->getId());
                }
                
            }
            else if(remove_gate.count( (gate_list[net_list[i]]-> getInput1Pos())-gate_list.size())==true)
            {
                int ss  = remove_gate[gate_list[net_list[i]]-> getInput1Pos()-gate_list.size()];
               
                if(ss<0)
                {
                    gate_list[net_list[i]]-> setInput1Pos( ss+gate_list.size()  );
                    gate_list[net_list[i]]-> setInput1(gate_list[  ss+gate_list.size()  ]->getId());
                    
                }
                else{
                    gate_list[net_list[i]]-> setInput1Pos( ss);
                    gate_list[net_list[i]]-> setInput1(gate_list[  ss  ]->getId());
                    gate_list[net_list[i]]-> setInput1Invered( (gate_list[net_list[i]]-> getInput1Invered())^true    );
                }
                
            }
            if(remove_gate.count( gate_list[net_list[i]]-> getInput2Pos())==true)
            {
                int ss = remove_gate[gate_list[net_list[i]]-> getInput2Pos()] ;
                
                if(ss<0)
                {
                    gate_list[net_list[i]]-> setInput2Pos( ss+gate_list.size()  );
                    gate_list[net_list[i]]-> setInput2(gate_list[  ss+gate_list.size()  ]->getId());
                    gate_list[net_list[i]]-> setInput2Invered( (gate_list[net_list[i]]-> getInput2Invered())^true    );
                }
                else{
                    gate_list[net_list[i]]-> setInput2Pos( ss);
                    gate_list[net_list[i]]-> setInput2(gate_list[  ss  ]->getId());
                }
            }
            else if(remove_gate.count( (gate_list[net_list[i]]-> getInput2Pos())-gate_list.size())==true)
            {
                int ss  = remove_gate[gate_list[net_list[i]]-> getInput2Pos()-gate_list.size()];
                
                if(ss<0)
                {
                    gate_list[net_list[i]]-> setInput2Pos( ss+gate_list.size()  );
                    gate_list[net_list[i]]-> setInput2(gate_list[  ss+gate_list.size()  ]->getId());
                }
                else{
                    gate_list[net_list[i]]-> setInput2Pos( ss);
                    gate_list[net_list[i]]-> setInput2(gate_list[  ss  ]->getId());
                    gate_list[net_list[i]]-> setInput2Invered( (gate_list[net_list[i]]-> getInput2Invered())^true    );
                }
                
            }
        }
        else if (gate_list[net_list[i]]->getGateType()==PO_GATE){
            if(remove_gate.count( gate_list[net_list[i]]-> getInput1Pos())==true)
            {
                int ss  = remove_gate[gate_list[net_list[i]]-> getInput1Pos()];
                
                if(ss<0)
                {
                    gate_list[net_list[i]]-> setInput1Pos( ss+gate_list.size()  );
                    gate_list[net_list[i]]-> setInput1(gate_list[  ss+gate_list.size()  ]->getId());
                    gate_list[net_list[i]]-> setInput1Invered( (gate_list[net_list[i]]-> getInput1Invered())^true    );
                }
                else{
                    gate_list[net_list[i]]-> setInput1Pos( ss);
                    gate_list[net_list[i]]-> setInput1(gate_list[  ss  ]->getId());
                }
            }
            else if(remove_gate.count( (gate_list[net_list[i]]-> getInput1Pos())-gate_list.size())==true)
            {
                int ss  = remove_gate[gate_list[net_list[i]]-> getInput1Pos()-gate_list.size()];
               
                if(ss<0)
                {
                    gate_list[net_list[i]]-> setInput1Pos( ss+gate_list.size()  );
                    gate_list[net_list[i]]-> setInput1(gate_list[  ss+gate_list.size()  ]->getId());
                    
                }
                else{
                    gate_list[net_list[i]]-> setInput1Pos( ss);
                    gate_list[net_list[i]]-> setInput1(gate_list[  ss  ]->getId());
                    gate_list[net_list[i]]-> setInput1Invered( (gate_list[net_list[i]]-> getInput1Invered())^true    );
                }
                
            }
            
        }
        
    }
    
    index=0 ;
    while(index<gate_list.size())
    {
        if(gate_list[index]->getGateType()==TMP_GATE)
        {
            gate_list.erase(gate_list.begin()+index);
            gate_number--;
        }
        else{
            gate_list[index]->clearOutput();
            gate_id_map[gate_list[index]->getId()] = index;
            index++;
        }
    }
    net_list.clear();
    fecs.clear();
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

/********************************************/
/*   Private member functions about fraig   */
/********************************************/












