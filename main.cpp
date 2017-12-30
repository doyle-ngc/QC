#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <cstdlib>
#include <functional>
#include <queue>
#include <stack>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <tuple>
#include <cmath>
#include "qc/bit.hpp"
#include "qc/gate.hpp"
#include "qc/circuit.hpp"
#include "qc/algorithm.hpp"
#include "qc/io.hpp"

struct Node{
  int id,x,y,k;
};

class state{
  public:
  int w;
  int h;
  int grid[6][6];
  int depth;
  int evaluation;
  bool operator<(const state &s) const {
    return evaluation > s.evaluation;
  }
};

std::vector<Node> node_list;
Node node;
std::vector<std::string> cnf;
std::vector<std::string> cnf_head;

state start;
state goal;

std::priority_queue<state > open;
//std::stack<state> open2;
std::vector<state> closed;
std::vector<state> searced_state;
int count = 0;
int w,h,qubit_no;
//int max = 0;
int cnf_no=0;

int getNodeNo(int x,int y,int k){
  for(auto e:node_list){
    if(e.x==x && e.y==y && e.k==k){
      return e.id;
    }
  }
  return -1;
}

int getXNo(int node_no){
  for(auto e:node_list){
    if(e.id == node_no){
      return e.x;
    }
  }
  return -1;
}

int getYNo(int node_no){
  for(auto e:node_list){
    if(e.id == node_no){
      return e.y;
    }
  }
  return -1;
}

int getKNo(int node_no){
  for(auto e:node_list){
    if(e.id == node_no){
      return e.k;
    }
  }
  return -1;
}

void addCNF0va(){
  std::string str;
  str += ("0");
  cnf.push_back(str);
}

void addCNF1va(int cons){
  std::string str;
  str += std::to_string(cons);
  cnf.push_back(str);
}

void addCNF2va(int cons1,int cons2){
  std::string str;
  str += ("-");
  str += std::to_string(cons1);
  str += (" ");
  str += ("-");
  str += std::to_string(cons2);
  str += (" ");
  str += ("0");
  cnf.push_back(str);
}

void addCNF3va(int cons){
  std::string str;
  str += ("-");
  str += std::to_string(cons);
  cnf.push_back(str);
}

void addCNF4va(int k){
  std::string str;
  str += ("p");
  str += (" ");
  str += ("cnf");
  str += (" ");
  str += std::to_string(k);
  str += (" ");
  str += std::to_string(cnf_no);
  cnf_head.push_back(str);
}


void cnfencoder(int x,int y,int k,int adjacency[200][200]){

  cnf.clear();
  cnf_head.clear();

  //int x,y,k;
  //int adjacency[k][k];
  
  int node_no=1;
  std::fstream outputfile("input.dimacs",std::ios::out);
  if(outputfile.fail()) std::cout<<"file open faild"<<std::endl;
  std::vector<int> cons;
  //adjacency[1][2] = 1;
  //adjacency[2][3] = 1;
  
  
  for(int i=0;i<k;i++){
    for(int j=0;j<y;j++){
      for(int l=0;l<x;l++){ 
        node.id=node_no;
        node.x=l;
        node.y=j;
        node.k=i;
        node_list.push_back(node);
        node_no++;
      }
    }
  }

  for(int i=0;i<k;i++){
    for(int j=0;j<y;j++){
      for(int l=0;l<x;l++){
        addCNF1va(getNodeNo(l,j,i));
      }
    }
    addCNF0va();
    cnf_no++;
  }

  for(int i=0;i<y;i++){
    for(int j=0;j<x;j++){
      for(int l=0;l<k;l++){
        addCNF1va(getNodeNo(j,i,l));
      }
      addCNF0va();
      cnf_no++;
    }
  }

  for(int i=0;i<y;i++){ 
    for(int j=0;j<x;j++){
      for(int l=0;l<k;l++){
        for(int m=0;m<k;m++){
          if(l != m){
            addCNF2va(getNodeNo(j,i,l),getNodeNo(j,i,m));
            cnf_no++;
          }
        }
      }
    }
  }

  for(int i=0;i<k;i++){
    for(int l=0;l<y;l++){
      for(int j=0;j<x;j++){
        for(int m=0;m<y;m++){
          for(int n=0;n<x;n++){
            if(j!=n || l!=m){
              addCNF2va(getNodeNo(j,l,i),getNodeNo(n,m,i));
              cnf_no++;
            }
          }
        }
      }
    }
  }

  for(int i=0;i<k;i++){
    for(int j=0;j<k;j++){
      if(adjacency[i][j]==1){
        for(int l=0;l<y;l++){
          for(int m=0;m<x;m++){
             addCNF3va(getNodeNo(m,l,i));
             if(m-1!=-1){
               addCNF1va(getNodeNo(m-1,l,j));
             }
             if(m+1!=x){
               addCNF1va(getNodeNo(m+1,l,j));
             }
             if(l-1!=-1){
               addCNF1va(getNodeNo(m,l-1,j));
             }
             if(l+1!=y){
               addCNF1va(getNodeNo(m,l+1,j));
             }
          addCNF0va();
          cnf_no++;
          }
        }
      }
    }
  }

  addCNF4va(x*y*k);

  for(auto e:cnf_head){
    outputfile<<e<<std::endl;
  }

  for(auto e:cnf){
    outputfile<<e<<std::endl;
  }

  outputfile.close();
}

