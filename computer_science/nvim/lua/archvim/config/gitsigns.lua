require'gitsigns'.setup {
    current_line_blame = false,
    current_line_blame_opts = {
        virt_text = true,
        virt_text_pos = 'eol',
        delay = 2000,
    },
}
local success, handler = pcall(require, "scrollbar.handlers.gitsigns")
if success then
    handler.setup()
end
