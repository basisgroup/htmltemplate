/*
 * HtmlContext.hpp
 *
 *  Created on: May 3, 2018
 *      Author: schnet
 */

#ifndef HTMLCONTEXT_HPP_
#define HTMLCONTEXT_HPP_

#include <json/json.h>
#include <list> // TODO why?
#include <vector>
#include <fstream>
#include <string>

namespace basis {

class HtmlContext
{
  public:
    HtmlContext(const std::string& configFile);
    virtual ~HtmlContext();

    bool work();

    static bool ReadJsonFile (Json::Value& root, const std::string& filePath);
    static std::string BuildFileName (const std::string& dir_, const std::string& fileName);

  private:
    bool parseJson(const Json::Value& root);
    bool createFiles();
    bool appendToAllFiles(std::vector<std::ofstream*> outputFiles, const std::ifstream& inputFile);
    bool appendSingleFile(std::ofstream* outputFile, const std::ifstream& inputFile);

    bool copyDirectory(const std::string& sourcePath, const std::string& destPath);
    bool copyFile(const std::string& sourcePath, const std::string& destPath);
    bool isDirectory(const std::string& filePath) const;
    bool fileExists(const std::string& filePath) const;
    bool directoryExists(const std::string& filePath) const;
    bool createDirectory(const std::string& filePath) const;

    static const std::string DYNAMIC_FILES_KEY;
    static const std::string FILE_ORDER_KEY;
    static const std::string ADDITIONAL_FILES_KEY;
    static const std::string FILE_EXTENSION_KEY;
    static const std::string WEBSITE_KEY;
    static const std::string INPUT_DIRECTORY_KEY;
    static const std::string OUTPUT_DIRECTORY_KEY;
    static const std::string STRICT_KEY;
    static const std::string VERBOSE_KEY;

    std::string m_configPath;
    std::string m_fileExtension;
    std::string m_inputDirectory;
    std::string m_outputDirectory;
    std::string m_website;
    bool m_strict;
    std::list<std::string> m_dynamicFiles;
    std::list<std::string> m_fileOrder;
    std::list<std::string> m_additionalFiles;
    bool m_verbose;

};

} /* namespace basis */

#endif /* HTMLCONTEXT_HPP_ */