int checkSAT(){
  std::fstream ifs("output",std::ios::in);
  if(ifs.fail()) std::cout<<"file open failed"<<std::endl;
  std::string str;

  while(!ifs.eof()){
    ifs>>str;
    if(str=="SAT"){
      return 1;
    }
    else if(str=="UNSAT"){
      return -1;
    }  
  }

  return 0;
}


bool isSameState(state s,state goal){
  for(int j=0;j<h;j++){
    for(int i = 0;i<w;i++){
      if(s.grid[i][j] != goal.grid[i][j]){
        return false;
      }
    }
  }
  return true;
}

int manhattanDist(state s,state goal){
  int eval = 0;
  for(int j=0;j<h;j++){
    for(int i=0;i<w;i++){
      for(int l=0;l<h;l++){
        for(int k=0;k<w;k++){
          if(s.grid[i][j] == goal.grid[k][l] && s.grid[i][j] <qubit_no){
            eval += (abs(i-k)+abs(j-l));
          }
        }
      }
    }
  }
  return eval;
}

void printState(state s){
  for(int j=0;j<h;j++){
    for(int i=0;i<w;i++){
      std::cout<<s.grid[i][j]<<" ";
    }
    std::cout<<std::endl;
  }
  //std::cout<<std::endl;
}

void  AStar(){
  int check_searched=0;
  if(open.empty()){
    std::cout<<"open list error"<<std::endl;
  }
  state now = open.top();
  open.pop();
  closed.push_back(now);
  std::cout<<"now state"<<std::endl;
  printState(now);
  std::cout<<"open list size:"<<open.size()<<std::endl;
  std::cout<<"closed list size:"<<closed.size()<<std::endl;
  std::cout<<"searched list size:"<<searced_state.size()<<std::endl;
  std::cout<<std::endl;
  if(isSameState(now,goal)){
    std::cout<<"finished"<<std::endl;
    std::cout<<"swap number:"<<now.depth<<std::endl;
  }  
  else{
     count++;
    state s = now;
    for(auto e:searced_state){
      if(isSameState(e,s) == true){
        check_searched =1;
        std::cout<<"check_searched"<<check_searched<<std::endl;
        }
    }
    if(check_searched == 0){
    for(int j=0;j<h;j++){
      for(int i=0;i<w-1;i++){
        std::swap(s.grid[i][j],s.grid[i+1][j]);
        printState(s);
        s.depth++;
        s.evaluation = manhattanDist(s,goal) + s.depth;
        std::cout<<"dist:"<<manhattanDist(s,goal)<<std::endl;
        std::cout<<"depth:"<<s.depth<<std::endl;
        std::cout<<"evaluation:"<<s.evaluation<<std::endl;
        std::cout<<std::endl;
        //if(open.size()<4000000){
          open.push(s);
        //}
        s = now;
      }
    }
    for(int i=0;i<w;i++){
      for(int j=0;j<h-1;j++){
        std::swap(s.grid[i][j],s.grid[i][j+1]);
        printState(s);
        s.depth++;
        s.evaluation = manhattanDist(s,goal) + s.depth;
        std::cout<<"dist:"<<manhattanDist(s,goal)<<std::endl;
        std::cout<<"depth:"<<s.depth<<std::endl;
        std::cout<<"evaluation:"<<s.evaluation<<std::endl;
        std::cout<<std::endl;
        //if(open.size()<4000000){
          open.push(s);
        //}
        s = now;
      }
    }
    searced_state.push_back(s);
    }
    AStar();
  }

int getCbitNo(qc::Gate gate){
  auto cbit_list = gate.get_cbits();
  auto cbit_it = cbit_list.begin();
  auto cbit = *cbit_it;

  return cbit.get_no();
}

int getTbitNo(qc::Gate gate){
  auto tbit_list = gate.get_tbits();
  auto tbit_it = tbit_list.begin();
  auto tbit = *tbit_it;

  return tbit.get_no();
}

auto searchSameCbitGate(qc::Circuit& circuit, qc::GatesIter& gate_iter){
  auto gate = *gate_iter;
  auto end_gate_iter = circuit.end_gates();
  end_gate_iter--;
  auto tbit_no = getTbitNo(gate);
  auto cbit_no = getCbitNo(gate);
  while(tbit_no != cbit_no && gate_iter != end_gate_iter){
    gate_iter++;
    gate = *gate_iter;
    /*gate.print();
      std::cout<<std::endl;*/
    cbit_no = getCbitNo(gate);  
  }

  return(gate_iter);
}

auto moveToSameCbitGate(qc::Circuit& circuit, qc::GatesIter& from_gate_iter){ 
  auto temp_from_gate_iter = from_gate_iter;
  auto end_gate_iter = circuit.end_gates();
  end_gate_iter--;
  auto same_cbit_gate_iter = searchSameCbitGate(circuit,from_gate_iter);
  auto same_gate = *same_cbit_gate_iter;
  //same_gate.print();
  //std::cout<<std::endl;
  from_gate_iter = temp_from_gate_iter;
  if(same_cbit_gate_iter == end_gate_iter){
    auto from_gate = *from_gate_iter;
    circuit.erase_gate(from_gate_iter);
    circuit.add_gate(from_gate);
    //std::cout<<"if"<<std::endl;
  }
  else{
    circuit.move_gate(same_cbit_gate_iter,from_gate_iter);
    //std::cout<<"else"<<std::endl;
  }
 
  return 0;
}
auto main(int argc, char* argv[]) -> int {
  auto input = argc >= 2 ? argv[1] : "circuit_data/alu4.blif";
  auto circuit = qc::io::open(input);
  int qubit_no=0;
  int pid,code,status;
  pid_t result;
  int x,y;
  int adjacency[200][200];
  qc::Circuit subcircuit;
  std::vector<qc::Gate> moved_gates;
  std::vector<qc::Circuit> subcircuits;
  //adjacency[1][2] = 1;
  //adjacency[0][2] = 1;

  circuit.print();
  std::cout<<std::endl;

  for(auto i = circuit.begin_gates();i!=circuit.end_gates(); i++){
    auto gate = *i;
    if(qubit_no < getCbitNo(gate)){
       qubit_no = getCbitNo(gate);
    }
    if(qubit_no < getTbitNo(gate)){
      qubit_no = getTbitNo(gate);
    }
    qubit_no++;
  }

  std::cout<<"qubit_no="<<qubit_no<<std::endl;
  x = sqrt(qubit_no);
  if(x*x != qubit_no){
    x++;
  }

  y = qubit_no / x;
  if(x*y != qubit_no){
    y++;
  }

  auto gate_iter = circuit.begin_gates();

  for(auto i = circuit.begin_gates();i!=circuit.end_gates(); i++){
    auto gate = *i;
    for(auto e:moved_gates){
      if(e = gate){
        moved_gates.clear();
        adjacency[][] = {0};
        Astar
      }
    }
    adjacency[getCbitNo(gate)][getTbitNo(gate)] = 1;
    adjacency[getTbitNo(gate)][getCbitNo(gate)] = 1;
    cnfencoder(x,y,x*y,adjacency);
    pid = fork();
    //std::cout<<"pid="<<pid<<std::endl;
    if(pid == -1){
      std::cout<<"pid error"<<std::endl;
    }
    if(pid == 0){
      execl("./glueminisat-simp","./glueminisat-simp","input.dimacs","output",NULL);
    }
    else{
      result = wait(&status);
    }

    if(result < 0){
      fprintf(stderr, "Wait Error.\n\n");
      exit(-1);
    }
      
      // 終了ステータスのチェック
    if(WIFEXITED(status)){
      //printf("子プロセス終了");
      code = WEXITSTATUS(status);
      //printf("コード : %d\n", code);
    }
    else{
      std::cout<<"wait失敗"<<std::endl;
      printf("終了コード : %d\n", status);
    }

    if(checkSAT() == 1){
      subcircuit.add_gate(gate);
      std::fstream ifs("output",std::ios::in);
      if(ifs.fail()) std::cout<<"file open failed"<<std::endl;
      std::string str;
      std::vector<std::string> result_str;
      std::vector<int> result_int;

      while(!ifs.eof()){
       ifs>>str;
      if(str!="SAT"){
        temp = std::stoi(str);
        //std::cout<<temp<<std::endl;
        if(temp>0){
          result_int.push_back(temp);
          }
          }
      }
    
    
    /*for(auto e:result_int){
      std::cout<<e<<std::endl;
    }*/

    for(auto e:result_int){
      start.grid[getXNo(e)][getYNo(e)] = getKNo(e);
    }
    }
    else if(checkSAT() == -1){
      moved_gates.push_back(*i);
      auto temp = i++;
      moveToSameCbitGate(circuit,i);
      i = temp--;
    }
    circuit.print();
    std::cout<<std::endl;
  }

  //moveToSameCbitGate(circuit,gate_iter);
  
  std::vector<std::vector<int> > grid_s(3,std::vector<int>(3));
  std::vector<std::vector<int> > grid_g(3,std::vector<int>(3));

  /*grid_s[0][0] = 3;
  grid_s[0][1] = 5;
  grid_s[0][2] = 2;
  grid_s[1][0] = 6;
  grid_s[1][1] = 1;
  grid_s[1][2] = 9;
  grid_s[2][0] = 4;
  grid_s[2][1] = 7;
  grid_s[2][2] = 8;
  for(auto i=0;i<3;i++){
    for(auto j=0;j<3;j++){
      std::cout<<grid_s[i][j];
    }
    std::cout<<std::endl;
    }

    std::cout<<std::endl;*/

  /* grid_g[0][0] = 9;
  grid_g[0][1] = 2;
  grid_g[0][2] = 6;
  grid_g[1][0] = 7;
  grid_g[1][1] = 4;
  grid_g[1][2] = 8;
  grid_g[2][0] = 1;
  grid_g[2][1] = 5;
  grid_g[2][2] = 3;
  for(auto i=0;i<3;i++){
    for(auto j=0;j<3;j++){
      std::cout<<grid_g[i][j];
    }
    std::cout<<std::endl;
    }*/
  
  /*for(auto i = circuit.begin_gates();i!=circuit.end_gates(); i++){
    qc::Circuit subcircuit;
    auto gate = *i;
    subcircuit.add_gate(gate);
    //subcircuits.push_back(subcircuit);
    }*/
  //auto gate = *gate_iter;
 
  //subcircuits[0].print();

 
  return 0;
}
