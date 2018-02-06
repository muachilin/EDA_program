#include <bitset>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <set>
#include <vector>
#include <hash_map>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
using namespace std;
using namespace __gnu_cxx;
// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions


bool sort_fec(const int &a, const int &b)
{
    if(a<0)
    {
        if(b<0)
        {
            return (cirMgr -> gate_list[a+cirMgr ->gate_list.size()]->getId() < cirMgr ->gate_list[b+cirMgr ->gate_list.size()]->getId());
        }
        else{
            return (cirMgr ->gate_list[a+cirMgr ->gate_list.size()]->getId() < cirMgr ->gate_list[b]->getId());
        }
    }
    else{
        if(b<0)
        {
            return (cirMgr ->gate_list[a]->getId() < cirMgr ->gate_list[b+cirMgr ->gate_list.size()]->getId());
        }
        else{
            return (cirMgr ->gate_list[a]->getId() < cirMgr ->gate_list[b]->getId());
            
        }
    }
    
}

size_t invert_value(size_t x)
{
    bitset<64> foo (x);
    foo.flip();
    size_t y = foo.to_ulong() ;
    return y;
}


bool sort_fec2(const vector<int> &a, const vector<int> &b)
{
    if(a[0]<0)
    {
        if(b[0]<0)
        {
            return (cirMgr -> gate_list[a[0]+cirMgr ->gate_list.size()]->getId() < cirMgr ->gate_list[b[0]+cirMgr ->gate_list.size()]->getId());
        }
        else{
            return (cirMgr ->gate_list[a[0]+cirMgr ->gate_list.size()]->getId() < cirMgr ->gate_list[b[0]]->getId());
        }
    }
    else{
        if(b[0]<0)
        {
            return (cirMgr ->gate_list[a[0]]->getId() < cirMgr ->gate_list[b[0]+cirMgr ->gate_list.size()]->getId());
        }
        else{
            return (cirMgr ->gate_list[a[0]]->getId() < cirMgr ->gate_list[b[0]]->getId());
            
        }
    }
    
}

