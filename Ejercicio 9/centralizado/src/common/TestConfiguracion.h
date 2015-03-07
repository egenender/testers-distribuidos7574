#pragma once

#include <vector>

class TestConfiguracion{
    std::vector<int>    m_Variables;
public:
    TestConfiguracion( int tipoDispositivo );
    int getCantVariables() const;
    int getVariable( int idVariable ) const;
};
