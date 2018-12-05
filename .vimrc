let g:solarized_termcolors=256

syntax enable
let g:solarized_termcolors=256
set background=dark
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

let g:syntastic_c_checkers = ['gcc']

let g:syntastic_always_populate_loc_list = 1
let g:syntastic_auto_loc_list = 1
let g:syntastic_check_on_open = 0
let g:syntastic_check_on_wq = 0

let g:syntastic_c_include_dirs = ['build/debug', 'build/debug/include', 'include']
let g:syntastic_c_check_header = 0
let g:syntastic_c_compiler_options = '-Wall -Wextra -Werror -Wuninitialized'

autocmd BufNewFile,BufRead *.test.cpp,*.mock.c let g:syntastic_cpp_compiler_options = '-D__testing=1'

let g:formatdef_astyle_google_c = '"astyle --style=google"'
let g:formatters_c = ['astyle_google_c']

