#ifndef BREAD_CRUMBS_H
#define BREAD_CRUMBS_H


#include <vector>
#include <string>
#include <cassert>

#include <boost/format.hpp>


class BreadCrumbs
{
public:
    void loadTemplates(char const* dir);
    void push(std::string const& v) { subdirs_.push_back(v); }
    void pop() { assert(!subdirs_.empty()); Subdirs::iterator last = subdirs_.end(); --last; subdirs_.erase(last); }
    size_t size() const { return subdirs_.size(); }
    bool empty() const { return subdirs_.empty(); }
    std::string getHtml() const;

private:
    typedef std::vector<std::string> Subdirs;
    Subdirs subdirs_;
    std::string linkFormat_;
};


#endif
