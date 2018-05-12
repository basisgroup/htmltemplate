/*
 * htmlTemplater.cpp
 *
 *  Created on: May 3, 2018
 *      Author: schnet
 */

#include "HtmlContext.hpp"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

void printUsage (const char* appName)
{
  printf("%s usage:\n\n", appName);
  printf("  -c  <config file>    Path to JSON configuration file\n");
  printf("  -h  <help>           This help message\n");
  printf("\n");
}

int main (int argc, char **argv)
{
  std::string configPath("site.json");
  int opt;

  while ((opt=getopt(argc, argv, "c:h")) != -1) {
    switch (opt) {
      case 'c':
        configPath = optarg;
        break;
      case 'h':
      default:
        printUsage(argv[0]);
        exit(1);
    }
  }
  /* create object */
  basis::HtmlContext html(configPath);
  if (html.work()) {
    printf("Successfully processed '%s'\n", configPath.c_str());
  }
  else {
    fprintf(stderr, "Error, could not create site from JSON configuration '%s'\n", configPath.c_str());
  }
  return 0;
}
