/* 
 * File:   Type.cpp
 * Author: louis
 * 
 * Created on October 21, 2016, 11:54 AM
 */

#include "Type.h"

Entier::Entier(int i) : Type::Type(),m_entier(i){
}

Chaine::Chaine(string s) : Type::Type(),m_chaine(s) {

}
