//#include "map.h"
//#include <map>
//#include <tchar.h>
//
//#include <boost/config/warning_disable.hpp>
//#include <boost/spirit/include/qi.hpp>
//#include <boost/foreach.hpp>
//#include <boost/fusion/include/adapt_struct.hpp>
//#include <boost/fusion/include/io.hpp>
//
//#ifdef _UNICODE
//#define tfopen _wfopen
//#define tfgets fgetws
//#else
//#define tfopen fopen
//#define tfgets fgets
//#endif
//
//
////struct SEGMENTS
////{
////    std::string name;   // 
////    LPVOID base;        // 基址
////};
////// 一个00x中可能含多个segment，但最终是根据pe的segment来确定最后的一一对应关系，IDA生成的是一一对应(因为是分析PE)
////typedef std::multimap<unsigned, SEGMENTS> MAP_SEGMENT; 
////
////MAP_SEGMENT m_segments;
//
//namespace maps
//{
//    struct segment 
//    {
//        unsigned start;
//        unsigned rva;
//        unsigned length;
//        std::tstring name;
//        std::tstring Class;
//    };
//
//    struct label
//    {
//        unsigned index;
//        unsigned rva;
//        std::tstring name;
//    };
//
//};
//
//
//// holyshit!fusion has a map inside! we can't use namespace map!
//BOOST_FUSION_ADAPT_STRUCT(
//                          maps::segment,
//                          (unsigned, start)
//                          (unsigned, rva)
//                          (unsigned, length)
//                          (std::tstring, name)
//                          (std::tstring, Class)
//                          );
// BOOST_FUSION_ADAPT_STRUCT(
//                          maps::label,
//                          (unsigned, index)
//                          (unsigned, rva)
//                          (std::tstring, name)
//                          )
//
//using namespace boost::spirit;
//using namespace boost::spirit::qi;
//
//#ifdef _UNICODE
//namespace tchar_ns = boost::spirit::unicode; // 需要添加BOOST_SPIRIT_UNICODE宏
//#else
//namespace tchar_ns = boost::spirit::ascii;
//#endif
//
//namespace maps
//{
//
//    template <typename Iterator = std::string::const_iterator, typename ExpressionAST = segment, typename SPACE_TYPE = space_type>
//    struct segment_parser : qi::grammar<Iterator, ExpressionAST(), SPACE_TYPE>
//    {
//        segment_parser() : segment_parser::base_type(start)
//        {
//            //quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];
//            quoted_string %= +char_;//+(char_ - '$');
//
//            //hex[ref(seg) = _1]>> ':' >> hex>> hex >> 'H' >> quoted_string[ref(name) = _1] >> quoted_string,
//            start %=
//                hex >> _T(':') >> hex
//                >>  hex >> _T('H')
//                >> qi::lexeme[+(char_ - _T('$')) >> *(qi::hold[ +(qi::char_(_T(' '))) >> +(char_ - _T('$'))])]
//                //>> quoted_string
//                ;
//        }
//
//        qi::rule<Iterator, std::string(), SPACE_TYPE> quoted_string;
//        qi::rule<Iterator, ExpressionAST(), SPACE_TYPE> start;
//    };
//
//    template <typename Iterator = std::string::const_iterator, typename ExpressionAST = label, typename SPACE_TYPE = space_type>
//    struct label_parser : qi::grammar<Iterator, ExpressionAST(), SPACE_TYPE>
//    {
//        label_parser() : label_parser::base_type(start)
//        {
//            quoted_string %= +char_;
//
//            start %=
//                hex >> _T(':')
//                >>  hex
//                >>  quoted_string
//                ;
//        }
//
//        qi::rule<Iterator, std::string(), SPACE_TYPE> quoted_string;
//        qi::rule<Iterator, ExpressionAST(), SPACE_TYPE> start;
//    };
//
//};
//
//bool IsHeader(const std::tstring& input, std::map<std::tstring, int> headers)
//{
//
//    using boost::spirit::qi::phrase_parse;
//
//    qi::symbols<TCHAR> keywords;
//
//    typedef std::pair<std::tstring, int> pair_t;
//    BOOST_FOREACH(pair_t p, headers)
//    {
//        keywords.add(p.first);
//    }
//    //keywords = _T("Start"),_T("Length"),_T("Name"),_T("Class")
//    //    ,_T("Stop"); // BootLoader.map
//
//    return phrase_parse(
//        input.begin(),                          /*< start iterator >*/
//        input.end(),                           /*< end iterator >*/
//        keywords >> keywords >> keywords >> keywords,   /*< the parser >*/
//        tchar_ns::space                           /*< the skip-parser >*/
//        );
//}
//
//
//
//#include <boost/spirit/include/qi_uint.hpp>
////#include <boost/lambda/lambda.hpp>
//#include <boost/spirit/include/phoenix_core.hpp>
//#include <boost/spirit/include/phoenix_operator.hpp>
//
//bool parse_segments(const std::tstring& input, maps::segment& p)
//{
//    maps::segment_parser<> g;
//    return phrase_parse(input.begin(), input.end(), g, tchar_ns::space, p);
//
//}
//
//CMap::CMap( const std::tstring& file )
//{
//    FILE* f = tfopen(file.c_str(), _T("rt"));
//    m_file.reset(f, fclose);
//}
//
//#include <boost/fusion/include/struct.hpp>
//#include <boost/fusion/include/nview.hpp>
//
//CMap::line_iterator CMap::begin()
//{
//    line_iterator ci(m_file);
//    if (m_file)
//    {
//        ci.increment();
//
//        // 定位segments
//        for (;ci != this->end(); ++ci)
//        {
//            std::map<std::tstring, int> s;
//            s.insert(std::make_pair(_T("Start"),0));
//            s.insert(std::make_pair(_T("Length"),0));
//            s.insert(std::make_pair(_T("Name"),0));
//            s.insert(std::make_pair(_T("Class"),0));
//            s.insert(std::make_pair(_T("Stop"),0));
//            if (IsHeader(*ci, s))
//            {
//                ++ci;
//                break;
//            }
//        }
//
//        for (;
//            ci!= this->end() && !(*ci).empty();
//            ++ci)
//        {
//            maps::segment p;
////
////            typedef boost::fusion::result_of::as_nview<maps::segment, 0, 1, 2, 3, 4>::type start_name;
////
////            //qi::rule<std::tstring::const_iterator, maps::segment()> r =
//////                lit(" ") >> hex >> ':'>> hex>> lit(" ") >> hex >> lit("H ") >> +(char_ - '$') % +qi::space >> +char_;
////            start_name rr(boost::fusion::as_nview<0, 1, 2, 3, 4>(p));
////            std::tstring::const_iterator b = (*ci).begin();
////            std::tstring::const_iterator e = (*ci).end();
////            qi::parse(b, e, 
////                lit(" ") >> hex >> ':'>> hex>> lit(" ") >> hex >> lit("H ") >> +(char_ - '$') % +qi::space >> +char_
////            , rr);
//            parse_segments(*ci, p);
//            p.start = 0;
//        }
//
//    }
//    return ci;
//}
//
//CMap::line_iterator CMap::end()
//{
//    boost::shared_ptr<FILE> f;
//    return line_iterator(f);
//}
//
//std::tstring& CMap::line_iterator::dereference() const
//{
//    return m_text;
//}
//
//void CMap::line_iterator::increment()
//{
//    if(feof(m_file.get()))
//    {
//        m_file.reset();
//    }
//    else
//    {
//        TCHAR mapline[256] = {0};
//        m_text = _T("");
//        if(tfgets(mapline, 256, m_file.get()))
//            m_text = mapline;
//    }
//}
//
//bool CMap::line_iterator::equal( line_iterator const& other ) const
//{
//    return other.m_file.get() == this->m_file.get();
//}
//
