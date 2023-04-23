#include <cassert>
#include <iostream>
#include <exception>
#include <string>
#include <vector>
#include <sstream>

#include <boost/tokenizer.hpp>
#include <boost/format.hpp>

#include "util/Properties.h"
#include "util/Formatter.h"
#include "runAndCapture.h"


/*struct PatentData
{
  PatentData(std::string const& number, std::string const& text)
    : number_(number), text_(text)
  {
  }

  std::string number_;
  std::string text_;
};

std::ostream& operator<<(std::ostream& os, PatentData const& pd)
{
  os << pd.number_ << ' ' << pd.text_;
  return os;
}

typedef std::vector< PatentData > PatentNums;*/


int parsePatentNums(std::string const& out, std::string const& patent_anchor/*, PatentNums& pns*/)
{
  typedef boost::tokenizer< boost::char_separator< char > > Tokenizer;

  boost::char_separator< char > sep("\n");

  Tokenizer tok(out, sep);

  // Iterate through patent data lines
  for( Tokenizer::const_iterator it = tok.begin(); it != tok.end(); ++it )
  {
    int index = -1;
    std::string number;
    std::string text;
    char buf[1024];

    std::istringstream istr(it->c_str());

    istr >> index >> number >> text >> std::ws;

    //std::cout << "index: " << index << ", number: " << number << ", text: " << text << std::endl;

    istr.getline(buf, sizeof(buf));
    //std::cout << "buf: " << buf << std::endl;

    if( istr )
    {
      //pns.push_back(PatentData(number, buf));
      std::cout << number << ' ' << buf << std::endl;
    }
  }

  return 0;
}

int runAndParse(std::string const& cmd, std::string const& anchor, std::string const& patent_anchor/*, PatentNums& pns*/)
{
  std::string out = runAndCapture(cmd);

  //std::cout << out << std::endl;

  std::string::size_type pos = out.find(anchor);

  if( pos == std::string::npos )
    throw std::runtime_error(Formatter("Failed to find anchor: ") << anchor << " in output: " << out);

  std::string body = out.substr(pos);

  parsePatentNums(body, patent_anchor/*, pns*/);

  return 0;
}

int main(int argc, char** argv)
{
  try
  {
    if( argc < 2 )
      throw std::runtime_error("Search terms are missing. Supply them as command-line parameters.");

    Properties props;
    props.load("patent.conf");

    std::string browser = props.get("browser");
    assert(!browser.empty());

    std::string url_begin = props.get("url_begin");
    assert(!url_begin.empty());

    std::string search_terms;
    for( int i = 1; i < argc; ++i )
    {
      if( i != 1 )
        search_terms += '+';

      search_terms += argv[i];
    }

    assert(!search_terms.empty());

    url_begin = boost::io::str(boost::format(url_begin) % search_terms);

    std::string cmd = browser + " " + url_begin;

    //std::cout << "cmd: " << cmd << std::endl;

    std::string const anchor = props.get("anchor");
    assert(!anchor.empty());

    std::string hits_line = props.get("hits_line");
    assert(!hits_line.empty());

    std::string out = runAndCapture(cmd);

    //std::cout << out << std::endl;

    //PatentNums pns;

    std::string::size_type pos = out.find(anchor);

    if( pos == std::string::npos )
      throw std::runtime_error(Formatter("Failed to find anchor: ") << anchor << " in output: " << out);

    std::string header = out.substr(0, pos);

    int begin_hits = -1, end_hits = -1, total_hits = -1;

    typedef boost::tokenizer< boost::char_separator< char > > Tokenizer;

    boost::char_separator< char > sep("\n");

    Tokenizer tok(header, sep);

    for( Tokenizer::const_iterator it = tok.begin(); it != tok.end(); ++it )
    {
      if( !it->empty() )
      {
        //std::cout << it->c_str() << ", format: " << hits_line << std::endl;
        //fprintf(stdout, "line: %s, format: %s\n", it->c_str(), hits_line.c_str());

        if( sscanf(it->c_str(), hits_line.c_str(), &begin_hits, &end_hits, &total_hits) == 3 )
        {
          //std::cout << "begin_hits: " << begin_hits << ", end_hits: " << end_hits << ", total_hits: " << total_hits << std::endl;
          break;
        }
      }
    }

    std::cerr << total_hits << std::endl;

    assert(begin_hits >= 0);
    assert(end_hits >= 0);
    assert(total_hits >= 0);

    std::string body = out.substr(pos);

    std::string patent_anchor = props.get("patent_anchor");
    assert(!patent_anchor.empty());

    parsePatentNums(body, patent_anchor/*, pns*/);

    int page_items = atoi(props.get("page_items").c_str());
    assert(page_items > 0);

    std::string url_continue = props.get("url_continue");
    assert(!url_continue.empty());

    int count = 1;
    int hits_left = total_hits - page_items;

    for( ; hits_left >= page_items; hits_left -= page_items )
    {
      std::string url = boost::io::str(boost::format(url_continue) % search_terms % search_terms % search_terms % ++count);

      std::string cmd = browser + " " + url;

      //std::cout << "hits_left: " << hits_left << ", cmd: " << cmd << std::endl;

      runAndParse(cmd, anchor, patent_anchor/*, pns*/);
    }

    if( hits_left > 0 )
    {
      std::string url_end = props.get("url_end");
      assert(!url_end.empty());

      std::string url = boost::io::str(boost::format(url_end) % search_terms % search_terms % search_terms % ++count % hits_left);

      std::string cmd = browser + " " + url;

      //std::cout << "hits_left: " << hits_left << ", cmd: " << cmd << std::endl;

      runAndParse(cmd, anchor, patent_anchor/*, pns*/);
    }

    //std::copy(pns.begin(), pns.end(), std::ostream_iterator< PatentData >(std::cout, "\n"));
  }
  catch( std::exception const& e )
  {
    std::cerr << "exception: " << e.what() << std::endl;
  }
  return 0;
}
