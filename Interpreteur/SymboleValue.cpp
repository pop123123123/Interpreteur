#include "SymboleValue.h"
#include "Exceptions.h"
#include <stdlib.h>

SymboleValue::SymboleValue(const Symbole & s) :
Symbole(s.getChaine()) {
    if (s == "<ENTIER>") {
        m_valeur = new Entier(atoi(s.getChaine().c_str())); // c_str convertit une string en char*
        m_defini = true;
    } else if (s == "<CHAINE>") {
        m_defini = true;
        m_valeur = new Entier(1);
    } else {
        m_defini = false;
    }
}

Type * SymboleValue::executer() {
    if (!m_defini) throw IndefiniException(); // on lève une exception si valeur non définie
    if (Symbole::operator==("<CHAINE>"))
        return new Chaine(Symbole::getChaine());
    else
        return m_valeur;
}

ostream & operator<<(ostream & cout, const SymboleValue & symbole) {
    cout << (Symbole) symbole << "\t\t - Valeur=";
    if (symbole.m_defini) cout << symbole.m_valeur << " " << symbole.getChaine();
    else cout << "indefinie ";
    return cout;
}

void SymboleValue::traduitEnPython(ostream& cout, unsigned int indentation) const {
    cout << string(indentation * 3, ' ');
    cout << Symbole::getChaine();
}
