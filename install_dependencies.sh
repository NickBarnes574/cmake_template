#!/bin/bash

sudo apt install libglew-dev
sudo apt install libsdl2-dev
sudo apt install libsdl2-ttf-dev
sudo apt install libsdl2-image-dev
sudo apt install mesa-utils

set -e  # Exit on error

echo "Removing old CMake version..."
sudo apt remove --purge -y cmake

echo "Installing required dependencies..."
sudo apt update
sudo apt install -y apt-transport-https ca-certificates gnupg software-properties-common wget

echo "Adding Kitware's GPG key..."
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo gpg --dearmor -o /usr/share/keyrings/kitware-archive-keyring.gpg

echo "Adding Kitware's APT repository..."
echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | sudo tee /etc/apt/sources.list.d/kitware.list > /dev/null

echo "Updating package lists..."
sudo apt update

echo "Installing latest CMake..."
sudo apt install -y cmake

echo "Verifying installation..."
cmake --version

echo "CMake upgrade completed successfully!"
