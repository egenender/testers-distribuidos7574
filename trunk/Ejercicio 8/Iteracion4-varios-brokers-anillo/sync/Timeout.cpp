/* 
 * File:   Timeout.cpp
 * Author: ferno
 * 
 * 
 */

#include "Timeout.h"

Timeout::Timeout() {
}

Timeout::Timeout(const Timeout& orig) {
}

Timeout::~Timeout() {
}

void Timeout::runTimeout(int timeInSecs, pid_t processToKill, int signal) {
    this->timeoutPid = fork();
    if(this->timeoutPid == 0) {
        sleep(timeInSecs);
        kill(processToKill, signal);
        exit(0);
    }
}

void Timeout::killTimeout() {
    kill(this->timeoutPid, SIGQUIT);
    int status;
    waitpid(this->timeoutPid, &status, 0);
}