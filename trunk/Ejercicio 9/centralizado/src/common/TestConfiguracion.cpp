#include "TestConfiguracion.h"

TestConfiguracion::TestConfiguracion( int tipoDispositivo ){
    //TODO <REF> Usar un enum y levantar cant variables para cada tipo
    //de config
    switch( tipoDispositivo ){
        case 0: m_Variables.push_back( 1 );
                m_Variables.push_back( 2 );
                break;
        case 1: m_Variables.push_back( 3 );
                m_Variables.push_back( 4 );
                m_Variables.push_back( 5 );
                break;
        case 2: m_Variables.push_back( 7 );
                m_Variables.push_back( 8 );
                m_Variables.push_back( 9 );
                m_Variables.push_back( 10 );
                break;
        default:
                m_Variables.push_back( 11 );
                break;
    }
}

int TestConfiguracion::getCantVariables() const{
    return m_Variables.size();
}

int TestConfiguracion::getVariable( int idVariable ) const{
    return m_Variables[idVariable];
}
