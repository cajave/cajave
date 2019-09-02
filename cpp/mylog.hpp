#ifndef _MY_MYLOG_HPP_
#define _MY_MYLOG_HPP_

#include <fstream>
#include <chrono>
#include <unordered_map>
#include <ctime>
#include <iomanip>

namespace sc = std::chrono;
using scs = sc::system_clock;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++/
namespace google {
namespace protobuf {
class LIBPROTOBUF_EXPORT Message : public MessageLite {
        //Debugging & Testing----------------------------------------------
        // Generates a human readable form of this message, useful for debugging
        // and other purposes.
        string DebugString() const;
        // Like DebugString(), but with less whitespace.
        string ShortDebugString() const;
        // Like DebugString(), but do not escape UTF-8 byte sequences.
        string Utf8DebugString() const;
        // Convenience function useful in GDB.  Prints DebugString() to stdout.
        void PrintDebugString() const;
    };
}
}
/++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/



namespace mylog {

std::ofstream& get_log_stream(const std::string& filename = "", std::ios_base::openmode mode = std::ios_base::out){
    if (filename.empty()){
        static std::ofstream out("default.log", mode);
        return out;
    }
    static std::unordered_map<std::string, std::ofstream> streams;
    auto it = streams.find(filename);
    if (it == streams.end()){
        streams.insert(std::make_pair(filename, std::move(std::ofstream(filename, mode))));
         return streams[filename];
    }
   return it->second;
}

struct EnterLeave{
    EnterLeave(std::ofstream& o, const std::string& tk, const std::string& file, 
        const std::string& func,  int line) : out_(o), token_(tk){
            print_pline(); 
            print_timestamp();
            out_ << std::endl << " file : " << (file) << " | fun : " << (func) << " | line : " << (line) << std::endl;
        }
    ~EnterLeave(){
        print_timestamp(); 
        print_pline();
        }
private : 
    void print_pline(){ 
        out_ << std::endl; 
        for(int i = 0; i < num_; i++)out_ << token_; 
        out_ << std::endl; 
     }
    void print_timestamp(){
        out_ << std::endl; 
        std::time_t time_now = scs::to_time_t(scs::now() - sc::hours(24));
        out_ << std::put_time(std::localtime(&time_now), "%c");
        out_ << std::endl; 
    }
    std::ofstream& out_;
    const std::string& token_;
    size_t num_ = 60;
};


std::ofstream& original_log = get_log_stream("original.log");

} // end of mylog namespace

#define ENTERLEAVE(stream, token) mylog::EnterLeave enter_leave(stream, token, __FILE__, __FUNCTION__, __LINE__)
#define TRACEVAR(stream, var)  (stream) << #var << " : " << (var) << std::endl
#define TRACEEXPR(stream, expr) (stream)<< #expr << " : " << std::endl << std::string((expr)) << std::endl

extern std::ofstream& mylog::original_log;
#define TRACE_PROTO_MSG(x) do { ENTERLEAVE(mylog::original_log, "+");\
    TRACEEXPR(mylog::original_log, (x).DebugString()); } while(0)

#endif



// TRACE_PROTO_MSG(global_data_message);
