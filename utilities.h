#ifndef UTILITIES_H
#define UTILITIES_H

#include <iostream>
#include <string>
#include <map>

namespace Utilities 
{
    /**
     * "Dynamic" object that serves as a base for templated objects
     */
    class Dynamic
    {
    public:
        /**
         * Dynamic object destructor
         * @return none
         */
        virtual ~Dynamic();
    };

    /**
     * "Dynamic" variable
     * @tparam Type type of variable
     */
    template<typename Type>
    class Variable : public Dynamic
    {
    protected:
        /** Value of this variable object */
        Type value;
    public:
        /**
         * Variable object constructor
         * @param new_value value of new variable
         * @return none
         */
        Variable(Type new_value);
        /**
         * Variable object destructor
         * @return none
         */
        virtual ~Variable();
        /**
         * Get current value
         * @return value of current leaf
         */
        Type getValue();
        /**
         * Get current value by reference
         * @return value of current leaf
         */
        Type& getReference();
        /**
         * Set value of variable
         * @param new_value new value
         * @return none
         */
        void setValue(Type new_value);
    };

    /**
     * A group of "dynamic" variables
     */
    class Variables
    {
    protected:
        /** Map of dynamic variables */
        std::map<std::string, Dynamic*> variables;
        /**
         * (PROTECTED) Retrieve variable object from map if it exists
         * @tparam Type type of variable
         * @param name name of variable
         * @return none
         */
        template<typename Type>
        Variable<Type>* getVariable(std::string name);
    public:
        /**
         * Variables object constructor
         * @return none
         */
        Variables();
        /**
         * Variables object destructor
         * @return none
         */
        virtual ~Variables();
        /**
         * Add blank variable to map
         * @tparam Type type of new variable
         * @param new_name name of new variable
         * @return none
         */
        template<typename Type>
        void newVar(std::string new_name);
        /**
         * Add new variable to map
         * @tparam Type type of new variable
         * @param new_name name of new variable
         * @param new_value value of new variable
         * @return none
         */
        template<typename Type>
        void newVar(std::string new_name, Type new_value);
        /**
         * Get value of a variable in map if it exists
         * @tparam Type type of variable
         * @param name name of variable
         * @return none
         */
        template<typename Type>
        Type getVar(std::string name);
        /**
         * Get variable in map by reference if it exists
         * @tparam Type type of variable
         * @param name name of variable
         * @return none
         */
        template<typename Type>
        Type& getRef(std::string name);
        /**
         * Set value of a variable in map if it exists
         * @tparam Type type of variable
         * @param name name of variable
         * @param new_value new value for variable
         * @return none
         */
        template<typename Type>
        Type setVar(std::string name, Type new_value);
    };
}

#include "utilities.icc"

#endif
