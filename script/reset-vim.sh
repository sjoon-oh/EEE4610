#!/bin/bash

# VIM plugin uninstallation script
# written by acoustikue@yonsei.ac.kr

home_dir=~
declare -a settings=(
"set nu"
"set autoindent"
"set ts=4"
"set sts=4"
"set cindent"
"set laststatus=2"
"set statusline=\\ %<%l:%v\\ [%P]%=%a\\ %h%m%r\\ %F\\ "
"set paste"
"set shiftwidth=4"
"set showmatch"
"set smartcase"
"set smarttab"
"set ruler"
"color slate"
"syntax on"
)

echo "VIM plugin reset script"
echo "  Latest 2021.02.05. Xubuntu 20.04"

if [ -d "${home_dir}/.vim" ]; then
	echo "  Deleting directory: ${home_dir}/.vim"
	vim +PluginClean +qall
	wait
	rm -rf ${home_dir}/.vim
wait
fi

# File reset
if [ -e ${home_dir}/.vimrc ]; then
	rm ${home_dir}/.vimrc
	wait
	touch ${home_dir}/.vimrc
	echo "  .vimrc file reset done."
else
	echo "  .vimrc not found. New file created."
	touch ${home_dir}/.vimrc
fi

echo ""
echo "  Setting minimals for VIM..."
echo "  ${home_dir}/.vimrc"
for setting in "${settings[@]}"; do
	echo "  ├── ${setting}"
	echo "${setting}" >> ${home_dir}/.vimrc
done
echo ""
echo "Done."

# Theme installation
# https://www.nordtheme.com/docs/ports/vim/installation

