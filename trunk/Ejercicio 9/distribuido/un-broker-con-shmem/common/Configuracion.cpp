#include "Configuracion.h"
#include <sstream>
#include <fstream>
#include <vector>

using namespace std;

const string Configuracion::UBICACION_POR_DEFECTO_ARCHIVO = "config.ini";

namespace StringUtils{

    inline std::vector<std::string>& Split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
              elems.push_back(item);
        }
        return elems;
    }


    inline std::vector<std::string> Split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        Split(s, delim, elems);
        return elems;
    }

    template<typename T>
    T Parse( const std::string& s ) {
        std::istringstream stream( s );
        T t;
        stream >> t;
        return t;
    }

}

bool Configuracion::LeerDeArchivo( std::string path ){
      ifstream archivoConfig( path.c_str() );
      if( !archivoConfig.is_open() )
            return false;
      while( !archivoConfig.eof() ){
            string linea;
            getline( archivoConfig, linea );
            vector<string> claveYValor = StringUtils::Split( linea, '=' );
            if( claveYValor.size() != 2 )
                  continue;
            m_Parametros[ claveYValor[0] ] = claveYValor[1];
      }
      return true;
}

string Configuracion::ObtenerParametroString( const string& nombreParam ) const{
      map<string,string>::const_iterator it = m_Parametros.find( nombreParam );
      if( it == m_Parametros.end() )
            return "";
      return it->second;
}

int Configuracion::ObtenerParametroEntero( const string& nombreParam ) const{
      return StringUtils::Parse<int>( ObtenerParametroString( nombreParam ) );
}

