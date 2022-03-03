#include <unistd.h>
#include <iostream>
#include <signal.h>
#include <sched.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdlib.h>
#include <errno.h>

#define NAME_PROCESS "prog"

void lock_process(int *fd) {
    int res_flock;
    *fd = open("/var/run/lock/file_lock_another_instance", O_CREAT | O_RDWR, 0666);
    if(*fd == -1){
        std::cout << "Error " << errno << " occured in open function" << std::endl;
        exit(EXIT_FAILURE);
    }
    res_flock = flock(*fd, LOCK_EX | LOCK_NB);
    if(res_flock == -1 && errno != EWOULDBLOCK){
        std::cout << "Error " << errno << " occured in flock function" << std::endl;
        exit(EXIT_FAILURE);
    }
    if (res_flock != 0 && errno == EWOULDBLOCK) {
        std::cout << "Another process is running in the background" << std::endl;
        exit(EXIT_SUCCESS);
    }
}

void unlock_process(int *fd) {
    int res_flock;
    res_flock = flock(*fd, LOCK_UN);
    if(res_flock == -1){
        std::cout << "Error " << errno << " occured in flock function" << std::endl;
    }
    remove("/var/run/lock/file_lock_another_instance");
}

void pause_program(){
    char a;
    std::cout << "Input any key and press enter to continue ";
    std::cin >> a;
}

int main(int argc, char **argv) {
	if(strcmp(strrchr(*argv,'/') + 1,NAME_PROCESS) !=0 ){
        std::cout << "Correct name of program should be: "<< NAME_PROCESS << std::endl;
        return 1;
    }
    int fd;
    char *res_getenv = NULL;
    res_getenv = getenv("ENVP_VAR");
    if (res_getenv != NULL && strcmp(res_getenv, "NEW") == 0) {
        std::cout << "Value of ENVP_VAR is NEW." << " Result pkill program: " << std::endl;
        system("pkill -SIGTERM prog");
        } else {
        std::cout << "Value of ENVP_VAR isn't NEW" << std::endl;
        lock_process(&fd);
        pause_program();
        unlock_process(&fd);
    }
    close(fd);
    return 0;
}