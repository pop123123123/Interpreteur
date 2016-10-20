#include "Procedure.h"
#include "SymboleValue.h"
#include "Interpreteur.h"
#include <string>

Procedure::Procedure(string& name, vector<SymboleValue*>* arguments, Noeud* sequinst, TableSymboles* symboles):m_args(arguments),m_sequence(sequinst),m_symboles(symboles){
    this->name = name;
}

Procedure::~Procedure()
{
    delete m_args;
    delete m_sequence;
    delete m_symboles;
}

int Procedure::execute(vector<Noeud*>* arguments) {
    if (arguments->size()==this->m_args->size()){
        for(int i = 0 ; i < arguments->size() ; i++){
            (*this->m_args)[i]->setValeur((*arguments)[i]->executer());
        }
        this->m_sequence->executer();
        
    }else{
        throw ProcedureException("Mauvais argument(s)");        
    }
}
