#!/bin/bash

# VIM plugin installing script
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

declare -a plugins=(
# Essential bundles
"Plugin 'VundleVim/Vundle.vim'"
#"Plugin 'tpope/vim-fugitive'"
#"Plugin 'git://git.wincent.com/command-t.git'"
#"Plugin 'file:///home/gmarik/path/to/plugin'"
#"Plugin 'rstacruz/sparkup', {'rtp': 'vim/'}"

# Add personal bundles from here
"Plugin 'scrooloose/nerdtree'"
"Plugin 'altercation/vim-colors-solarized'"
)

# Personal key mappings
declare -a mappings=(
"nmap  <F9> :NERDTreeToggle<CR>"
)

instruction="
  [Plugins]
    :PluginInstall
    :PluginClean
    :PlugUpdate
    :PlugUpgrade

  [NERDTree]
    ctrl + w + w : Move between windows.
    u : Move current working directory below.
    r : Reload.
    i : Open the current file to new window.
    t : Open the cursored file to new tab.
    a : New file.
    m : Move file.
    d : Delete file.
    c : Copy file.
"

echo "VIM plugin install script"
echo "  Latest 2021.02.05. Xubuntu 20.04"

if [ -d "${home_dir}/.vim" ]; then
	echo "  Directory exists! Skipping clone operation."
else
	echo "  Git repo cloneing..."
	git clone https://github.com/VundleVim/Vundle.vim.git ${home_dir}/.vim/bundle/Vundle.vim
	echo "  Done."
fi
wait

# Plugin install
echo "  Plugin recording..."
echo ""

echo "  ${home_dir}/.vimrc"
echo "set nocompatible" >> ${home_dir}/.vimrc
echo "filetype off" >> ${home_dir}/.vimrc
echo "set rtp+=~/.vim/bundle/Vundle.vim" >> ${home_dir}/.vimrc
echo "filetype off" >> ${home_dir}/.vimrc

echo "call vundle#begin()" >> ${home_dir}/.vimrc
for plugin in "${plugins[@]}"; do
	echo "  ├── ${plugin}"
	echo "${plugin}" >> ${home_dir}/.vimrc
done

echo "call vundle#end()" >> ${home_dir}/.vimrc
echo "filetype plugin indent on" >> ${home_dir}/.vimrc

echo ""
echo "  Done."
echo "  Installing plugin for VIM..."
vim +PluginInstall +qall
echo "  Done."

echo "  Key mapping..."
for mapping in "${mappings[@]}"; do
	echo "  ├── ${mapping}"
	echo "${mapping}" >> ${home_dir}/.vimrc
done

echo ""
echo "${instruction}"


# wget https://raw.githubusercontent.com/jjeaby/jscript/master/.vimrc -P ${home_dir}/
