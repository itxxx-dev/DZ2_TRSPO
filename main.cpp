#include <iostream>
#include <random>
using namespace std;
#include <thread>   // для самих потоков
#include <vector>
#include <mutex>    // для мьютексов и lock_guard
#include <chrono>
#include <fstream>
#include <iomanip>
vector<double> results_of_threads;// total number of points
vector<int64_t> N_values = {1000000, 10000000, 100000000, 1000000000, 10000000000};
vector<int> M_values = {1, 2, 4, 8, 16, 32, 64, 128};

class Technical {
public:
    Technical(int64_t k, int64_t index) {
        this->index = index;
        this->k = k;
    };
    int64_t getK() {
        return this->k;
    }
    int getIndex() {
        return this->index;
    }
private:
    int64_t k;
    int64_t index;
};

void pi_thread_worker(Technical tech) {
    int64_t k = tech.getK();
    int index = tech.getIndex();
    random_device rd;// device setup(for seed purposes, as i think)
    mt19937 engine(rd()); // engine initialization with seed from the random device upper
    uniform_real_distribution<double> dist(-1, 1);// here we simply defined the distribution range
    int64_t Nins = 0;
    double x, y;
    int64_t i = 0;
    while (i < k) {
        x = dist(engine);
        y = dist(engine);
        if (x*x + y*y <= 1) {
            Nins++;
        }
        i++;
    }
    double pi = (4.0 * Nins)/k;
    results_of_threads[index] = pi;
}
int main() {
    ofstream my_file("results/pi_monte_carlo_parallel_results.csv");
    my_file << "N/M,1,2,4,8,16,32,64,128" << endl;
    for (int64_t n: N_values) {
        my_file << n;
        for (int64_t m: M_values) {
            {
                int64_t k = n/m;
                results_of_threads.resize(m);
                vector<thread> threads;
                auto start = chrono::high_resolution_clock::now();
                for (int i = 0; i<m; i++) {
                    Technical tech(k, i);
                    thread t(pi_thread_worker, tech);
                    threads.push_back(move(t));


                }

                double mediary = 0;

                for (int i = 0; i<m; i++) {
                    threads.at(i).join();
                    mediary += results_of_threads[i];
                }
                mediary/=(double)m;
                auto stop  = chrono::high_resolution_clock::now();
                chrono::duration<double> diff = stop - start;
                double exe_time = diff.count();
                my_file << "," << fixed << setprecision(6) << exe_time;
                // cout << "Estimated value of Pi is: " << mediary << endl;
                // cout << "Execution time in seconds: " << exe_time;
                results_of_threads.clear();
                cout << "N = " << n << " | M = " << m << " | Time: " << exe_time << " s | Pi: " << mediary << endl;
            }

        }
        my_file << "\n";
        // Завершено виконання лабораторної роботи
    }
    return 0;
}