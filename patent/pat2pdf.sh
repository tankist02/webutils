#!/usr/bin/env bash
#
# pat2pdf version 1.22
# Copyright (c)2000 Oren Tirosh <oren@hishome.net>
# Copyright (c)2002 Thomas Boege <thomas@boegenielsen.dk>
# Released under the GPL
#
# This script connects to the USPTO patent database, retrieves the TIFF
# patent images and converts them into a single pdf file using GhostScript.
#
# Yes, I know, you would have written it in perl/python/(insert your
# favourite scripting language here)
#
# It requires an http fetcher (lynx by default), GhostScript
# and tiff2ps (part of libtiff)
#
# For best results (small PDFs) use GhostScript 5.5 or higher.
#
#Usage:
# pat2pdf <number>|<application number>
#
# Result is a file in the current directory named pat<patnum>.pdf
#
#Bugs:
#
# Error checking and recovery could be better.
#
#Homepage:
# http://www.tothink.com/pat2pdf
#


# Some utility functions:

# Nonzero if first string contains second string (+globbing chars ?*[x-y])
contains()
  { [ -n "$1" -a -z "${1##*$2*}" ]
}

# Change this if you prefer wget, curl, etc. Don't forget to also update
# the calls to verify the presence of the executable.
url2stdout() {
  lynx -dump -source "$1" 2>/dev/null
#  wget -Y off -t 13 -O - $1 2>/dev/null
}

# Write to stderr
werr() {
  echo "$*" 1>&2
}

# Die with a message
die() {
  werr "$*"
  exit 1
}

# extract a field from a string and echo to stdout
# $1 - source string
# $2 - before target field
# $3 - prefix of target field
# $4 - after target field
extract() {
  [ -n "$1" ] || return 1;
  [ -z "${1##*$2$3*$4*}" ] || return 1;
  STRIP="$3${1##*$2$3}";
  STRIP="${STRIP%%$4*}";
  echo "$STRIP"
}

# verify the presence of a required executable
verify() {
  [ -x "$(which $1)" ] || die "Error: required executable $1 not found"
}

pat_issued () {
werr "...fetching search results page for patent $PATNUM"
SITENAME="http://patft.uspto.gov"

RESULTPAGE=$( url2stdout "${SITENAME}/netacgi/nph-Parser?TERM1=${PATNUM}&Sect1=PTO1&Sect2=HITOFF&d=PALL&p=1&u=%2Fnetahtml%2Fsrchnum.htm&r=0&f=S&l=50" ) || die "Error fetching search results web page."
contains "$RESULTPAGE" "No patents have matched" && die "No patents have matched your query."

PATENTURL=$( extract "$RESULTPAGE" "URL=" "/netacgi/nph-Parse" "\">" ) || die "Error isolating URL from results page."
PATENTURL="${SITENAME}${PATENTURL}"

werr "...fetching patent page"
PATENTPAGE=$( url2stdout "$PATENTURL" | head -n 70 ) || die "Error fetching patent web page."

TITLE=$( extract "$PATENTPAGE" "font size=\"+1\">" "" "</font>" ) || TITLE="##Error isolating patent title - continuing anyway##"
echo "U.S. Patent $PATNUM: $TITLE"

IMAGEURL=$( extract "$PATENTPAGE" "a href=" "http://patimg" ">" ) ||  die "Error isolating image page URL from patent page."

IMAGESERVER=$( extract "$IMAGEURL" "" "http://patimg" "/.piw" ) || die "Error isolating image server name."
}

