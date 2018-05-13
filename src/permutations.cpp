#include<iostream>
#include<string>
#include<list>

using namespace std;

void print( list<int> l){
    for(list<int>::iterator it=l.begin(); it!=l.end() ; ++it)
            cout << " " << *it;
    cout<<endl;
}

void print( list<list<int>> l){
    for(list<list<int>>::iterator it=l.begin(); it!=l.end() ; ++it)
         print(*it) ;
}

void subsetTest(int arr[], int size, int left, int index, list<int> &l, list<list<int>> &all){

	if(left==0){
        //print(l);
        all.push_back(l);
        return;
    }
    for(int i=index; i<size;i++){
    	l.push_back(arr[i]);
        subsetTest(arr,size,left-1,i+1,l, all);
        l.pop_back();
    }

}

int mainTestSubSet(){
    int array[5]={1,2,3,4,5};
    list<int> lt;
    list<list<int>> all;
    subsetTest(array,5,3,0,lt,all);
    cout << "ALL--------------" << endl;
    print(all);
    return 0;
}
