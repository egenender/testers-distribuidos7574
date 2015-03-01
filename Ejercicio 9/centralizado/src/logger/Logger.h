/* 
 * File:   Logger.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:24 PM
 */

#pragma once

#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "../sync/LockFile.h"

class Logger {

private:
    LockFile _output;
    unsigned short int _logLevel;
    struct tm* _currentTime;
    static Logger* _logger;
    static const int VT100_BLANCO;
    static const int VT100_ROJO;
    static const std::string INICIO_HEADER_COLOR_VT100;
    static const std::string FIN_HEADER_COLOR_VT100;
    static const std::string FOOTER_COLOR_VT100;

    Logger(const char* filename) : _output(filename), _logLevel(Logger::LOG_NOTICE), _currentTime(NULL) {};
    virtual ~Logger() {
        //delete _output;
    };

    static std::string prependCaller(const std::string& msg, const std::string& caller);

public:
    // TODO: ¿Algun level mas? Creo que son suficientes
    static const unsigned short int LOG_DEBUG = 1;
    static const unsigned short int LOG_NOTICE = 2;
    static const unsigned short int LOG_WARNING = 3;
    static const unsigned short int LOG_CRITICAL = 4;

    /*
     * filename: nombre del archivo a crear
     * logLevel: representa uno de los 4 niveles soportados por la clase
     * */
    static void initialize(const char* filename, unsigned short int logLevel) {
        // TODO: Ptr Error check
        if (!_logger) {
            _logger = new Logger(filename);
            _logger->_logLevel = logLevel;
        }
    };

    static void destroy() {
        if (_logger)
            delete _logger;
    }

    static void setLogLevel( unsigned short int logLevel) {
        _logger->_logLevel = logLevel;
    }

    /*
     * Loggea mensaje si y solo si el logLevel es mayor al setteado en la inicializacion
     * */
    static void log(const std::string& msg, unsigned short int logLevel);

    /*
     * Loggea mensaje si y solo si el logLevel es mayor al setteado en la inicializacion
     * Prefija el mensaje con el nombre de quien loguea
     * */
    static void log(const std::string& msg, unsigned short int logLevel, const std::string& caller);

    /*
     * Las siguientes funciones loguean el mensaje sólo si el logLevel seteado en la inicialización
     * es mayor a lo que indica el nombre de la función.
     * El segundo parámetro es prefijado ante el mensaje, si aparece
     * */
    static void debug(const std::string& msg);
    static void debug(const std::string& msg, const std::string& caller);
    static void notice(const std::string& msg);
    static void notice(const std::string& msg, const std::string& caller);
    static void warn(const std::string& msg);
    static void warn(const std::string& msg, const std::string& caller);
    static void error(const std::string& msg);
    static void error(const std::string& msg, const std::string& caller);

};

