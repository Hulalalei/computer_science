require("toggleterm").setup{
  -- size can be a number or function which is passed the current terminal
  size = function(term)
    if term.direction == "horizontal" then
      return 10
    elseif term.direction == "vertical" then
      return vim.o.columns * 0.4
    end
  end,
  open_mapping = [[<C-t>]],
  hide_numbers = true, -- hide the number column in toggleterm buffers
  -- shade_filetypes = {},
  autochdir = true,
  shade_terminals = false,
  -- shading_factor = '1', -- the degree by which to darken to terminal colour, default: 1 for dark backgrounds, 3 for light
  start_in_insert = true,
  insert_mappings = true, -- whether or not the open mapping applies in insert mode
  terminal_mappings = true, -- whether or not the open mapping applies in the opened terminals
  persist_size = true,
  persist_mode = true,
  -- direction = 'vertical',
  direction = 'tab',
  -- direction = 'vertical',
  close_on_exit = true, -- close the terminal window when the process exits
  auto_scroll = false, -- automatically scroll to the bottom on terminal output
  -- This field is only relevant if direction is set to 'float'
  float_opts = {
    -- The border key is *almost* the same as 'nvim_open_win'
    -- see :h nvim_open_win for details on borders however
    -- the 'curved' border is a custom border type
    -- not natively supported but implemented in this plugin.
    border = 'curved',
    width = 70,
    height = 18,
    winblend = 3,
  },
    highlights = {
        -- highlights which map to a highlight group name and a table of it's values
        -- NOTE: this is only a subset of values, any group placed here will be set for the terminal window split
        Normal = {
            guibg = "none",
        },
        NormalFloat = {
            link = 'Normal',
        },
        FloatBorder = {
            guibg = "none",
        },
    },
  winbar = {
    enabled = false,
    name_formatter = function(term) --  term: Terminal
      return term.name
    end
  },
}

-- local Terminal = require('toggleterm.terminal').Terminal

function _G.set_terminal_keymaps()
  local opts = {buffer = 0}
  vim.keymap.set('n', '<Esc>', [[<Cmd>quit<CR>]], opts)
  -- vim.keymap.set('n', '<C-\\>', [[<Cmd>quit<CR>]], opts)
  vim.keymap.set('n', 'q', [[<Cmd>quit<CR>]], opts)
  vim.keymap.set('n', '<C-c>', [[<Cmd>startinsert<CR><C-c>]], opts)
  vim.keymap.set('n', '<CR>', [[<Cmd>startinsert<CR>]], opts)
  vim.keymap.set('n', '<Up>', [[<Cmd>startinsert<CR><Up>]], opts)
  vim.keymap.set('n', '<Down>', [[<Cmd>startinsert<CR><Down>]], opts)
  vim.keymap.set('n', '<Right>', [[<Cmd>startinsert<CR><Right>]], opts)
  vim.keymap.set('n', '<Left>', [[<Cmd>startinsert<CR><Left>]], opts)
  vim.keymap.set('t', '<Esc>', [[<C-\><C-n>]], opts)
  vim.keymap.set('t', '<S-Insert>', [[<C-\><C-n>"+pi]], opts)
  -- vim.opt_local.switchbuf = 'split'
  -- vim.keymap.set('t', '<C-r>', [['<C-\><C-n>"'.nr2char(getchar()).'pi']], {expr=true,buffer=0})
  -- vim.keymap.set('t', 'jk', [[<C-\><C-n>]], opts)
  -- vim.keymap.set('t', 'kj', [[<C-\><C-n>]], opts)
  -- vim.keymap.set('t', '<C-h>', [[<Cmd>wincmd h<CR>]], opts)
  -- vim.keymap.set('t', '<C-j>', [[<Cmd>wincmd j<CR>]], opts)
  -- vim.keymap.set('t', '<C-k>', [[<Cmd>wincmd k<CR>]], opts)
  -- vim.keymap.set('t', '<C-l>', [[<Cmd>wincmd l<CR>]], opts)
end

-- if you only want these mappings for toggle term use term://*toggleterm#* instead
vim.cmd('autocmd! TermOpen term://* lua set_terminal_keymaps()')
