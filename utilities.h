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
        /**
         * Dynamic object destructor
         * @return none
         */
        virtual ~Dynamic();
    };

    template<typename Type>
    class Variable : public Dynamic
    {
    /**
     * "Dynamic" variable
     */
    protected:
        Type value;
    public:
        /**
         * Variable object constructor
         * @tparam Type type of variable
         * @param new_value value of new variable
         * @return none
         */
        Variable(Type new_value);
        /**
         * Variable object destructor
         * @tparam Type type of variable
         * @return none
         */
        virtual ~Variable();
        /**
         * Get current value
         * @tparam Type type of variable
         * @return value of current leaf
         */
        Type getValue();
        /**
         * Get current value by reference
         * @tparam Type type of variable
         * @return value of current leaf
         */
        Type& getReference();
        /**
         * Set value of variable
         * @tparam Type type of variable
         * @param new_value new value
         * @return none
         */
        void setValue(Type new_value);
    };

    class Variables
    {
    /**
     * A group of "dynamic" variables
     */
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