void
CirMgr::randomSim()
{
   
    simlog_vector.clear();
    bool save_sim_log = false;
    bool no_new_grp = false;
    int sim_sum = 0; int about_to_stop=0;
    int stop_range = (log(gate_list.size())/log(10))+1;
    if(stop_range<2)
    {
        stop_range*=10;
    }
    else if(stop_range>4){
        stop_range*=5;
    }
    else{
        stop_range*=7;
    }
    po_sim.clear();
    fecs.clear();
 
    if(_simLog!=0){ save_sim_log = true;}
    
    while(no_new_grp==false||(no_new_grp==true&&about_to_stop<stop_range)){
        
        po_sim.clear();
        
        no_new_grp=true;
        vector<string>sim_file;
        sim_file.clear();
        
        for(int i=0;i<64;++i)
        {
            string tmp;
            for(int j=0;j<input_number;++j)
            {
                size_t a = (rnGen(100000)%2);
                char c = '0'+a;
                tmp.push_back(c);
                
            }
            if(save_sim_log==true){
                simlog_vector.push_back(tmp);
            }
            sim_file.push_back(tmp);
        }
        
        
        
        simnum.clear();
        
        for(int i=0;i<input_number;++i)
        {
            size_t tmp = 0;
            int index = 0;
            if(sim_file.size()<64)
            {
                
                for(auto&j:sim_file)
                {
                    size_t ia = (size_t)(j[i] - '0');
                    tmp ^= ((size_t)(ia << index));
                    index++;
                }
                for(int j=sim_file.size();j<64;++j)
                {
                    tmp ^= ((size_t)(0<<j));
                }
            }
            else{
                
                for(auto&j:sim_file)
                {
                    if(index>=64){break;}
                    size_t ia = (size_t)(j[i] - '0');
                    tmp ^= ((size_t)(ia << index));
                    index++;
                }
            }
            simnum.push_back(tmp);
        }
    
        for(auto&i:net_list)
        {
            
            if(gate_list[i]->getGateType()==CONST_GATE || gate_list[i]->getGateType()==UNDEF_GATE)
            { gate_list[i]->setSimValue(0); }
            
            else{   gate_list[i]->setSimValue(-123); }
            
        }
        
        for(int i=0;i<net_list.size();++i)
        {
            if(gate_list[net_list[i]]->getGateType()==PO_GATE)
            {
                simulation(net_list[i],false);
                if(save_sim_log==true){ po_sim.push_back(gate_list[net_list[i]]->getSimValue());}
            }
        }
        if(fecs.empty()==true){
            hash_map<size_t,vector<int>>sim_result;
            sim_result.clear();
            for(auto&i:net_list)
            {
                if(gate_list[i]->getSimValue()!=-123 && (gate_list[i]->getGateType()==AIG_GATE))
                {
                    if(sim_result.count( (gate_list[i]->getSimValue()))  ==true)
                    {
                        sim_result[gate_list[i]->getSimValue()].push_back( i );
                    }
                    else if(sim_result.count(invert_value( gate_list[i]->getSimValue() ))  ==true)
                    {
                        sim_result[ invert_value( gate_list[i]->getSimValue() ) ].push_back( (  i-gate_list.size()));
                    }
                    else{
                        sim_result[gate_list[i]->getSimValue()].push_back( i );
                    }
                }
            }
            
            for(auto&i:sim_result)
            {
                
                if((i.second).size()>1)
                {
                    
                    fecs.push_back(i.second);
                }
            }
            if(fecs.size()==0){break;}
            no_new_grp=false;about_to_stop=0;
        }
        else{
            no_new_grp=true;
            vector<vector<int>>tmp_list;
            for(auto&i:fecs)
            {
                
                hash_map<size_t,vector<int>>tmp_group;
                tmp_group.clear();
                for(auto&j:i)
                {
                    if( j<0){
                        size_t ss = invert_value(  gate_list[j+gate_list.size()]->getSimValue()  );
                        tmp_group[ss].push_back(j);
                    }
                    else{
                        if(tmp_group.count(gate_list[j]->getSimValue())==false)
                        {
                            tmp_group[gate_list[j]->getSimValue()].push_back(j);
                        }
                        else{
                            tmp_group[gate_list[j]->getSimValue()].push_back(j);
                        }
                    }
                }
                
                if(tmp_group.size()>1)
                {
                    no_new_grp = false;
                    for(auto&j:tmp_group)
                    {
                        if(j.second.size()>1)
                        {
                            vector<int>ss;
                            for(auto&k:j.second)
                            {
                                ss.push_back(k);
                            }
                            tmp_list.push_back(ss);
                        }
                        
                    }
                }
                else{
                    tmp_list.push_back (i);
                }
            }
            
            if(no_new_grp == false){about_to_stop=0;fecs = tmp_list;}
            else{ about_to_stop ++;}
        }
        
        if(save_sim_log==true){
            for(int i=0;i<64;++i)
            {
                simlog_vector[sim_sum*64+i].push_back(' ');
            }
        
            for(int j=0;j<output_number;++j)
            {
                for(int i=0;i<64;++i)
                {
                    bitset<64>foo(po_sim[j]);
                    char ffoo = foo[i]+'0';
                    simlog_vector[sim_sum*64+i].push_back(ffoo);
                }
            }
        }
        sim_sum++;
        
        
    }
    
    
    size_t ii=0;
    size_t constindex=0;
    for(auto&i:fecs)
    {
        
        if(i[0]<0){
            if(gate_list[i[0]+gate_list.size()]->getSimValue()==0)
            {
                constindex=ii; i.push_back(gate_id_map[0]);break;
            }
        }
        else {
            if(gate_list[i[0]]->getSimValue()==0)
            {
                constindex=ii;i.push_back(gate_id_map[0]);break;
            }
        }
        ii++;
        
    }
    for(auto&i:fecs)
    {
        sort(i.begin(),i.end(),sort_fec);
    }
    if(constindex!=0)
    {
        
        swap(fecs[0],fecs[constindex]);
        sort(fecs.begin()+1,fecs.end(),sort_fec2);
    }
    else{
        sort(fecs.begin(),fecs.end(),sort_fec2);
    }
    cout<<(sim_sum*64)<<" patterns simulated."<<endl;
    
    
    if(save_sim_log==true){
        
        for(int i=0;i<simlog_vector.size();++i)
        {
            (*_simLog)<<simlog_vector[i]<<endl;
        }
        
    }
    
}


