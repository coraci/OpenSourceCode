#ifndef   _MACRO_LogModule
#define   _MACRO_LogModule
#include <stdio.h>
#include <pthread.h>
#include "macro_define.h"

#include <string>
#include <time.h>
#include <stdlib.h>

using namespace std;

/* ÿ���̵߳�buffer size*/
#define   _LOG_BUFFSIZE  1024*1024*4
/* ��ǰ���̵� Stream IO buffer size*/
#define   _SYS_BUFFSIZE  1024*1024*8
/* log �ļ��ַ���·����󳤶�*/
#define	  _LOG_PATH_LEN  250
/* ��־��Ӧ��ģ����*/
#define   _LOG_MODULE_LEN 32

typedef  enum LogLevel {  
	LL_DEBUG = 1,
	LL_TRACE = 2,
	LL_NOTICE = 3, 
	LL_WARNING = 4, 
	LL_ERROR = 5,
}LogLevel;


class CreateLogFile
{
public:
    CreateLogFile()
    {
        // ����һ����д�ļ�
        m_pFile = 0;
        m_strFileName = "";

        time_t ntime;
        tm* ptimeinfo;

        time(&ntime);
        ptimeinfo = localtime(&ntime);

        char szBuffer[1024] = {0};

        long now = clock();
        char nowBuf[64] = {0};
        sprintf(nowBuf,"%ld",now);

        sprintf(szBuffer,"%4d-%02d-%02d",1900+ptimeinfo->tm_year, 1+ptimeinfo->tm_mon,
            ptimeinfo->tm_mday);

        m_strFileName = szBuffer;

        m_strFileName += "-";
        m_strFileName += nowBuf;

        m_strFileName += ".log";

        m_pFile = fopen(m_strFileName.c_str(),"w+");

    }
    int GetRandomNum(int a,int b)//
    {
        static int initFlag = 0;
        if (initFlag == 0)
        {
            srand((unsigned)time(NULL));
            initFlag = 1;
        }

        return rand()%(b-a)+a;
    }

    void	Record(const string& str)
    {
        // ����Ϣд�뵽�ļ���
        if (m_pFile == 0)
        {
            return;
        }

        fwrite(str.c_str(),str.size(),1,m_pFile);
        fflush(m_pFile);
    }

    ~CreateLogFile()
    {
        // �ر��ļ�
        if (m_pFile != 0)
        {
            fflush(m_pFile);
            fclose(m_pFile);
        }
    }

private:
    string m_strFileName;		// ��־�ļ�����
    FILE* m_pFile;					// �ļ�ָ��

};

class Log_Writer
{
	public:
		Log_Writer()
		{
			m_system_level = LL_NOTICE;
			//fp = stderr;
			fp = NULL;
			m_issync = false;
			m_isappend = true;
			m_filelocation[0] ='\0'; 
			pthread_mutex_init(&m_mutex, NULL);
		}
		~Log_Writer(){
			logclose();
		}
		bool loginit(LogLevel l, const  char *filelocation, bool append = true, bool issync = false);
		bool log(LogLevel l,char *logformat,...);
		LogLevel get_level();
		bool logclose();
	private:
		const char* logLevelToString(LogLevel l);
		bool checklevel(LogLevel l);
		int premakestr(char* m_buffer, LogLevel l);
		bool _write(char *_pbuffer, int len);
	private:
		enum LogLevel m_system_level;
		FILE* fp;
		bool m_issync;
		bool m_isappend;
		char m_filelocation[_LOG_PATH_LEN];
		pthread_mutex_t m_mutex;
		static __thread char m_buffer[_LOG_BUFFSIZE];
		//The __thread specifier may be applied to any global, file-scoped static, function-scoped static, 
		//or static data member of a class. It may not be applied to block-scoped automatic or non-static data member
		//in the log  scence,It's safe!!!!
		//һ���Ա�֮���˳������õ���__thread������Դleak,ͬʱҲ���õ��Ķ��Log_Writer����ţ�
		//��Ϊһ���߳�ͬһʱ��ֻ��һ��Log_Writer�ڸɻ����֮��m_buffer��reset��
		//���Լ���һ���߳��û����Log_Write����(��Ϊһ���߳��ڵ�����ֻ̬�д���) Ҳ���̰߳�ȫ�ģ�����
};
extern Log_Writer WARN_W;
extern Log_Writer INFO_W;

/**
 * LogLevel ��־����
 * p_modulename ģ���� ��mysql
 * p_logdir  ��־���Ŀ¼
 * */
bool log_init(LogLevel l, const char* p_modulename, const char* p_logdir);
#endif
