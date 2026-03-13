require("aerial").setup({
    -- optionally use on_attach to set keymaps when aerial has attached to a buffer
    -- on_attach = function(bufnr)
    --     -- Jump forwards/backwards with '{' and '}'
    --     -- vim.keymap.set("n", "{", "<cmd>AerialPrev<CR>", { buffer = bufnr })
    --     -- vim.keymap.set("n", "}", "<cmd>AerialNext<CR>", { buffer = bufnr })
    -- end,
    nerd_font = require'archvim.options'.nerd_fonts and "auto" or false,
    use_icon_provider = require'archvim.options'.nerd_fonts,
    dense = not require'archvim.options'.nerd_fonts,
    layout = {
        max_width = { 40, 0.25 },
        min_width = 16,
        resize_to_content = true,
        preserve_equality = true,
    },
    keymaps = {
        ["q"] = {
            callback = function() vim.cmd [[ :AerialClose ]] end,
            desc = "Close the aerial window",
            nowait = true,
        },
    },
})
-- You probably also want to set a keymap to toggle aerial
vim.keymap.set("n", "<F12>", "<cmd>AerialToggle!<CR>")
local found_telescope, telescope = pcall(require, "telescope")
if found_telescope then
    telescope.load_extension("aerial")
end
