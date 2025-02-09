#!/bin/bash

# This was:
# cd cmake-build-debug/test/
# ./tests

# Now like this, to match the github action:
mkdir output
cmake-build-debug/test/tests --output_format=XML --report_level=detailed  2> output/test-results.xml
xmllint --format output/test-results.xml > output/test-results-formatted.xml
xsltproc test/boost-to-junit-xml.xslt output/test-results.xml > output/junit-test-results.xml
