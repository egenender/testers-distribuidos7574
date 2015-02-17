/* 
 * File:   Timeout.h
 * Author: ferno
 *
 * Esta clase se encargara de llevar un proceso timeout y de lanzar determinada señal a 
 * determinado proceso una vez cumplido el tiempo determinado
 */

#ifndef TIMEOUT_H
#define	TIMEOUT_H

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <cstdlib>
#include <unistd.h>

class Timeout {
public:
    Timeout();
    Timeout(const Timeout& orig);
    virtual ~Timeout();
    
    void runTimeout(int timeInSecs, pid_t processToKill, int signal);
    void killTimeout();

private:
    pid_t timeoutPid;
};

#endif	/* TIMEOUT_H */

