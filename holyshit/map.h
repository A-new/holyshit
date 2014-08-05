#pragma once

#include <windows.h>
#include <string>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>

#ifdef _UNICODE
#define tstring wstring
#else
#define tstring string
#endif

class CMap
{
public:
    class line_iterator;
    CMap(const std::tstring& file);
    line_iterator begin();
    line_iterator end();

    class line_iterator 
        : public boost::iterator_facade<
        line_iterator
        , std::tstring
        , boost::incrementable_traversal_tag>
    {
        LPVOID Addr();
        std::tstring Symbol();

    private:
        friend class CMap;
        line_iterator(boost::shared_ptr<FILE> f){ m_file = f;}
    private:    
        friend class boost::iterator_core_access;
        std::tstring& dereference() const;
        void increment();
        bool equal(line_iterator const& other) const;

        boost::shared_ptr<FILE> m_file;
        mutable std::tstring m_text; // shit! mutable
    };

private:
    boost::shared_ptr<FILE> m_file;
};