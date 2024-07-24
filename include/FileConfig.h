//
// 
//

#pragma once

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Vector.h"
#include "Matrix.h"
using MB::vec3f;
using MB::vec2f;
using MB::vec4f;
using MB::matrix4f;
using MB::matrix4;
/*
* \brief Generic configuration Class
*
*/


class Config {
    // Data
protected:
    std::string m_fileName;
    std::string m_Delimiter;  //!< separator between key and value
    std::string m_Comment;    //!< separator between value and comments
    std::map<std::string,std::string> m_Contents;  //!< extracted keys and values

    typedef std::map<std::string,std::string>::iterator mapi;
    typedef std::map<std::string,std::string>::const_iterator mapci;
    // Methods
public:
    static  void Trim( std::string& inout_s );
    Config( std::string filename,std::string delimiter = "=",std::string comment = "#" );
    Config();
    void LoadConfig(std::string filename);
    template<class T> T Read( const std::string& in_key ) const;  //!<Search for key and read value or optional default value, call as read<T>
    template<class T> T Read( const std::string& in_key, const T& in_value ) const;
    template<class T> bool ReadInto( T& out_var, const std::string& in_key ) const;
    template<class T>
    bool ReadInto( T& out_var, const std::string& in_key, const T& in_value ) const;
    bool FileExist(std::string filename);
    void ReadFile(std::string filename,std::string delimiter = "=",std::string comment = "#" );

    // Check whether key exists in configuration
    bool KeyExists( const std::string& in_key ) const;

    // Modify keys and values
    template<class T> void Add( const std::string& in_key, const T& in_value );
    void Remove( const std::string& in_key );
    void Save();

    // Check or change configuration syntax
    std::string GetDelimiter() const { return m_Delimiter; }
    std::string GetComment() const { return m_Comment; }
    std::string SetDelimiter( const std::string& in_s )
    { std::string old = m_Delimiter;  m_Delimiter = in_s;  return old; }
    std::string SetComment( const std::string& in_s )
    { std::string old = m_Comment;  m_Comment =  in_s;  return old; }


    // Write or read configuration
    friend std::ostream& operator<<( std::ostream& os, const Config& cf );
    friend std::istream& operator>>( std::istream& is, Config& cf );

    // Read an array of values from the configuration
    template<class T>
    std::vector<T> ReadArray(const std::string& key, const std::string& separator=",#_&%$ @") const;
    template<class T>
    void WriteArrayToFile(const std::string& key, const std::vector<T>& values, const std::string& separators=",#_&%$ @");

    vec2f Readvec2f(const std::string& key, const std::string& separator = ",#_&%$ @") ;
    vec3f Readvec3f(const std::string& key, const std::string& separator = ",#_&%$ @");
    vec4f Readvec4f(const std::string& key, const std::string& separator = ",#_&%$ @");


    // New method for reading matrices
    template<class T>
    std::vector<std::vector<T>> ReadMatrix(const std::string& key, const std::string& rowSeparator="|", const std::string& colSeparator = ",") const;

    matrix4f Readmatrix4f(const std::string& key, const std::string& rowSeparator = "|", const std::string& colSeparator = ",") const;
protected:
    template<class T> static std::string T_as_string( const T& t );
    template<class T> static T string_as_T( const std::string& s );
    template<class T>
    static std::vector<T> ParseArray(const std::string& s);

    template <typename T>
    MB::matrix4<T> convertToMatrix4(const std::vector<std::vector<T>>& matrixData) const;
    // Exception types
public:
    struct File_not_found {
        std::string filename;
        File_not_found( const std::string& filename_ = std::string() )
                : filename(filename_) {} };
    struct Key_not_found {  // thrown only by T read(key) variant of read()
        std::string key;
        Key_not_found( const std::string& key_ = std::string() )
                : key(key_) {} };
};


/* static */
template<class T>
std::string Config::T_as_string( const T& t )
{
    // Convert from a T to a string
    // Type T must support << operator
    std::ostringstream ost;
    ost << t;
    return ost.str();
}


/* static */
template<class T>
T Config::string_as_T( const std::string& s )
{
    // Convert from a string to a T
    // Type T must support >> operator
    T t;
    std::istringstream ist(s);
    ist >> t;
    return t;
}


/* static */
template<>
inline std::string Config::string_as_T<std::string>( const std::string& s )
{
    // Convert from a string to a string
    // In other words, do nothing
    return s;
}


/* static */
template<>
inline bool Config::string_as_T<bool>( const std::string& s )
{
    // Convert from a string to a bool
    // Interpret "false", "F", "no", "n", "0" as false
    // Interpret "true", "T", "yes", "y", "1", "-1", or anything else as true
    bool b = true;
    std::string sup = s;
    for( std::string::iterator p = sup.begin(); p != sup.end(); ++p )
        *p = toupper(*p);  // make string all caps
    if( sup==std::string("FALSE") || sup==std::string("F") ||
        sup==std::string("NO") || sup==std::string("N") ||
        sup==std::string("0") || sup==std::string("NONE") )
        b = false;
    return b;
}


template<class T>
T Config::Read( const std::string& key ) const
{
    // Read the value corresponding to key
    mapci p = m_Contents.find(key);
    if( p == m_Contents.end() )
    {
    	return string_as_T<T>("-1");
    	} //throw Key_not_found(key);
    return string_as_T<T>( p->second );
}


