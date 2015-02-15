/* 
 * File:   terminadorEquipoEspecial.cpp
 * Author: ferno
 *
 * Created on February 12, 2015, 7:56 PM
 */

#include <cstdlib>
#include <sys/msg.h>
#include <cerrno>
#include <cstring>
#include <sstream>
#include <fstream>

#include "common/common.h"
#include "logger/Logger.h"

/*
 * 
 */
int main(int argc, char** argv) {
    
    Logger::initialize(logFileName.c_str(), Logger::LOG_DEBUG);
    
    // Creo archivo de IPCs
    std::fstream ipcFile(ipcFileName.c_str(), std::ios::out);
    if (ipcFile.bad() || ipcFile.fail()) {
	std::string err = std::string("Error creando el archivo de IPCs. Error: ") + std::string(strerror(errno));
        Logger::error(err.c_str(), __FILE__);
        throw err;
    }
    ipcFile.close();
    
    key_t key = ftok(ipcFileName.c_str(), MSGQUEUE_DESPACHADOR);
    int idQueue = msgget(key, 0660 | IPC_CREAT | IPC_EXCL);
    if (idQueue == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << MSGQUEUE_DESPACHADOR << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    msgctl(idQueue ,IPC_RMID, NULL);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_EQUIPO_ESPECIAL);
    idQueue = msgget(key, 0660 | IPC_CREAT | IPC_EXCL);
    if (idQueue == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << MSGQUEUE_ENVIO_EQUIPO_ESPECIAL << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    msgctl(idQueue ,IPC_RMID, NULL);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCIONES_EQUIPO_ESPECIAL);
    idQueue = msgget(key, 0660 | IPC_CREAT | IPC_EXCL);
    if (idQueue == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << MSGQUEUE_RECEPCIONES_EQUIPO_ESPECIAL << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    msgctl(idQueue ,IPC_RMID, NULL);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION);
    idQueue = msgget(key, 0660 | IPC_CREAT | IPC_EXCL);
    if (idQueue == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_ASIGNACION << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    msgctl(idQueue ,IPC_RMID, NULL);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_ASIGNACION);
    idQueue = msgget(key, 0660 | IPC_CREAT | IPC_EXCL);
    if (idQueue == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_ASIGNACION << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    msgctl(idQueue ,IPC_RMID, NULL);
    
    key = ftok(ipcFileName.c_str(), MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_GENERAL);
    idQueue = msgget(key, 0660 | IPC_CREAT | IPC_EXCL);
    if (idQueue == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << MSGQUEUE_RECEPCION_TESTERS_SHMEM_PLANILLA_GENERAL << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    msgctl(idQueue ,IPC_RMID, NULL);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_GENERAL);
    idQueue = msgget(key, 0660 | IPC_CREAT | IPC_EXCL);
    if (idQueue == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << MSGQUEUE_ENVIO_TESTERS_SHMEM_PLANILLA_GENERAL << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    msgctl(idQueue ,IPC_RMID, NULL);

    key = ftok(ipcFileName.c_str(), MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS);
    idQueue = msgget(key, 0660 | IPC_CREAT | IPC_EXCL);
    if (idQueue == -1) {
        std::stringstream ss; ss << "No se pudo crear cola de id " << MSGQUEUE_REQ_TESTERS_SHMEM_PLANILLAS << ". Errno: " << strerror(errno);
        Logger::error(ss.str(), __FILE__);
    }
    msgctl(idQueue ,IPC_RMID, NULL);

    return 0;
}