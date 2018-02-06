/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include <bitset>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirGate::reportGate()", "CirGate::reportFanin()" and
//       "CirGate::reportFanout()" for cir cmds. Feel free to define
//       your own variables and functions.

extern CirMgr *cirMgr;

/**************************************/
/*   class CirGate member functions   */
/**************************************/
size_t
CirGate::invert_value_gate(size_t x)const
{
    bitset<64> foo (x);
    foo.flip();
    size_t y = foo.to_ulong() ;
    return y;
}

void
CirGate::reportGate() const
{
    cout<<"================================================================================"<<endl;
    
    cout<<"= ";
    if(gate_type==PI_GATE)
    {
        cout<<"PI("<<gate_id<<")";
        if(gate_name!="000"){
            cout<<"\"";
            for(auto&i:gate_name)
            {
                cout<<i;
            }
            cout<<"\"";
        }
        
        cout<<", line "<<line_id<<endl;
        cout<<"= FECs:"<<endl;
        cout<<"= Value: ";
        if(sim_value==-123)
        {
            cout<<"00000000";
            for(int i=0;i<7;++i)
            {
                cout<<"_00000000";
            }
            cout<<endl;
        }
        else{
            bitset<64> foo (sim_value);
            string sim_string = foo.to_string();
            for(int i=0;i<8;++i)
            {
                cout<<sim_string[i];
            }
            
            for(int i=0;i<7;++i)
            {
                cout<<"_";
                for(int j=(8+8*i);j<(16+8*i);++j)
                {
                    cout<<sim_string[j];
                }
            }
            cout<<endl;
            
        }
    
        
    }
    else if(gate_type==PO_GATE)
    {
        cout<<"PO("<<gate_id<<")";
        if(gate_name!="000"){
            cout<<"\"";
            for(auto&i:gate_name)
            {
                cout<<i;
            }
            cout<<"\"";
            
        }
        
        cout<<", line "<<line_id<<endl;
        
        cout<<"= FECs:"<<endl;
        
        cout<<"= Value: ";
        if(sim_value==-123)
        {
            cout<<"00000000";
            for(int i=0;i<7;++i)
            {
                cout<<"_00000000";
            }
            cout<<endl;
        }
        else{
            bitset<64> foo (sim_value);
            string sim_string = foo.to_string();
            for(int i=0;i<8;++i)
            {
                cout<<sim_string[i];
            }
            
            for(int i=0;i<7;++i)
            {
                cout<<"_";
                for(int j=(8+8*i);j<(16+8*i);++j)
                {
                    cout<<sim_string[j];
                }
            }
            cout<<endl;
            
        }
        
        
    }
    else if(gate_type==AIG_GATE)
    {
        cout<<"AIG("<<gate_id<<"), line "<<line_id<<endl;
        cout<<"= FECs:";
        if((cirMgr->getFEC()).empty()==true)
        {
            cout<<endl;
        }
        else{
            vector<int>this_gate_fec;
            bool invv = false;
            bool gate_is_in = false;
            
            vector<vector<int>>tmp_fec = (cirMgr->getFEC());
           
            for(int i=0;i<tmp_fec.size();++i)
            {
                invv = false;
                gate_is_in = false;
                if(tmp_fec[i][0]<0)
                {
                    if(cirMgr->gate_list[tmp_fec[i][0]+cirMgr->gate_list.size()]->getSimValue()==sim_value)
                    {
                        
                        
                        for(int j=0;j<tmp_fec[i].size();++j)
                        {
                            if(tmp_fec[i][j]<0)
                            {
                                if(cirMgr->gate_list[tmp_fec[i][j]+cirMgr->gate_list.size()]->getId()==gate_id)
                                {
                                    gate_is_in = true;break;
                                    
                                }
                            }
                            else{
                                
                                if(cirMgr->gate_list[tmp_fec[i][j]]->getId()==gate_id)
                                {
                                    gate_is_in = true;break;
                                    
                                }
                                
                            }
                        }
                        if(gate_is_in==true){
                            this_gate_fec = tmp_fec[i]; invv=true;
                            break;
                        }
                    }
                    else if(cirMgr->gate_list[tmp_fec[i][0]+cirMgr->gate_list.size()]->getSimValue()==invert_value_gate(sim_value))
                    {
                        
                        
                        for(int j=0;j<tmp_fec[i].size();++j)
                        {
                            if(tmp_fec[i][j]<0)
                            {
                                if(cirMgr->gate_list[tmp_fec[i][j]+cirMgr->gate_list.size()]->getId()==gate_id)
                                {
                                    gate_is_in = true;break;
                                    
                                }
                            }
                            else{
                                
                                if(cirMgr->gate_list[tmp_fec[i][j]]->getId()==gate_id)
                                {
                                    gate_is_in = true;break;
                                    
                                }
                                
                            }
                        }
                        if(gate_is_in==true){
                            this_gate_fec = tmp_fec[i]; invv=false;
                            break;
                        }
                    }
                    
                    
                }
                
                else{
                    if(cirMgr->gate_list[tmp_fec[i][0]]->getSimValue()==sim_value)
                    {
                        
                        for(int j=0;j<tmp_fec[i].size();++j)
                        {
                            if(tmp_fec[i][j]<0)
                            {
                                if(cirMgr->gate_list[tmp_fec[i][j]+cirMgr->gate_list.size()]->getId()==gate_id)
                                {
                                    gate_is_in = true;break;
                                    
                                }
                            }
                            else{
                                
                                if(cirMgr->gate_list[tmp_fec[i][j]]->getId()==gate_id)
                                {
                                    gate_is_in = true;break;
                                    
                                }
                                
                            }
                        }
                        
                        if(gate_is_in==true){
                            this_gate_fec = tmp_fec[i];invv=false;
                            break;
                        }
                    }
                    else if(cirMgr->gate_list[tmp_fec[i][0]]->getSimValue()==invert_value_gate( sim_value))
                    {
                        
                        for(int j=0;j<tmp_fec[i].size();++j)
                        {
                            if(tmp_fec[i][j]<0)
                            {
                                if(cirMgr->gate_list[tmp_fec[i][j]+cirMgr->gate_list.size()]->getId()==gate_id)
                                {
                                    gate_is_in = true;break;
                                    
                                }
                            }
                            else{
                                
                                if(cirMgr->gate_list[tmp_fec[i][j]]->getId()==gate_id)
                                {
                                    gate_is_in = true;break;
                                    
                                }
                                
                            }
                        }
                        
                        if(gate_is_in==true){
                            this_gate_fec = tmp_fec[i];invv=true;
                            break;
                        }
                    }
                }
                
                
            }
            
            if(gate_is_in==true){
                if(invv==false)
                {
                    for(int i=0;i<this_gate_fec.size();++i)
                    {
                        if(this_gate_fec[i]<0)
                        {
                            if(cirMgr->gate_list[this_gate_fec[i]+cirMgr->gate_list.size()]->getId()!=gate_id)
                            {
                                cout<<" !"<<cirMgr->gate_list[this_gate_fec[i]+cirMgr->gate_list.size()]->getId();
                            
                            }
                        }
                        else{
                        
                            if(cirMgr->gate_list[this_gate_fec[i]]->getId()!=gate_id)
                            {
                                cout<<" "<<cirMgr->gate_list[this_gate_fec[i]]->getId();
                        
                            }
                        
                        }
                    
                    }
                
                }
                else{
                    for(int i=0;i<this_gate_fec.size();++i)
                    {
                        if(this_gate_fec[i]<0)
                        {
                            if(cirMgr->gate_list[this_gate_fec[i]+cirMgr->gate_list.size()]->getId()!=gate_id)
                            {
                                cout<<" "<<cirMgr->gate_list[this_gate_fec[i]+cirMgr->gate_list.size()]->getId();
                            
                            }
                        }
                        else{
                        
                            if(cirMgr->gate_list[this_gate_fec[i]]->getId()!=gate_id)
                            {
                                cout<<" !"<<cirMgr->gate_list[this_gate_fec[i]]->getId();
                            
                            }
                        
                        }
                    
                    }
                
                }
            
            }
            
            
            cout<<endl;
        }
        
        
        
        cout<<"= Value: ";
        if(sim_value==-123)
        {
            cout<<"00000000";
            for(int i=0;i<7;++i)
            {
                cout<<"_00000000";
            }
            cout<<endl;
        }
        else{
            bitset<64> foo (sim_value);
            string sim_string = foo.to_string();
            for(int i=0;i<8;++i)
            {
                cout<<sim_string[i];
            }
            
            for(int i=0;i<7;++i)
            {
                cout<<"_";
                for(int j=(8+8*i);j<(16+8*i);++j)
                {
                    cout<<sim_string[j];
                }
            }
            cout<<endl;
            
        }
    }
    else if(gate_type==UNDEF_GATE)
    {
        cout<<"UNDEF("<<gate_id<<"), line "<<0<<endl;
        cout<<"= FECs:"<<endl;
        cout<<"= Value: ";
        
            cout<<"00000000";
            for(int i=0;i<7;++i)
            {
                cout<<"_00000000";
            }
            cout<<endl;
    
    }
    else if(gate_type==CONST_GATE)
    {
        cout<<"CONST("<<gate_id<<"), line "<<0<<endl;
        cout<<"= FECs:";
        if((cirMgr->getFEC()).empty()==true)
        {
            cout<<endl;
        }
        else{
            vector<int>this_gate_fec;
            bool invv = false;
            vector<vector<int>>tmp_fec = (cirMgr->getFEC());
            if(tmp_fec[0][0]<0)
            {
                if(cirMgr->gate_list[tmp_fec[0][0]+cirMgr->gate_list.size()]->getId()==0)
                {
                    this_gate_fec = tmp_fec[0];
                    invv =true;
                    
                }
            }
            else{
                if(cirMgr->gate_list[tmp_fec[0][0]]->getId()==0)
                {
                    this_gate_fec = tmp_fec[0];
                    invv =false;
                    
                }
                
                
            }
            
            if(invv==false)
            {
                for(int i=0;i<this_gate_fec.size();++i)
                {
                    if(this_gate_fec[i]<0)
                    {
                        if(cirMgr->gate_list[this_gate_fec[i]+cirMgr->gate_list.size()]->getId()!=gate_id)
                        {
                            cout<<" !"<<cirMgr->gate_list[this_gate_fec[i]+cirMgr->gate_list.size()]->getId();
                            
                        }
                    }
                    else{
                        
                        if(cirMgr->gate_list[this_gate_fec[i]]->getId()!=gate_id)
                        {
                            cout<<" "<<cirMgr->gate_list[this_gate_fec[i]]->getId();
                            
                        }
                        
                    }
                    
                }
                
            }
            else{
                for(int i=0;i<this_gate_fec.size();++i)
                {
                    if(this_gate_fec[i]<0)
                    {
                        if(cirMgr->gate_list[this_gate_fec[i]+cirMgr->gate_list.size()]->getId()!=gate_id)
                        {
                            cout<<" "<<cirMgr->gate_list[this_gate_fec[i]+cirMgr->gate_list.size()]->getId();
                            
                        }
                    }
                    else{
                        
                        if(cirMgr->gate_list[this_gate_fec[i]]->getId()!=gate_id)
                        {
                            cout<<" !"<<cirMgr->gate_list[this_gate_fec[i]]->getId();
                            
                        }
                        
                    }
                    
                }
                
            }
            
            
            cout<<endl;
        }
        cout<<"= Value: ";
        
        cout<<"00000000";
        for(int i=0;i<7;++i)
        {
            cout<<"_00000000";
        }
        cout<<endl;
    }
    cout<<"================================================================================"<<endl;

}

