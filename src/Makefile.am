AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = htmlTemplater

htmlTemplater_SOURCES = \
    json/jsoncpp.cpp \
    HtmlContext.cpp \
    htmlTemplater.cpp

AM_CPPFLAGS = -I$(top_srcdir)/src -std=c++11
AM_LDFLAGS = -lboost_system -lboost_filesystem

