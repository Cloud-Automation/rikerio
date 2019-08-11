let g:solarized_termcolors=256

syntax enable
let g:solarized_termcolors=256
set background=light
colorscheme solarized

autocmd BufEnter * colorscheme solarized
autocmd BufEnter *.md colorscheme default

nmap <F8> :TagbarToggle<CR>

filetype plugin indent on
set tabstop=4
set shiftwidth=4
set expandtab

let g:cmake_build_type = 'Debug'

au BufWrite *.h,*.h.in,*.c,*.cpp :Autoformat

let g:autoformat_autoindent = 0
let g:autoformat_retab = 0
let g:autoformat_remove_trailing_spaces = 0

let g:formatdef_astyle_google_cpp = '"astyle --style=google"'
let g:formatters_cpp = ['astyle_google_cpp']

let g:ale_linters = { 'cpp' : ['g++'] }
let g:ale_cpp_gcc_options = '-Iinclude -Ibuild/debug/gtest-install/include -Ibuild/debug/include -Itest -Wall -pedantic -Wextra -std=c++11'