void
CirGate::reportFanin(int level) const
{
   assert (level >= 0);
    cirMgr->trace.clear();
    cirMgr->traverse_num=0;
    for(int i=0;i<cirMgr->gate_list.size();++i)
    {
        if(cirMgr->gate_list[i]->getId()==gate_id)
        {
            gate_in_traverse(i,0,false,level);
            
            break;
        }
    }
}


void
CirGate::gate_in_traverse(int i,int level,bool inv,int levelnum)const
{
    
    if(level<=levelnum){
        if(cirMgr->gate_list[i] -> getGateType() == UNDEF_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cirMgr->gate_list[i]->printGate();
            cout<<endl;
            
        }
        else if(cirMgr->gate_list[i] -> getGateType() == PI_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cirMgr->gate_list[i]->printGate();
            cout<<endl;
        }
        else if(cirMgr->gate_list[i] -> getGateType() == CONST_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cout<<"CONST "<<cirMgr->gate_list[i]->getId()<<endl;
            
        }
        else if(cirMgr->gate_list[i] -> getGateType() == PO_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cirMgr->gate_list[i]->printGate();
            cout<<endl;
            int input1 = cirMgr->gate_list[i] -> getInput1Id();
            bool input1inverted = cirMgr->gate_list[i] -> getInput1Invered();
            int input1id;
            for(int j=0;j<cirMgr->gate_list.size();++j)
            {
                if(cirMgr->gate_list[j]->getId()==input1&&(cirMgr->trace.count(j)==0))
                {
                    input1id = j;
                }
            }
            cirMgr->traverse_num++;
            gate_in_traverse(input1id,level+1,input1inverted,levelnum);
            
            
        }
        else{
            if(cirMgr->trace.count(i)==1&&level+1<=levelnum)
            {
                for(int j=0;j<level;++j)
                {
                    cout<<"  ";
                }
                if(inv==true){cout<<"!";}
                cirMgr->gate_list[i]->printGate();
                cout<<" (*)"<<endl;
            }
            
            else
            {
                for(int j=0;j<level;++j)
                {
                    cout<<"  ";
                }
                if(inv==true){cout<<"!";}
                cirMgr->gate_list[i]->printGate();
                cout<<endl;
                int input1 = cirMgr->gate_list[i] -> getInput1Id();
                int input2 = cirMgr->gate_list[i] -> getInput2Id();
                bool input1inverted = cirMgr->gate_list[i] -> getInput1Invered();
                bool input2inverted = cirMgr->gate_list[i] -> getInput2Invered();
                int input1id=-2;
                int input2id=-2;
                for(int j=0;j<cirMgr->gate_list.size();++j)
                {
                    if(cirMgr->gate_list[j]->getId()==input1&&(cirMgr->trace.count(j)>=0))
                    {
                        input1id = j;
                        
                    }
                    else if(cirMgr->gate_list[j]->getId()==input1&&(cirMgr->trace.count(j)==1))
                    {
                        input1id=-1;
                    }
                    if(cirMgr->gate_list[j]->getId()==input2&&(cirMgr->trace.count(j)>=0))
                    {
                        input2id = j;
                        
                    }
                    else if(cirMgr->gate_list[j]->getId()==input2&&(cirMgr->trace.count(j)==1))
                    {
                        input2id=-1;
                    }
                }
                
                if(input1id>=0) {cirMgr->traverse_num++;gate_in_traverse(input1id,level+1,input1inverted,levelnum);}
                if(input2id>=0) {cirMgr->traverse_num++;gate_in_traverse(input2id,level+1,input2inverted,levelnum);}
                if(level+1<=levelnum){
                    cirMgr->trace[i]=0;
                }
                
            }
        }
        
    }
    
}



