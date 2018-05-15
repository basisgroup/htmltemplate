# HTML Templater
A non-graphical, open source website templater. I created it because I couldn't find a simple, free tool that could create webpages with a templated look and feel. Currently, there is only a Linux version. However, I am intending on supporting other Operating Systems (if there is any demand).

## JSON Configuration


## Building
This project uses Autotools. Here are the commands you'll need to build the project:

```
git clone https://github.com/basisgroup/htmltemplate.git
cd htmltemplate
./autogen.sh && configure && make
```
If that line doesn't successfully complete, you are probably missing some dependencies. You can grab them from the package managers like so:

**Ubuntu/Debian**
```
apt install autoconf automake gcc g++ libtool make 
```

**RedHat/CentOS**
```
yum install autoconf automake gcc gcc-c++ libtool make
```

## Running
Getting help:
```
./src/htmlTemplater -h
./src/htmlTemplater usage:

  -c  <config file>    Path to JSON configuration file
  -h  <help>           This help message

```

Running the example:
```
./src/htmlTemplater -c examples/basis.json
Successfully parsed JSON configuration file
Opened file: '/opt/www/basis/index.html'
Opened file: '/opt/www/basis/template.html'
Opened file: '/opt/www/basis/pointers.html'
Opened file: '/opt/www/basis/pilot.html'
Opened file 'examples/basis/header1.txt'
Opened file 'examples/basis/header2.txt'
Opened dynamic file 'examples/basis/index.body'
Opened dynamic file 'examples/basis/template.body'
Opened dynamic file 'examples/basis/pointers.body'
Opened dynamic file 'examples/basis/pilot.body'
Opened file 'examples/basis/footer.txt'
Closing files
Successfully created files
Successfully processed 'examples/basis.json'

```

## Configuration File
The configuration files are JSON based. Here is an example:

```
{
  "website" : "http://www.basisfunctional.com",
  "dynamicFiles" : ["index","template", "pointers", "pilot"],
  "fileOrder" : ["header1.txt", "header2.txt", "*.body", "footer.txt"],
  "additionalFiles" : ["layout.css", "images"],
  "fileExtension" : "html",
  "inputDirectory" : "basis",
  "outputDirectory" : "/opt/www/basis",
  "strict" : false,
  "verbose" : true
}
```

Here is a brief of the above configuration:

**website** *not required* - Target website name

**dynamicFiles** *required* - A list of the files that will be created

**fileOrder** *required* - An ordered list of input files. Entries that begin with the wildcard '*' field are dynamic files. The  wildcard will be replaced with all the entries in the dynamicFiles list

**additionalFiles** *not required* - Additional files that are to be copied directly to outputDirectory. Note: directories will be copied recursively

**fileExtension** *required* - File extension for output files

**inputDirectory** *not required* - Location of input files. If omitted the current working directory will be used

**outputDirectory** *not required* - Destination of output files. If omitted the current working directory will be used

**strict** *not required* - Boolean that sets where or not all dynamicFiles need to exist. In this example, the program would exit with an error if strict was set to true and 'template.body' didn't exist; however, if strict was set to false and 'template.body' didn't exist it would simply move onto the next file

**verbose** *not required* - Boolean to enable verbose output
