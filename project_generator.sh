#!/bin/bash

# Initialize a flag to track the "-r" option
create_remote=false

# Process command-line options
while getopts ":r" opt; do
  case ${opt} in
    r )
      create_remote=true
      ;;
    \? )
      echo "Usage: $0 [-r] project_name"
      exit 1
      ;;
  esac
done
shift $((OPTIND -1))

# Check if a directory name was provided
if [ $# -eq 0 ]; then
    echo "Usage: $0 [-r] project_name"
    exit 1
fi

project_name=$1
base_path="projects/$project_name"

# Create the common directory structure
mkdir -p "$base_path"/docs
mkdir -p "$base_path"/local/include
mkdir -p "$base_path"/local/src

# Add .gitkeep files
touch "$base_path"/docs/.gitkeep
touch "$base_path"/local/include/.gitkeep
touch "$base_path"/local/src/.gitkeep

# Create the remote directory structure if "-r" option is set
if $create_remote; then
    mkdir -p "$base_path"/remote/include
    mkdir -p "$base_path"/remote/src

    # Add .gitkeep files in remote directories
    touch "$base_path"/remote/include/.gitkeep
    touch "$base_path"/remote/src/.gitkeep

    echo "Project structure with local and remote directories created under $base_path"
else
    echo "Project structure with local directory created under $base_path"
fi