pat_appl () {
SITENAME="http://appft1.uspto.gov"

werr "...fetching search results page for patent application $PATNUM"
RESULTURL="${SITENAME}/netacgi/nph-Parser?Sect1=PTO2&Sect2=HITOFF&u=%2Fnetahtml%2FPTO%2Fsearch-adv.html&r=0&p=1&f=S&l=50&Query=DN%2F$PATNUM&d=PG01"

RESULTPAGE=$( url2stdout $RESULTURL )|| die "Error fetching search results web page."
contains "$RESULTPAGE" "No patents have matched" && die "No patents have matched your query."

PATENTURL=$( extract "$RESULTPAGE" "HREF=" "/netacgi/nph-Parse" ">" ) || die "Error isolating URL from results page."
PATENTURL="${SITENAME}${PATENTURL}"

werr "...fetching patent page"
PATENTPAGE=$( url2stdout "$PATENTURL" |head -n 70 ) ||  die "Error fetching patent web page."

TITLE=$( extract "$PATENTPAGE" "font size=\"+1\">" "" "</font>" ) || TITLE="##Error isolating patent title - continuing anyway##"
echo "U.S. Patent Application $PATNUM: $TITLE"

IMAGEURL=$( extract "$PATENTPAGE" "a href=" "http://aiw" ">" ) ||  die "Error isolating image page URL from patent page."

IMAGESERVER=$( extract "$IMAGEURL" "" "http://aiw" ".aiw?" ) || die "Error isolating image server name."
}


#main()

verify "lynx"
verify "tiff2ps"
verify "ps2pdf"
verify "sed"
verify "head"
verify "mv"
verify "rm"

PATNUM=$( echo $1 | sed 's@,@@g' )

MAX_PAGES=$2

[ -z "$PATNUM" ] && die "usage: pat2pdf <number>|<application number> [max_pages]"


[ -z "${PATNUM##[a-zA-Z1-9][a-zA-Z0-9][0-9][0-9]*}" ] || die "Use a 7 digit patent number."

[ ${#PATNUM} -gt 11 ] && die "Too big number"
[ ${#PATNUM} -eq 11 ] && pat_appl
[ ${#PATNUM} -le 7 ] && pat_issued


werr "...fetching images page"
IMAGEPAGE=$( url2stdout "$IMAGEURL" ) || die "Error fetching images page."

NUMPAGES=$( extract "$IMAGEPAGE" "-- NumPages=" "" " --" ) || die "Error getting number of pages."

if [ "$MAX_PAGES" != "" ] ; then
  #echo "MAX_PAGES: $MAX_PAGES"
	#[ -z "${MAX_PAGES##[0-9][0-9]*}" ] || die "Second command line parameter max_pages is not valid."

  echo "There are totally $NUMPAGES pages for this patent."
	echo "Max pages to extract is set to $MAX_PAGES."
	NUMPAGES=$MAX_PAGES
fi

TIFFURL=$( extract "$IMAGEPAGE" "embed src=?" "/.DImg" "? width=" ) || die "Error getting TIFF file URL."
TIFFURL="$IMAGESERVER$TIFFURL"

contains "$TIFFURL" "PageNum=1" || die "Error processing TIFF file URL"
TIFFURL1="${TIFFURL%%PageNum=1*}PageNum="
TIFFURL2="${TIFFURL##*PageNum=1}"

PAGE=1
{
  while [ "$PAGE" -le "$NUMPAGES" ] ; do
    werr "...fetching page $PAGE of $NUMPAGES"
    url2stdout "${TIFFURL1}${PAGE}${TIFFURL2}" > "tmppat${PATNUM}.tiff" || die "Error retrieving TIFF page."
    tiff2ps "tmppat${PATNUM}.tiff" 2>/dev/null || die "tiff2ps error"
    PAGE=$[$PAGE+1]
  done
} | {
  ps2pdf - "tmppat${PATNUM}.pdf" || die "GhostScript error."
}

rm -f "tmppat${PATNUM}.tiff"

# kludge around differences in versions of pat2pdf
mv -f "tmppat${PATNUM}.pdf.pdf" "tmppat${PATNUM}.pdf" &>/dev/null
mv -f "tmppat${PATNUM}.pdf" "pat${PATNUM}.pdf" || die "Error renaming pdf file."

werr Done.
werr pat${PATNUM}.pdf