void
CirMgr::fileSim(ifstream& patternFile)
{
    simlog_vector.clear();
    bool save_sim_log = false;
    vector<string>sim_file;
    sim_file.clear();
    string buffer;
    bool no_new_grp = false;
    
    if(_simLog!=0){save_sim_log=true;}
    while(1)
    {
        patternFile>>buffer;
        
        
        if(patternFile.eof()==true){break;}
        if(buffer.size()!=input_number)
        {
            cerr<<endl;
            cerr<<"Error: Pattern("<<buffer<<") length("<<buffer.size()<<") does not match the number of inputs("<<input_number<<") in a circuit!!"<<endl;
            cerr<<"0 patterns simulated."<<endl;
            return;
        }
        if(save_sim_log==true){ simlog_vector.push_back(buffer);}
        sim_file.push_back(buffer);
    }
    
   
    int sim_file_size = sim_file.size();
    fecs.clear();
    int sim_sum=0;
    while(sim_file.empty()==false){
        po_sim.clear();
        no_new_grp=true;
        simnum.clear();
        
    for(int i=0;i<input_number;++i)
    {
        size_t tmp = 0;
        int index = 0;
        if(sim_file.size()<64)
        {
            
            for(auto&j:sim_file)
            {
                if(j[i]!='0'&&j[i]!='1')
                {
                    cerr<<endl;
                    cerr<<"Error: Pattern("<<j<<") contains a non-0/1 character('"<<j[i]<<"')."<<endl;
                    cerr<<"0 patterns simulated."<<endl;
                    return;
                }
                size_t ia = (size_t)(j[i] - '0');
                tmp ^= ((size_t)(ia << index));
                index++;
            }
            
            for(int j=sim_file.size();j<64;++j)
            {
                tmp ^= ((size_t)(0<<j));
            }
        }
        else{
           
            for(auto&j:sim_file)
            {
                
                if(index>=64){break;}
                
                if(j[i]!='0'&&j[i]!='1')
                {
                    cerr<<endl;
                    cerr<<"Error: Pattern("<<j<<") contains a non-0/1 character('"<<j[i]<<"')."<<endl;
                    cerr<<"0 patterns simulated."<<endl;
                    return;
                }
                
               
                size_t ia = (size_t)(j[i] - '0');
                tmp ^= ((size_t)(ia << index));
                index++;
            }
        }
        simnum.push_back(tmp);
      }
       
        if(sim_file.size()<64)
        {
            sim_file.clear();
            
        }
        else if(sim_file.size()==64)
        {
           sim_file.clear();
            
        }
        else{
            sim_file.erase(sim_file.begin(),sim_file.begin()+64);
        }
    
    for(auto&i:net_list)
    {
        
        if(gate_list[i]->getGateType()==CONST_GATE || gate_list[i]->getGateType()==UNDEF_GATE)
        { gate_list[i]->setSimValue(0); }
        
        else{   gate_list[i]->setSimValue(-123);}
    
        
    }
        
   
            for(int i=0;i<net_list.size();++i)
            {
                if(gate_list[net_list[i]]->getGateType()==PO_GATE)
                {
                    simulation(net_list[i],false);
                    if(save_sim_log==true){po_sim.push_back(gate_list[net_list[i]]->getSimValue());}
                }
            }
       
        if(fecs.empty()==true){
            hash_map<size_t,vector<int>>sim_result;
            sim_result.clear();
            for(auto&i:net_list)
            {
                if(gate_list[i]->getSimValue()!=-123 && (gate_list[i]->getGateType()==AIG_GATE))
                {
                    if(sim_result.count( (gate_list[i]->getSimValue()))  ==true)
                    {
                        sim_result[gate_list[i]->getSimValue()].push_back( i );
                    }
                    else if(sim_result.count(invert_value( gate_list[i]->getSimValue() ))  ==true)
                    {
                        sim_result[ invert_value( gate_list[i]->getSimValue() ) ].push_back( (  i-gate_list.size()));
                    }
                    else{
                        sim_result[gate_list[i]->getSimValue()].push_back( i );
                    }
                }
            }
            for(auto&i:sim_result)
            {
            
                if((i.second).size()>1)
                {
                    fecs.push_back(i.second);
                }
            }
            if(fecs.empty()==true){break;}
        }
        else{
            no_new_grp=true;
            vector<vector<int>>tmp_list;
            for(auto&i:fecs)
            {
                hash_map<size_t,vector<int>>tmp_group;
                tmp_group.clear();
                for(auto&j:i)
                {
                    if( j<0){
                        size_t ss = invert_value(  gate_list[j+gate_list.size()]->getSimValue()  );
                        tmp_group[ss].push_back(j);
                    }
                   else{
                        tmp_group[gate_list[j]->getSimValue()].push_back(j);
                   }
                }
                if(tmp_group.size()>1)
                {
                    no_new_grp = false;
                    for(auto&j:tmp_group)
                    {
                        if(j.second.size()>1)
                        {
                            tmp_list.push_back(j.second);
                        }
                    }
                }
                else{
                    tmp_list.push_back (i);
                    
                }
            }
            if(no_new_grp == false){fecs = tmp_list;}
        }
        
        if(save_sim_log==true){
        
            for(int i=0;i<64;++i)
            {
                simlog_vector[sim_sum*64+i].push_back(' ');
            }
            
            for(int j=0;j<output_number;++j)
            {
                for(int i=0;i<64;++i)
                {
                    bitset<64>foo(po_sim[j]);
                    char ffoo = foo[i]+'0';
                    simlog_vector[sim_sum*64+i].push_back(ffoo);
                }
            }
        }
        
        sim_sum++;
        
    }
    size_t ii=0;
    size_t constindex=0;
   for(auto&i:fecs)
    {
        
        if(i[0]<0){
            if(gate_list[i[0]+gate_list.size()]->getSimValue()==0)
            {
                constindex=ii; i.push_back(gate_id_map[0]);break;
            }
        }
        else {
            if(gate_list[i[0]]->getSimValue()==0)
            {
                constindex=ii;i.push_back(gate_id_map[0]);break;
            }
        }
        ii++;
        
    }
    for(auto&i:fecs)
    {
        sort(i.begin(),i.end(),sort_fec);
    }
    if(constindex!=0)
    {
        
        swap(fecs[0],fecs[constindex]);
        sort(fecs.begin()+1,fecs.end(),sort_fec2);
    }
    else{
        sort(fecs.begin(),fecs.end(),sort_fec2);
    }
    gate_list[gate_id_map[0]]->setSimValue(0);
    cout<<sim_file_size<<" patterns simulated."<<endl;
    
    
    
    if(save_sim_log==true){
        
        for(int i=0;i<simlog_vector.size();++i)
        {
            (*_simLog)<<simlog_vector[i]<<endl;
        }
        
    }
 
}


