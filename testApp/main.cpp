#include <iostream>
#include <unistd.h>
#include <string.h>
#include "my_log_proxy_wrapper.h"
#include <signal.h>

void signal_exit_handler(int sig)
{
    my_log_deinit();
    exit(0);
}

int main(int argc, char ** argv)
{
    my_log_init();
    my_log_create(0, "test_0", "./log0/", ".log", 10*1024, 10, 300*1024*1024, 4*1024);
    my_log_create(1, "test_1", "./log1/", ".log", 10*1024, 10, 300*1024*1024, 4*1024);
    signal(SIGTERM, signal_exit_handler);
    signal(SIGINT, signal_exit_handler);
    signal(SIGPIPE, SIG_IGN);

    while (true)
    {   
        char buf[1024];
        memset(buf, 88, 1023);
        std::cout << "buf: " << buf << std::endl;
        my_log_write_buffer(0, buf, 2);
        my_log_write_buffer(1, buf, 1);
        sleep(1);
    }
    
    return 0;
}