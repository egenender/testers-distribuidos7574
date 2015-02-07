#pragma once

#include <string>
#include <map>

class Configuracion{
      std::map<std::string, std::string>	m_Parametros;
      std::string ObtenerValorParametro( const std::string& nombreParam ) const;
public:
      static const std::string UBICACION_POR_DEFECTO_ARCHIVO;
      Configuracion(){}
      bool LeerDeArchivo( std::string path = UBICACION_POR_DEFECTO_ARCHIVO );
      int ObtenerParametroEntero( const std::string& nombreParam ) const;
      std::string ObtenerParametroString( const std::string& nombreParam ) const;
};

