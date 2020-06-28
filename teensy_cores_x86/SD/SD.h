/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#ifndef __SD_H__
#define __SD_H__

#include "../mock_arduino.h"
#include "utility/SdFat.h"
#include "../Teensy/Print.h"
#include <cstdio>
#include "utility/SdFatUtil.h"

#include <iostream>
#include <fstream>
#include <stdint.h>

using namespace std;

#define FILE_READ xO_READ
#define FILE_WRITE (xO_READ | xO_WRITE | xO_CREAT)
namespace SDLib {

class File {
 private:
    uint32_t _size;
    bool _isDirectory;
    std::streampos fileSize( const char* filePath );

     char* _data;
     uint32_t _position;

public:
    File(const char *name, uint8_t mode = xO_READ);
    File(SdFile f, const char *n); // wraps an underlying SdFile
    File(void);      // 'empty' constructor
    virtual int write(uint8_t);
    virtual int write(const uint8_t *buf, size_t size);
    virtual int read();
    virtual int peek();
    virtual int available();
    virtual void flush();
    int read(void *buf, uint16_t nbyte);
    bool seek(uint32_t pos);
    uint32_t position();
    uint32_t size();
    void close();
    operator bool();
    char * name();

    bool isDirectory(void);
    static bool is_directory( const char* pzPath );

    void setMockData( char *data, uint32_t size);
};

class SDClass {
private:
  // These are required for initialisation and use of sdfatlib
    Sd2Card card;
    SdVolume volume;
    SdFile root;

    // my quick&dirty iterator, should be replaced
    SdFile getParentDir(const char *filepath, int *indx);

    static std::string _sdCardFolderLocation;

    char *_fileData;
    uint32_t _fileSize;

    public:

    static std::string getSDCardFolderPath();

    static void setSDCardFolderPath(std::string path);
    
    void setSDCardFileData(char *data, uint32_t size) {
        _fileData = data;
        _fileSize = size;
    }

    // This needs to be called to set up the connection to the SD card
    // before other methods are used.
    bool begin(uint8_t csPin = 0);
    bool begin(uint32_t clock, uint8_t csPin);

    // Open the specified file/directory with the supplied mode (e.g. read or
    // write, etc). Returns a File object for interacting with the file.
    // Note that currently only one file can be open at a time.
    File open(const char *filename, uint8_t mode = FILE_READ);
    File open(const std::string &filename, uint8_t mode = FILE_READ) { return open( filename.c_str(), mode ); }

    // Methods to determine if the requested file path exists.
    bool exists(const char *filepath);
    bool exists(const std::string &filepath) { return exists(filepath.c_str()); }

    // Create the requested directory heirarchy--if intermediate directories
    // do not exist they will be created.
    bool mkdir(const char *filepath);
    bool mkdir(const std::string &filepath) { return mkdir(filepath.c_str()); }

    // Delete the file.
    bool remove(const char *filepath);
    bool remove(const std::string &filepath) { return remove(filepath.c_str()); }

    bool rmdir(const char *filepath);
    bool rmdir(const std::string &filepath) { return rmdir(filepath.c_str()); }

private:

    // This is used to determine the mode used to open a file
    // it's here because it's the easiest place to pass the
    // information through the directory walking function. But
    // it's probably not the best place for it.
    // It shouldn't be set directly--it is set via the parameters to `open`.
    int fileOpenMode;

    friend class File;
    friend bool callback_openPath(SdFile&, const char *, bool, void *);
};

extern SDClass SD;

};

// We enclose File and SD classes in namespace SDLib to avoid conflicts
// with others legacy libraries that redefines File class.

// This ensure compatibility with sketches that uses only SD library
using namespace SDLib;

// This allows sketches to use SDLib::File with other libraries (in the
// sketch you must use SDFile instead of File to disambiguate)
typedef SDLib::File    SDFile;
typedef SDLib::SDClass SDFileSystemClass;
#define SDFileSystem   SDLib::SD

#endif