void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
    cirMgr->trace.clear();
    cirMgr->traverse_num=0;
    
    for(int i=0;i<cirMgr->gate_list.size();++i)
    {
        if(cirMgr->gate_list[i]->getId()==gate_id)
        {
            gate_out_traverse(i,0,false,level);
            break;
        }
    }
}

bool sortfunc(const pair<int,int> &a,
              const pair<int,int> &b)
{
    return (cirMgr->gate_list[a.first] -> getId() < cirMgr->gate_list[b.first] -> getId());
}


void
CirGate::gate_out_traverse(int i,int level,bool inv,int levelnum)const
{
    if(level<=levelnum){
        if(cirMgr->gate_list[i] -> getGateType() == UNDEF_GATE)
        {
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cirMgr->gate_list[i]->printGate();
            cout<<endl;
            vector<pair<unsigned,bool>> vec=cirMgr -> gate_list[i] ->getOutput();
            sort(vec.begin(),vec.end(),sortfunc);
            
            
            for(auto&j:vec)
            {
                
                gate_out_traverse(j.first,level+1,j.second,levelnum);
            }
            
        }
        else if(cirMgr->gate_list[i] -> getGateType() == PI_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cirMgr->gate_list[i]->printGate();
            cout<<endl;
            vector<pair<unsigned,bool>> vec=cirMgr -> gate_list[i] ->getOutput();
            sort(vec.begin(),vec.end(),sortfunc);
            
            
            for(auto&j:vec)
            {
                
                gate_out_traverse(j.first,level+1,j.second,levelnum);
            }
        }
        else if(cirMgr->gate_list[i] -> getGateType() == CONST_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cout<<"CONST "<<cirMgr->gate_list[i]->getId()<<endl;
            vector<pair<unsigned,bool>> vec=cirMgr -> gate_list[i] ->getOutput();
           sort(vec.begin(),vec.end(),sortfunc);
            
            for(auto&j:vec)
            {
                
                gate_out_traverse(j.first,level+1,j.second,levelnum);
            }
            
        }
        else if(cirMgr->gate_list[i] -> getGateType() == PO_GATE)
        {
            
            for(int j=0;j<level;++j)
            {
                cout<<"  ";
            }
            if(inv==true){cout<<"!";}
            cirMgr->gate_list[i]->printGate();
            cout<<endl;
            
        }
        else if(cirMgr->gate_list[i] -> getGateType() == AIG_GATE){
            if(cirMgr->trace.count(i)==1&&level+1<=levelnum)
            {
                for(int j=0;j<level;++j)
                {
                    cout<<"  ";
                }
                if(inv==true){cout<<"!";}
                cirMgr->gate_list[i]->printGate();
                cout<<" (*)"<<endl;
            }
            
            else
            {
                for(int j=0;j<level;++j)
                {
                    cout<<"  ";
                }
                if(inv==true){cout<<"!";}
                cirMgr->gate_list[i]->printGate();
                cout<<endl;
                bool has_o=false;
                vector<pair<unsigned,bool>> vec=cirMgr -> gate_list[i] ->getOutput();
               sort(vec.begin(),vec.end(),sortfunc);
                
                
                for(auto&j:vec)
                {
                    has_o=true;
                    
                    gate_out_traverse(j.first,level+1,j.second,levelnum);
                }
                if(level+1<=levelnum &&  has_o==true){
                    
                    cirMgr->trace[i]=0;
                }
                
            }
        }
        
    }
    
    
    
}


