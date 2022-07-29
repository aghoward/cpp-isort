func! cppisort#run()
    let winview=winsaveview()
    let path = expand("%")
    let path = fnameescape(path)

    let l:import_sort_executable = 'isort-cpp'
    if executable(l:import_sort_executable)
        call system(l:import_sort_executable . ' ' . path)
        silent exec "e"
    else
        " Executable bin doesn't exist
        echoerr 'Can not find isort-cpp'
        return 1
    endif
    call winrestview(winview)
endfun

func! cppisort#stop_auto()
    autocmd! cppisort
endfun

func! cppisort#start_auto()
    augroup cppisort
        autocmd! cppisort
        " do not do import sort on save in diff mode
        if !&diff
            au BufWritePost *.h,*.hpp,*.cc,*.cpp,*.c call cppisort#run()
        endif
    augroup END
endfun

