-- neovim 9.0+ required

require('archvim.options')
require('archvim.plugins')
require('archvim.mappings')
require('archvim.custom')

function print(...)
    vim.notify(vim.inspect({...}))
end
