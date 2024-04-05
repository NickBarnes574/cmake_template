#!/bin/bash

# Initialize flags to track the "-r" and "-d" options
create_remote=false
create_docs=false
create_tests=false

# Process command-line options
while getopts ":rdt" opt; do
  case ${opt} in
    r )
      create_remote=true
      ;;
    d )
      create_docs=true
      ;;
    t )
      create_tests=true
      ;;
    \? )
      echo "Usage: $0 [-r] [-d] [-t] project_name"
      exit 1
      ;;
  esac
done
shift $((OPTIND -1))

# Check if a directory name was provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 [-r] [-d] [-t] project_name"
    exit 1
fi

project_name=$1
base_path="$project_name"

# Create the docs directory structure if "-d" option is set
if $create_docs; then
    mkdir -p "$base_path"/docs
    touch "$base_path"/docs/.gitkeep
fi

# Create the local directory structure
mkdir -p "$base_path"/local/include
mkdir -p "$base_path"/local/src

# Create the tests directory structure if "-t" option is set
if $create_tests; then
    mkdir -p "$base_path"/local/tests
    touch "$base_path"/local/tests/.gitkeep
fi


# Add .gitkeep files in local directories
touch "$base_path"/local/include/.gitkeep
touch "$base_path"/local/src/.gitkeep

# Create the remote directory structure if "-r" option is set
if $create_remote; then
    mkdir -p "$base_path"/remote/include
    mkdir -p "$base_path"/remote/src

    # Add .gitkeep files in remote directories
    touch "$base_path"/remote/include/.gitkeep
    touch "$base_path"/remote/src/.gitkeep
fi

# Output message based on created directories
echo "Successfully created directory structure under $base_path"
