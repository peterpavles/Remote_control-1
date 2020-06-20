#include "MyLog.h"

MyLog::MyLog(string logPath)
    :m_logPath(logPath)
{
}

MyLog::~MyLog()
{
}

void MyLog::PrintLog(string str)
{
    m_file.open(m_logPath, ios::app);
    m_file << str << endl;
    m_file.close();
}

void MyLog::ClearLog()
{
    m_file.open(m_logPath, ios::out | ios::trunc);
    m_file.close();
}