template<class T>
T Config::Read( const std::string& key, const T& value ) const
{
    // Return the value corresponding to key or given default value
    // if key is not found
    mapci p = m_Contents.find(key);
    if( p == m_Contents.end() ) return value;
    return string_as_T<T>( p->second );
}


template<class T>
bool Config::ReadInto( T& var, const std::string& key ) const
{
    // Get the value corresponding to key and store in var
    // Return true if key is found
    // Otherwise leave var untouched
    mapci p = m_Contents.find(key);
    bool found = ( p != m_Contents.end() );
    if( found ) var = string_as_T<T>( p->second );
    return found;
}


template<class T>
bool Config::ReadInto( T& var, const std::string& key, const T& value ) const
{
    // Get the value corresponding to key and store in var
    // Return true if key is found
    // Otherwise set var to given default
    mapci p = m_Contents.find(key);
    bool found = ( p != m_Contents.end() );
    if( found )
        var = string_as_T<T>( p->second );
    else
        var = value;
    return found;
}


template<class T>
void Config::Add( const std::string& in_key, const T& value )
{
    // Add a key with given value
    std::string v = T_as_string( value );
    std::string key=in_key;
    //Config::trim(key);
    //Config::trim(v);
    m_Contents[key] = v;
    return;
}

template<class T>
std::vector<T> Config::ReadArray(const std::string& key, const std::string& separators) const
{
    mapci p = m_Contents.find(key);
    if (p == m_Contents.end())
    {
        return std::vector<T>();
    }

    std::vector<T> result;
    std::string value = p->second;

    size_t startPos = 0;
    size_t endPos = std::string::npos;
    std::string selectedSeparator;

    while (true)
    {
        size_t minPos = std::string::npos;
        size_t separatorLength = 0;

        for (const auto& separator : separators)
        {
            size_t pos = value.find(separator, startPos);
            if (pos != std::string::npos && (pos < minPos || minPos == std::string::npos))
            {
                minPos = pos;
                selectedSeparator = separator;
                separatorLength = 1;  // Assuming all separators are single characters
            }
        }

        if (minPos != std::string::npos)
        {
            endPos = minPos;
        }

        if (endPos != std::string::npos)
        {
            std::string element = value.substr(startPos, endPos - startPos);
            Trim(element);

            if (!element.empty())
            {
                result.push_back(string_as_T<T>(element));
            }

            startPos = endPos + separatorLength;
            endPos = std::string::npos;
        }
        else
        {
            break;
        }
    }

    std::string lastElement = value.substr(startPos);
    Trim(lastElement);

    if (!lastElement.empty())
    {
        result.push_back(string_as_T<T>(lastElement));
    }

    return result;
}

template<class T>
void Config::WriteArrayToFile(const std::string& key, const std::vector<T>& values, const std::string& separators)
{
    std::ofstream file("output.txt", std::ios::app); // Open the file in append mode

    if (file.is_open())
    {
        file << key << " = ";

        for (size_t i = 0; i < values.size(); ++i)
        {
            file << values[i];

            if (i < values.size() - 1)
            {
                file << separators;
            }
        }

        file << std::endl;
        file.close();
    }
    else
    {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
    }
}


/* Read a matrix from the configuration file */
template<class T>
std::vector<std::vector<T>> Config::ReadMatrix(const std::string& key, const std::string& rowSeparator, const std::string& colSeparator) const
{
    mapci p = m_Contents.find(key);
    if (p == m_Contents.end())
    {
        // Key not found
        return std::vector<std::vector<T>>();
    }

    std::string value = p->second;
    std::vector<std::vector<T>> matrix;

    // Extract rows
    std::istringstream rows(value);
    std::string row;
    while (std::getline(rows, row, rowSeparator[0]))
    {
        std::vector<T> matrixRow;

        // Extract columns
        std::istringstream cols(row);
        std::string col;
        while (std::getline(cols, col, colSeparator[0]))
        {
            // Remove leading/trailing whitespace
            col = col.substr(col.find_first_not_of(" \t"));
            col = col.substr(0, col.find_last_not_of(" \t") + 1);

            // Convert to T and add to the row
            T element = string_as_T<T>(col);
            matrixRow.push_back(element);
        }

        // Add the row to the matrix
        matrix.push_back(matrixRow);
    }

    return matrix;
}

template <typename T>
MB::matrix4<T>  Config::convertToMatrix4(const std::vector<std::vector<T>>& matrixData) const
{
    matrix4<T> result;

    // Check if the matrixData has 4 rows
    if (matrixData.size() != 4)
    {
        // Handle the error or throw an exception
        // depending on your requirements
        std::cerr << "Error: Input matrix must have 4 rows." << std::endl;
        return result;
    }

    // Check if each row has 4 columns
    for (const auto& row : matrixData)
    {
        if (row.size() != 4)
        {
            // Handle the error or throw an exception
            // depending on your requirements
            std::cerr << "Error: Each row of the input matrix must have 4 columns." << std::endl;
            return result;
        }
    }

    // Populate the elements of the matrix4
    for (size_t i = 0; i < 4; i++)
    {
        for (size_t j = 0; j < 4; j++)
        {
            result(i, j) = matrixData[i][j];
        }
    }

    return result;
}

