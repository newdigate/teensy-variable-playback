#include <boost/test/unit_test.hpp>
#include "IndexedFileFixture.h"

#include <SD.h>
#include "IndexableFile.h"

BOOST_AUTO_TEST_SUITE(test_indexablefile)

    BOOST_FIXTURE_TEST_CASE(basic_test, IndexableFileFixture) {

        const uint16_t sample_size = 30;
        int16_t file_contents[sample_size] = {0};
        for (int i=0; i<sample_size; i++) {
            file_contents[i] = i;
        }
        SD.setSDCardFileData((char*)file_contents, sample_size * 2);

        newdigate::IndexableFile<16, 2> indexable("blah.h");               // use max 2 buffers, with 16 elements each....

        /*
        for (int i=0; i<sample_size; i++) {
            std::cout << i << " " << (int)indexable[i] << std::endl;
        } 

        for (int i=sample_size; i>0; i--) {
            std::cout << i-1 << " " << (int)indexable[i-1] << std::endl;
        }
        */
        indexable.close();
    }

BOOST_AUTO_TEST_SUITE_END()
