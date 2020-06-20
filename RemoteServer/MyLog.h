#include <fstream>
#include "string"
using namespace std;
class MyLog
{
public:
    MyLog(string logPath);
    ~MyLog();

    void PrintLog(string str);
    void ClearLog();
    
private:
    string m_logPath;
    ofstream m_file;
};
