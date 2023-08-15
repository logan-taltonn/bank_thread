
#include <string> // string
#include <unistd.h> // getopt
#include <fstream> // ifstream
#include <vector> // vector
// #include <stdlib.h> // exit
#include <chrono> // chrono time
#include <thread> // thread, join
#include <iostream> // cout, endl
#include "BankAccount.h" // BankAccount class
using namespace std;

// helper function for printing relevant information
void print_helper(int _approach_num, const chrono::time_point<chrono::steady_clock>& start,
    const chrono::time_point<chrono::steady_clock>& end, BankAccount& a);


// thread function the teller will use to process a transaction
void teller_thread(BankAccount* b, int _amount) {
    // call transaction(..., false) function for b
    b->transaction(_amount, false); // not thread safe, thus false
}

// thread function the teller will use to process a threadsafe transaction
void teller_threadsafe(BankAccount* c, int _amount) {
    // call transaction(..., true) function for c
    c->transaction(_amount, true); // thread safe, thus true, runs function that has locking
}

int main(int argc, char* argv[]) {

    // variables for whole runtime
    string fname = "transactions.csv"; // name of input file
    int num_trans = 2000; // number of transactions

    // getopt to read flags
    // "your program's input will be the =i flag for the name of the file"
    int opt;
    while ((opt = getopt(argc, argv, "i:")) != -1) {
        switch (opt) {
            case 'i':
                fname = optarg;
                break;
        }
    }

    // fill the contents of the file into an array
    ifstream in_file(fname);
    if (!in_file.is_open()) {
        perror("ifstream");
        exit(EXIT_FAILURE);
    }
    int* trans_arr = new int[num_trans]; 
    for (int i = 0; i < num_trans; ++i) {
        in_file >> trans_arr[i];
    }
    in_file.close();

    // // approach 1: completely synchronous
    // BankAccount A;
    // // begin timer
    // const chrono::time_point<chrono::steady_clock> start1 = chrono::steady_clock::now();
    // // process transactions, one at a time
    // for (int i = 0; i < num_trans; ++i) {
    //     A.transaction(trans_arr[i], false);
    // }
    // // end timer and print result
    // const auto end1 = chrono::steady_clock::now();
    // print_helper(1, start1, end1, A);


    // approach 2: a vector of threads to carry out each transaction
    BankAccount B;
    vector<thread> t_vec;
    // begin timer
    const chrono::time_point<chrono::steady_clock> start2 = chrono::steady_clock::now();
    // create *num_trans* threads and push_back() each one to t_vec
    for (int i = 0; i < num_trans; ++i) {
        t_vec.push_back(thread(teller_thread, &B, trans_arr[i]));
    }
    // join threads
    for (size_t i = 0; i < t_vec.size(); ++i) {
        t_vec.at(i).join();
    }
    // end timer and print result
    const auto end2 = chrono::steady_clock::now();
    print_helper(2, start2, end2, B);


    // approach 3: implement mutex to ensure that each transaction (will not affect another transaction / thread-safe)
    BankAccount C;
    // remove everything before C.print_balance()
    vector<thread> t_vec2;
    // begin timer
    const chrono::time_point<chrono::steady_clock> start3 = chrono::steady_clock::now();
    // create *num_trans* threads and push_back() each one to t_vec2
    for (int i = 0; i < num_trans; ++i) {
        t_vec2.push_back(thread(teller_threadsafe, &C, trans_arr[i]));
    }
    // join all threads in t_vec2
    for (size_t i = 0; i < t_vec2.size(); ++i) {
        t_vec2.at(i).join();
    }
    // end timer and print result
    const auto end3 = chrono::steady_clock::now();
    print_helper(3, start3, end3, C);
    // clean-up
    delete[] trans_arr;
}

// print helper
void print_helper(int _approach_num, const chrono::time_point<chrono::steady_clock>& start, 
    const chrono::time_point<chrono::steady_clock>& end, BankAccount& a) {
    cout << "Approach " << _approach_num << " took " << (end - start) / 1ms;
    cout << "ms to achieve a final balance of ";
    a.print_balance(); cout << endl;
}
