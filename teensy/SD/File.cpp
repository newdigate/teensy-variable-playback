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

uint32_t File::numInstances = 0;
uint32_t File::numOpenFiles = 0;

char *SDClass::_fileData = NULL;
uint32_t SDClass::_fileSize = 0;

std::streampos File::fileSize( const char* filePath ){
    return SDClass::_fileSize;
}

File::File(SdFile f, const char *n) {
    _position = 0;
    numInstances++;
    numOpenFiles++;
    Serial.printf("File::numInstances = %d; (SdFile: %s)\n", numInstances, n);
}

File::File(const char *n, uint8_t mode) {
    _position = 0;
    numInstances++;
    numOpenFiles++;
    Serial.printf("File::numInstances = %d; (filename: %s, numOpenFiles: %d)\n", numInstances, n, numOpenFiles);
}

File::File(void) {
  _size = 0;
  numInstances++;
  Serial.printf("File::numInstances = %d; empty ()\n", numInstances);
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
    if (_position >= SDClass::_fileSize) {
        printf("!!! CRITICAL: read outside bounds of file...");
        return 0;
    }

    int result = SDClass::_fileData[_position];
    _position++;
    return result;
}

// buffered read for more efficient, high speed reading
int File::read(void *buf, uint16_t nbyte) {
    char * target = (char*)buf;
    for (int i=0; i < nbyte; i++) {
        if  (_position >= SDClass::_fileSize)
            return i;
        int byteRead = File::read();
        target[i] = static_cast<char>(byteRead);
    }
    return nbyte;
}

int File::available() {
    return (_position < SDClass::_fileSize);
}

void File::flush() {
}
bool File::seek(uint32_t pos) {
    if (pos < SDClass::_fileSize) {
        _position = pos;
        return true;
    }
    return false;
}

uint32_t File::position() {
    return _position;
}

uint32_t File::size() {
    return SDClass::_fileSize;
}

void File::close() {
    numOpenFiles--;
    Serial.printf("File::close() - open files: %d\n", numOpenFiles);
}

File::operator bool() {
    return  true;
}

bool File::is_directory( const char* pzPath )
{
    return false;
}