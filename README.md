
# stm32l152re - configuration for nvim + stm32cubeide

compile_commands.json - uses for lsp
can be generated with bear or in my case with compiledb.
Debug in upload in cubeide, develop in nvim - thas is what I've done.

By the way u have to replace default ide linker with STM32L152RETx_CUBE_FLASH.ld.

Now it works.

In short, for another stm32 device - just initilize 2 projects with cubeide and make. Then join it and enjoy result.
## Authors

- [@hyper-hronoz](https://www.github.com/hyper-hronoz)


