#include "Procedure.h"

Procedure::Procedure(string& name, vector<SymboleValue*>* arguments, Noeud* sequinst, TableSymboles* symboles):m_args(arguments),m_sequence(sequinst),m_symboles(symboles){
    this->name = name;
}

Procedure::~Procedure()
{
    delete m_args;
    delete m_sequence;
    delete m_symboles;
}

