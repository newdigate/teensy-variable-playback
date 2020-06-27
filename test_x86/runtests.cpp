#include <stdint.h>
#include <iostream>
#include "SD/SD.h"
#include <unistd.h>
#include <dirent.h>
#include "../common/ResamplingSdReader.h"
using namespace std;

//TODO: Change this
const char *sdcardlocation = const_cast<const char *>("/Users/nicholasnewdigate/Development/SD");

void test_resampling_forward();
void test_resampling_reverse();

int main(int argc, char **argv){
	std::cout << "starting app...\n";
    initialize_mock_arduino();

    test_resampling_forward();
    test_resampling_reverse();
    std::cout << "complete...\n";
}

void test_resampling_forward() {

    SD.setSDCardFolderPath(sdcardlocation);

    ResamplingSdReader reader;
    reader.begin();
    reader.setPlaybackRate(1.0);
    reader.play("test2.bin");
    reader.setLoopType(looptype_none);
    int16_t buffer[256];

    int j = 0, bytesRead = 0, total_bytes_read = 0;
    do {
        bytesRead = reader.read( buffer, 512 );
        total_bytes_read += bytesRead;
        printf("j:%d bytesRead: %d \n", j, bytesRead);
        
        for (int i=0; i < bytesRead/2; i++) {
            printf("\t\t[%x]:%x", i, buffer[i]);
        }
        
        printf("\n");
        j++;
    } while (bytesRead > 0);
    printf("total_bytes_read: %d \n", total_bytes_read);
}

void test_resampling_reverse() {

    SD.setSDCardFolderPath(sdcardlocation);

    ResamplingSdReader reader;
    reader.begin();
    reader.setPlaybackRate(-1.0);
    reader.play("test2.bin");
    reader.setLoopType(looptype_none);
    int16_t buffer[256];

    int j = 0, bytesRead = 0, total_bytes_read = 0;
    do {
        bytesRead = reader.read( buffer, 512 );
        total_bytes_read += bytesRead;
        printf("j:%d bytesRead: %d \n", j, bytesRead);
        for (int i=0; i < bytesRead/2; i++) {
            printf("\t\t[%x]:%x", i, buffer[i]);
        }
        printf("\n");
        j++;
    } while (bytesRead > 0);
    printf("total_bytes_read: %d \n", total_bytes_read);
}

void millis_test() {
  unsigned long start = millis();
  cout << "millis() test start: " << start << endl;
  while( millis() - start < 10000 ) {
	cout << millis() << endl;
	delay(1);
  }
  unsigned long end = millis();
  cout << "End of test - duration: " << end - start << "ms" << endl;
}

void delay_test() {
  unsigned long start = millis();
  cout << "delay() test start: " << start << endl;
  while( millis() - start < 10000 ) {
	cout << millis() << endl;
	delay(250);
  }
  unsigned long end = millis();
  cout << "End of test - duration: " << end - start << "ms" << endl;
}