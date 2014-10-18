#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/ArriboDeResultados.o \
	${OBJECTDIR}/Dispositivo.o \
	${OBJECTDIR}/Planilla.o \
	${OBJECTDIR}/Tecnico.o \
	${OBJECTDIR}/TesterA.o \
	${OBJECTDIR}/TesterB.o \
	${OBJECTDIR}/common/AtendedorDispositivos.o \
	${OBJECTDIR}/common/AtendedorTesters.o \
	${OBJECTDIR}/common/DespachadorTecnicos.o \
	${OBJECTDIR}/common/DespachadorTesters.o \
	${OBJECTDIR}/common/Programa.o \
	${OBJECTDIR}/common/Resultado.o \
	${OBJECTDIR}/common/iPlanillaTesterA.o \
	${OBJECTDIR}/common/iPlanillaTesterB.o \
	${OBJECTDIR}/iniciador.o \
	${OBJECTDIR}/ipc/Semaphore.o \
	${OBJECTDIR}/logger/Logger.o \
	${OBJECTDIR}/sync/LockFile.o \
	${OBJECTDIR}/terminador.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ejercicio2testers

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ejercicio2testers: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ejercicio2testers ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/ArriboDeResultados.o: ArriboDeResultados.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ArriboDeResultados.o ArriboDeResultados.cpp

${OBJECTDIR}/Dispositivo.o: Dispositivo.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Dispositivo.o Dispositivo.cpp

${OBJECTDIR}/Planilla.o: Planilla.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Planilla.o Planilla.cpp

${OBJECTDIR}/Tecnico.o: Tecnico.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Tecnico.o Tecnico.cpp

${OBJECTDIR}/TesterA.o: TesterA.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TesterA.o TesterA.cpp

${OBJECTDIR}/TesterB.o: TesterB.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/TesterB.o TesterB.cpp

${OBJECTDIR}/common/AtendedorDispositivos.o: common/AtendedorDispositivos.cpp 
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/AtendedorDispositivos.o common/AtendedorDispositivos.cpp

${OBJECTDIR}/common/AtendedorTesters.o: common/AtendedorTesters.cpp 
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/AtendedorTesters.o common/AtendedorTesters.cpp

${OBJECTDIR}/common/DespachadorTecnicos.o: common/DespachadorTecnicos.cpp 
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/DespachadorTecnicos.o common/DespachadorTecnicos.cpp

${OBJECTDIR}/common/DespachadorTesters.o: common/DespachadorTesters.cpp 
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/DespachadorTesters.o common/DespachadorTesters.cpp

${OBJECTDIR}/common/Programa.o: common/Programa.cpp 
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/Programa.o common/Programa.cpp

${OBJECTDIR}/common/Resultado.o: common/Resultado.cpp 
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/Resultado.o common/Resultado.cpp

${OBJECTDIR}/common/iPlanillaTesterA.o: common/iPlanillaTesterA.cpp 
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/iPlanillaTesterA.o common/iPlanillaTesterA.cpp

${OBJECTDIR}/common/iPlanillaTesterB.o: common/iPlanillaTesterB.cpp 
	${MKDIR} -p ${OBJECTDIR}/common
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/common/iPlanillaTesterB.o common/iPlanillaTesterB.cpp

${OBJECTDIR}/iniciador.o: iniciador.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/iniciador.o iniciador.cpp

${OBJECTDIR}/ipc/Semaphore.o: ipc/Semaphore.cpp 
	${MKDIR} -p ${OBJECTDIR}/ipc
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ipc/Semaphore.o ipc/Semaphore.cpp

${OBJECTDIR}/logger/Logger.o: logger/Logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/logger
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/logger/Logger.o logger/Logger.cpp

${OBJECTDIR}/sync/LockFile.o: sync/LockFile.cpp 
	${MKDIR} -p ${OBJECTDIR}/sync
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/sync/LockFile.o sync/LockFile.cpp

${OBJECTDIR}/terminador.o: terminador.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/terminador.o terminador.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/ejercicio2testers

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
