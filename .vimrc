let g:solarized_termcolors=256
syntax enable
set background=light
colorscheme solarized8

" Tabs

set tabstop=4
set shiftwidth=4
set tabstop=8 softtabstop=0 expandtab shiftwidth=4 smarttab

au BufWrite * :Autoformat

let g:autoformat_autoindent = 0
let g:autoformat_retab = 0
let g:autoformat_remove_trailing_spaces = 0
let g:formatdef_astyle_google_cpp = '"astyle --style=google"'
let g:formatters_cpp = ['astyle_google_cpp']

let g:ale_linters = { 'cpp' : ['g++'] }
let g:ale_cpp_gcc_options = '-Iinclude -Ibuild/debug -Ibuild/debug/include -Wall -Wextra -std=c++17'


let g:ale_c_gcc_options = '-Iinclude -Ibuild/debug -Ibuild/debug/include -Wall -Wextra -std=c++17'
