/* 
 * File:   LockFile.h
 * Author: knoppix
 *
 * Created on October 4, 2014, 7:26 PM
 */

#ifndef LOCKFILE_H
#define	LOCKFILE_H

# include <unistd.h>
# include <fcntl.h>
# include <string>

class LockFile {
private :

	struct flock fl;
	int fd;
	std :: string nombre;

public :

	LockFile(const std::string nombre);
	~LockFile();

	int tomarLock();
	int liberarLock();
	ssize_t escribir(const void* buffer, const ssize_t buffsize) const;
};

#endif	/* LOCKFILE_H */

