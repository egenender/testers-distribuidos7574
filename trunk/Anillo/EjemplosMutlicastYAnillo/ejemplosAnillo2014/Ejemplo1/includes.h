#ifndef INCLUDES_H
#define	INCLUDES_H

#include <stddef.h>

#include "net-idManagerProtocol.h"

#define ROBOT_AMOUNT 4
#define PLATFORM_CAPACITY 10
#define DISPOSITIVE_TYPES 2
#define NET_MESSAGE_SIZE 8192

#include <unistd.h>

namespace IPC {

	static const char * path = "/home/knoppix/Maestri/Ejercicio2/E2V4";

	enum class QueueIdentifier
		: int {
			INVALID_IDENTIFIER = 0,
		ACTIVADO_BROKER = 1,
		ACTIVADO_FROM_DISP_TO_WRAPPER = 2,
		ACTIVADO_FROM_UNRWAPPER_TO_INTERFACE = 3,
		ARMADO_BROKER = 4,
		ARMADO_FROM_DISP_TO_WRAPPER = 5,
		ARMADO_FROM_UNWRAPPER_TO_INTERFACE = 6,
		DISPOSITIVOS_BROKER = 7,
		DISPOSITIVOS_FROM_PLATAFORMA_TO_WRAPPER = 8,
		DISPOSITIVOS_FROM_UNWRAPPER_TO_DISP = 9,
		FROM_EXCLUSION_TO_INTERFACE = 10,
		FROM_INTERFACE_TO_EXCLUSION = 11,
		FROM_INTERFACE_TO_PLATAFORMA = 12,
		FROM_NET_TO_UNWRAPPER = 13,
		FROM_PLATAFORMA_TO_INTERFACE = 14,
		FROM_WRAPPER_TO_NET = 15,
		ID_MANAGER_BROKER = 16,
		ID_MANAGER_FROM_INTERFACE_TO_WRAPPER = 17,
		ID_MANAGER_FROM_UNWRAPPER_TO_INTERFACE = 18,
		PLATAFORMA_BROKER = 19,
		PLATAFORMA_FROM_UNWRAPPER = 20,
		PLATAFORMA_TO_WRAPPER = 21,
		SALIDA_BROKER = 22,
		SALIDA_FROM_INTERFACE_TO_WRAPPER = 23,
		SALIDA_FROM_UNWRAPPER_TO_INTERFACE = 24,
		TO_BROKER = 25,
		TO_BROKER_FROM_RECEIVER = 26,
		TO_SENDER_FROM_BROKER = 27,
		TO_BROKER_FROM_BROKER = 28

	};

	enum class SemaphoreIdentifier
		: int {
			INVALID_IDENTIFIER = 0,
		MUTEX_EXCLUSION,
		MUTEX_PLATAFORMA,
		SEM_EXCLUSION,
		SEM_ESPERA,
		MUTEX_ID_MANAGER,
		MUTEX_BROKER_SYNC,
		TENGO_TOKEN,
		MUTEX_TOKEN,
		DEVUELVO_TOKEN
	};

	enum class SharedMemoryIdentifier
		: int {
			INVALID_IDENTIFIER = 0,
		EXCLUSION,
		PLATAFORMA,
		BROKER_PLAT,
		BROKER_TOKEN
	};

	enum class MessageTypes
		: long {
			ANY = 0,
		ROBOT_1_ARMADO = 1,
		ROBOT_1_SACADO = 2,
		ROBOT_2 = 3,
		DISPOSITIVO = 4,
		PLATAFORMA = 5,
		BROKER = 6,
		UNWRAPPER = 7,
		FIRST_AVAILABLE_MTYPE = 8
	};
}

struct dispositivo {
		long id;
		long tipo;
};

namespace ColaExclusion {
	enum operation {
		ESPERAR_SI_SACANDO,
		AVISAR_SI_ESPERANDO_PARA_SACAR,
		ESPERAR_SI_ARMANDO,
		AVISAR_SI_ESPERANDO_PARA_ARMAR
	};

	typedef struct {
			long mtype;
			unsigned number;
			enum operation operation;
	} message;

	enum RobotStatus {
		BUSY, IDLE, WAITING
	};

	struct shared {
			enum RobotStatus armando[ROBOT_AMOUNT];
			enum RobotStatus sacando[ROBOT_AMOUNT];
	};
}

namespace ColaPlataforma {
	enum operation {
		RESERVAR, COLOCAR_DISPOSITIVO, TOMAR_DISPOSITIVO
	};

	typedef struct {
			long mtype;
			enum operation operation;
			struct dispositivo dispositivo;
			union {
					unsigned lugar;
					unsigned numero;
			};
	} message;

	enum class SlotStatus {
		RESERVED, OCCUPIED, FREE
	};

	enum class RobotStatus {
		WAITING, NOT_WAITING
	};

	typedef struct {

			struct {
					struct dispositivo dispositivo;
					SlotStatus status;
			} slot[PLATFORM_CAPACITY];
			RobotStatus robotStatus[ROBOT_AMOUNT];
			unsigned amount;
	} shared;

	typedef struct {
			long mtype;
			shared shm;
	} syncMessage;
}

namespace ColaArmado {

	typedef struct {
			long mtype;
			struct dispositivo dispositivo;
	} message;
}

namespace ColaActivado {

	typedef struct {
			long mtype;
			struct dispositivo dispositivo;
	} message;
}

namespace ColaSalida {

	typedef struct {
			long mtype;
			struct dispositivo dispositivo;
	} message;
}

// TODO: actualizar diagrama renombrar a cola De Dispositivo
namespace ColaDispositivo {
	typedef struct {
			long mtype;
	} message;
}

namespace Broker {
	enum class Request {
		DAME_DISPOSITIVO_PARA_ARMAR,
		AVISAME_SI_ESTOY_ARMADO,
		DAME_DISPOSITIVO_PARA_SACAR_DE_PLATAFORMA,
		DAME_DISPOSITIVO_PARA_SACAR_DE_CINTA_SALIDA,
		NEW_ID,
		DAME_SHM
	};

	struct message {
			long mtype;
			union {
					long type;
					long connNumber;
			};
			Request request;
	};
}

namespace Net {
	enum class interfaceMessageType {
		ACTIVADO,
		ARMADO,
		DISPOSITIVO,
		PLATAFORMA_SYNC,
		SALIDA,
		BROKER_REQUEST,
		ID_REQUEST,
		ID_REPLY
	};
	struct interfaceMessage {
			long destination;
			interfaceMessageType type;
			union {

					ColaActivado::message activado;
					ColaArmado::message armado;
					ColaDispositivo::message dispositivo;
					ColaSalida::message salida;
					Broker::message broker_request;
					ColaPlataforma::syncMessage syncMessage;
					IdManager::messageRequest id_request;
					IdManager::messageReply id_reply;
			};
	};

	struct message {
			size_t size;
			char message[NET_MESSAGE_SIZE];
	};
}

namespace Broker {
	struct token {
			ColaPlataforma::shared shm;
	};
	struct tokenShm {
			unsigned necesitoToken;
			struct token token;
	};

	struct outgoingMessage {
			long mtype;
			Net::interfaceMessage interfaceMessage;
	};
}

#endif	/* INCLUDES_H */
