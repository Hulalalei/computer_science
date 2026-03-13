require'genius'.setup {
    default_bot = 'openai',
    completion_delay_ms = -1,
    config_openai = os.getenv('ARCHIBATE_COMPUTER') and {
        -- base_url = "https://api.deepseek.com/beta",
        -- api_key = os.getenv("DEEPSEEK_API_KEY"),
        -- infill_options = {
        --     max_tokens = 120,
        --     model = "deepseek-coder",
        --     temperature = 0.8,
        -- },
        base_url = "https://api.openai.com",
        -- base_url = "http://127.0.0.1:8080/https://api.openai.com\nhttps://api.openai.com",
        api_key = os.getenv("OPENAI_API_KEY"),
    } or nil,
}

-- vim.cmd [[
-- inoremap <C-Space> <Cmd>GeniusComplete<CR>
-- ]]
