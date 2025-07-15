#include <chrono>
#include <cstdint>
#include <cstdio>
#include <stdio.h>
#include <map>
#include <sys/types.h>
#include <sstream>
#include <iomanip>

using Clock = std::chrono::high_resolution_clock;
using FunctionName = std::string;

static Clock::time_point t_start;

struct Alloc {
    std::string fn_name;
    int invocation_num;
    Clock::time_point alloc_time;
    uint64_t start_address;
    uint64_t size;
};

std::string to_string(const Clock::time_point& tp) {
    auto duration = tp.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    return std::to_string(millis) + "ms";
}

std::string to_string(const Alloc& alloc) {
    std::ostringstream oss;
    
    oss << "Alloc{"
        << "fn_name: " << alloc.fn_name
        << ", invocation_num: " << alloc.invocation_num
        << ", alloc_time: " << to_string(alloc.alloc_time)  // or custom stream
        << ", start_address: 0x" << std::hex << alloc.start_address
        << ", end_address: "  << alloc.size << " bytes"
        << "}";
    
    return oss.str();
}

/// Increments each time a function is called, helps set the ordering for function calls.
static int invocation_counter = 0;

/// The overarching datastructure the measure allocations
static std::map<FunctionName, std::vector<Alloc>> stack_allocations;

extern "C" void capture_stack_alloc(const char* fn_name, uint64_t start_addr, uint64_t size) {
    std::string name(fn_name);
    Clock::time_point now = Clock::now();
    
    Alloc alloc = {
        name,
        invocation_counter,
        now,
        start_addr,
        size,
    };
    
    stack_allocations[name].push_back(alloc);
}

extern "C" void start_timer(){
    t_start = Clock::now();
}

extern "C" void end_timer(const char* fn_name) {
    auto t_end = Clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end - t_start).count();
    fprintf(stdout, "[%s]: elapsed time (ns): %lld\n", fn_name, duration);
}

// int call_count = 0;

// class Timer {
//     private:
//     std::chrono::time_point<std::chrono::system_clock> m_start;
//     std::chrono::time_point<std::chrono::system_clock> m_end;
//     bool m_running;
    
//     public:
//     extern "C" void start_time() {
//         this->m_start = std::chrono::system_clock::now();
//         this->m_running = true;
//     };
    
//     extern "C" void end_timer() {
//         this->m_end = std::chrono::system_clock::now();
//         this->m_running = false;
        
//         long duration = chrono::duration<chrono::milliseconds>(this->m_end, this->m_start).count();
        
//         fprintf(stdout, "elapsed time (ms): ", duration);
//     }
// }

// extern "C" void record_func_call() {
//     call_count++;
//     fprintf(stderr, "Function call count: %d\n", call_count);
// }

