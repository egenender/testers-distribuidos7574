#pragma once

#include <vector>

class TestConfiguracion{
    std::vector<int>    m_Variables;
public:
    TestConfiguracion( int tipoDispositivo );
    int CantVariables() const;
    int getVariable( int idVariable ) const;
};
