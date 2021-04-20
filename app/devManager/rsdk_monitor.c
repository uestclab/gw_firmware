#include "common.h"
#include "rsdk_monitor.h"

#include <sys/types.h>
#include <unistd.h>

//system("sh /run/media/mmcblk1p1/script/run_rsdk.sh");


void killprocess(char *processname)
{
    char cmd[128]="";
    sprintf(cmd,"kill -9 $(pidof %s)",processname);
    system(cmd);
    //usage : killprocess("process");
}

int promiscuous_read(g_handler_para* g_handler)
{  
    char path[64];  

    char value_str[3];  

    int fd;  

    snprintf(path, sizeof(path), "/sys/class/net/eth0/flags");  

    fd = open(path, O_RDONLY);  

    if (fd < 0){
        zlog_error(g_handler->log_handler,"Failed to open /sys/class/net/eth0/flags for reading!\n");
        return -1;
    }  

    if (read(fd, value_str, 3) < 0){
        zlog_error(g_handler->log_handler,"Failed to read value!\n");
        return -1;
    }
    close(fd);

    int value = atoi(value_str);
    zlog_info(g_handler->log_handler, "promiscuous value = 0x%x, value_str = %s", value,value_str);

    int ret = value & 0x0100;

    return ret;
}

int rx_packets_read(g_handler_para* g_handler)
{  
    char path[64];  

    char value_str[3];  

    int fd;  

    snprintf(path, sizeof(path), "/sys/class/net/eth0/statistics/rx_packets");  

    fd = open(path, O_RDONLY);  

    if (fd < 0){
        zlog_error(g_handler->log_handler,"Failed to open /sys/class/net/eth0/statistics/rx_packets for reading!\n");
        return -1;
    }  

    if (read(fd, value_str, 3) < 0){
        zlog_error(g_handler->log_handler,"Failed to read value!\n");
        return -1;
    }
    close(fd);

    int value = atoi(value_str);
    zlog_info(g_handler->log_handler, "rx_packets value = 0x%x, value_str = %s ", value, value_str);

    return value;
}


void* rsdk_work_thread(void* args){
	g_handler_para* g_handler = (g_handler_para*)args;
    g_args_para* g_args = g_handler->g_args;
    zlog_info(g_handler->log_handler, "start rsdk monitor work \n");
    int ret = 0;

    while(1){
        sleep(60);
        if(rx_packets_read(g_handler) == 0){
            killprocess("rsdk_linux.img");
            zlog_info(g_handler->log_handler, "run_rsdk_fun fun \n");
            system("sh /run/media/mmcblk1p1/script/run_rsdk.sh");
            continue;
        }
        int value_1 = rx_packets_read(g_handler);
        sleep(5);
        int value_2 = rx_packets_read(g_handler);
        if(value_2 == value_1){
            zlog_error(g_handler->log_handler, "rx_packets is not increase");
            killprocess("rsdk_linux.img");
            zlog_info(g_handler->log_handler, "run_rsdk_fun fun \n");
            system("sh /run/media/mmcblk1p1/script/run_rsdk.sh");           
        }
    }


	// postMsg(MSG_EXIT_MON_GPIO,NULL,0,NULL,0,g_handler->g_msg_queue);
	return NULL;
    
}

// add to monitor
void start_rsdk_monitor(g_handler_para* g_handler, ThreadPool* g_threadpool){
    AddWorker(rsdk_work_thread,(void*)g_handler,g_threadpool);
}
