/*
 * HtmlContext.cpp
 *
 *  Created on: May 3, 2018
 *      Author: schnet
 */

#include "HtmlContext.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

namespace basis {

const std::string HtmlContext::DYNAMIC_FILES_KEY("dynamicFiles");
const std::string HtmlContext::FILE_ORDER_KEY("fileOrder");
const std::string HtmlContext::ADDITIONAL_FILES_KEY("additionalFiles");
const std::string HtmlContext::FILE_EXTENSION_KEY("fileExtension");
const std::string HtmlContext::WEBSITE_KEY("website");
const std::string HtmlContext::INPUT_DIRECTORY_KEY("inputDirectory");
const std::string HtmlContext::OUTPUT_DIRECTORY_KEY("outputDirectory");
const std::string HtmlContext::STRICT_KEY("strict");
const std::string HtmlContext::VERBOSE_KEY("verbose");

HtmlContext::HtmlContext (const std::string& configPath) :
    m_configPath(configPath),
    m_fileExtension("html"),
    m_inputDirectory(""),
    m_outputDirectory(""),
    m_website(""),
    m_strict(false),
    m_verbose(false)
{
}

HtmlContext::~HtmlContext ()
{
}

bool HtmlContext::ReadJsonFile (Json::Value& root, const std::string& filePath)
{
  /* open configuration file */
  std::ifstream configFile(filePath.c_str(), std::ifstream::binary);
  if (!configFile.is_open()) {
    fprintf(stderr, "Error, cannot open JSON configuration file '%s'\n", filePath.c_str());
    return false;
  }
  /* read file into stringstream */
  std::stringstream ss;
  ss << configFile.rdbuf();
  /* now read into JSON object */
  Json::CharReaderBuilder reader;
  std::string errs;
  bool rv = Json::parseFromStream(reader, ss, &root, &errs);
  /* error check */
  if (!rv) {
    fprintf(stderr, "Error, cannot parse JSON configuration file '%s'\n", filePath.c_str());
    printf("'%s'\n", errs.c_str());
    return false;
  }
  /* close file */
  configFile.close();
  return rv;
}

bool HtmlContext::parseJson (const Json::Value& root)
{
  /* check for (optional) boolean verbose flag */
  if (root.isMember(VERBOSE_KEY)) {
    /* set verbose if it's correct */
    m_verbose = root[VERBOSE_KEY].isBool() && root[VERBOSE_KEY].asBool();
  }
  /* check for (required) files */
  if (root.isMember(DYNAMIC_FILES_KEY) && root[DYNAMIC_FILES_KEY].isArray())
  {
    Json::Value tmp = root.get(DYNAMIC_FILES_KEY, 0);
    for (Json::Value::iterator it=tmp.begin(); it!=tmp.end(); ++it)
    {
      if ((*it).isString()) {
        m_dynamicFiles.push_back((*it).asString());
      }
      else {
        std::cerr << "Error, cannot cast 'n" << *it << "' as string" << std::endl;
        return false;
      }
    }
  }
  else {
    fprintf(stderr, "Error, missing required (array) field '%s'\n", DYNAMIC_FILES_KEY.c_str());
    return false;
  }
  /* check for (required) file order */
  if (root.isMember(FILE_ORDER_KEY) && root[FILE_ORDER_KEY].isArray()) {
    Json::Value tmp = root.get(FILE_ORDER_KEY, 0);
    for (Json::Value::iterator it=tmp.begin(); it!=tmp.end(); ++it) {
      if ((*it).isString()) {
        m_fileOrder.push_back((*it).asString());
      }
      else {
        std::cerr << "Error, cannot cast '" << *it << "' as string" << std::endl;
        return false;
      }
    }
  }
  else {
    fprintf(stderr, "Error, missing required (array) field '%s'\n", FILE_ORDER_KEY.c_str());
    return false;
  }
  /* check for (optional) html */
  if (root.isMember(FILE_EXTENSION_KEY)) {
    if (root[FILE_EXTENSION_KEY].isString()) {
      m_fileExtension = root[FILE_EXTENSION_KEY].asString();
    }
    else {
      fprintf(stderr, "Error, cannot parse JSON field '%s' as a string...Ignoring value\n", FILE_EXTENSION_KEY.c_str());
    }
  }
  /* check for (optional) input directory */
  if (root.isMember(INPUT_DIRECTORY_KEY)) {
    if (root[INPUT_DIRECTORY_KEY].isString())  {
      m_inputDirectory = root[INPUT_DIRECTORY_KEY].asString();
      if (!m_inputDirectory.empty()) {
        m_inputDirectory += "/";
      }
    }
    else {
      fprintf(stderr, "Error, cannot parse JSON field '%s' as a string...Ignoring value\n", INPUT_DIRECTORY_KEY.c_str());
    }
  }
  /* check for (optional) output directory */
  if (root.isMember(OUTPUT_DIRECTORY_KEY)) {
    if (root[OUTPUT_DIRECTORY_KEY].isString()) {
      m_outputDirectory = root[OUTPUT_DIRECTORY_KEY].asString();
      if (!m_outputDirectory.empty()) {
        m_outputDirectory += "/";
      }
    }
    else {
      fprintf(stderr, "Error, cannot parse JSON field '%s' as a string...Ignoring value\n", OUTPUT_DIRECTORY_KEY.c_str());
    }
  }
  /* check for (optional) website */
  if (root.isMember(WEBSITE_KEY)) {
    if (root[WEBSITE_KEY].isString()) {
      m_website = root[WEBSITE_KEY].asString();
    }
    else {
      fprintf(stderr, "Error, cannot parse JSON field '%s' as a string...Ignoring value\n", WEBSITE_KEY.c_str());
    }
  }
  /* check for (optional) output directory */
  if (root.isMember(STRICT_KEY)) {
    if (root[STRICT_KEY].isBool()) {
      m_strict = root[STRICT_KEY].asBool();
    }
    else {
      fprintf(stderr, "Error, cannot parse JSON field '%s' as a boolean...Ignoring value\n", STRICT_KEY.c_str());
    }
  }
  /* check for (optional) addtional files */
  if (root.isMember(ADDITIONAL_FILES_KEY)) {
    /* if it exists, it has to be correctly formatted (or errror) */
    if (root[ADDITIONAL_FILES_KEY].isArray()) {
      Json::Value tmp = root.get(ADDITIONAL_FILES_KEY, 0);
      for (Json::Value::iterator it=tmp.begin(); it!=tmp.end(); ++it) {
        if ((*it).isString()) {
          m_additionalFiles.push_back((*it).asString());
        }
        else {
          std::cerr << "Error, parsing '" << ADDITIONAL_FILES_KEY << "'. Cannot cast '" << *it << "' as string" << std::endl;
          return false;
        }
      }
    }
    else {
      fprintf(stderr, "Error, optional field '%s' must be an array of file/directory names\n", ADDITIONAL_FILES_KEY.c_str());
      return false;
    }
  }
  return true;
}

bool HtmlContext::work ()
{
  /* read config file */
  Json::Value root;
  if (!ReadJsonFile(root, m_configPath)) {
    return false;
  }
  else if (m_verbose) {
    printf("Successfully read configuration file '%s'\n", m_configPath.c_str());
  }
  /* parse JSON into member objects */
  if (!parseJson(root)) {
    fprintf(stderr, "Error, could not parse '%s' into valid configuration\n", m_configPath.c_str());
    return false;
  }
  else if (m_verbose) {
    printf("Successfully parsed JSON configuration file\n");
  }
  /* finally call create */
  if (!createFiles()) {
    fprintf(stderr, "Error, could not create files\n");
    return false;
  }
  else if (m_verbose) {
    printf("Successfully created files\n");
  }
  return true;
}

// TODO OS agnostic
bool HtmlContext::isDirectory (const std::string& filePath) const
{
  struct stat fstat;
  if(!stat(filePath.c_str(), &fstat)) {
    return fstat.st_mode & S_IFDIR;
  }
  else {
    fprintf(stderr, "Error, received error code when checking for '%s'\n", filePath.c_str());
  }
  return false;
}

// TODO OS agnostic
bool HtmlContext::fileExists (const std::string& filePath) const
{
  struct stat fstat;
  return stat(filePath.c_str(), &fstat) == 0;
}

bool HtmlContext::directoryExists (const std::string& filePath) const
{
  struct stat fstat;
  return (stat(filePath.c_str(), &fstat) == 0) && (fstat.st_mode & S_IFDIR);
}

bool HtmlContext::createDirectory (const std::string& filePath) const
{
  return mkdir(filePath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
}

bool HtmlContext::createFiles ()
{
  /* output files and output file iterator */
  std::vector<std::ofstream*> outputFiles;
  for (std::list<std::string>::const_iterator it=m_dynamicFiles.begin(); it!=m_dynamicFiles.end(); ++it) {
    std::stringstream ss;
    ss << m_outputDirectory << *it << "." << m_fileExtension;
    std::ofstream* curFile = new std::ofstream(ss.str().c_str(), std::ofstream::out|std::ofstream::binary);
    if (!curFile->is_open()) {
      delete curFile;
      std::cerr << "Error, could not open '" << ss.str() << "'" << std::endl;
      return false;
    }
    else if (m_verbose) {
      std::cout << "Opened file: '" << ss.str() << "'" << std::endl;
    }
    outputFiles.push_back(curFile);
  }
  /* step through 'file order' */
  for (std::list<std::string>::const_iterator it=m_fileOrder.begin(); it!=m_fileOrder.end(); ++it) {
    if ((*it).empty()) {
      std::cout << "Ignoring empty file entry" << std::endl;
    }
    if ((*it).at(0)=='*') {
      /* dynamic page, build file suffix from iterator */
      std::string fileSuffix = (*it).substr(1);

      /* get 2 iterators, 1 for input & 1 for output */
      std::list<std::string>::const_iterator iIt = m_dynamicFiles.begin();
      std::vector<std::ofstream*>::iterator oIt = outputFiles.begin();
      for ( ; iIt!=m_dynamicFiles.end(); ++iIt, ++oIt) {
        std::stringstream ss;
        ss << m_inputDirectory << *iIt << fileSuffix;
        std::ifstream curFile(ss.str().c_str(), std::ifstream::in|std::ifstream::binary);
        if (curFile.is_open()) {
          if (m_verbose) {
            std::cout << "Opened dynamic file '" << ss.str() << "'" << std::endl;
          }
          appendSingleFile(*oIt, curFile);
        }
        else if (m_strict) {
          std::cerr << "Error, mode is set to strict and input file '" << ss.str() << "' does not exist" << std::endl;
          it = m_fileOrder.end();
          break;
        }
        else if (m_verbose) {
          std::cout << "Skipping '" << ss.str() << "' as it does not exist" << std::endl;
        }

      }
    }
    else {
      std::stringstream ss;
      ss << m_inputDirectory << *it;
      std::ifstream curFile(ss.str().c_str(), std::ifstream::in|std::ifstream::binary);
      if (curFile.is_open()) {
        if (m_verbose) {
          std::cout << "Opened file '" << ss.str() << "'" << std::endl;
        }
        appendToAllFiles(outputFiles, curFile);
      }
      else {
        std::cerr << "Error, could not open required file '" << ss.str() << "'" << std::endl;
        break;
      }
    }
  }

  /* copy additional files */
  for (std::list<std::string>::const_iterator it=m_additionalFiles.begin(); it!=m_additionalFiles.end(); ++it) {
    /* build up full input file name */
    std::stringstream ss;
    ss << m_inputDirectory << *it;
    /* check if the file/directory exists */
    if (fileExists(ss.str())) {
      if (isDirectory(ss.str())) {
        copyDirectory(ss.str(), BuildFileName(m_outputDirectory, *it));
      }
      else {
        copyFile(ss.str(), BuildFileName(m_outputDirectory, *it));
      }
    }
    else {
      fprintf(stderr, "Warning, additional file '%s' does not exist. Skipping\n", ss.str().c_str());
    }
  }

  if (m_verbose) {
    printf("Closing files\n");
  }
  /* finally, free data */
  for (std::vector<std::ofstream*>::iterator it=outputFiles.begin(); it!=outputFiles.end(); ++it)
  {
    delete *it;
  }
  return true;
}

bool HtmlContext::appendToAllFiles (std::vector<std::ofstream*> outputFiles, const std::ifstream& inputFile)
{
  /* read input file into a string */
  std::stringstream ss;
  ss << inputFile.rdbuf();
  std::string str = ss.str();
  /* output it to file */
  for (std::vector<std::ofstream*>::iterator it=outputFiles.begin(); it!=outputFiles.end(); ++it) {
    (*it)->write(str.c_str(), str.size());
  }
  return true;
}

bool HtmlContext::appendSingleFile (std::ofstream* outputFile, const std::ifstream& inputFile)
{
  /* read input file into a string */
  std::stringstream ss;
  ss << inputFile.rdbuf();
  std::string str = ss.str();
  /* output it to file */
  outputFile->write(str.c_str(), str.size());
  return true;
}

std::string HtmlContext::BuildFileName (const std::string& filePath, const std::string& fileName)
{
  return filePath + "/" + fileName;
}

bool HtmlContext::copyDirectory (const std::string& sourcePath, const std::string& destPath)
{
  /* create directory if it doesn't exist */
  if(!directoryExists(destPath) && !createDirectory(destPath)) {
    printf("Error, unable to create destination directory: '%s'\n", destPath.c_str());
    return false;
  }
  DIR* dirp = opendir(sourcePath.c_str());
  struct dirent * dp;
  while ((dp=readdir(dirp))) {
    /* create string */
    std::string curFile(dp->d_name);
    if (curFile!="." && curFile!="..") {
      /* build source file */
      std::string sourceFile = BuildFileName(sourcePath, curFile);
      /* build destination file */
      std::string destFile = BuildFileName(destPath, curFile);
      /* check if directory */
      if (isDirectory(sourceFile)) {
        copyDirectory(sourceFile, destFile);
      }
      else {
        copyFile(sourceFile, destFile);
      }
    }
  }
  closedir(dirp);
  return true;
}

bool HtmlContext::copyFile (const std::string& sourcePath, const std::string& destPath)
{
  std::ifstream src(sourcePath, std::ios::binary);
  std::ofstream dst(destPath, std::ios::binary);
  dst << src.rdbuf();
  return true;
}

} /* namespace basis */
