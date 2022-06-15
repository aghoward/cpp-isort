if exists('g:cpp_isort_loaded')
    finish
endif
let g:cpp_isort_loaded = 1

if !exists('g:cpp_isort_auto')
    let g:cpp_isort_auto = 0
endif

if g:cpp_isort_auto
    call cppisort#start_auto()
endif

command! CppSortImport call cppisort#run()
command! StopCppAutoSortImport call cppisort#stop_auto()
command! StartCppAutoSortImport call cppisort#start_auto()
