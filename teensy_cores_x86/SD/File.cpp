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
    return _size;
}

File::File(SdFile f, const char *n) {
    _position = 0;
}

File::File(const char *n, uint8_t mode) {
    _position = 0;
}

File::File(void) {
  _size = 0;
}

// returns a pointer to the file name
char *File::name(void) {
    return NULL;
}

// a directory is a special type of file
bool File::isDirectory(void) {
  return false;
}

int File::write(uint8_t val) {
    return write(&val, 1);
}

int File::write(const uint8_t *buf, size_t size) {
    return 0;
}

int File::peek() {
    return 0;
}

int File::read() {
    if (_position >= _size) {
        printf("!!! CRITICAL: read outside bounds of file...");
        return 0;
    }

    int result = _data[_position];
    _position++;
    return result;
}

// buffered read for more efficient, high speed reading
int File::read(void *buf, uint16_t nbyte) {
    char * target = (char*)buf;
    for (int i=0; i < nbyte; i++) {
        if  (_position >= _size)
            return i;
        int byteRead = File::read();
        target[i] = static_cast<char>(byteRead);
    }
    return nbyte;
}

int File::available() {
    return (_position < _size);
}

void File::flush() {
}
bool File::seek(uint32_t pos) {
    if (pos < _size) {
        _position = pos;
        return true;
    }
    return false;
}

uint32_t File::position() {
    return _position;
}

uint32_t File::size() {
    return _size;
}

void File::close() {
}

File::operator bool() {
    return  true;
}

bool File::is_directory( const char* pzPath )
{
    return false;
}

void File::setMockData( char *data, uint32_t size) {
    _data = data;
    _size = size;
}
