/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#include "SD.h"
#include <dirent.h>

using namespace std;

std::streampos File::fileSize( const char* filePath ){
    
    std::streampos fsize = 0;
    std::ifstream file( filePath, std::ios::binary );
    
    fsize = file.tellg();
    file.seekg( 0, std::ios::end );
    fsize = file.tellg() - fsize;
    file.close();
    
    return fsize;
}

File::File(SdFile f, const char *n) {
    std::string actualFileName = SDClass::getSDCardFolderPath() + std::string("/") + std::string(n);
    cout << actualFileName;
    mockFile.open(actualFileName);
    _size = fileSize(actualFileName.c_str());
    _isDirectory = is_directory(actualFileName.c_str());
}
File::File(const char *n, uint8_t mode) {
    std::string actualFileName = SDClass::getSDCardFolderPath() + std::string("/") + std::string(n);
    // cout << actualFileName;
    switch (mode) {
        case xO_READ : mockFile.open(actualFileName); break;
        case xO_WRITE : mockFile.open(actualFileName, std::fstream::out | std::fstream::app); break;
        default:
            break;
    }
    _size = fileSize(actualFileName.c_str());
}

File::File(void) {
  _name[0] = 0;
    _size = 0;
}

// returns a pointer to the file name
char *File::name(void) {
  return _name;
}

// a directory is a special type of file
bool File::isDirectory(void) {
  return _isDirectory;
}

int File::write(uint8_t val) {
    return write(&val, 1);
}

int File::write(const uint8_t *buf, size_t size) {
    size_t t;
    if (!mockFile.is_open()) {
        return 0;
    }
    
    _size += size;
    char * memblock = (char *)buf;
    mockFile.write(memblock, size);

    return t;
}

int File::peek() {
  if (! mockFile.is_open())
    return 0;

  return mockFile.peek();
}

int File::read() {
    char p[1];
    mockFile.read(p, 1);
    return p[0];
}

// buffered read for more efficient, high speed reading
int File::read(void *buf, uint16_t nbyte) {
    char *bbb = (char *)buf;
    mockFile.read(bbb, nbyte);
    return nbyte;
}

int File::available() {
    if (! mockFile.is_open()) return 0;
    uint32_t p = position();
    uint32_t n = size() - p;

    return n > 0X7FFF ? 0X7FFF : n;
}

void File::flush() {
  if (mockFile.is_open())
      mockFile.flush();
}
bool File::seek(uint32_t pos) {
    if (! mockFile.is_open()) return false;
    mockFile.seekp(pos, ios_base::seekdir::cur);
    return true;
}

uint32_t File::position() {
    if (! mockFile.is_open()) return -1;
    return mockFile.tellp();
}

uint32_t File::size() {
    return _size;
}

void File::close() {
    if (mockFile.is_open())
        mockFile.close();
}

File::operator bool() {
    return  mockFile.is_open();
}

bool File::is_directory( const char* pzPath )
{
    if ( pzPath == NULL) return false;

    DIR *pDir;
    bool bExists = false;

    pDir = opendir (pzPath);

    if (pDir != NULL)
    {
        bExists = true;
        (void) closedir (pDir);
    }

    return bExists;
}