size_t
CirMgr::simulation(size_t index,bool inv)
{
    
    size_t tmp_value=0;
        if(cirMgr->gate_list[index]->getGateType() == PI_GATE)
        {
            if(cirMgr->gate_list[index]->getSimValue()!=((size_t)(-123))){
                
                if(inv==true){
                    return (size_t)(invert_value( cirMgr->gate_list[index]->getSimValue() ));
                }
                else{
                    return  (size_t)(cirMgr->gate_list[index]->getSimValue());
                }
            }
            else if(inv==true){
                size_t aa=  ((simnum[ input_id[   cirMgr->gate_list[index]->getId()  ] ]));
                
                cirMgr->gate_list[index]->setSimValue( aa  );
                return  (size_t)(invert_value(   simnum[ input_id[   cirMgr->gate_list[index]->getId()  ] ]  ));
            }
            else {
                size_t aa=  ((simnum[ input_id[   cirMgr->gate_list[index]->getId()  ] ]));
                
                cirMgr->gate_list[index]->setSimValue( aa );
                return  (size_t) ( (simnum[ input_id[   cirMgr->gate_list[index]->getId()  ] ]) );
            }
        }
        else if(cirMgr->gate_list[index]->getGateType() == UNDEF_GATE)
        {
            size_t ze =0;
            cirMgr->gate_list[index]->setSimValue( 0  );
            if(inv==true){ return (size_t) (invert_value(0));}
            else{return  (size_t)((ze));}
        }
        else if(cirMgr->gate_list[index]->getGateType() == CONST_GATE)
        {
           size_t ze =0;
           
            cirMgr->gate_list[index]->setSimValue( 0   );
             if(inv==true){ return (size_t) (invert_value(0));}
            else{return (size_t)( (ze));}
        }

        else if(cirMgr->gate_list[index]->getGateType() == PO_GATE)
        {
           
            
            if(cirMgr->gate_list[index]->getSimValue()!=((size_t)(-123))){
                
                if(inv==true){
                    return (size_t)(invert_value( cirMgr->gate_list[index]->getSimValue() ));
                }
                else{
                    
                    return (size_t)((cirMgr->gate_list[index]->getSimValue()));
                }
            
            }
            else if(inv==true){
                tmp_value = (size_t) (   (simulation( cirMgr->gate_list[index]->getInput1Pos(),cirMgr->gate_list[index]->getInput1Invered())  )) ;
                cirMgr->gate_list[index]->setSimValue( invert_value(tmp_value) );
                return  tmp_value;
            }
            else{
                tmp_value = (size_t) (simulation( cirMgr->gate_list[index]->getInput1Pos(),cirMgr->gate_list[index]->getInput1Invered())) ;
                cirMgr->gate_list[index]->setSimValue(  tmp_value );
                return  tmp_value;
            }
        }
        else if(cirMgr->gate_list[index]->getGateType() == AIG_GATE){
            if(cirMgr->gate_list[index]->getSimValue()!= ((size_t)(-123)) ){
                
                if(inv==true){
                    return (size_t)(invert_value( cirMgr->gate_list[index]->getSimValue() ));
                }
                else{
                    return   (size_t)((cirMgr->gate_list[index]->getSimValue()));
                }
            }
            else if(inv==true){
                
                tmp_value = (  (simulation( cirMgr->gate_list[index]->getInput1Pos(),cirMgr->gate_list[index]->getInput1Invered()) & simulation( cirMgr->gate_list[index]->getInput2Pos(),cirMgr->gate_list[index]->getInput2Invered())  ) );
                
                cirMgr->gate_list[index]->setSimValue(  tmp_value  );
                return (size_t)(invert_value(tmp_value));
            
            }
            else{
                
                tmp_value =  (size_t)(simulation( cirMgr->gate_list[index]->getInput1Pos(),cirMgr->gate_list[index]->getInput1Invered()) & simulation( cirMgr->gate_list[index]->getInput2Pos(),cirMgr->gate_list[index]->getInput2Invered()));
                cirMgr->gate_list[index]->setSimValue(tmp_value );
                return  tmp_value;
                
            }
        }
        

}

