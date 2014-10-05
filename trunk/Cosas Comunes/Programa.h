#ifndef PROGRAMA_H_
#define PROGRAMA_H_

class Programa{
	private:
		static const int MAXIMO_TIEMPO = 10;
		static const int MINIMO_TIEMPO = 0;
	public:
		typedef struct program{
			//Seguramente haya que agregar el mtype en algun momento
			unsigned int tiempo;
			int prob_error; //0 a 100%
		}program_t;
		
		program_t getPrograma() const;
};

#endif
