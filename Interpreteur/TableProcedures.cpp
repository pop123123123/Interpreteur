#include "TableProcedures.h"

TableProcedures::TableProcedures() : m_procedures() {
}

TableProcedures::TableProcedures(const TableProcedures& orig) {
}

TableProcedures::~TableProcedures() {
}

void TableProcedures::addProcedure(string name, vector<SymboleValue*>* arguments, Noeud* sequinst, TableSymboles* symboles) {
    this->m_procedures[name] = new Procedure(name,arguments,sequinst,symboles);
}
