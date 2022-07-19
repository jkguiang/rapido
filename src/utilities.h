#ifndef UTILITIES_H
#define UTILITIES_H

#include <fstream>
#include <iostream>
#include <functional>
#include <vector>
#include <string>
#include <map>
#include <cmath>

namespace Utilities 
{
    /**
     * Object for computing variance of a data set without having to store every data 
     * point for a second pass
     *
     * Stolen from D. Knuth, Art of Computer Programming, Vol 2, page 232, 3rd edition
     */
    class RunningStat
    {
    private:
        /** Number of values pushed */
        int n_values;
        /** Sum of values pushed */
        float summed_values;
        /** Max of values pushed */
        float max_value;
        /** Min of values pushed */
        float min_value;
        /** Current value of mean */
        float new_M;
        /** Previous value of mean */
        float old_M;
        /** Current value of S (proportional to variance) */
        float new_S;
        /** Previous value of S (proportional to variance) */
        float old_S;
    public:
        /**
         * RunningStat object constructor
         * @return none
         */
        RunningStat();
        /**
         * Push a new value and update running statistics
         * @param value new value to push
         * @return none
         */
        void push(float value);
        /**
         * Get number of values pushed
         * @return number of values pushed
         */
        int size();
        /**
         * Get sum of values pushed
         * @return sum of values pushed
         */
        float sum();
        /**
         * Get max of values pushed
         * @return max of values pushed
         */
        float max();
        /**
         * Get min of values pushed
         * @return min of values pushed
         */
        float min();
        /**
         * Get approximate mean of values pushed
         * @return approximate mean of values pushed
         */
        float mean();
        /**
         * Get approximate variance of values pushed
         * @return approximate variance of values pushed
         */
        float variance();
        /**
         * Get approximate standard deviation of values pushed
         * @return approximate standard deviation of values pushed
         */
        float stddev();
    };

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
        /** Variable value */
        Type value;
        /** Variable reset value */
        Type reset_value;
    public:
        /**
         * Variable object default constructor
         * @return none
         */
        Variable();
        /**
         * Variable object overload constructor
         * @param new_reset_value reset value of new variable object
         * @return none
         */
        Variable(Type new_reset_value);
        /**
         * Variable object destructor
         * @return none
         */
        virtual ~Variable();
        /**
         * Get current variable value
         * @return current value of this variable object
         */
        Type getValue();
        /**
         * Get reference to variable value
         * @return reference to value for this variable object
         */
        Type& getReference();
        /**
         * Set variable value
         * @param new_value new value
         * @return none
         */
        void setValue(Type new_value);
        /**
         * Set variable reset value
         * @param new_reset_value new reset value (e.g. -999; default is the default type 
         *                        constructor)
         * @return none
         */
        void setResetValue(Type new_reset_value);
        /**
         * Reset the current variable value to the reset value
         * @return none
         */
        void resetValue();
    };

    /**
     * A group of "dynamic" variables
     */
    class Variables
    {
    protected:
        /** Map of Utilities::Variable objects */
        std::map<std::string, Dynamic*> variables;
        /** Map of Utilities::Variable::resetValue functions captured in lambdas*/
        std::map<std::string, std::function<void()>> resetters;
        /**
         * (PROTECTED) Retrieve variable object from map if it exists
         * @tparam Type type of variable
         * @param name name of variable
         * @return none
         */
        template<typename Type>
        Variable<Type>* getVar(std::string name);
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
         * Add new variable to map with reset value
         * @tparam Type type of variable
         * @param new_name name of variable
         * @param new_reset_value reset value of new variable
         * @return none
         */
        template<typename Type>
        void newVar(std::string new_name, Type new_reset_value);
        /**
         * Get variable value in map if it exists
         * @tparam Type type of variable
         * @param name name of variable
         * @return none
         */
        template<typename Type>
        Type getVal(std::string name);
        /**
         * Get variable value in map by reference if it exists
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
        void setVal(std::string name, Type new_value);
        /**
         * Set value of a variable in map to its reset value if it exists
         * @tparam Type type of variable
         * @param name name of variable
         * @return none
         */
        template<typename Type>
        void resetVal(std::string name);
        /**
         * Set value of each variable in map to its respective reset value.
         *
         * Uses a map of "resetters" because Utilities::Variable<Type>::resetValue() 
         * cannot be called across an arbitrary number of such objects, due to the 
         * fact that the value of Type for each object would need to be supplied. The 
         * "resetters" circumvent this issue by capturing the function call in a 
         * lambda function for later use.
         * @return none
         */
        void resetVars();
    };
}

#include "utilities.icc"

#endif
