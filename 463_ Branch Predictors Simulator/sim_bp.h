#ifndef SIM_BP_H
#define SIM_BP_H
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

typedef struct bp_params{
    unsigned long int K;
    unsigned long int M1=0;
    unsigned long int M2=0;
    unsigned long int N=0;
    char*             bp_name;
}bp_params;

// Put additional data structures here as per your requirement
class gShare {
public:
    unsigned long int index;
    unsigned long int GBH_size;
    unsigned long int BHR_bits;
    int misprediction;
};


vector <int> predictor;

void initialize_vector(unsigned long int low_order_size){
    int size = pow (2,low_order_size);
    for(int i = 0; i < size; i++){
        predictor.push_back(2);
    }
}

long unsigned int index_extract(unsigned long int index, unsigned long int size){
    unsigned long int index_final; //store final value
    index_final = index >> 2; //right shift by 2
    unsigned long int shift_value = 0;
    for(unsigned long int i = 0; i < size; i++){
        shift_value = shift_value << 1; //left shift one
        shift_value = shift_value | 1; //or with 1
    }
    index_final = index_final & shift_value;
    //cout << hex << index_final << endl;
    
    return index_final;
}


void BHR_update(gShare *branch, char actual_pred) {
    unsigned long int GBH = branch->BHR_bits;
    unsigned long int fin = 0;
    unsigned long int size_to_or;
    size_to_or = pow(2, (branch->GBH_size - 1));
    if(actual_pred == 't'){
       fin = GBH >> 1;
       fin = fin | size_to_or;
    }else {
       fin = GBH >> 1;
       //fin = fin << 1;
    }
    
    branch->BHR_bits = fin;
}

void branch_check(unsigned long int index, unsigned long int size, unsigned long int N, char outcome, gShare *branch){
    long unsigned int index_bits;
    long unsigned int branch_bits;
    long unsigned int branch_index;
    long unsigned int mask;
    long unsigned int shift_size = size - N;
    long unsigned int BHR = branch->BHR_bits;
    index_bits = index_extract(index, size);
    
    branch_index = index_bits;
    if(N != 0) {
        branch_bits = index_bits >> shift_size;//BHR_index(*branch, index_bits);
        //cout << hex <<branch_bits << endl;
        branch_index = branch_bits^BHR;
        branch_index = branch_index << shift_size;
        mask = index_extract(index, shift_size);
        branch_index = branch_index | mask;
        BHR_update(branch, outcome);
    }
    
    if(predictor[branch_index] >= 2  && outcome == 'n') {
        branch->misprediction++;
        
    } else if(predictor[branch_index] < 2  && outcome == 't'){
        branch->misprediction++;
    }
    
    if(outcome == 't') {
        if(predictor[branch_index] < 3){ predictor[branch_index]++; }

    } else {
        if(predictor[branch_index] > 0){ predictor[branch_index]--; }
    }
    
}

void print_predictor(unsigned long int size){
    unsigned long int predic_size;
    predic_size = pow(2,size);
    cout << "FINAL BIMODAL CONTENTS"<< endl;
    for(unsigned long int i =0; i < predic_size; i++){
        cout << i << "\t\t" << predictor.at(i) << endl;
    }
}


#endif
