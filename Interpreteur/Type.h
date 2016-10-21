/* 
 * File:   Type.h
 * Author: louis
 *
 * Created on October 21, 2016, 11:54 AM
 */

#ifndef TYPE_H
#define	TYPE_H

#include <string>

#include "Exceptions.h"

using namespace std;

class Type {
public:
    Type();
    virtual ~Type();
    virtual inline Type & operator +(const Type & e0) const = 0 ;
    virtual inline Type & operator -(const Type & e0) const = 0 ;
    virtual inline Type & operator *(const Type & e0) const = 0 ;
    virtual inline Type & operator /(const Type & e0) const = 0 ;
    virtual inline bool operator <(const Type & e0) const = 0 ;
    virtual inline bool operator >(const Type & e0) const = 0 ;
    virtual inline bool operator <=(const Type & e0) const = 0 ;
    virtual inline bool operator >=(const Type & e0) const = 0 ;
    virtual inline bool operator ==(const Type & e0) const = 0 ;
    virtual inline bool operator !=(const Type & e0) const = 0 ;
private:

};

class Entier : public Type {
public:
    Entier( int i );
    virtual ~Entier();
    inline int getValue() const { return this->m_entier; }
    
    inline Type & operator +(const Type & e0) const { try{this->getValue() + dynamic_cast<const Entier*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline Type & operator -(const Type & e0) const { try{this->getValue() - dynamic_cast<const Entier*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline Type & operator *(const Type & e0) const { try{this->getValue() * dynamic_cast<const Entier*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline Type & operator /(const Type & e0) const { try{this->getValue() / dynamic_cast<const Entier*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline bool operator <(const Type & e0) const { try{this->getValue() < dynamic_cast<const Entier*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline bool operator >(const Type & e0) const { try{this->getValue() > dynamic_cast<const Entier*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline bool operator <=(const Type & e0) const { try{this->getValue() <= dynamic_cast<const Entier*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline bool operator >=(const Type & e0) const { try{this->getValue() >= dynamic_cast<const Entier*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline bool operator ==(const Type & e0) const { try{this->getValue() == dynamic_cast<const Entier*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline bool operator !=(const Type & e0) const { try{this->getValue() != dynamic_cast<const Entier*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
private:
    int m_entier;
};


class Chaine : public Type {
public:
    Chaine(string s);
    virtual ~Chaine();
    inline string getValue() const { return this->m_chaine; }
    inline Type & operator +(const Type & e0) const { try{this->getValue() + dynamic_cast<const Chaine*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline bool operator <(const Type & e0) const { try{this->getValue() < dynamic_cast<const Chaine*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline bool operator >(const Type & e0) const { try{this->getValue() > dynamic_cast<const Chaine*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline bool operator <=(const Type & e0) const { try{this->getValue() <= dynamic_cast<const Chaine*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline bool operator >=(const Type & e0) const { try{this->getValue() >= dynamic_cast<const Chaine*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline bool operator ==(const Type & e0) const { try{this->getValue() == dynamic_cast<const Chaine*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline bool operator !=(const Type & e0) const { try{this->getValue() != dynamic_cast<const Chaine*>(&e0)->getValue();}catch(exception e){throw ExecutionException();} }
    inline Type & operator -(const Type & e0) const { throw ExecutionException(); }
    inline Type & operator *(const Type & e0) const { throw ExecutionException(); }
    inline Type & operator /(const Type & e0) const { throw ExecutionException(); }
private:
    string m_chaine;
};

#endif	/* TYPE_H */

