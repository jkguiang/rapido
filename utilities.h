#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <string>
#include <map>

namespace Utilities 
{
    class Dynamic
    {
    /**
     * "Dynamic" object that serves as a base for templated objects
     */
    public:
        virtual ~Dynamic();
    };

    template<typename Type>
    class Variable : public Dynamic
    {
    /**
     * "Dynamic" variable
     */
    private:
        Type value;
    public:
        Variable(Type new_value);
        virtual ~Variable();
        Type getValue();
        Type& getReference();
        Type* getPointer();
        void setValue(Type new_value);
    };

    class Variables
    {
    /**
     * A group of "dynamic" variables
     */
    private:
        std::map<std::string, Dynamic*> variables;
        template<typename Type>
        Variable<Type>* getVariable(std::string name);
    public:
        Variables();
        virtual ~Variables();
        template<typename Type>
        void add(std::string new_name);
        template<typename Type>
        void add(std::string new_name, Type new_value);
        template<typename Type>
        Type get(std::string name);
        template<typename Type>
        Type set(std::string name, Type new_value);
    };
}

#include "utilities.icc"

#endif
