#!/bin/bash

# Check if the required arguments are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <writefile> <writestr>"
    exit 1
fi

writefile="$1"
writestr="$2"

# Create the directory if it doesn't exist
mkdir -p "$(dirname "$writefile")"

# Write the content to the file, overwrite if it already exists
echo "$writestr" > "$writefile"

# Check if the file was created successfully
if [ $? -eq 0 ]; then
    echo "File $writefile created successfully with content:"
    echo "$writestr"
else
    echo "Error: Failed to create or write to file $writefile"
    exit 1
fi
