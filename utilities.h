#ifndef UTILITIES_H
#define UTILITIES_H

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>

namespace Utilities 
{
    /**
     * Object for handling CSV I/O
     */
    class CSVFile
    {
        public:
            /** fstream object for writing files */
            std::ofstream& ofstream;
            /** Name (e.g. output.csv) of CSV file */
            std::string name;
            /** Headers for CSV columns */
            std::vector<std::string> headers;
            /** Buffer for staging column values */
            std::vector<std::string> buffer;

            /**
             * CSVFile object constructor
             * @param new_ofstream reference of an existing ofstream object
             * @param new_name name of new CSV file (e.g. output.csv)
             * @param new_headers headers for new CSV file columns
             * @return none
             */
            CSVFile(std::ofstream& new_ofstream, std::string new_name, 
                    std::vector<std::string> new_headers);
            /**
             * CSVFile object destructor
             * @return none
             */
            virtual ~CSVFile();
            /**
             * Clone CSVFile object and copy the existing CSV file to a new file
             * @param new_name name of new CSV file (e.g. output.csv)
             * @return new CSVFile object
             */
            CSVFile clone(std::string new_name);
            /**
             * Push a new column entry to buffer
             * @tparam Type type of column entry
             * @param value new column entry
             * @return none
             */
            template<typename Type>
            void pushCol(Type value);
            /**
             * Write buffer to CSV file
             * @param append Toggle "append" mode (optional)
             * @return none
             */
            void writeRow(bool append = true);
    };
    typedef std::vector<CSVFile> CSVFiles;

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
