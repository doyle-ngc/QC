#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <unistd.h>

struct Node{
  int id,x,y,k;
};

std::vector<Node> node_list;
Node node;
std::vector<std::string> cnf;

int getNodeNo(int x,int y,int k){
  for(auto e:node_list){
    if(e.x==x && e.y==y && e.k==k){
      return e.id;
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
  cnf.push_back(str);
}


void cnfencoder(int x,int y,int k,int adjacency[][200]){

  //int x,y,k;
  //int adjacency[k][k];
  int count=1;
  std::fstream outputfile("input.dimacs",std::ios::out);
  if(outputfile.fail()) std::cout<<"file open faild"<<std::endl;
  std::vector<int> cons;
  //adjacency[1][2] = 1;
  //adjacency[2][3] = 1;
  
  
  for(int i=0;i<k;i++){
    for(int j=0;j<y;j++){
      for(int l=0;l<x;l++){ 
        node.id=count;
        node.x=l;
        node.y=j;
        node.k=i;
        node_list.push_back(node);
        count++;
      }
    }
  }

  addCNF4va(k);

  for(int i=0;i<k;i++){
    for(int j=0;j<y;j++){
      for(int l=0;l<x;l++){
        addCNF1va(getNodeNo(l,j,i));
      }
    }
    addCNF0va();
  }

  for(int i=0;i<k;i++){
    for(int l=0;l<y;l++){
      for(int j=0;j<x;j++){
        for(int m=0;m<y;m++){
          for(int n=0;n<x;n++){
            if(j!=n || l!=m){
              //std::cout<<getNodeNo(j,l,i)<<std::endl;
              //std::cout<<getNodeNo(n,m,i)<<std::endl;
              //std::cout<<std::endl;
              addCNF2va(getNodeNo(j,l,i),getNodeNo(n,m,i));
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
          }
        }
      }
    }
  }

  for(auto e:cnf){
    outputfile<<e<<std::endl;
  }

  outputfile.close();

  //execl("./glueminisat-simp","input.dimacs","output",NULL);

}

int main(){

  int x=3;
  int y=3;
  int k=8;
  int adjacency[200][200];
  int temp;
  int count=0;
  adjacency[0][2]=1;
  adjacency[2][3]=1;

  cnfencoder(x,y,k,adjacency);

  std::fstream ifs("output",std::ios::in);
  if(ifs.fail()) std::cout<<"file open failed"<<std::endl;
  std::string str;
  std::vector<std::string> result_str;
  std::vector<int> result_int;

  while(!ifs.eof()){
    ifs>>str;
    if(str!="SAT"){
    temp = std::stoi(str);
    std::cout<<temp<<std::endl;
    if(temp>0){
    result_int.push_back(temp);
    }
    }
  }

  std::cout<<std::endl;

  for(auto e:result_int){
    std::cout<<e<<std::endl;
  }

  return 0;
}
