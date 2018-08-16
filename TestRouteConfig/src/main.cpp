
#include "Debug_log.h"
#include "version.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xRedisClient.h"
#include "UtilsPorting.h"

using namespace xrc;

unsigned int APHash(const char *str) {
    unsigned int hash = 0;
    int i;
    for (i=0; *str; i++) {
        if ((i&  1) == 0) {
            hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
        } else {
            hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
        }
    }
    return (hash&  0x7FFFFFFF);
}

enum {
 CACHE_TYPE_1, 
 CACHE_TYPE_2,
 CACHE_TYPE_MAX,
};

volatile sig_atomic_t g_srv_shutdown = 0;
static volatile sig_atomic_t graceful_shutdown = 1;
static volatile sig_atomic_t handle_sig_alarm = 1;
static volatile sig_atomic_t handle_sig_hup = 0;
static volatile sig_atomic_t forwarded_sig_hup = 0;

#define SERVER_NAME	( "TESTRoute" )


static int signal_ignore(int sig)
{
	struct sigaction sa ;

	sa.sa_handler = SIG_IGN;
	sa.sa_flags = 0;

	if (sigemptyset(&sa.sa_mask) == -1 || sigaction(sig, &sa, 0) == -1) 
	{
		return -1;
	}
	return 0;
}

static void signal_handler(int sig) {
	switch (sig) {
	case SIGTERM: g_srv_shutdown = 1; break;
	case SIGINT:
	     if (graceful_shutdown) g_srv_shutdown = 1;
	     else graceful_shutdown = 1;

	     break;
	case SIGALRM: handle_sig_alarm = 1; break;
	case SIGHUP:  handle_sig_hup = 1; break;
	case SIGCHLD:  break;
	}
    LOG_TRACE(7, true, "signal_handler", " "
        <<" sig:"<<sig
        <<" g_srv_shutdown:"<<g_srv_shutdown
        );
}

void signal_set()
{
	signal(SIGPIPE, SIG_IGN);
	signal_ignore(SIGHUP);
	signal(SIGUSR1, SIG_IGN);
	signal(SIGALRM, signal_handler);
	signal(SIGTERM, signal_handler);
	signal(SIGHUP,  signal_handler);
	signal(SIGCHLD, signal_handler);
	signal(SIGINT,  signal_handler);
}

RedisNode RedisList1[2]=
{
    {0,"10.136.24.74", 9125, "", 2, 5, 0},
    {0,"10.136.24.213", 9125, "", 2, 5, 1},    
};
    
const char *g_key = "MCUNODE";

bool GetMcuNodeList(xRedisClient &xRedis, RedisDBIdx &dbi)
{
    ArrayReply array;

    uint32_t randIndex = rand()%3;
    string ispType;

    switch(randIndex)
    {
        case 0:
            ispType = "1";
            break;
        case 1:
            ispType = "2";
            break;            
        case 2:            
        default:
             ispType = "4";
            break;       
    }
    
    if (!xRedis.GetMcuNodeList(dbi, ispType, "3", "2", array))
    {
        //LOG_TRACE(LOG_ERR, false, __PRETTY_FUNCTION__, " get mcu node list error ");
        printf("get mcu node list error\n");
        
        return false;
    }

	if (0 != (array.size() % 2))
    {
        //LOG_TRACE(LOG_ERR, false, __PRETTY_FUNCTION__, " get mcu node list error ");
        printf("get mcu node list, array size error\n");
        
        return false;
    }

#if 1   
	for (int i = 0; i < array.size(); i+=2)
    {
        printf("%d,%d\n", atoi(array[i].str.c_str()), atoi(array[i+1].str.c_str()));
    
        /*LOG_TRACE(LOG_INFO, false, __PRETTY_FUNCTION__, " get mcu node list "
                  << " mcuid: "<<  atoi(array[i].str.c_str())
                  << " relay: "<<  atoi(array[i+1].str.c_str()));*/
    }
#endif

    return true;
}

int main (int argc, char **argv)
{
    printf("service start \n");

    signal_set(); 

    xRedisClient xRedis;
 
    xRedis.Init(CACHE_TYPE_MAX);
    
    if (!xRedis.ConnectRedisCache(RedisList1, sizeof(RedisList1) / sizeof(RedisNode), 1, CACHE_TYPE_1))
    {
        printf("connect redis fail\n");
        return false;
    } 
    
    RedisDBIdx  dbi(&xRedis);
    if (!dbi.CreateDBIndex(g_key, APHash, CACHE_TYPE_1))
    {
        printf("create db index fail\n");
    
        return false;
    }

    uint32_t begin = GetTickCount() ;
    uint32_t now;
    uint32_t counter=0;
    
    #if 1
    for(uint32_t i=0;;++i)
    {    
        now = GetTickCount() ;
        if (now - begin > 1000*60)
        {
            printf("rate:%d\n", counter/60);
            begin = now;
            counter = 0;
        }
        
        GetMcuNodeList(xRedis, dbi);
        counter++;
    }
    #else
    GetMcuNodeList(xRedis, dbi);
    #endif
    xRedis.Release();

    printf("service stop \n");

    return 0;
}



