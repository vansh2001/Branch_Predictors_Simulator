#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim_bp.h"
#include <vector>
#include <cmath>

using namespace std;
/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
*/
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
    char outcome;           // Variable holds branch outcome
    unsigned long int addr = 0; // Variable holds the address read from input file
    
    if (!(argc == 4 || argc == 5 || argc == 7))
    {
        printf("Error: Wrong number of inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }
    
    params.bp_name  = argv[1];
    //printf("%s", params.bp_name);
    
    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
    {
        if(argc != 4)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M2       = strtoul(argv[2], NULL, 10);
        trace_file      = argv[3];
        printf("COMMAND\n%s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);

    }
    else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
    {
        if(argc != 5)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.M1       = strtoul(argv[2], NULL, 10);
        params.N        = strtoul(argv[3], NULL, 10);
        trace_file      = argv[4];
        printf("COMMAND\n%s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

    }
    else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
    {
        if(argc != 7)
        {
            printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
            exit(EXIT_FAILURE);
        }
        params.K        = strtoul(argv[2], NULL, 10);
        params.M1       = strtoul(argv[3], NULL, 10);
        params.N        = strtoul(argv[4], NULL, 10);
        params.M2       = strtoul(argv[5], NULL, 10);
        trace_file      = argv[6];
        printf("COMMAND\n%s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

    }
    else
    {
        printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
        exit(EXIT_FAILURE);
    }
    
    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }
    
    char str[2];
    unsigned long int size = 0;
    unsigned long int pc_bits = 0;
    gShare *gshare;
    gshare = new gShare();
    
    if(!strcmp(params.bp_name, "bimodal")){
        //printf("bimodal\n");
        size = params.M2;
        
    } else {
        //printf("gshare\n");
        size = params.M1;
        pc_bits = params.N;
        gshare->GBH_size = pc_bits;
    }
    
    //printf("%s", params.bp_name);
    int c_predictions = 0;
    int mispredictions = 0;
    initialize_vector(size);
    while(fscanf(FP, "%lx %s", &addr, str) != EOF)
    {
        c_predictions++;
        
        outcome = str[0];
        branch_check(addr, size, pc_bits, outcome, gshare);
        
        //index_extract(addr, params.M2);
        if (outcome == 't')
            //printf("%lx %s\n", addr, "t");           // Print and test if file is read correctly
            printf("");
        else if (outcome == 'n')
            //printf("%lx %s\n", addr, "n");          // Print and test if file is read correctly
            mispredictions++;
        /*************************************
            Add branch predictor code here
        **************************************/
    }
    
    double mispre = (double)gshare->misprediction / (double)c_predictions;
    mispre = mispre * 100;
    cout << "number of predictions:\t  " << c_predictions <<endl;
    cout << "number of mispredictions: " << gshare->misprediction << endl;
    printf("mispredictions rate:\t  %.2f%%\n", mispre);
    
    
    print_predictor(size);
    
    return 0;
}
