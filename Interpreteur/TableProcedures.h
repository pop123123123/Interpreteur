#ifndef TABLEPROCEDURES_H
#define	TABLEPROCEDURES_H

#include "Procedure.h"
#include <vector>

using namespace std;

class TableProcedures {
public:
    TableProcedures();
    TableProcedures(const TableProcedures& orig);
    virtual ~TableProcedures();
private:
    vector<Procedure> m_procedures;
};

#endif	/* TABLEPROCEDURES_H */

