#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include "thread.h"

using namespace std;

mutex m1;
cv cv1;
vector<pair<int,int>> q1;
vector<int> v1;
vector<pair<int,int>> d1;
vector<bool> l1;
int cur_num = 0, max_num = 0, val_num = 0, ini_num = 0, prev_num = 0;
void disk(void *a) {
  intptr_t index = intptr_t(a);
  while(1){
    if(v1[index] == 2 || val_num == 0) break;
    m1.lock();
    if(l1[index] == 1 || q1.size() >= val_num)	m1.unlock();
    else{
      if(v1[index] == 0){
	q1.push_back({index,d1[index].first});
	cout << "requester " << index << " track " << d1[index].first << endl;
	++v1[index];
	l1[index] = 1;	
	int tmp_num = 0;
	for(int i=0; i<ini_num; ++i){
		tmp_num += l1[i];
	}
	val_num = min(ini_num-tmp_num+(int)q1.size(),max_num);
	m1.unlock();
      }
      else{
	q1.push_back({index,d1[index].second});
	cout << "requester " << index << " track " << d1[index].second << endl;
	++v1[index];
	--cur_num;
	l1[index] = 1;
	int tmp_num = 0;
	for(int i=0; i<ini_num; ++i){
		tmp_num += l1[i];
	}
	val_num = min(ini_num-tmp_num+(int)q1.size(),max_num);
	m1.unlock();
      }
    }
  }
}
bool cmp(pair<int,int> a, pair<int,int> b){
	if(abs(a.second-prev_num) < abs(b.second-prev_num))	return true;
	else return false;
}
void build(void* a) {
    char** argv = (char**)a;
    while(argv[cur_num+2] != NULL) cur_num++;
    max_num = atoi(argv[1]);
    val_num = min(cur_num,max_num);
    ini_num = cur_num;
    for(int i=0; i<cur_num; ++i){
        v1.push_back(0);
	l1.push_back(0);
        ifstream f_tmp(argv[i+2]);
        int num1, num2;
        f_tmp >> num1 >> num2;
        d1.push_back({num1,num2});
    }
    for(int i=0; i<cur_num; ++i) thread t((thread_startfunc_t)disk, (void*)i);
    while(cur_num){
      while(q1.size() < val_num && cur_num);
      if(!cur_num) break;
      m1.lock();
	sort(q1.begin(),q1.end(),cmp);
      pair<int,int> p1 = q1[0];
      q1.erase(q1.begin());
      cout << "service requester " << p1.first << " track " << p1.second << endl;
	prev_num = p1.second;
      if(v1[p1.first] == 1)	l1[p1.first] = 0;
	int tmp_num = 0;
	for(int i=0; i<ini_num; ++i){
		tmp_num += l1[i];
	}
	val_num = min(ini_num-tmp_num+(int)q1.size(),max_num);
      m1.unlock();
    }
    m1.lock();
    while(q1.size()){
      pair<int,int> p1 = q1[0];
      q1.erase(q1.begin());
      cout << "service requester " << p1.first << " track " << p1.second << endl;
    }
    m1.unlock();
}

int main(int argc, char* argv[])
{
	cpu::boot((thread_startfunc_t) build, (void *) argv , 0);
}



