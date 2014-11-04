/* 
 * File:   Logger.cpp
 * Author: knoppix
 * 
 * Created on October 4, 2014, 7:24 PM
 */

#include "Logger.h"

#include <iostream>

Logger* Logger::_logger = NULL;

void Logger::log(const std::string& msg, unsigned short int logLevel) {
	if (logLevel >= _logger->_logLevel) {
		time_t timer;
		time(&timer);
		_logger->_currentTime = localtime(&timer);
		std::ostringstream oss;
		oss << std::setfill('0')
			<< std::setw(2) << _logger->_currentTime->tm_mday << "/"
			<< std::setw(2) << _logger->_currentTime->tm_mon + 1 << "/"
			<< _logger->_currentTime->tm_year + 1900 << " "
			<< std::setw(2) << _logger->_currentTime->tm_hour << ":"
			<< std::setw(2) << _logger->_currentTime->tm_min << ":"
			<< std::setw(2) << _logger->_currentTime->tm_sec << " "
			<< msg << "\n";
		_logger->_output.tomarLock();
		_logger->_output.escribir((const void*)oss.str().c_str(), (long int)oss.str().size());
		// Comentar esto cuando haga falta
		std::cout << oss.str() << std::flush;
		_logger->_output.liberarLock();
	}
}

std::string Logger::prependCaller(const std::string& msg, const std::string& caller) {
	return "[ " + caller + " ] " + msg;
}

void Logger::log(const std::string& msg, unsigned short int logLevel, const std::string& caller) {
	log( prependCaller(msg, caller), LOG_DEBUG );
}

void Logger::debug(const std::string& msg) {
	log( msg, LOG_DEBUG );
}

void Logger::debug(const std::string& msg, const std::string& caller) {
	log( msg, LOG_NOTICE, caller );
}

void Logger::notice(const std::string& msg) {
	log( msg, LOG_NOTICE );
}

void Logger::notice(const std::string& msg, const std::string& caller) {
	log( msg, LOG_WARNING, caller );
}

void Logger::warn(const std::string& msg) {
	log( msg, LOG_WARNING );
}

void Logger::warn(const std::string& msg, const std::string& caller) {
	log( msg, LOG_WARNING, caller );
}

void Logger::error(const std::string& msg) {
	log( msg, LOG_CRITICAL );
}

void Logger::error(const std::string& msg, const std::string& caller) {
	log( msg, LOG_CRITICAL, caller );
}

